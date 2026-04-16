<?php
declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    fwrite(STDERR, "Run this script via CLI.\n");
    exit(1);
}

$playerId = isset($argv[1]) ? trim((string)$argv[1]) : '';
$limit = isset($argv[2]) ? max(1, (int)$argv[2]) : 10;

if ($playerId === '') {
    fwrite(STDERR, "Usage: php diagnose_player_pipeline.php <playerId> [limit]\n");
    exit(1);
}

$baseDir = __DIR__;
$dataDir = $baseDir . '/data';
$profilesDir = $dataDir . '/profiles';
$safePlayerId = preg_replace('/[^a-zA-Z0-9._-]/', '_', $playerId);
$profilePath = $profilesDir . '/' . $safePlayerId . '.json';
$profile = [];
if (is_file($profilePath)) {
    $profileRaw = file_get_contents($profilePath);
    $decoded = is_string($profileRaw) ? json_decode($profileRaw, true) : null;
    if (is_array($decoded)) {
        $profile = $decoded;
    }
}

$processed = [];
if (isset($profile['_processedMatchIds']) && is_array($profile['_processedMatchIds'])) {
    foreach ($profile['_processedMatchIds'] as $pid) {
        if (is_string($pid) && $pid !== '') {
            $processed[$pid] = true;
        }
    }
}

$matchRows = [];
foreach (glob($dataDir . '/*.json') ?: [] as $matchPath) {
    if (!is_file($matchPath)) {
        continue;
    }
    $raw = file_get_contents($matchPath);
    $match = is_string($raw) ? json_decode($raw, true) : null;
    if (!is_array($match) || !isset($match['players']) || !is_array($match['players'])) {
        continue;
    }

    $mode = (string)($match['mode'] ?? '');
    $matchId = (string)($match['matchId'] ?? '');
    $receivedAt = (string)($match['receivedAt'] ?? $match['endTime'] ?? $match['startTime'] ?? '');
    $winnerClientNum = isset($match['winnerClientNum']) ? (int)$match['winnerClientNum'] : -9999;

    foreach ($match['players'] as $p) {
        if (!is_array($p)) {
            continue;
        }
        if ((string)($p['playerId'] ?? '') !== $playerId) {
            continue;
        }

        $clientNum = (int)($p['clientNum'] ?? -1);
        $won = ($winnerClientNum >= 0 && $clientNum === $winnerClientNum) ? 1 : 0;
        $snap = (isset($p['profile']) && is_array($p['profile']) && !empty($p['profile']['valid'])) ? $p['profile'] : [];

        $matchRows[] = [
            'receivedAt' => $receivedAt,
            'matchId' => $matchId,
            'mode' => $mode,
            'position' => (int)($p['position'] ?? 0),
            'won' => $won,
            'processed' => isset($processed[$matchId]) ? 1 : 0,
            'snapRev' => (int)($snap['snapshotRevision'] ?? 0),
            'snapEpoch' => (int)($snap['snapshotEpoch'] ?? 0),
            'snapWins' => (int)($snap['wins'] ?? 0),
        ];
    }
}

usort($matchRows, static function (array $a, array $b): int {
    return strcmp((string)$b['receivedAt'], (string)$a['receivedAt']);
});
$matchRows = array_slice($matchRows, 0, $limit);

echo "[ladder-pipeline] diagnose playerId={$playerId} limit={$limit}\n";
echo "[ladder-pipeline] profile-summary wins=" . (int)($profile['wins'] ?? 0)
    . " gamesPlayed=" . (int)($profile['gamesPlayed'] ?? 0)
    . " lastIngestedMatchId=" . (string)($profile['lastIngestedMatchId'] ?? '')
    . " lastServerMatchSeq=" . (string)($profile['lastServerMatchSeq'] ?? '') . "\n";
echo "\n";
echo str_pad('receivedAt', 22)
    . str_pad('matchId', 28)
    . str_pad('mode', 18)
    . str_pad('pos', 6)
    . str_pad('won', 6)
    . str_pad('processed', 11)
    . str_pad('snapRev', 9)
    . str_pad('snapEpoch', 12)
    . "snapWins\n";

if (!$matchRows) {
    echo "(no matches found for player)\n";
    exit(0);
}

foreach ($matchRows as $row) {
    echo str_pad((string)$row['receivedAt'], 22)
        . str_pad((string)$row['matchId'], 28)
        . str_pad((string)$row['mode'], 18)
        . str_pad((string)$row['position'], 6)
        . str_pad((string)$row['won'], 6)
        . str_pad((string)$row['processed'], 11)
        . str_pad((string)$row['snapRev'], 9)
        . str_pad((string)$row['snapEpoch'], 12)
        . (string)$row['snapWins'] . "\n";
}
