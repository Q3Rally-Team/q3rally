<?php
/**
 * Q3Rally Ladder – Lap ghosts
 * ghosts.php
 *
 * Best-lap ghosts recorded by game servers (dedicated servers and registered
 * offline clients) and uploaded via POST /api/v1/ghosts.
 *
 * Ranking: one ghost per player and bucket. A bucket is
 *   map / track variant (length, reversed) / vehicle / physics version + map checksum
 * Ghosts recorded with another physics version or a different map build land
 * in another bucket, so old records stay archived but are never compared with
 * current ones.
 *
 * Storage (all under data/ghosts/):
 *   <map>/tl<n>_rev<r>/<vehicle>/p<physics>_c<checksum>/index.json
 *   <map>/tl<n>_rev<r>/<vehicle>/p<physics>_c<checksum>/<playerId>.json
 *
 * Endpoints (wired up in index.php):
 *   POST /api/v1/ghosts            upload (Bearer key)
 *   GET  /api/v1/ghosts?map=...    ranking list without ghost data
 *                                  (&physics=&checksum=: that bucket, without
 *                                   them only the current bucket)
 *                                  (&perVehicle=K: best K per vehicle,
 *                                   &format=text: one ghost per line,
 *                                   ghostId<TAB>lapMs<TAB>vehicle<TAB>playerName;
 *                                   used by the game engine)
 *   GET  /api/v1/ghosts/catalog    maps, track variants, map builds and vehicles
 *                                  that have ghosts (for the ghost ranking page)
 *   GET  /api/v1/ghosts/<ghostId>  one ghost incl. data (?format=raw: .ghost text)
 */

declare(strict_types=1);

const GHOSTS_DIR                      = DATA_DIR . '/ghosts';
const LADDER_GHOST_MAX_DATA_BYTES     = 480000;
const LADDER_GHOST_MAX_FRAMES         = 8192;
const LADDER_GHOST_MIN_LAP_MS         = 5000;
const LADDER_GHOST_MAX_LAP_MS         = 3600000;
const LADDER_GHOST_MAX_START_MS       = 1000;    // first sample after lap start
const LADDER_GHOST_FINISH_TOLERANCE   = 100;     // last sample vs. lap time (ms)
const LADDER_GHOST_MAX_GAP_MS         = 5000;    // max time between two samples
const LADDER_GHOST_MAX_SEGMENT_SPEED  = 6000.0;  // units/s between two samples
const LADDER_GHOST_MAX_AVERAGE_SPEED  = 4000.0;  // units/s over the whole lap
const LADDER_GHOST_MAX_STILL_JUMP     = 64.0;    // units for samples with equal time
const LADDER_GHOST_MIN_COURSE_RATIO   = 0.6;     // driven path vs. course length
const LADDER_GHOST_MAX_COURSE_RATIO   = 4.0;
const LADDER_GHOST_MAX_PER_BUCKET     = 200;
const LADDER_GHOST_LIST_DEFAULT       = 10;
const LADDER_GHOST_LIST_MAX           = 100;
const LADDER_GHOST_PER_VEHICLE_MAX    = 20;

// ── Keys and paths ───────────────────────────────────────────────────────────

function ghost_key(string $raw): string
{
    $key = preg_replace('/[^a-z0-9_-]+/', '_', strtolower(trim($raw)));
    return trim((string)$key, '_');
}

function ghost_bucket_name(int $physicsVersion, int $mapChecksum): string
{
    return 'p' . $physicsVersion . '_c' . $mapChecksum;
}

function ghost_variant_name(int $trackLength, int $trackReversed): string
{
    return 'tl' . $trackLength . '_rev' . $trackReversed;
}

function ghost_bucket_dir(string $map, string $variant, string $vehicle, string $bucket): string
{
    return GHOSTS_DIR . '/' . $map . '/' . $variant . '/' . $vehicle . '/' . $bucket;
}

/** Public id: map.variant.vehicle.bucket.playerId – every part is [a-z0-9_-]. */
function ghost_make_id(string $map, string $variant, string $vehicle, string $bucket, string $playerId): string
{
    return implode('.', [$map, $variant, $vehicle, $bucket, strtolower($playerId)]);
}

function ghost_parse_id(string $ghostId): ?array
{
    $parts = explode('.', $ghostId);
    if (count($parts) !== 5) {
        return null;
    }
    foreach ($parts as $part) {
        if ($part === '' || !preg_match('/^[a-z0-9_-]+$/', $part)) {
            return null;
        }
    }
    if (!preg_match('/^tl[0-2]_rev[01]$/', $parts[1]) || !preg_match('/^p\d+_c-?\d+$/', $parts[3])) {
        return null;
    }
    if (!profile_is_valid_uuid($parts[4])) {
        return null;
    }
    return [
        'map'      => $parts[0],
        'variant'  => $parts[1],
        'vehicle'  => $parts[2],
        'bucket'   => $parts[3],
        'playerId' => $parts[4],
    ];
}

function ghost_clean_name(string $name): string
{
    $name = preg_replace('/\^[0-9a-zA-Z]/', '', $name) ?? $name;
    $name = preg_replace('/[^\x20-\x7e]/', '', $name) ?? '';
    $name = trim($name);
    if ($name === '') {
        $name = 'Player';
    }
    return substr($name, 0, 36);
}

/** Single text-list field: printable ASCII without tabs, quotes or backslashes. */
function ghost_text_field(string $value): string
{
    $value = preg_replace('/[^\x20-\x7e]|["\\\\;]/', '', $value) ?? '';
    return trim($value);
}

// ── Validation ───────────────────────────────────────────────────────────────

function ghost_fail(string $code, string $message, array $details = []): void
{
    throw new LadderApiException(422, $code, $message, $details);
}

function ghost_int_field(array $payload, string $field, int $min, int $max): int
{
    $value = $payload[$field] ?? null;
    if (is_string($value) && preg_match('/^-?\d+$/', $value)) {
        $value = (int)$value;
    }
    if (!is_int($value) || $value < $min || $value > $max) {
        ghost_fail('GHOST_FIELD_INVALID', $field . ' is missing or out of range.', ['field' => $field]);
    }
    return $value;
}

/**
 * Parse the .ghost text. Returns header values and frames as [t, x, y, z].
 */
function ghost_parse_data(string $data): array
{
    $header = [];
    $frames = [];
    $lines = preg_split('/\r\n|\r|\n/', $data) ?: [];

    foreach ($lines as $lineNumber => $line) {
        $line = trim($line);
        if ($line === '' || $line[0] === '#') {
            continue;
        }
        if (ctype_digit($line[0]) || $line[0] === '-') {
            $fields = preg_split('/\s+/', $line) ?: [];
            if (count($fields) !== 13) {
                ghost_fail('GHOST_DATA_INVALID', 'Ghost frame has the wrong number of fields.', ['line' => $lineNumber + 1]);
            }
            foreach ($fields as $field) {
                if (!is_numeric($field)) {
                    ghost_fail('GHOST_DATA_INVALID', 'Ghost frame contains a non-numeric value.', ['line' => $lineNumber + 1]);
                }
            }
            $frames[] = [(int)$fields[0], (float)$fields[1], (float)$fields[2], (float)$fields[3]];
            if (count($frames) > LADDER_GHOST_MAX_FRAMES) {
                ghost_fail('GHOST_TOO_LARGE', 'Ghost has too many frames.');
            }
            continue;
        }
        $parts = preg_split('/\s+/', $line, 2) ?: [];
        $header[strtolower($parts[0])] = isset($parts[1]) ? trim($parts[1]) : '';
    }

    return ['header' => $header, 'frames' => $frames];
}

/**
 * Plausibility checks on the recorded line. The server already discards laps
 * with resets/teleports; these checks catch broken or forged uploads.
 */
function ghost_check_plausibility(array $frames, int $lapMs, int $courseLengthUnits): array
{
    $count = count($frames);
    if ($count < 2) {
        ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost needs at least two frames.', ['reason' => 'too_few_frames']);
    }
    if ($frames[0][0] < 0 || $frames[0][0] > LADDER_GHOST_MAX_START_MS) {
        ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost does not start at the lap start.', ['reason' => 'start_offset']);
    }
    if (abs($frames[$count - 1][0] - $lapMs) > LADDER_GHOST_FINISH_TOLERANCE) {
        ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost does not end at the lap time.', ['reason' => 'finish_offset']);
    }

    $path = 0.0;
    for ($i = 1; $i < $count; $i++) {
        $dt = $frames[$i][0] - $frames[$i - 1][0];
        if ($dt < 0) {
            ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost timestamps are not in order.', ['reason' => 'time_order', 'frame' => $i]);
        }
        if ($dt > LADDER_GHOST_MAX_GAP_MS) {
            ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost has a gap between samples.', ['reason' => 'time_gap', 'frame' => $i]);
        }
        $dist = sqrt(
            ($frames[$i][1] - $frames[$i - 1][1]) ** 2 +
            ($frames[$i][2] - $frames[$i - 1][2]) ** 2 +
            ($frames[$i][3] - $frames[$i - 1][3]) ** 2
        );
        if ($dt === 0) {
            if ($dist > LADDER_GHOST_MAX_STILL_JUMP) {
                ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost jumps without time passing.', ['reason' => 'teleport', 'frame' => $i]);
            }
        } elseif ($dist * 1000.0 / $dt > LADDER_GHOST_MAX_SEGMENT_SPEED) {
            ghost_fail('GHOST_IMPLAUSIBLE', 'Ghost moves faster than any vehicle.', ['reason' => 'segment_speed', 'frame' => $i]);
        }
        $path += $dist;
    }

    if ($path * 1000.0 / $lapMs > LADDER_GHOST_MAX_AVERAGE_SPEED) {
        ghost_fail('GHOST_IMPLAUSIBLE', 'Average speed is not plausible.', ['reason' => 'average_speed']);
    }
    if ($courseLengthUnits > 0) {
        $ratio = $path / $courseLengthUnits;
        if ($ratio < LADDER_GHOST_MIN_COURSE_RATIO || $ratio > LADDER_GHOST_MAX_COURSE_RATIO) {
            ghost_fail('GHOST_IMPLAUSIBLE', 'Driven distance does not match the course length.', [
                'reason' => 'course_length',
                'ratio'  => round($ratio, 3),
            ]);
        }
    }

    return ['pathUnits' => (int)round($path)];
}

function ghost_validate_payload(array $payload): array
{
    $player = is_array($payload['player'] ?? null) ? $payload['player'] : [];
    $playerId = strtolower(trim((string)($player['id'] ?? '')));
    if (!profile_is_valid_uuid($playerId)) {
        ghost_fail('GHOST_FIELD_INVALID', 'player.id must be a profile UUID.', ['field' => 'player.id']);
    }

    $mapRaw = (string)($payload['map'] ?? '');
    $vehicleRaw = (string)($payload['vehicle'] ?? '');
    $map = ghost_key($mapRaw);
    $vehicle = ghost_key($vehicleRaw);
    if ($map === '' || strlen($map) > 64) {
        ghost_fail('GHOST_FIELD_INVALID', 'map is missing or invalid.', ['field' => 'map']);
    }
    if ($vehicle === '' || strlen($vehicle) > 64) {
        ghost_fail('GHOST_FIELD_INVALID', 'vehicle is missing or invalid.', ['field' => 'vehicle']);
    }

    $trackLength = ghost_int_field($payload, 'trackLength', 0, 2);
    $trackReversed = ghost_int_field($payload, 'trackReversed', 0, 1);
    $lapMs = ghost_int_field($payload, 'lapMs', LADDER_GHOST_MIN_LAP_MS, LADDER_GHOST_MAX_LAP_MS);
    $frameCount = ghost_int_field($payload, 'frames', 2, LADDER_GHOST_MAX_FRAMES);
    $physicsVersion = ghost_int_field($payload, 'physicsVersion', 1, 1000000);
    $mapChecksum = ghost_int_field($payload, 'mapChecksum', PHP_INT_MIN, PHP_INT_MAX);
    $courseLengthUnits = ghost_int_field($payload, 'courseLengthUnits', 0, 100000000);
    $gametype = ghost_int_field($payload, 'gametype', 0, 64);

    $data = $payload['data'] ?? null;
    if (!is_string($data) || $data === '') {
        ghost_fail('GHOST_FIELD_INVALID', 'data is missing.', ['field' => 'data']);
    }
    if (strlen($data) > LADDER_GHOST_MAX_DATA_BYTES) {
        ghost_fail('GHOST_TOO_LARGE', 'Ghost data is too large.');
    }

    $parsed = ghost_parse_data($data);
    $header = $parsed['header'];
    $frames = $parsed['frames'];

    $checks = [
        'map'            => strcasecmp((string)($header['map'] ?? ''), $mapRaw) === 0,
        'vehicle'        => strcasecmp((string)($header['vehicle'] ?? ''), $vehicleRaw) === 0,
        'track_length'   => (string)($header['track_length'] ?? '') === (string)$trackLength,
        'track_reversed' => (string)($header['track_reversed'] ?? '') === (string)$trackReversed,
        'best_time_ms'   => (string)($header['best_time_ms'] ?? '') === (string)$lapMs,
        'frames'         => count($frames) === $frameCount,
    ];
    if (isset($header['physics_version'])) {
        $checks['physics_version'] = $header['physics_version'] === (string)$physicsVersion;
    }
    if (isset($header['map_checksum'])) {
        $checks['map_checksum'] = $header['map_checksum'] === (string)$mapChecksum;
    }
    foreach ($checks as $field => $ok) {
        if (!$ok) {
            ghost_fail('GHOST_HEADER_MISMATCH', 'Ghost header does not match the upload metadata.', ['field' => $field]);
        }
    }

    $plausibility = ghost_check_plausibility($frames, $lapMs, $courseLengthUnits);

    $dedicated = $payload['server']['dedicated'] ?? null;
    return [
        'playerId'          => $playerId,
        'playerName'        => ghost_clean_name((string)($player['name'] ?? '')),
        'map'               => $map,
        'vehicle'           => $vehicle,
        'trackLength'       => $trackLength,
        'trackReversed'     => $trackReversed,
        'lapMs'             => $lapMs,
        'frames'            => $frameCount,
        'physicsVersion'    => $physicsVersion,
        'mapChecksum'       => $mapChecksum,
        'courseLengthUnits' => $courseLengthUnits,
        'pathUnits'         => $plausibility['pathUnits'],
        'sprintTrack'       => !empty($payload['sprintTrack']),
        'gametype'          => $gametype,
        'source'            => ($dedicated === true || $dedicated === 1 || $dedicated === '1') ? 'online' : 'offline',
        'serverName'        => keys_strip_color_codes((string)($payload['server']['name'] ?? '')),
        'data'              => $data,
    ];
}

// ── Bucket index ─────────────────────────────────────────────────────────────

function ghost_index_read(string $dir): array
{
    $path = $dir . '/index.json';
    if (!is_file($path)) {
        return [];
    }
    $raw = file_get_contents($path);
    $decoded = $raw === false ? null : json_decode($raw, true);
    return is_array($decoded) ? $decoded : [];
}

function ghost_sort_entries(array $entries): array
{
    usort($entries, static function (array $a, array $b): int {
        return [(int)$a['lapMs'], (string)$a['receivedAt']] <=> [(int)$b['lapMs'], (string)$b['receivedAt']];
    });
    return array_values($entries);
}

function ghost_public_entry(array $entry, int $rank): array
{
    unset($entry['data']);
    $entry['rank'] = $rank;
    return $entry;
}

// ── Handlers ─────────────────────────────────────────────────────────────────

function handle_ghost_post(): void
{
    $payload = json_decode(ladder_read_body(), true);
    if (!is_array($payload)) {
        throw new LadderApiException(400, 'INVALID_JSON', 'Invalid JSON payload.');
    }

    $serverName = is_string($payload['server']['name'] ?? null) ? $payload['server']['name'] : '';
    $keyRecord = keys_require_auth($serverName, false);

    $record = ghost_validate_payload($payload);
    if (keys_is_offline($keyRecord)) {
        // Offline keys run their own server: never an online ghost, and only
        // ghosts of the player the key belongs to (keys_offline_player).
        $record['source'] = 'offline';
        if (keys_offline_player($keyRecord, $record['playerId']) !== $record['playerId']) {
            throw new LadderApiException(403, 'GHOST_PLAYER_MISMATCH', 'This offline key belongs to another player.');
        }
    }
    $variant = ghost_variant_name($record['trackLength'], $record['trackReversed']);
    $bucket = ghost_bucket_name($record['physicsVersion'], $record['mapChecksum']);
    $dir = ghost_bucket_dir($record['map'], $variant, $record['vehicle'], $bucket);
    $ghostId = ghost_make_id($record['map'], $variant, $record['vehicle'], $bucket, $record['playerId']);

    if (!is_dir($dir) && !mkdir($dir, 0775, true) && !is_dir($dir)) {
        throw new LadderApiException(500, 'PERSIST_FAILED', 'Unable to create ghost directory.');
    }

    // Serialise concurrent uploads into the same bucket.
    $lock = fopen($dir . '/.lock', 'c');
    if ($lock === false || !flock($lock, LOCK_EX)) {
        throw new LadderApiException(500, 'PERSIST_FAILED', 'Unable to lock ghost bucket.');
    }

    try {
        $entries = ghost_index_read($dir);
        $previous = null;
        foreach ($entries as $i => $entry) {
            if (($entry['playerId'] ?? '') === $record['playerId']) {
                $previous = $entry;
                unset($entries[$i]);
                break;
            }
        }

        if ($previous !== null && (int)$previous['lapMs'] <= $record['lapMs']) {
            send_json([
                'ghostId'   => $ghostId,
                'stored'    => false,
                'reason'    => 'NOT_FASTER',
                'bestLapMs' => (int)$previous['lapMs'],
            ], 200);
        }

        // Full bucket: a new player's ghost must beat the slowest one,
        // otherwise it would be stored and dropped again right away.
        if ($previous === null && count($entries) >= LADDER_GHOST_MAX_PER_BUCKET) {
            $slowest = ghost_sort_entries($entries)[count($entries) - 1];
            if ((int)$slowest['lapMs'] <= $record['lapMs']) {
                send_json([
                    'ghostId'      => $ghostId,
                    'stored'       => false,
                    'reason'       => 'BUCKET_FULL',
                    'slowestLapMs' => (int)$slowest['lapMs'],
                ], 200);
            }
        }

        $record['ghostId'] = $ghostId;
        $record['receivedAt'] = gmdate('c');

        $json = json_encode($record, JSON_UNESCAPED_SLASHES);
        if ($json === false || !ladder_write_atomic($dir . '/' . $record['playerId'] . '.json', $json . "\n")) {
            throw new LadderApiException(500, 'PERSIST_FAILED', 'Unable to persist ghost.');
        }

        $entry = $record;
        unset($entry['data']);
        $entries[] = $entry;
        $entries = ghost_sort_entries($entries);

        // Keep the bucket bounded: drop the slowest ghosts beyond the limit
        // (never the new one, see the BUCKET_FULL check above).
        while (count($entries) > LADDER_GHOST_MAX_PER_BUCKET) {
            $dropped = array_pop($entries);
            if ($dropped['playerId'] === $record['playerId']) {
                $entries[] = $dropped;
                break;
            }
            @unlink($dir . '/' . $dropped['playerId'] . '.json');
        }

        $rank = null;
        foreach ($entries as $i => $candidate) {
            if ($candidate['playerId'] === $record['playerId']) {
                $rank = $i + 1;
                break;
            }
        }

        if (!ladder_write_atomic($dir . '/index.json', json_encode($entries, JSON_UNESCAPED_SLASHES) . "\n")) {
            throw new LadderApiException(500, 'PERSIST_FAILED', 'Unable to update ghost index.');
        }
    } finally {
        flock($lock, LOCK_UN);
        fclose($lock);
    }

    ladder_pipeline_log('php-ghost-stored', [
        'ghostId' => $ghostId,
        'lapMs'   => $record['lapMs'],
        'rank'    => $rank,
        'source'  => $record['source'],
    ]);

    send_json([
        'ghostId'        => $ghostId,
        'stored'         => true,
        'rank'           => $rank,
        'previousLapMs'  => $previous !== null ? (int)$previous['lapMs'] : null,
    ], 201);
}

function handle_ghost_list(): void
{
    $map = ghost_key((string)($_GET['map'] ?? ''));
    if ($map === '') {
        send_error(400, 'map is required.', 'MAP_REQUIRED');
    }
    $trackLength = max(0, min(2, (int)($_GET['tl'] ?? 0)));
    $trackReversed = ((int)($_GET['rev'] ?? 0)) ? 1 : 0;
    $vehicle = isset($_GET['vehicle']) ? ghost_key((string)$_GET['vehicle']) : '';
    $limit = isset($_GET['limit']) ? (int)$_GET['limit'] : LADDER_GHOST_LIST_DEFAULT;
    $limit = max(1, min(LADDER_GHOST_LIST_MAX, $limit));

    $bucket = '';
    if (isset($_GET['physics']) && isset($_GET['checksum'])) {
        $bucket = ghost_bucket_name((int)$_GET['physics'], (int)$_GET['checksum']);
    }

    $pattern = GHOSTS_DIR . '/' . $map . '/' . ghost_variant_name($trackLength, $trackReversed)
        . '/' . ($vehicle !== '' ? $vehicle : '*') . '/' . ($bucket !== '' ? $bucket : 'p*_c*') . '/index.json';
    $indexFiles = glob($pattern) ?: [];

    // Without physics/checksum only the current bucket is listed: lap times
    // from another physics version or map build are not comparable.
    if ($bucket === '') {
        $bucket = ghost_current_bucket($indexFiles);
    }

    $entries = [];
    foreach ($indexFiles as $indexFile) {
        if (basename(dirname($indexFile)) !== $bucket) {
            continue;
        }
        foreach (ghost_index_read(dirname($indexFile)) as $entry) {
            if (is_array($entry)) {
                $entries[] = $entry;
            }
        }
    }
    $entries = ghost_sort_entries($entries);

    // Best K per vehicle, so one fast car class cannot push every other
    // vehicle out of the list.
    $perVehicle = isset($_GET['perVehicle']) ? (int)$_GET['perVehicle'] : 0;
    $perVehicle = max(0, min(LADDER_GHOST_PER_VEHICLE_MAX, $perVehicle));
    if ($perVehicle > 0) {
        $seen = [];
        $kept = [];
        foreach ($entries as $entry) {
            $vehicleKey = (string)($entry['vehicle'] ?? '');
            $seen[$vehicleKey] = ($seen[$vehicleKey] ?? 0) + 1;
            if ($seen[$vehicleKey] <= $perVehicle) {
                $kept[] = $entry;
            }
        }
        $entries = $kept;
    }
    $entries = array_slice($entries, 0, $limit);

    if (($_GET['format'] ?? '') === 'text') {
        http_response_code(200);
        header('Content-Type: text/plain; charset=us-ascii');
        header('Cache-Control: public, max-age=30');
        foreach ($entries as $entry) {
            echo ghost_text_field((string)($entry['ghostId'] ?? '')), "\t",
                (int)($entry['lapMs'] ?? 0), "\t",
                ghost_text_field((string)($entry['vehicle'] ?? '')), "\t",
                ghost_text_field(ghost_clean_name((string)($entry['playerName'] ?? ''))), "\n";
        }
        exit;
    }

    $result = [];
    foreach ($entries as $i => $entry) {
        $result[] = ghost_public_entry($entry, $i + 1);
    }

    header('Cache-Control: public, max-age=30');
    send_json(['ghosts' => $result, 'count' => count($result), 'bucket' => $bucket], 200);
}

/**
 * Current bucket among the given bucket index files, as on the ranking page:
 * highest physics version, then the map build whose first ghost is newest.
 * Returns '' when there is none.
 */
function ghost_current_bucket(array $indexFiles): string
{
    $buckets = [];
    foreach ($indexFiles as $indexFile) {
        $name = basename(dirname($indexFile));
        if (!preg_match('/^p(\d+)_c(-?\d+)$/', $name, $m)) {
            continue;
        }
        $first = $buckets[$name]['first'] ?? '';
        foreach (ghost_index_read(dirname($indexFile)) as $entry) {
            $received = is_array($entry) ? (string)($entry['receivedAt'] ?? '') : '';
            if ($received !== '' && ($first === '' || $received < $first)) {
                $first = $received;
            }
        }
        $buckets[$name] = ['physics' => (int)$m[1], 'first' => $first];
    }
    $best = '';
    foreach ($buckets as $name => $info) {
        if ($best === ''
            || [$info['physics'], $info['first']] > [$buckets[$best]['physics'], $buckets[$best]['first']]) {
            $best = (string)$name;
        }
    }
    return $best;
}

/**
 * Overview for the ranking page: every map with its track variants, the map
 * builds/physics versions (buckets, newest first) and the vehicles per bucket.
 */
function handle_ghost_catalog(): void
{
    $maps = [];
    foreach (glob(GHOSTS_DIR . '/*/tl*_rev*/*/p*_c*/index.json') ?: [] as $indexFile) {
        $bucketDir = dirname($indexFile);
        $bucket = basename($bucketDir);
        $vehicle = basename(dirname($bucketDir));
        $variant = basename(dirname($bucketDir, 2));
        $map = basename(dirname($bucketDir, 3));
        if (!preg_match('/^tl([0-2])_rev([01])$/', $variant, $vm) || !preg_match('/^p(\d+)_c(-?\d+)$/', $bucket, $bm)) {
            continue;
        }
        $entries = ghost_index_read($bucketDir);
        if (!$entries) {
            continue;
        }
        $last = '';
        $first = '';
        foreach ($entries as $entry) {
            $received = (string)($entry['receivedAt'] ?? '');
            if ($received > $last) {
                $last = $received;
            }
            if ($received !== '' && ($first === '' || $received < $first)) {
                $first = $received;
            }
        }

        $variantRef = &$maps[$map][$variant];
        if (!isset($variantRef)) {
            $variantRef = [
                'variant'       => $variant,
                'trackLength'   => (int)$vm[1],
                'trackReversed' => (int)$vm[2],
                'count'         => 0,
                'buckets'       => [],
            ];
        }
        $bucketRef = &$variantRef['buckets'][$bucket];
        if (!isset($bucketRef)) {
            $bucketRef = [
                'bucket'         => $bucket,
                'physicsVersion' => (int)$bm[1],
                'mapChecksum'    => (int)$bm[2],
                'count'          => 0,
                'firstReceivedAt' => '',
                'lastReceivedAt' => '',
                'vehicles'       => [],
            ];
        }
        $bucketRef['vehicles'][$vehicle] = count($entries);
        $bucketRef['count'] += count($entries);
        if ($last > $bucketRef['lastReceivedAt']) {
            $bucketRef['lastReceivedAt'] = $last;
        }
        if ($first !== '' && ($bucketRef['firstReceivedAt'] === '' || $first < $bucketRef['firstReceivedAt'])) {
            $bucketRef['firstReceivedAt'] = $first;
        }
        $variantRef['count'] += count($entries);
        unset($variantRef, $bucketRef);
    }

    ksort($maps, SORT_NATURAL);
    $result = [];
    foreach ($maps as $map => $variants) {
        ksort($variants, SORT_NATURAL);
        $variantList = [];
        foreach ($variants as $variant) {
            $buckets = array_values($variant['buckets']);
            // Current build first: highest physics version, then the build that
            // appeared last (its first ghost is the newest). Late uploads from
            // clients with an old map build do not make that build current.
            usort($buckets, static function (array $a, array $b): int {
                return [$b['physicsVersion'], $b['firstReceivedAt']] <=> [$a['physicsVersion'], $a['firstReceivedAt']];
            });
            foreach ($buckets as &$bucket) {
                ksort($bucket['vehicles'], SORT_NATURAL);
            }
            unset($bucket);
            $variant['buckets'] = $buckets;
            $variantList[] = $variant;
        }
        $result[] = ['map' => $map, 'variants' => $variantList];
    }

    header('Cache-Control: public, max-age=60');
    send_json(['maps' => $result], 200);
}

function handle_ghost_get(string $ghostId): void
{
    $parts = ghost_parse_id(strtolower($ghostId));
    if ($parts === null) {
        send_error(400, 'Invalid ghost id.', 'GHOST_ID_INVALID');
    }

    $path = ghost_bucket_dir($parts['map'], $parts['variant'], $parts['vehicle'], $parts['bucket'])
        . '/' . $parts['playerId'] . '.json';
    if (!is_readable($path)) {
        send_error(404, 'Ghost not found.', 'GHOST_NOT_FOUND');
    }
    $raw = file_get_contents($path);
    $record = $raw === false ? null : json_decode($raw, true);
    if (!is_array($record)) {
        throw new RuntimeException('Stored ghost is corrupted.');
    }

    if (($_GET['format'] ?? '') === 'raw') {
        http_response_code(200);
        header('Content-Type: text/plain; charset=us-ascii');
        header('X-Content-Type-Options: nosniff');
        header('Cache-Control: public, max-age=300');
        if (!empty($_GET['download'])) {
            // Websites on other origins cannot force a download with <a download>.
            $fileName = $parts['map'] . '_' . $parts['variant'] . '_' . $parts['vehicle'] . '_'
                . (int)($record['lapMs'] ?? 0) . '.ghost';
            header('Content-Disposition: attachment; filename="' . $fileName . '"');
        }
        echo (string)($record['data'] ?? '');
        exit;
    }

    header('Cache-Control: public, max-age=300');
    send_json($record, 200);
}
