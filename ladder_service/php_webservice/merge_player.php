<?php
/**
 * Q3Rally Ladder – merge two player ids of the same player
 *
 * A player whose profile id changed on the client (new UUID) has matches,
 * ghosts and a profile under the old and the new id. This tool moves
 * everything to the new id:
 *   - players[].playerId in every match file,
 *   - lap ghosts (per bucket the faster one stays),
 *   - the offline key binding,
 *   - the profile: both profiles are rebuilt from the matches of the new id,
 *     oldest first (the client snapshot in each match plus the server counters),
 *   - the match index.
 * Without --apply it only reports what it would change. Before applying it
 * copies everything it changes to data/private/merge-<time>/.
 *
 * Usage (command line only):
 *   php merge_player.php <oldPlayerId> <newPlayerId> [--apply]
 */

declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(403);
    exit("Run from command line only.\n");
}

define('LADDER_LIBRARY_ONLY', true);
require __DIR__ . '/index.php';

$args = array_values(array_filter(array_slice($argv, 1), static fn($a) => $a !== '--apply'));
$apply = in_array('--apply', $argv, true);
if ($apply) {
    // No uploads may change matches, profiles or the index while merging
    // (released when the script ends).
    ladder_lock_acquire();
}
if (count($args) !== 2) {
    fwrite(STDERR, "Usage: php merge_player.php <oldPlayerId> <newPlayerId> [--apply]\n");
    exit(1);
}
$old = strtolower(trim($args[0]));
$new = strtolower(trim($args[1]));
if (!profile_is_valid_uuid($old) || !profile_is_valid_uuid($new) || $old === $new) {
    fwrite(STDERR, "Both ids must be different player UUIDs.\n");
    exit(1);
}

$backupDir = KEYS_DIR . '/merge-' . gmdate('Ymd-His');
if ($apply) {
    if (!is_dir($backupDir)) {
        mkdir($backupDir, 0770, true);
    }
    // The profile rebuild logs every field (ladder_pipeline_log): into a file.
    ini_set('error_log', $backupDir . '/merge.log');
}
function merge_backup(string $path, string $backupDir, bool $apply): void
{
    if (!$apply || !is_file($path)) {
        return;
    }
    $rel = ltrim(substr($path, strlen(DATA_DIR)), '/');
    $target = $backupDir . '/' . $rel;
    if (!is_dir(dirname($target))) {
        mkdir(dirname($target), 0770, true);
    }
    if (!is_file($target)) {
        copy($path, $target);
    }
}
function merge_write_json(string $path, $data, int $flags = JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES): void
{
    $tmp = $path . '.' . bin2hex(random_bytes(4)) . '.tmp';
    file_put_contents($tmp, json_encode($data, $flags) . "\n");
    rename($tmp, $path);
}

// ── 1. matches ───────────────────────────────────────────────────────────────
$matches = [];       // payloads that contain the (new) id after the merge
$changedMatches = 0;
foreach (list_match_files() as $file) {
    $payload = json_decode((string)file_get_contents($file), true);
    if (!is_array($payload) || !isset($payload['players']) || !is_array($payload['players'])) {
        continue;
    }
    $changed = false;
    $contains = false;
    foreach ($payload['players'] as $i => $player) {
        if (!is_array($player)) {
            continue;
        }
        $id = strtolower((string)($player['playerId'] ?? ''));
        if ($id === $old) {
            $payload['players'][$i]['playerId'] = $new;
            $changed = true;
        }
        if ($id === $old || $id === $new) {
            $contains = true;
        }
    }
    if ($changed) {
        $changedMatches++;
        merge_backup($file, $backupDir, $apply);
        if ($apply) {
            merge_write_json($file, $payload);
        }
    }
    if ($contains) {
        $matches[] = $payload;
    }
}
printf("Matches: %d with the old id, %d of the player in total\n", $changedMatches, count($matches));

// ── 2. ghosts ────────────────────────────────────────────────────────────────
$ghostMoves = 0;
foreach (glob(GHOSTS_DIR . '/*/*/*/*/' . $old . '.json') ?: [] as $oldFile) {
    $dir = dirname($oldFile);
    $oldRecord = json_decode((string)file_get_contents($oldFile), true);
    $newFile = $dir . '/' . $new . '.json';
    $newRecord = is_file($newFile) ? json_decode((string)file_get_contents($newFile), true) : null;
    $keepOld = is_array($oldRecord) && (!is_array($newRecord) || (int)$oldRecord['lapMs'] < (int)$newRecord['lapMs']);
    $ghostMoves++;
    printf("Ghost %s: %s\n", substr($dir, strlen(GHOSTS_DIR) + 1), $keepOld ? 'old ghost moves to the new id' : 'new ghost stays');
    if (!$apply) {
        continue;
    }
    merge_backup($oldFile, $backupDir, true);
    merge_backup($newFile, $backupDir, true);
    merge_backup($dir . '/index.json', $backupDir, true);
    if ($keepOld) {
        $oldRecord['playerId'] = $new;
        $oldRecord['ghostId'] = preg_replace('/\.' . preg_quote($old, '/') . '$/', '.' . $new, (string)$oldRecord['ghostId']);
        merge_write_json($newFile, $oldRecord, JSON_UNESCAPED_SLASHES);
    }
    unlink($oldFile);
    $entries = [];
    foreach (ghost_index_read($dir) as $entry) {
        if (in_array(strtolower((string)($entry['playerId'] ?? '')), [$old, $new], true)) {
            continue;
        }
        $entries[] = $entry;
    }
    $kept = json_decode((string)file_get_contents($newFile), true);
    if (is_array($kept)) {
        unset($kept['data']);
        $entries[] = $kept;
    }
    merge_write_json($dir . '/index.json', ghost_sort_entries($entries), JSON_UNESCAPED_SLASHES);
}
printf("Ghosts: %d bucket(s) with the old id\n", $ghostMoves);

// ── 3. offline key binding ───────────────────────────────────────────────────
$bound = 0;
foreach (keys_load() as $record) {
    if (strtolower((string)($record['playerId'] ?? '')) === $old) {
        $bound++;
    }
}
printf("Keys bound to the old id: %d\n", $bound);
if ($apply && $bound > 0) {
    merge_backup(KEYS_FILE, $backupDir, true);
    keys_mutate(static function (array &$keys) use ($old, $new): void {
        $newTaken = false;
        foreach ($keys as $record) {
            if (strtolower((string)($record['playerId'] ?? '')) === $new) {
                $newTaken = true;
            }
        }
        foreach ($keys as &$record) {
            if (strtolower((string)($record['playerId'] ?? '')) === $old) {
                if ($newTaken) {
                    unset($record['playerId'], $record['playerBoundAt']);
                } else {
                    $record['playerId'] = $new;
                    $newTaken = true;
                }
            }
        }
        unset($record);
    });
}

// ── 4. profile: rebuild from the matches, oldest first ──────────────────────
usort($matches, static function (array $a, array $b): int {
    $ta = (string)($a['receivedAt'] ?? $a['startTime'] ?? '');
    $tb = (string)($b['receivedAt'] ?? $b['startTime'] ?? '');
    return [$ta, (int)($a['serverMatchSeq'] ?? 0)] <=> [$tb, (int)($b['serverMatchSeq'] ?? 0)];
});
$oldProfile = profile_load($old);
$newProfile = profile_load($new);
printf("Profiles: old %s (%d games), new %s (%d games)\n",
    $oldProfile ? 'yes' : 'no', (int)($oldProfile['gamesPlayed'] ?? 0),
    $newProfile ? 'yes' : 'no', (int)($newProfile['gamesPlayed'] ?? 0));
if ($apply) {
    merge_backup(profile_path($old), $backupDir, true);
    merge_backup(profile_path($new), $backupDir, true);
    @unlink(profile_path($old));
    @unlink(profile_path($new));
    foreach ($matches as $payload) {
        // Only this player is replayed; the others keep their profiles.
        foreach ($payload['players'] as $i => $player) {
            if (is_array($player) && strtolower((string)($player['playerId'] ?? '')) !== $new) {
                unset($payload['players'][$i]['playerId']);
            } elseif (is_array($player)) {
                $payload['players'][$i]['playerId'] = $new;
            }
        }
        profile_upsert_from_payload($payload);
    }
    $rebuilt = profile_load($new);
    printf("Rebuilt profile: %d games\n", (int)($rebuilt['gamesPlayed'] ?? 0));
}

// ── 5. index ─────────────────────────────────────────────────────────────────
if ($apply) {
    merge_backup(INDEX_FILE, $backupDir, true);
    index_rebuild();
    printf("Index rebuilt. Backup: %s\n", $backupDir);
} else {
    echo "Dry run. Run again with --apply to merge.\n";
}
