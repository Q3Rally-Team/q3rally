<?php
/**
 * Q3Rally Ladder – Server Key Registry
 * keys.php
 *
 * Manages per-server API keys. Stored as a single JSON file:
 *   data/server_keys.json
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

const KEYS_FILE              = __DIR__ . '/data/server_keys.json';
const KEYS_INACTIVITY_DAYS   = 90;   // auto-suspend after N days without upload
const KEYS_ADMIN_PASSWORD    = '';   // set via env LADDER_ADMIN_PASSWORD
const KEYS_NOTIFY_EMAIL      = '';   // set via env LADDER_NOTIFY_EMAIL (new registrations)

// ── Load / save ───────────────────────────────────────────────────────────────

function keys_load(): array
{
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

function keys_save(array $keys): void
{
    $dir = dirname(KEYS_FILE);
    if (!is_dir($dir)) {
        mkdir($dir, 0775, true);
    }
    file_put_contents(KEYS_FILE, json_encode($keys, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES) . "\n", LOCK_EX);
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
function keys_require_auth(string $incomingServerName): array
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

    // Update last-used metadata
    keys_touch($provided);

    return $record;
}

/**
 * Update lastUsedAt, lastUsedIp and matchCount for a key.
 */
function keys_touch(string $key): void
{
    $keys = keys_load();
    $idx  = -1;
    foreach ($keys as $i => $record) {
        if (isset($record['key']) && hash_equals($record['key'], $key)) {
            $idx = $i;
            break;
        }
    }
    if ($idx === -1) {
        return;
    }
    $keys[$idx]['lastUsedAt']  = gmdate('c');
    $keys[$idx]['lastUsedIp']  = $_SERVER['REMOTE_ADDR'] ?? null;
    $keys[$idx]['matchCount']  = (int)($keys[$idx]['matchCount'] ?? 0) + 1;
    keys_save($keys);
}

// ── Auto-suspend inactive keys ────────────────────────────────────────────────

function keys_suspend_inactive(): void
{
    $keys    = keys_load();
    $changed = false;
    $cutoff  = time() - (KEYS_INACTIVITY_DAYS * 86400);

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
        $ts = strtotime($ref);
        if ($ts !== false && $ts < $cutoff) {
            $record['status']      = 'suspended';
            $record['suspendedAt'] = gmdate('c');
            $record['suspendReason'] = 'auto: inactivity > ' . KEYS_INACTIVITY_DAYS . ' days';
            $changed = true;
        }
    }
    unset($record);

    if ($changed) {
        keys_save($keys);
    }
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

    $keys   = keys_load();
    $keys[] = $record;
    keys_save($keys);

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

    session_start();
    if (!empty($_SESSION['ladder_admin_authed'])) {
        return;
    }

    if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['password'])) {
        if (hash_equals($configured, $_POST['password'])) {
            $_SESSION['ladder_admin_authed'] = true;
            return;
        }
        $_SESSION['ladder_admin_error'] = 'Wrong password.';
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
    <input type="password" name="password" placeholder="Admin password" autofocus>
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
    $name = keys_strip_color_codes($record['serverName'] ?? '');
    return (bool) preg_match('/_OFFLINE$/i', trim($name));
}

// ── Delete ────────────────────────────────────────────────────────────────────

/**
 * Permanently remove a key record from the store.
 * Only permitted for revoked keys.
 */
function keys_delete(string $key): bool
{
    $keys    = keys_load();
    $initial = count($keys);

    $keys = array_values(array_filter($keys, static function ($record) use ($key) {
        if (!isset($record['key'])) {
            return true;
        }
        if (!hash_equals($record['key'], $key)) {
            return true;
        }
        // Only allow deletion of revoked keys
        return ($record['status'] ?? '') !== 'revoked';
    }));

    if (count($keys) === $initial) {
        return false; // nothing was removed
    }

    keys_save($keys);
    return true;
}

function keys_approve(string $key): bool
{
    $keys = keys_load();
    foreach ($keys as &$record) {
        if (isset($record['key']) && hash_equals($record['key'], $key)) {
            $record['status']     = 'active';
            $record['approvedAt'] = gmdate('c');
            keys_save($keys);
            return true;
        }
    }
    return false;
}

function keys_revoke(string $key): bool
{
    $keys = keys_load();
    foreach ($keys as &$record) {
        if (isset($record['key']) && hash_equals($record['key'], $key)) {
            $record['status']    = 'revoked';
            $record['revokedAt'] = gmdate('c');
            keys_save($keys);
            return true;
        }
    }
    return false;
}
