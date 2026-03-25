<?php
/**
 * Q3Rally Ladder – Admin Interface
 * admin.php
 *
 * Approve, revoke and inspect server keys.
 * Protected by LADDER_ADMIN_PASSWORD (set via env).
 */

declare(strict_types=1);

require_once __DIR__ . '/keys.php';

keys_suspend_inactive();
keys_require_admin();

// Handle actions
$message = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $action = $_POST['action'] ?? '';
    $key    = $_POST['key']    ?? '';

    if ($action === 'approve' && $key !== '') {
        keys_approve($key);
        $message = 'Key approved.';
    } elseif ($action === 'revoke' && $key !== '') {
        keys_revoke($key);
        $message = 'Key revoked.';
    }
}

$keys = keys_load();

usort($keys, static function ($a, $b) {
    $order = ['pending' => 0, 'active' => 1, 'suspended' => 2, 'revoked' => 3];
    return ($order[$a['status'] ?? ''] ?? 9) <=> ($order[$b['status'] ?? ''] ?? 9);
});

function status_badge(string $status): string
{
    $map = [
        'pending'   => ['#ffd966', 'rgba(255,200,0,0.15)',  'rgba(255,200,0,0.4)'],
        'active'    => ['#4fd1a5', 'rgba(29,158,117,0.15)', 'rgba(29,158,117,0.45)'],
        'suspended' => ['#ffaa55', 'rgba(255,150,0,0.15)',  'rgba(255,150,0,0.4)'],
        'revoked'   => ['#ff8090', 'rgba(220,53,69,0.15)',  'rgba(220,53,69,0.4)'],
    ];
    [$color, $bg, $border] = $map[$status] ?? ['#B7BCD6', 'rgba(255,255,255,0.08)', 'rgba(255,255,255,0.2)'];
    return "<span style='display:inline-block;padding:2px 10px;border-radius:999px;font-size:.75rem;font-weight:600;"
         . "letter-spacing:.06em;text-transform:uppercase;background:{$bg};border:1px solid {$border};"
         . "color:{$color}'>{$status}</span>";
}

$pending = array_filter($keys, fn($k) => ($k['status'] ?? '') === 'pending');
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
    .page{max-width:1100px;margin:0 auto}
    h1{margin:0 0 4px;font-size:1.5rem}
    .subtitle{color:var(--text-muted);font-size:.9rem;margin:0 0 28px}
    .alert{background:rgba(29,158,117,.15);border:1px solid rgba(29,158,117,.45);color:#4fd1a5;padding:10px 16px;border-radius:10px;margin-bottom:20px;font-size:.9rem}
    table{width:100%;border-collapse:collapse;background:var(--surface);border:1px solid var(--border);border-radius:16px;overflow:hidden;font-size:.88rem}
    th{background:var(--surface2);color:var(--text-muted);font-size:.75rem;text-transform:uppercase;letter-spacing:.08em;padding:12px 16px;text-align:left;border-bottom:1px solid var(--border);font-weight:600}
    td{padding:12px 16px;border-bottom:1px solid var(--border);vertical-align:top}
    tr:last-child td{border-bottom:none}
    tr:hover td{background:rgba(255,255,255,.02)}
    .key-mono{font-family:monospace;font-size:.8rem;color:var(--accent);word-break:break-all}
    .actions{display:flex;gap:8px;flex-wrap:wrap}
    button{padding:6px 14px;border-radius:8px;border:none;font-size:.82rem;font-weight:600;cursor:pointer;transition:opacity .15s}
    button:hover{opacity:.85}
    .btn-approve{background:rgba(29,158,117,.25);color:#4fd1a5;border:1px solid rgba(29,158,117,.4)}
    .btn-revoke{background:rgba(220,53,69,.2);color:#ff8090;border:1px solid rgba(220,53,69,.35)}
    .meta{font-size:.78rem;color:var(--text-muted)}
    .section-header{display:flex;align-items:center;justify-content:space-between;margin:32px 0 12px}
    .count-badge{background:var(--accent-soft);border:1px solid rgba(93,139,255,.35);color:var(--accent);border-radius:999px;padding:2px 10px;font-size:.78rem;font-weight:600}
  </style>
</head>
<body>
<div class="page">
  <h1>Q3Rally Ladder Admin</h1>
  <p class="subtitle">Server key management</p>

  <?php if ($message !== ''): ?>
    <div class="alert"><?= htmlspecialchars($message) ?></div>
  <?php endif; ?>

  <?php if (!empty($pending)): ?>
  <div class="section-header">
    <h2 style="margin:0;font-size:1rem">Pending approval</h2>
    <span class="count-badge"><?= count($pending) ?></span>
  </div>
  <table>
    <thead><tr><th>Server</th><th>Owner</th><th>Key</th><th>Registered</th><th>Actions</th></tr></thead>
    <tbody>
    <?php foreach ($pending as $r): ?>
      <tr>
        <td><strong><?= htmlspecialchars($r['serverName'] ?? '–') ?></strong></td>
        <td><?= htmlspecialchars($r['ownerName'] ?? '–') ?><br>
            <span class="meta"><?= htmlspecialchars($r['ownerEmail'] ?? '') ?></span></td>
        <td><span class="key-mono"><?= htmlspecialchars($r['key'] ?? '') ?></span></td>
        <td class="meta"><?= htmlspecialchars($r['createdAt'] ?? '–') ?></td>
        <td><div class="actions">
          <form method="post">
            <input type="hidden" name="action" value="approve">
            <input type="hidden" name="key" value="<?= htmlspecialchars($r['key'] ?? '') ?>">
            <button type="submit" class="btn-approve">Approve</button>
          </form>
          <form method="post">
            <input type="hidden" name="action" value="revoke">
            <input type="hidden" name="key" value="<?= htmlspecialchars($r['key'] ?? '') ?>">
            <button type="submit" class="btn-revoke">Reject</button>
          </form>
        </div></td>
      </tr>
    <?php endforeach; ?>
    </tbody>
  </table>
  <?php endif; ?>

  <div class="section-header">
    <h2 style="margin:0;font-size:1rem">All keys</h2>
    <span class="count-badge"><?= count($keys) ?></span>
  </div>
  <table>
    <thead><tr>
      <th>Server</th><th>Owner</th><th>Status</th><th>Key (preview)</th>
      <th>Matches</th><th>Last used</th><th>Last IP</th><th>Actions</th>
    </tr></thead>
    <tbody>
    <?php foreach ($keys as $r): ?>
      <tr>
        <td><strong><?= htmlspecialchars($r['serverName'] ?? '–') ?></strong></td>
        <td><?= htmlspecialchars($r['ownerName'] ?? '–') ?><br>
            <span class="meta"><?= htmlspecialchars($r['ownerEmail'] ?? '') ?></span></td>
        <td><?= status_badge($r['status'] ?? 'unknown') ?></td>
        <td><span class="key-mono"><?= htmlspecialchars(substr($r['key'] ?? '', 0, 12)) ?>…</span></td>
        <td><?= (int)($r['matchCount'] ?? 0) ?></td>
        <td class="meta"><?= htmlspecialchars($r['lastUsedAt'] ?? '–') ?></td>
        <td class="meta"><?= htmlspecialchars($r['lastUsedIp'] ?? '–') ?></td>
        <td><div class="actions">
          <?php if (in_array($r['status'] ?? '', ['pending', 'suspended'], true)): ?>
          <form method="post">
            <input type="hidden" name="action" value="approve">
            <input type="hidden" name="key" value="<?= htmlspecialchars($r['key'] ?? '') ?>">
            <button type="submit" class="btn-approve">Approve</button>
          </form>
          <?php endif; ?>
          <?php if (($r['status'] ?? '') !== 'revoked'): ?>
          <form method="post">
            <input type="hidden" name="action" value="revoke">
            <input type="hidden" name="key" value="<?= htmlspecialchars($r['key'] ?? '') ?>">
            <button type="submit" class="btn-revoke">Revoke</button>
          </form>
          <?php endif; ?>
        </div></td>
      </tr>
    <?php endforeach; ?>
    <?php if (empty($keys)): ?>
      <tr><td colspan="8" style="text-align:center;color:var(--text-muted);padding:32px">No keys registered yet.</td></tr>
    <?php endif; ?>
    </tbody>
  </table>
</div>
</body>
</html>
