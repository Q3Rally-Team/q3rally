<?php
/**
 * Q3Rally Ladder – Profile Tracking Migration
 *
 * Normalisiert bestehende Profile auf die dedizierte Match-Dedupe-Struktur:
 * - _processedMatchIds (max. 200)
 * - _lastProcessedMatch { matchId, timestamp, checksum }
 *
 * Nutzung:
 *   php migrate_profiles.php
 */

declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(403);
    exit("Run from command line only.\n");
}

define('PROFILES_DIR', __DIR__ . '/data/profiles');

if (!is_dir(PROFILES_DIR)) {
    echo "Profiles directory does not exist: " . PROFILES_DIR . "\n";
    exit(0);
}

$files = glob(PROFILES_DIR . '/*.json');
if (!is_array($files)) {
    echo "Failed to scan profiles directory.\n";
    exit(1);
}

$updated = 0;
$skipped = 0;
$errors = 0;

foreach ($files as $file) {
    $raw = file_get_contents($file);
    if ($raw === false) {
        $errors++;
        echo "ERROR unreadable: " . basename($file) . "\n";
        continue;
    }

    $profile = json_decode($raw, true);
    if (!is_array($profile)) {
        $errors++;
        echo "ERROR invalid JSON: " . basename($file) . "\n";
        continue;
    }

    $ids = array_values(array_filter((array)($profile['_processedMatchIds'] ?? []), 'is_string'));
    $ids = array_values(array_filter(array_map(
        static function(string $value): string {
            $value = trim($value);
            return preg_replace('/[^A-Za-z0-9._-]/', '_', $value) ?? '';
        },
        $ids
    ), static fn(string $value): bool => $value !== ''));

    if (empty($ids) && isset($profile['_lastProcessedMatchId']) && is_string($profile['_lastProcessedMatchId'])) {
        $legacy = trim($profile['_lastProcessedMatchId']);
        $legacy = preg_replace('/[^A-Za-z0-9._-]/', '_', $legacy) ?? '';
        if ($legacy !== '') {
            $ids[] = $legacy;
        }
    }

    $ids = array_slice(array_values(array_unique($ids)), -200);

    $receivedAt = is_string($profile['lastSeen'] ?? null) && trim($profile['lastSeen']) !== ''
        ? (string)$profile['lastSeen']
        : gmdate('c');
    $lastId = end($ids);
    $lastId = is_string($lastId) ? $lastId : '';

    $newTracking = [
        '_processedMatchIds' => $ids,
        '_lastProcessedMatch' => [
            'matchId' => $lastId,
            'timestamp' => $receivedAt,
            'checksum' => $lastId !== '' ? hash('sha256', $lastId . '|' . ((string)($profile['playerId'] ?? ''))) : '',
        ],
    ];

    $before = json_encode([
        '_processedMatchIds' => $profile['_processedMatchIds'] ?? null,
        '_lastProcessedMatch' => $profile['_lastProcessedMatch'] ?? null,
    ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);

    $profile['_processedMatchIds'] = $newTracking['_processedMatchIds'];
    $profile['_lastProcessedMatch'] = $newTracking['_lastProcessedMatch'];
    unset($profile['_lastProcessedMatchId']);

    $after = json_encode([
        '_processedMatchIds' => $profile['_processedMatchIds'],
        '_lastProcessedMatch' => $profile['_lastProcessedMatch'],
    ], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);

    if ($before === $after) {
        $skipped++;
        continue;
    }

    $encoded = json_encode($profile, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
    if ($encoded === false || file_put_contents($file, $encoded, LOCK_EX) === false) {
        $errors++;
        echo "ERROR write failed: " . basename($file) . "\n";
        continue;
    }

    $updated++;
}

echo "Profiles scanned : " . count($files) . "\n";
echo "Updated         : $updated\n";
echo "Skipped         : $skipped\n";
echo "Errors          : $errors\n";

exit($errors > 0 ? 1 : 0);
