<?php
/**
 * Q3Rally Ladder – Index Rebuild Script
 * rebuild_index.php
 *
 * Run ONCE from the command line to build match_index.json
 * from all existing individual match files:
 *
 *   php rebuild_index.php
 *
 * This only needs to be run once after uploading the new index.php.
 * After that, the index is maintained automatically on every upload.
 *
 * Progress is printed to stdout. Safe to re-run – rebuilds from scratch.
 */

declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(403);
    exit('Run from command line only.');
}

define('DATA_DIR',          __DIR__ . '/data');
define('INDEX_FILE',        __DIR__ . '/data/match_index.json');
define('LADDER_RATE_FILE_PREFIX', 'rl_');

$files = glob(DATA_DIR . '/*.json');
if (!is_array($files)) {
    echo "ERROR: Could not read data directory.\n";
    exit(1);
}

$skip = ['match_index.json', 'version.json', 'server_keys.json'];

$total    = 0;
$skipped  = 0;
$errors   = 0;
$entries  = [];
$start    = microtime(true);

echo "Q3Rally Ladder – Index Rebuild\n";
echo str_repeat('─', 50) . "\n";
echo "Data dir: " . DATA_DIR . "\n";
echo "Files found: " . count($files) . "\n\n";

foreach ($files as $file) {
    $basename = basename($file);

    if (in_array($basename, $skip, true)) {
        $skipped++;
        continue;
    }
    if (strncmp($basename, LADDER_RATE_FILE_PREFIX, strlen(LADDER_RATE_FILE_PREFIX)) === 0) {
        $skipped++;
        continue;
    }

    $raw = file_get_contents($file);
    if ($raw === false) {
        echo "  SKIP (unreadable): $basename\n";
        $errors++;
        continue;
    }

    $payload = json_decode($raw, true);
    if (!is_array($payload)) {
        echo "  SKIP (invalid JSON): $basename\n";
        $errors++;
        continue;
    }

    $dedicated = $payload['server']['dedicated'] ?? null;
    $source    = ($dedicated === true || $dedicated === 1 || $dedicated === '1')
        ? 'online' : ($payload['source'] ?? 'offline');

    $players = [];
    foreach ((array)($payload['players'] ?? []) as $p) {
        if (!is_array($p)) {
            continue;
        }
        $players[] = [
            'name'        => (string)($p['cleanName'] ?? $p['name'] ?? $p['displayName'] ?? ''),
            'score'       => (int)($p['score'] ?? $p['rawScore'] ?? 0),
            'bestLapMs'   => (int)($p['bestLapMs'] ?? 0),
            'totalRaceMs' => (int)($p['totalRaceMs'] ?? 0),
            'kills'       => (int)($p['kills'] ?? 0),
            'deaths'      => (int)($p['deaths'] ?? 0),
            'position'    => (int)($p['position'] ?? 0),
            'isBot'       => (bool)($p['isBot'] ?? false),
            'team'        => (int)($p['team'] ?? 0),
            'captures'    => (int)($p['captures'] ?? 0),
            'damageDealt' => (int)($p['damageDealt'] ?? 0),
        ];
    }

    $entries[] = [
        'matchId'     => (string)($payload['matchId'] ?? ''),
        'mode'        => (string)($payload['mode'] ?? ''),
        'gametype'    => (int)($payload['gametype'] ?? 0),
        'map'         => (string)($payload['map'] ?? ''),
        'startTime'   => (string)($payload['startTime'] ?? $payload['startTimeIso'] ?? ''),
        'source'      => $source,
        'serverName'  => (string)($payload['server']['name'] ?? ''),
        'playerCount' => (int)($payload['playerCount'] ?? count($players)),
        'players'     => $players,
    ];

    $total++;

    if ($total % 1000 === 0) {
        $elapsed = round(microtime(true) - $start, 1);
        echo "  Processed: $total ({$elapsed}s)...\n";
    }
}

echo "\nWriting index... ";
$json = json_encode($entries, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
if ($json === false || file_put_contents(INDEX_FILE, $json, LOCK_EX) === false) {
    echo "ERROR: Could not write index file.\n";
    exit(1);
}

$elapsed  = round(microtime(true) - $start, 2);
$sizeMb   = round(filesize(INDEX_FILE) / 1024 / 1024, 2);

echo "done.\n\n";
echo str_repeat('─', 50) . "\n";
echo "Matches indexed : $total\n";
echo "Skipped         : $skipped\n";
echo "Errors          : $errors\n";
echo "Index size      : {$sizeMb} MB\n";
echo "Time            : {$elapsed}s\n";
echo str_repeat('─', 50) . "\n";
echo "Index written to: " . INDEX_FILE . "\n\n";
echo "Done! The index will be maintained automatically from now on.\n";
