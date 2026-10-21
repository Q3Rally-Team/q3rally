<?php
/**
 * Q3Rally Ladder – Index Rebuild Script
 * rebuild_index.php
 *
 * Builds data/match_index.json again from all stored match files, with the
 * same fields the ladder writes on every upload (index_extract_entry in
 * index.php). Needed after an update that adds fields to the index (1.0.17:
 * the weather of a match); otherwise the index is maintained automatically.
 *
 * Run from the command line in this folder, as the web server user so the
 * file keeps the right owner:
 *
 *   sudo -u www-data php rebuild_index.php
 *
 * Safe to re-run: it rebuilds from scratch, under the ladder lock, so
 * uploads that arrive meanwhile wait and are not lost.
 */

declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(403);
    exit("Run from command line only.\n");
}

define('LADDER_LIBRARY_ONLY', true);
require __DIR__ . '/index.php';

$start = microtime(true);

echo "Q3Rally Ladder – Index Rebuild (" . LADDER_VERSION . ")\n";
echo str_repeat('─', 50) . "\n";
echo "Data dir: " . DATA_DIR . "\n";

$entries = index_rebuild();

$weather = [];
foreach ($entries as $entry) {
    $w = (string)($entry['weather'] ?? 'dry');
    $weather[$w] = ($weather[$w] ?? 0) + 1;
}
ksort($weather);

$elapsed = round(microtime(true) - $start, 2);
$sizeMb  = is_file(INDEX_FILE) ? round(filesize(INDEX_FILE) / 1024 / 1024, 2) : 0;

echo "Matches indexed : " . count($entries) . "\n";
foreach ($weather as $w => $n) {
    echo sprintf("  %-14s: %d\n", $w, $n);
}
echo "Index size      : {$sizeMb} MB\n";
echo "Time            : {$elapsed}s\n";
echo str_repeat('─', 50) . "\n";
echo "Index written to: " . INDEX_FILE . "\n";
