<?php
declare(strict_types=1);
require_once __DIR__ . '/keys.php';

$error = ''; $success = false; $newKey = ''; $ownerEmail = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $serverName  = trim($_POST['server_name']  ?? '');
    $ownerName   = trim($_POST['owner_name']   ?? '');
    $ownerEmail  = trim($_POST['owner_email']  ?? '');
    $agree       = !empty($_POST['agree']);
    $serverNameClean = keys_strip_color_codes($serverName);

    if ($serverNameClean === '' || $ownerName === '' || $ownerEmail === '') {
        $error = 'All fields are required.';
    } elseif (!filter_var($ownerEmail, FILTER_VALIDATE_EMAIL)) {
        $error = 'Please enter a valid e-mail address.';
    } elseif (strlen($serverNameClean) > 64) {
        $error = 'Server name must be 64 characters or less.';
    } elseif (!$agree) {
        $error = 'You must agree to the terms.';
    } else {
        foreach (keys_load() as $record) {
            if (keys_normalize_server_name($record['serverName'] ?? '') === keys_normalize_server_name($serverNameClean)) {
                $error = 'A key for this server name already exists. Contact the admin if you lost your key.';
                break;
            }
        }
        if ($error === '') {
            $newKey  = keys_register($serverNameClean, $ownerName, $ownerEmail);
            $success = true;
        }
    }
}
?>
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Q3Rally Ladder – Server Registration</title>
  <style>
    :root{--bg:radial-gradient(1200px 800px at 50% 0%,rgba(255,255,255,.06),#05050c 60%);--surface:rgba(18,21,33,.92);--surface2:rgba(28,31,46,.97);--border:rgba(255,255,255,.12);--border-strong:rgba(255,255,255,.22);--text:#F5F6FF;--text-muted:#B7BCD6;--accent:#5D8BFF;--accent-soft:rgba(93,139,255,.18);--accent-border:rgba(93,139,255,.45);--success:rgba(29,158,117,.15);--success-border:rgba(29,158,117,.5);--success-text:#4fd1a5;--danger:rgba(220,53,69,.15);--danger-border:rgba(220,53,69,.45);--danger-text:#ff8090;--warn:rgba(255,180,0,.12);--warn-border:rgba(255,180,0,.4);--warn-text:#ffd966}
    *,*::before,*::after{box-sizing:border-box}
    body{margin:0;min-height:100vh;background:var(--bg);color:var(--text);font-family:'Inter',ui-sans-serif,system-ui,sans-serif;display:flex;align-items:flex-start;justify-content:center;padding:48px 18px}
    .layout{width:min(960px,100%);display:grid;gap:28px;grid-template-columns:1fr 1fr;align-items:start}
    @media(max-width:720px){.layout{grid-template-columns:1fr}}
    .card{background:var(--surface);border:1px solid var(--border);border-radius:24px;padding:32px;backdrop-filter:blur(18px)}
    .logo{display:flex;align-items:center;gap:14px;margin-bottom:24px}
    .logo img{width:48px;height:48px;border-radius:12px;border:1px solid var(--border-strong);padding:8px;background:rgba(255,255,255,.04)}
    h1{margin:0;font-size:1.35rem}
    h2{margin:0 0 18px;font-size:.88rem;color:var(--text-muted);font-weight:600;text-transform:uppercase;letter-spacing:.08em}
    .subtitle{margin:4px 0 0;color:var(--text-muted);font-size:.88rem}
    .divider{border:none;border-top:1px solid var(--border);margin:22px 0}
    label{display:block;font-size:.78rem;font-weight:600;text-transform:uppercase;letter-spacing:.08em;color:var(--text-muted);margin-bottom:6px}
    input[type=text],input[type=email]{width:100%;background:var(--surface2);border:1px solid var(--border-strong);color:var(--text);padding:11px 14px;border-radius:10px;font-size:.97rem;outline:none;transition:border-color .2s,box-shadow .2s}
    input[type=text]:focus,input[type=email]:focus{border-color:var(--accent);box-shadow:0 0 0 3px var(--accent-soft)}
    .field{margin-bottom:18px}
    .hint{font-size:.78rem;color:var(--text-muted);margin-top:5px;line-height:1.5}
    .hint code,.step-desc code{color:var(--accent);background:rgba(93,139,255,.1);padding:1px 5px;border-radius:4px;font-size:.82rem}
    .checkbox-row{display:flex;gap:10px;align-items:flex-start;margin-bottom:22px}
    .checkbox-row input{margin-top:3px;flex-shrink:0;accent-color:var(--accent);width:16px;height:16px}
    button[type=submit]{width:100%;background:var(--accent);color:#fff;border:none;border-radius:10px;padding:13px;font-size:1rem;font-weight:600;cursor:pointer;transition:background .2s,transform .1s}
    button[type=submit]:hover{background:#4a7aee;transform:translateY(-1px)}
    .alert{padding:13px 16px;border-radius:12px;margin-bottom:20px;font-size:.9rem;line-height:1.5}
    .alert-error{background:var(--danger);border:1px solid var(--danger-border);color:var(--danger-text)}
    .alert-success{background:var(--success);border:1px solid var(--success-border);color:var(--success-text)}
    .alert-warn{background:var(--warn);border:1px solid var(--warn-border);color:var(--warn-text)}
    .steps{display:flex;flex-direction:column;gap:0}
    .step{display:flex;gap:16px;padding:18px 0;border-bottom:1px solid var(--border)}
    .step:last-child{border-bottom:none;padding-bottom:0}
    .step-num{flex-shrink:0;width:30px;height:30px;border-radius:50%;background:var(--accent-soft);border:1px solid var(--accent-border);color:var(--accent);font-size:.85rem;font-weight:700;display:flex;align-items:center;justify-content:center}
    .step-body{flex:1}
    .step-title{font-weight:600;font-size:.95rem;margin:0 0 6px}
    .step-desc{font-size:.84rem;color:var(--text-muted);line-height:1.6;margin:0}
    .codeblock{background:rgba(0,0,0,.5);border:1px solid var(--border);border-radius:10px;padding:12px 14px;font-family:monospace;font-size:.82rem;color:var(--accent);margin-top:8px;line-height:1.7;white-space:pre;overflow-x:auto}
    .key-box{background:rgba(0,0,0,.45);border:1px solid var(--border);border-radius:10px;padding:14px 16px;font-family:monospace;font-size:.85rem;word-break:break-all;color:var(--accent);margin:10px 0;user-select:all;line-height:1.6}
    .badge{display:inline-block;padding:2px 10px;border-radius:999px;font-size:.72rem;font-weight:600;letter-spacing:.06em;text-transform:uppercase}
    .badge-pending{background:rgba(255,200,0,.15);border:1px solid rgba(255,200,0,.4);color:#ffd966}
    .tag{display:inline-block;background:rgba(255,255,255,.07);border:1px solid var(--border);border-radius:6px;padding:1px 7px;font-size:.76rem;color:var(--text-muted)}
  </style>
</head>
<body>
<div class="layout">

  <div class="card">
    <div class="logo">
      <img src="/logo.png" alt="Q3Rally" onerror="this.style.display='none'">
      <div>
        <h1>Server Registration</h1>
        <p class="subtitle">Request a Ladder API key for your server</p>
      </div>
    </div>

    <?php if ($success): ?>
      <div class="alert alert-success">
        <strong>Registration submitted!</strong><br>
        Your key is awaiting admin approval. You will be notified at
        <strong><?= htmlspecialchars($ownerEmail) ?></strong> once it is active.
      </div>
      <p style="color:var(--text-muted);font-size:.85rem;margin:0 0 6px">
        Your API key &nbsp;<span class="badge badge-pending">pending</span>
      </p>
      <div class="key-box"><?= htmlspecialchars($newKey) ?></div>
      <div class="alert alert-warn" style="margin-top:14px">
        <strong>Save this key now – it will not be shown again.</strong><br>
        Once approved, add these lines to your server config:
        <div class="codeblock">set sv_ladderEnabled  "1"
set sv_ladderUrl      "https://ladder.q3rally.com/index.php"
set sv_ladderApiKey   "<?= htmlspecialchars($newKey) ?>"</div>
      </div>

    <?php else: ?>
      <?php if ($error !== ''): ?>
        <div class="alert alert-error"><?= htmlspecialchars($error) ?></div>
      <?php endif; ?>

      <form method="post" novalidate>
        <div class="field">
          <label for="server_name">Server name &nbsp;<span class="tag">sv_hostname</span></label>
          <input type="text" id="server_name" name="server_name" maxlength="64" autocomplete="off"
                 value="<?= htmlspecialchars($_POST['server_name'] ?? '') ?>"
                 placeholder="Q3Rally EU #1" required>
          <p class="hint">
            Must match your <code>sv_hostname</code> exactly.<br>
            Q3 color codes like <code>^1red</code> or <code>^7white</code> are stripped automatically.
          </p>
        </div>
        <div class="field">
          <label for="owner_name">Your name / handle</label>
          <input type="text" id="owner_name" name="owner_name" maxlength="64"
                 value="<?= htmlspecialchars($_POST['owner_name'] ?? '') ?>"
                 placeholder="SomeAdmin" required>
        </div>
        <div class="field">
          <label for="owner_email">Contact e-mail</label>
          <input type="email" id="owner_email" name="owner_email"
                 value="<?= htmlspecialchars($_POST['owner_email'] ?? '') ?>"
                 placeholder="admin@example.com" required>
          <p class="hint">Only used to notify you about your key status. Not publicly visible.</p>
        </div>
        <hr class="divider">
        <div class="checkbox-row">
          <input type="checkbox" id="agree" name="agree" <?= !empty($_POST['agree']) ? 'checked' : '' ?>>
          <span><label for="agree" style="text-transform:none;letter-spacing:0;font-weight:400;color:var(--text-muted)">
            I understand that match data submitted by my server will be publicly visible on the ladder.
            I will not submit manipulated or fake match data.
          </label></span>
        </div>
        <button type="submit">Request API key</button>
      </form>
    <?php endif; ?>
  </div>

  <div class="card">
    <h2>How it works</h2>
    <div class="steps">

      <div class="step">
        <div class="step-num">1</div>
        <div class="step-body">
          <p class="step-title">Find your server name</p>
          <p class="step-desc">
            Open your server config (e.g. <code>server.cfg</code>) and look for <code>sv_hostname</code>.
            Enter this name exactly in the form on the left.<br><br>
            Example:<br>
            <code>set sv_hostname "Q3Rally EU #1"</code>
          </p>
        </div>
      </div>

      <div class="step">
        <div class="step-num">2</div>
        <div class="step-body">
          <p class="step-title">Submit the form &amp; save your key</p>
          <p class="step-desc">
            After submitting you will see your API key once.
            <strong style="color:var(--warn-text)">Copy it immediately</strong> –
            it is not stored in recoverable form and cannot be shown again.
            If you lose it you will need to contact the admin.
          </p>
        </div>
      </div>

      <div class="step">
        <div class="step-num">3</div>
        <div class="step-body">
          <p class="step-title">Wait for approval</p>
          <p class="step-desc">
            Your key starts as <span class="badge badge-pending">pending</span>.
            The Q3Rally admin reviews and approves it – usually within 24 hours.
            You will get an e-mail when it is active.
          </p>
        </div>
      </div>

      <div class="step">
        <div class="step-num">4</div>
        <div class="step-body">
          <p class="step-title">Add three lines to your server config</p>
          <p class="step-desc">Once approved, add the following and restart your server:</p>
          <div class="codeblock">set sv_ladderEnabled  "1"
set sv_ladderUrl      "https://ladder.q3rally.com/index.php"
set sv_ladderApiKey   "your-key-here"</div>
          <p class="step-desc" style="margin-top:10px">
            The key is handled like <code>rconPassword</code> –
            it is never written to logs or shown in status queries.
          </p>
        </div>
      </div>

      <div class="step">
        <div class="step-num">5</div>
        <div class="step-body">
          <p class="step-title">Done – uploads happen automatically</p>
          <p class="step-desc">
            After every match the server sends the result to the ladder.
            Failed uploads are retried automatically.
            No further action needed.<br><br>
            Keys inactive for more than 90 days are suspended automatically.
            Contact the admin to reactivate.
          </p>
        </div>
      </div>

    </div>
    <hr class="divider">
    <p style="font-size:.78rem;color:var(--text-muted);margin:0;line-height:1.6">
      <strong style="color:var(--text)">Questions or problems?</strong><br>
      Contact the Q3Rally admin or post in the community forum.
    </p>
  </div>

</div>
</body>
</html>
