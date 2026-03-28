<?php
/**
 * Q3Rally Ladder – Admin Interface
 * admin.php  v1.0.5
 *
 * Tabs: Pending · Online · Offline · Revoked
 * Actions: approve, revoke, delete (revoked only)
 */

declare(strict_types=1);

require_once __DIR__ . '/keys.php';
require_once __DIR__ . '/version.php';

keys_suspend_inactive();
keys_require_admin();

// ── Handle actions ────────────────────────────────────────────────────────────
$message     = '';
$messageType = 'success';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $action = $_POST['action'] ?? '';
    $key    = $_POST['key']    ?? '';

    if ($action === 'approve' && $key !== '') {
        keys_approve($key);
        $message = 'Key approved.';
    } elseif ($action === 'revoke' && $key !== '') {
        keys_revoke($key);
        $message = 'Key revoked.';
    } elseif ($action === 'delete' && $key !== '') {
        if (keys_delete($key)) {
            $message = 'Key permanently deleted.';
        } else {
            $message     = 'Delete failed – key must be revoked first.';
            $messageType = 'error';
        }
    }
}

// ── Load & partition keys ─────────────────────────────────────────────────────
$allKeys = keys_load();

$pending = [];
$online  = [];
$offline = [];
$revoked = [];

foreach ($allKeys as $r) {
    $status = $r['status'] ?? 'unknown';
    if ($status === 'revoked') {
        $revoked[] = $r;
    } elseif (keys_is_offline($r)) {
        $offline[] = $r;
    } elseif ($status === 'pending') {
        $pending[] = $r;
    } else {
        $online[] = $r;
    }
}

// Sort each group: pending first by date, others by last-used desc
$sortByCreated = fn($a, $b) => strcmp($a['createdAt'] ?? '', $b['createdAt'] ?? '');
$sortByUsed    = fn($a, $b) => strcmp($b['lastUsedAt'] ?? '', $a['lastUsedAt'] ?? '');

usort($pending, $sortByCreated);
usort($online,  $sortByUsed);
usort($offline, $sortByUsed);
usort($revoked, fn($a,$b) => strcmp($b['revokedAt'] ?? $b['createdAt'] ?? '', $a['revokedAt'] ?? $a['createdAt'] ?? ''));

// Active tab from query string
$activeTab = $_GET['tab'] ?? (count($pending) > 0 ? 'pending' : 'online');
if (!in_array($activeTab, ['pending','online','offline','revoked'], true)) {
    $activeTab = 'online';
}

// ── Helpers ───────────────────────────────────────────────────────────────────
function status_badge(string $status): string
{
    $map = [
        'pending'   => ['#ffd966', 'rgba(255,200,0,0.15)',  'rgba(255,200,0,0.4)'],
        'active'    => ['#4fd1a5', 'rgba(29,158,117,0.15)', 'rgba(29,158,117,0.45)'],
        'suspended' => ['#ffaa55', 'rgba(255,150,0,0.15)',  'rgba(255,150,0,0.4)'],
        'revoked'   => ['#ff8090', 'rgba(220,53,69,0.15)',  'rgba(220,53,69,0.4)'],
    ];
    [$color, $bg, $border] = $map[$status] ?? ['#B7BCD6', 'rgba(255,255,255,0.08)', 'rgba(255,255,255,0.2)'];
    return "<span style='display:inline-block;padding:2px 10px;border-radius:999px;font-size:.75rem;"
         . "font-weight:600;letter-spacing:.06em;text-transform:uppercase;background:{$bg};"
         . "border:1px solid {$border};color:{$color}'>{$status}</span>";
}

function action_buttons(array $r, bool $showDelete = false): string
{
    $key    = htmlspecialchars($r['key'] ?? '');
    $status = $r['status'] ?? '';
    $out    = '<div style="display:flex;gap:8px;flex-wrap:wrap">';

    if (in_array($status, ['pending','suspended'], true)) {
        $out .= '<form method="post"><input type="hidden" name="action" value="approve">'
              . "<input type='hidden' name='key' value='{$key}'>"
              . '<button type="submit" class="btn-approve">Approve</button></form>';
    }
    if ($status !== 'revoked') {
        $out .= '<form method="post"><input type="hidden" name="action" value="revoke">'
              . "<input type='hidden' name='key' value='{$key}'>"
              . '<button type="submit" class="btn-revoke">Revoke</button></form>';
    }
    if ($showDelete && $status === 'revoked') {
        $out .= '<form method="post" onsubmit="return confirm(\'Permanently delete this key? This cannot be undone.\')">'
              . '<input type="hidden" name="action" value="delete">'
              . "<input type='hidden' name='key' value='{$key}'>"
              . '<button type="submit" class="btn-delete">Delete</button></form>';
    }

    $out .= '</div>';
    return $out;
}

function key_row(array $r, bool $showDelete = false): string
{
    $serverName = htmlspecialchars($r['serverName'] ?? '–');
    $ownerName  = htmlspecialchars($r['ownerName']  ?? '–');
    $ownerEmail = htmlspecialchars($r['ownerEmail'] ?? '');
    $keyPreview = htmlspecialchars(substr($r['key'] ?? '', 0, 12));
    $matches    = (int)($r['matchCount'] ?? 0);
    $lastUsed   = htmlspecialchars($r['lastUsedAt'] ?? '–');
    $lastIp     = htmlspecialchars($r['lastUsedIp'] ?? '–');
    $status     = $r['status'] ?? 'unknown';

    return "<tr>
        <td><strong>{$serverName}</strong></td>
        <td>{$ownerName}<br><span class='meta'>{$ownerEmail}</span></td>
        <td>" . status_badge($status) . "</td>
        <td><span class='key-mono'>{$keyPreview}…</span></td>
        <td>{$matches}</td>
        <td class='meta'>{$lastUsed}</td>
        <td class='meta'>{$lastIp}</td>
        <td>" . action_buttons($r, $showDelete) . "</td>
    </tr>";
}
?>
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Q3Rally Ladder Admin</title>
  <style>
    :root{--bg:#05050c;--surface:rgba(18,21,33,.97);--surface2:rgba(28,31,46,.97);--border:rgba(255,255,255,.1);--text:#F5F6FF;--text-muted:#B7BCD6;--accent:#5D8BFF;--accent-soft:rgba(93,139,255,.18)}
    *,*::before,*::after{box-sizing:border-box}
    body{margin:0;background:var(--bg);color:var(--text);font-family:'Inter',ui-sans-serif,sans-serif;padding:32px 18px}
    .page{max-width:1200px;margin:0 auto}

    /* Header */
    .page-header{display:flex;align-items:baseline;justify-content:space-between;flex-wrap:wrap;gap:12px;margin-bottom:28px}
    h1{margin:0;font-size:1.4rem}
    .version-badge{font-size:.75rem;color:var(--text-muted);background:var(--surface2);border:1px solid var(--border);border-radius:6px;padding:3px 10px}

    /* Alerts */
    .alert{padding:10px 16px;border-radius:10px;margin-bottom:20px;font-size:.9rem}
    .alert-success{background:rgba(29,158,117,.15);border:1px solid rgba(29,158,117,.45);color:#4fd1a5}
    .alert-error{background:rgba(220,53,69,.15);border:1px solid rgba(220,53,69,.45);color:#ff8090}

    /* Tabs */
    .tabs{display:flex;gap:4px;margin-bottom:24px;border-bottom:1px solid var(--border);padding-bottom:0}
    .tab-btn{background:none;border:none;color:var(--text-muted);font-size:.88rem;font-weight:600;padding:10px 18px;cursor:pointer;border-bottom:2px solid transparent;margin-bottom:-1px;text-decoration:none;display:inline-flex;align-items:center;gap:7px;transition:color .15s,border-color .15s}
    .tab-btn:hover{color:var(--text)}
    .tab-btn.active{color:var(--text);border-bottom-color:var(--accent)}
    .tab-count{background:var(--accent-soft);border:1px solid rgba(93,139,255,.35);color:var(--accent);border-radius:999px;padding:1px 8px;font-size:.72rem;font-weight:700}
    .tab-count.warn{background:rgba(255,200,0,.15);border-color:rgba(255,200,0,.4);color:#ffd966}

    /* Table */
    table{width:100%;border-collapse:collapse;background:var(--surface);border:1px solid var(--border);border-radius:16px;overflow:hidden;font-size:.88rem}
    th{background:var(--surface2);color:var(--text-muted);font-size:.74rem;text-transform:uppercase;letter-spacing:.08em;padding:12px 16px;text-align:left;border-bottom:1px solid var(--border);font-weight:600}
    td{padding:12px 16px;border-bottom:1px solid var(--border);vertical-align:top}
    tr:last-child td{border-bottom:none}
    tr:hover td{background:rgba(255,255,255,.02)}
    .key-mono{font-family:monospace;font-size:.8rem;color:var(--accent)}
    .meta{font-size:.78rem;color:var(--text-muted)}

    /* Buttons */
    button{padding:6px 14px;border-radius:8px;border:none;font-size:.82rem;font-weight:600;cursor:pointer;transition:opacity .15s}
    button:hover{opacity:.85}
    .btn-approve{background:rgba(29,158,117,.25);color:#4fd1a5;border:1px solid rgba(29,158,117,.4)}
    .btn-revoke{background:rgba(220,53,69,.2);color:#ff8090;border:1px solid rgba(220,53,69,.35)}
    .btn-delete{background:rgba(180,0,0,.35);color:#ff6060;border:1px solid rgba(200,0,0,.5)}

    /* Empty state */
    .empty{text-align:center;color:var(--text-muted);padding:40px;font-size:.9rem}

    /* Changelog */
    details.changelog{margin-top:40px;background:var(--surface);border:1px solid var(--border);border-radius:16px;overflow:hidden}
    details.changelog summary{padding:14px 20px;cursor:pointer;font-size:.88rem;font-weight:600;color:var(--text-muted);list-style:none;display:flex;align-items:center;gap:8px}
    details.changelog summary::before{content:'▸';font-size:.7rem;transition:transform .2s}
    details[open].changelog summary::before{transform:rotate(90deg)}
    .changelog-body{padding:0 20px 20px}
    .cl-version{margin-top:18px}
    .cl-version h3{font-size:.82rem;font-weight:700;color:var(--accent);margin:0 0 6px;display:flex;align-items:center;gap:10px}
    .cl-date{font-size:.75rem;color:var(--text-muted);font-weight:400}
    .cl-version ul{margin:0;padding-left:20px;display:flex;flex-direction:column;gap:4px}
    .cl-version li{font-size:.82rem;color:var(--text-muted);line-height:1.5}
  </style>
</head>
<body>
<div class="page">

  <div class="page-header">
    <h1>Q3Rally Ladder Admin</h1>
    <span class="version-badge">v<?= htmlspecialchars(LADDER_VERSION) ?></span>
  </div>

  <?php if ($message !== ''): ?>
    <div class="alert alert-<?= $messageType === 'error' ? 'error' : 'success' ?>">
      <?= htmlspecialchars($message) ?>
    </div>
  <?php endif; ?>

  <!-- Tabs -->
  <nav class="tabs">
    <?php
    $tabs = [
        'pending' => ['label' => 'Pending',  'count' => count($pending), 'warn' => true],
        'online'  => ['label' => 'Online',   'count' => count($online),  'warn' => false],
        'offline' => ['label' => 'Offline',  'count' => count($offline), 'warn' => false],
        'revoked' => ['label' => 'Revoked',  'count' => count($revoked), 'warn' => false],
    ];
    foreach ($tabs as $id => $tab):
        $active = $activeTab === $id ? ' active' : '';
        $countClass = $tab['warn'] && $tab['count'] > 0 ? ' warn' : '';
    ?>
    <a href="?tab=<?= $id ?>" class="tab-btn<?= $active ?>">
      <?= $tab['label'] ?>
      <span class="tab-count<?= $countClass ?>"><?= $tab['count'] ?></span>
    </a>
    <?php endforeach; ?>
  </nav>

  <!-- Tab content -->
  <?php
  $currentKeys  = $$activeTab;   // $pending / $online / $offline / $revoked
  $showDelete   = ($activeTab === 'revoked');
  $tableHeaders = '<tr>
      <th>Server</th><th>Owner</th><th>Status</th>
      <th>Key (preview)</th><th>Matches</th><th>Last used</th><th>Last IP</th><th>Actions</th>
  </tr>';
  ?>

  <?php if ($activeTab === 'pending' && count($pending) === 0): ?>
    <div class="empty">No pending registrations.</div>
  <?php elseif (count($currentKeys) === 0): ?>
    <div class="empty">No keys in this category.</div>
  <?php else: ?>
  <table>
    <thead><?= $tableHeaders ?></thead>
    <tbody>
      <?php foreach ($currentKeys as $r): ?>
        <?= key_row($r, $showDelete) ?>
      <?php endforeach; ?>
    </tbody>
  </table>
  <?php endif; ?>

  <!-- Changelog -->
  <details class="changelog">
    <summary>Changelog</summary>
    <div class="changelog-body">
      <?php foreach (LADDER_CHANGELOG as $ver => $entry): ?>
      <div class="cl-version">
        <h3>v<?= htmlspecialchars($ver) ?> <span class="cl-date"><?= htmlspecialchars($entry['date']) ?></span></h3>
        <ul>
          <?php foreach ($entry['changes'] as $line): ?>
            <li><?= htmlspecialchars($line) ?></li>
          <?php endforeach; ?>
        </ul>
      </div>
      <?php endforeach; ?>
    </div>
  </details>

</div>
</body>
</html>
