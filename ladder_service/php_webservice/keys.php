<?php
/**
 * Q3Rally Ladder – Server Key Registry
 * keys.php
 *
 * Manages per-server API keys. Stored as a single JSON file:
 *   data/private/server_keys.json
 * data/private/ is not reachable via HTTP (.htaccess, see README for nginx).
 * A key file in the old location data/server_keys.json is moved there
 * automatically. Every change runs under an exclusive lock and replaces the
 * file atomically (write temp file + rename), so concurrent requests can
 * neither lose updates nor read a half written file.
 *
 * Key record structure:
 * {
 *   "key":         "hex-string",
 *   "serverName":  "Q3Rally EU #1",
 *   "ownerName":   "SomeAdmin",
 *   "ownerEmail":  "admin@example.com",
 *   "status":      "pending"|"active"|"revoked",
 *   "createdAt":   "2024-04-05T18:30:11Z",
 *   "approvedAt":  "2024-04-05T19:00:00Z"|null,
 *   "lastUsedAt":  "2024-04-05T20:00:00Z"|null,
 *   "lastUsedIp":  "203.0.113.10"|null,
 *   "matchCount":  42
 * }
 */

declare(strict_types=1);

const KEYS_DIR               = __DIR__ . '/data/private';
const KEYS_FILE              = KEYS_DIR . '/server_keys.json';
const KEYS_LOCK_FILE         = KEYS_DIR . '/server_keys.lock';
const KEYS_LEGACY_FILE       = __DIR__ . '/data/server_keys.json';
const KEYS_REGISTER_LIMIT    = 10;   // registrations per IP and window
const KEYS_REGISTER_WINDOW   = 3600; // seconds
const KEYS_ADMIN_FAIL_LIMIT  = 5;    // failed admin logins per IP and window
const KEYS_ADMIN_FAIL_GLOBAL = 30;   // failed admin logins from all IPs per window
const KEYS_ADMIN_FAIL_WINDOW = 900;  // seconds
const KEYS_INACTIVITY_DAYS   = 90;   // auto-suspend after N days without upload
const KEYS_ADMIN_PASSWORD    = '';   // set via env LADDER_ADMIN_PASSWORD
const KEYS_NOTIFY_EMAIL      = '';   // set via env LADDER_NOTIFY_EMAIL (new registrations)

// ── Load / save ───────────────────────────────────────────────────────────────

function keys_load(): array
{
    keys_prepare_storage();
    if (!is_file(KEYS_FILE)) {
        return [];
    }
    $raw = file_get_contents(KEYS_FILE);
    if ($raw === false) {
        return [];
    }
    $data = json_decode($raw, true);
    return is_array($data) ? $data : [];
}

/**
 * Creates data/private/ with a deny-all .htaccess and moves a key file from
 * the old, web-reachable location data/server_keys.json into it.
 */
function keys_prepare_storage(): void
{
    static $prepared = false;
    if ($prepared) {
        return;
    }
    if (!is_dir(KEYS_DIR) && !mkdir(KEYS_DIR, 0770, true) && !is_dir(KEYS_DIR)) {
        throw new RuntimeException('Unable to create key storage.');
    }
    $htaccess = KEYS_DIR . '/.htaccess';
    if (!is_file($htaccess)) {
        @file_put_contents($htaccess,
            "<IfModule mod_authz_core.c>\n    Require all denied\n</IfModule>\n"
            . "<IfModule !mod_authz_core.c>\n    Order deny,allow\n    Deny from all\n</IfModule>\n");
    }
    $prepared = true;

    if (!is_file(KEYS_FILE) && is_file(KEYS_LEGACY_FILE)) {
        $lock = keys_lock();
        try {
            if (!is_file(KEYS_FILE) && is_file(KEYS_LEGACY_FILE) && !rename(KEYS_LEGACY_FILE, KEYS_FILE)) {
                throw new RuntimeException('Unable to move the key file into data/private/.');
            }
        } finally {
            keys_unlock($lock);
        }
    }
}

/** @return resource */
function keys_lock()
{
    $handle = fopen(KEYS_LOCK_FILE, 'c');
    if ($handle === false || !flock($handle, LOCK_EX)) {
        throw new RuntimeException('Unable to lock key storage.');
    }
    return $handle;
}

/** @param resource $handle */
function keys_unlock($handle): void
{
    flock($handle, LOCK_UN);
    fclose($handle);
}

/** Writes the key list to a temp file and renames it over the key file. */
function keys_write_atomic(array $keys): void
{
    $json = json_encode(array_values($keys), JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    if ($json === false) {
        throw new RuntimeException('Unable to encode key storage.');
    }
    $tmp = KEYS_FILE . '.' . bin2hex(random_bytes(6)) . '.tmp';
    if (file_put_contents($tmp, $json . "\n") === false || !rename($tmp, KEYS_FILE)) {
        @unlink($tmp);
        throw new RuntimeException('Unable to write key storage.');
    }
}

/**
 * Read-modify-write under the exclusive lock. The mutator gets the key list
 * by reference; the file is only rewritten when the list changed.
 * Returns whatever the mutator returns.
 */
function keys_mutate(callable $mutator)
{
    keys_prepare_storage();
    $lock = keys_lock();
    try {
        $keys = keys_load();
        $before = $keys;
        $result = $mutator($keys);
        if ($keys !== $before) {
            keys_write_atomic($keys);
        }
        return $result;
    } finally {
        keys_unlock($lock);
    }
}

/** Short, non-secret id of a key (stored with matches to know who reported them). */
function keys_key_id(string $key): string
{
    return substr(hash('sha256', $key), 0, 16);
}

function keys_save(array $keys): void
{
    keys_mutate(static function (array &$current) use ($keys): void {
        $current = array_values($keys);
    });
}

// ── Lookup ────────────────────────────────────────────────────────────────────

function keys_find_by_key(string $key): ?array
{
    foreach (keys_load() as $record) {
        if (isset($record['key']) && hash_equals($record['key'], $key)) {
            return $record;
        }
    }
    return null;
}

function keys_find_index_by_key(string $key): int
{
    foreach (keys_load() as $i => $record) {
        if (isset($record['key']) && hash_equals($record['key'], $key)) {
            return $i;
        }
    }
    return -1;
}

// ── Validation ────────────────────────────────────────────────────────────────

/**
 * Strip Quake 3 color codes (^0-^9, ^a-^z) from a string.
 * e.g. "^1Q3Rally ^7EU #1" -> "Q3Rally EU #1"
 */
function keys_strip_color_codes(string $name): string
{
    return preg_replace('/\^[0-9a-zA-Z]/', '', $name) ?? $name;
}

/**
 * Normalize a server name for comparison:
 * strip color codes, trim whitespace, lowercase.
 */
function keys_normalize_server_name(string $name): string
{
    return strtolower(trim(keys_strip_color_codes($name)));
}

/**
 * Authenticate an incoming API request.
 * Returns the matching key record or exits with 401/403.
 */
function keys_require_auth(string $incomingServerName, bool $countMatch = true, bool $countUsage = true): array
{
    $header   = $_SERVER['HTTP_AUTHORIZATION'] ?? '';
    $provided = strncasecmp($header, 'Bearer ', 7) === 0 ? trim(substr($header, 7)) : '';

    if ($provided === '') {
        http_response_code(401);
        header('WWW-Authenticate: Bearer realm="Q3Rally Ladder"');
        header('Content-Type: application/json');
        echo json_encode(['error' => 'Authorization header missing.']);
        exit;
    }

    $record = keys_find_by_key($provided);

    if ($record === null) {
        http_response_code(401);
        header('Content-Type: application/json');
        echo json_encode(['error' => 'Unknown API key.']);
        exit;
    }

    if (($record['status'] ?? '') === 'pending') {
        http_response_code(403);
        header('Content-Type: application/json');
        echo json_encode(['error' => 'API key pending approval. Please wait for admin confirmation.']);
        exit;
    }

    if (($record['status'] ?? '') !== 'active') {
        http_response_code(403);
        header('Content-Type: application/json');
        echo json_encode(['error' => 'API key is revoked or inactive.']);
        exit;
    }

    // Server name must match registered name (color codes stripped, case-insensitive)
    $registeredName = keys_normalize_server_name($record['serverName'] ?? '');
    $providedName   = keys_normalize_server_name($incomingServerName);
    if ($registeredName !== '' && $providedName !== '' && $registeredName !== $providedName) {
        http_response_code(403);
        header('Content-Type: application/json');
        echo json_encode(['error' => 'Server name does not match registered key.']);
        exit;
    }

    // Update last-used metadata (ghost uploads do not count as matches,
    // other requests such as DELETE count nothing)
    keys_touch($provided, $countMatch, $countUsage);

    return $record;
}

/**
 * Update lastUsedAt, lastUsedIp and matchCount (or ghostCount) for a key.
 */
function keys_touch(string $key, bool $countMatch = true, bool $countUsage = true): void
{
    keys_mutate(static function (array &$keys) use ($key, $countMatch, $countUsage): void {
        foreach ($keys as $i => $record) {
            if (!isset($record['key']) || !hash_equals((string)$record['key'], $key)) {
                continue;
            }
            $keys[$i]['lastUsedAt'] = gmdate('c');
            $keys[$i]['lastUsedIp'] = $_SERVER['REMOTE_ADDR'] ?? null;
            if ($countUsage) {
                if ($countMatch) {
                    $keys[$i]['matchCount'] = (int)($keys[$i]['matchCount'] ?? 0) + 1;
                } else {
                    $keys[$i]['ghostCount'] = (int)($keys[$i]['ghostCount'] ?? 0) + 1;
                }
            }
            return;
        }
    });
}

// ── Auto-suspend inactive keys ────────────────────────────────────────────────

function keys_suspend_inactive(): void
{
    $cutoff = time() - (KEYS_INACTIVITY_DAYS * 86400);

    keys_mutate(static function (array &$keys) use ($cutoff): void {
        foreach ($keys as &$record) {
            if (($record['status'] ?? '') !== 'active') {
                continue;
            }
            $lastUsed = $record['lastUsedAt'] ?? null;
            // If never used and approved more than N days ago → suspend
            $ref = $lastUsed ?? ($record['approvedAt'] ?? null);
            if ($ref === null) {
                continue;
            }
            $ts = strtotime((string)$ref);
            if ($ts !== false && $ts < $cutoff) {
                $record['status']        = 'suspended';
                $record['suspendedAt']   = gmdate('c');
                $record['suspendReason'] = 'auto: inactivity > ' . KEYS_INACTIVITY_DAYS . ' days';
            }
        }
        unset($record);
    });
}

// ── Registration ──────────────────────────────────────────────────────────────

function keys_register(string $serverName, string $ownerName, string $ownerEmail): string
{
    $key = bin2hex(random_bytes(32));

    $record = [
        'key'         => $key,
        'serverName'  => trim($serverName),
        'ownerName'   => trim($ownerName),
        'ownerEmail'  => trim($ownerEmail),
        'status'      => 'pending',
        'createdAt'   => gmdate('c'),
        'approvedAt'  => null,
        'lastUsedAt'  => null,
        'lastUsedIp'  => null,
        'matchCount'  => 0,
    ];

    keys_append($record);

    // Notify admin
    $notifyEmail = getenv('LADDER_NOTIFY_EMAIL') ?: KEYS_NOTIFY_EMAIL;
    if ($notifyEmail !== '') {
        $subject = '[Q3Rally Ladder] New server registration: ' . $serverName;
        $body    = "A new server has registered for the Q3Rally Ladder:\n\n"
                 . "Server:  {$serverName}\n"
                 . "Owner:   {$ownerName}\n"
                 . "E-Mail:  {$ownerEmail}\n\n"
                 . "Approve or revoke at: https://ladder.q3rally.com/admin.php\n";
        mail($notifyEmail, $subject, $body, "From: noreply@q3rally.com");
    }

    return $key;
}

// ── Admin helpers ─────────────────────────────────────────────────────────────

function keys_require_admin(): void
{
    $configured = getenv('LADDER_ADMIN_PASSWORD') ?: KEYS_ADMIN_PASSWORD;
    if ($configured === '') {
        // No admin password set → deny all admin access
        http_response_code(503);
        echo 'Admin interface not configured.';
        exit;
    }

    keys_admin_session_start();
    header('X-Frame-Options: DENY');
    header("Content-Security-Policy: frame-ancestors 'none'");
    header('Cache-Control: no-store');
    header('Referrer-Policy: no-referrer');

    if (!empty($_SESSION['ladder_admin_authed'])) {
        return;
    }

    if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['password'])) {
        $password = is_string($_POST['password']) ? $_POST['password'] : '';
        if (keys_ip_counter('admin', KEYS_ADMIN_FAIL_WINDOW) >= KEYS_ADMIN_FAIL_LIMIT
            || keys_ip_counter('admin', KEYS_ADMIN_FAIL_WINDOW, '', false) >= KEYS_ADMIN_FAIL_GLOBAL) {
            $_SESSION['ladder_admin_error'] = 'Too many failed logins. Please try again in 15 minutes.';
        } elseif ($password !== '' && hash_equals($configured, $password)) {
            // New session id after the login (no session fixation).
            session_regenerate_id(true);
            $_SESSION['ladder_admin_authed'] = true;
            keys_ip_counter('admin', KEYS_ADMIN_FAIL_WINDOW, 'clear');
            return;
        } else {
            keys_ip_counter('admin', KEYS_ADMIN_FAIL_WINDOW, 'add');
            keys_ip_counter('admin', KEYS_ADMIN_FAIL_WINDOW, 'add', false);
            usleep(500000);
            $_SESSION['ladder_admin_error'] = 'Wrong password.';
        }
    }

    // Show login form
    http_response_code(200);
    header('Content-Type: text/html; charset=UTF-8');
    $error = $_SESSION['ladder_admin_error'] ?? '';
    unset($_SESSION['ladder_admin_error']);
    echo <<<HTML
    <!doctype html><html lang="en"><head><meta charset="utf-8">
    <title>Q3Rally Ladder Admin</title>
    <style>body{background:#05050c;color:#F5F6FF;font-family:sans-serif;display:flex;align-items:center;justify-content:center;min-height:100vh;margin:0}
    form{background:rgba(18,21,33,.92);border:1px solid rgba(255,255,255,.12);border-radius:16px;padding:32px;display:flex;flex-direction:column;gap:16px;min-width:320px}
    input{background:rgba(28,31,46,.97);border:1px solid rgba(255,255,255,.2);color:#F5F6FF;padding:10px 14px;border-radius:8px;font-size:1rem}
    button{background:#5D8BFF;color:#fff;border:none;border-radius:8px;padding:10px 20px;font-size:1rem;cursor:pointer}
    .err{color:#ff6b6b;font-size:.9rem}</style></head><body>
    <form method="post"><h2 style="margin:0">Ladder Admin</h2>
    HTML;
    if ($error) {
        echo '<p class="err">' . htmlspecialchars($error) . '</p>';
    }
    echo <<<HTML
    <input type="password" name="password" placeholder="Admin password" autocomplete="current-password" autofocus>
    <button type="submit">Login</button></form></body></html>
    HTML;
    exit;
}

// ── Source classification ─────────────────────────────────────────────────────

/**
 * Returns true when the key was registered by the in-game offline wizard.
 * Convention: the wizard always appends "_OFFLINE" (case-insensitive) to the
 * player name when building the server name, e.g. "PlayerName_OFFLINE".
 */
function keys_is_offline(array $record): bool
{
    if (strtolower((string)($record['type'] ?? '')) === 'offline') {
        return true;
    }
    $name = keys_strip_color_codes((string)($record['serverName'] ?? ''));
    return (bool) preg_match('/_OFFLINE$/i', trim($name));
}

/**
 * Offline keys belong to one player: the profile that ran the in-game
 * registration (the key is stored with that profile). The first upload binds
 * the key to the player's id; afterwards the key only reports results for that
 * player, and no other offline key can take the same id. $candidate is the
 * player id of this upload ('' = no single player to bind). Returns the bound
 * player id, '' when the key is (still) unbound or the id belongs to another key.
 */
function keys_offline_player(array $keyRecord, string $candidate): string
{
    $key = (string)($keyRecord['key'] ?? '');
    $candidate = strtolower(trim($candidate));

    return (string)keys_mutate(static function (array &$keys) use ($key, $candidate): string {
        $index = -1;
        foreach ($keys as $i => $record) {
            if ($key !== '' && hash_equals((string)($record['key'] ?? ''), $key)) {
                $index = $i;
                break;
            }
        }
        if ($index < 0) {
            return '';
        }
        $bound = strtolower((string)($keys[$index]['playerId'] ?? ''));
        if ($bound !== '' || $candidate === '') {
            return $bound;
        }
        foreach ($keys as $i => $record) {
            if ($i !== $index && ($record['status'] ?? '') !== 'revoked'
                && strtolower((string)($record['playerId'] ?? '')) === $candidate) {
                return '';
            }
        }
        $keys[$index]['playerId'] = $candidate;
        $keys[$index]['playerBoundAt'] = gmdate('c');
        return $candidate;
    });
}

/** Admin: release the player binding of a key (new install, wrong binding). */
function keys_unbind_player(string $key): bool
{
    return (bool)keys_mutate(static function (array &$keys) use ($key): bool {
        foreach ($keys as &$record) {
            if (isset($record['key']) && hash_equals((string)$record['key'], $key) && isset($record['playerId'])) {
                unset($record['playerId'], $record['playerBoundAt']);
                return true;
            }
        }
        return false;
    });
}

/** Adds one key record. */
function keys_append(array $record): void
{
    keys_mutate(static function (array &$keys) use ($record): void {
        $keys[] = $record;
    });
}

/**
 * Registration throttle per IP (register.php and POST /api/v1/register).
 * Returns false when the IP registered too often in the current window.
 */
function keys_register_rate_ok(): bool
{
    if (keys_ip_counter('register', KEYS_REGISTER_WINDOW) >= KEYS_REGISTER_LIMIT) {
        return false;
    }
    keys_ip_counter('register', KEYS_REGISTER_WINDOW, 'add');
    return true;
}

/**
 * Timestamps within $window stored in data/private/rl_<bucket>_<ip>.json
 * (or rl_<bucket>_all.json with $perIp = false). $mode: '' only counts,
 * 'add' adds now, 'clear' empties the list. Returns the count before the change.
 */
function keys_ip_counter(string $bucket, int $window, string $mode = '', bool $perIp = true, int $limit = 0): int
{
    keys_prepare_storage();
    $ip = $perIp ? preg_replace('/[^a-fA-F0-9:.]/', '_', (string)($_SERVER['REMOTE_ADDR'] ?? 'unknown')) : 'all';
    $file = KEYS_DIR . '/rl_' . $bucket . '_' . $ip . '.json';
    $now = time();

    $handle = fopen($file, 'c+');
    if ($handle === false || !flock($handle, LOCK_EX)) {
        return 0;
    }
    try {
        $decoded = json_decode((string)stream_get_contents($handle), true);
        $hits = array_values(array_filter(is_array($decoded) ? $decoded : [],
            static fn($t) => is_int($t) && $t > $now - $window));
        $count = count($hits);
        if ($mode === 'add' && ($limit <= 0 || $count < $limit)) {
            $hits[] = $now;
        } elseif ($mode === 'clear') {
            $hits = [];
        }
        ftruncate($handle, 0);
        rewind($handle);
        fwrite($handle, (string)json_encode($hits));
        return $count;
    } finally {
        flock($handle, LOCK_UN);
        fclose($handle);
    }
}

/** Full key for the short key id used in admin forms, NULL if unknown. */
function keys_key_by_id(string $keyId): ?string
{
    if ($keyId === '') {
        return null;
    }
    foreach (keys_load() as $record) {
        $key = (string)($record['key'] ?? '');
        if ($key !== '' && hash_equals(keys_key_id($key), $keyId)) {
            return $key;
        }
    }
    return null;
}

/** Admin session: own cookie name, HttpOnly, SameSite=Strict, Secure on HTTPS. */
function keys_admin_session_start(): void
{
    if (session_status() === PHP_SESSION_ACTIVE) {
        return;
    }
    $https = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off')
        || strtolower((string)($_SERVER['HTTP_X_FORWARDED_PROTO'] ?? '')) === 'https';
    session_name('q3r_ladder_admin');
    session_set_cookie_params([
        'lifetime' => 0,
        'path'     => '/',
        'secure'   => $https,
        'httponly' => true,
        'samesite' => 'Strict',
    ]);
    session_start();
}

/** Per-session token for the admin forms. */
function keys_admin_csrf_token(): string
{
    if (empty($_SESSION['ladder_admin_csrf']) || !is_string($_SESSION['ladder_admin_csrf'])) {
        $_SESSION['ladder_admin_csrf'] = bin2hex(random_bytes(32));
    }
    return $_SESSION['ladder_admin_csrf'];
}

function keys_admin_csrf_ok(): bool
{
    $sent = $_POST['csrf'] ?? '';
    return is_string($sent) && $sent !== '' && hash_equals(keys_admin_csrf_token(), $sent);
}

// ── Delete ────────────────────────────────────────────────────────────────────

/**
 * Permanently remove a key record from the store.
 * Only permitted for revoked keys.
 */
function keys_delete(string $key): bool
{
    return (bool)keys_mutate(static function (array &$keys) use ($key): bool {
        $initial = count($keys);
        $keys = array_values(array_filter($keys, static function ($record) use ($key) {
            if (!isset($record['key'])) {
                return true;
            }
            if (!hash_equals((string)$record['key'], $key)) {
                return true;
            }
            // Only allow deletion of revoked keys
            return ($record['status'] ?? '') !== 'revoked';
        }));
        return count($keys) !== $initial;
    });
}

function keys_approve(string $key): bool
{
    return (bool)keys_mutate(static function (array &$keys) use ($key): bool {
        foreach ($keys as &$record) {
            if (isset($record['key']) && hash_equals((string)$record['key'], $key)) {
                $record['status']     = 'active';
                $record['approvedAt'] = gmdate('c');
                return true;
            }
        }
        return false;
    });
}

function keys_revoke(string $key): bool
{
    return (bool)keys_mutate(static function (array &$keys) use ($key): bool {
        foreach ($keys as &$record) {
            if (isset($record['key']) && hash_equals((string)$record['key'], $key)) {
                $record['status']    = 'revoked';
                $record['revokedAt'] = gmdate('c');
                return true;
            }
        }
        return false;
    });
}
