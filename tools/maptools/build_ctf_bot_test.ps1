param(
    [string]$Q3Map2 = '',
    [string]$Bspc = ''
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$radiantInstall = Join-Path (Split-Path $repoRoot -Parent) 'Q3RallyRadiant\install'

if (-not $Q3Map2) {
    $Q3Map2 = Join-Path $radiantInstall 'q3map2.exe'
}
if (-not $Bspc) {
    $Bspc = Join-Path $radiantInstall 'mbspc.exe'
}
if (-not (Test-Path -LiteralPath $Q3Map2)) {
    throw 'Q3Map2 fehlt. Mit -Q3Map2 <Pfad> angeben.'
}
if (-not (Test-Path -LiteralPath $Bspc)) {
    throw 'MBSPC fehlt. Mit -Bspc <Pfad> angeben.'
}

$mapFile = Join-Path $repoRoot 'baseq3r\maps\q3r_ctf_bot_test.map'
$bspFile = Join-Path $repoRoot 'baseq3r\maps\q3r_ctf_bot_test.bsp'
$aasFile = Join-Path $repoRoot 'baseq3r\maps\q3r_ctf_bot_test.aas'
$logRoot = Join-Path $repoRoot 'dist\ctf-bot-test-work'
New-Item -ItemType Directory -Force -Path $logRoot | Out-Null

$common = @('-game', 'quake3', '-fs_basegame', 'baseq3r', '-fs_basepath', $repoRoot,
            '-threads', '8')
$phases = @(
    @{ Name = 'bsp'; Args = @('-meta', '-custinfoparms') },
    @{ Name = 'vis'; Args = @('-vis', '-saveprt') },
    @{ Name = 'light'; Args = @('-light', '-custinfoparms', '-fast', '-filter') }
)

foreach ($phase in $phases) {
    $log = Join-Path $logRoot ('compile-' + $phase.Name + '.log')
    $phaseArgs = $phase.Args
    & $Q3Map2 @common @phaseArgs $mapFile 2>&1 | Tee-Object -FilePath $log
    if ($LASTEXITCODE -ne 0) {
        throw "Q3Map2 $($phase.Name) fehlgeschlagen: $log"
    }
    if (Select-String -LiteralPath $log -Pattern '^ERROR:|^FATAL:|\*+ leaked \*+' -Quiet) {
        throw "Q3Map2 meldet einen Geometriefehler: $log"
    }
}

$aasLog = Join-Path $logRoot 'compile-aas.log'
Push-Location $logRoot
try {
    & $Bspc -forcesidesvisible -optimize -reach -bsp2aas $bspFile 2>&1 |
        Tee-Object -FilePath $aasLog
    $bspcExitCode = $LASTEXITCODE
}
finally {
    Pop-Location
}
if ($bspcExitCode -ne 0 -or -not (Test-Path -LiteralPath $aasFile)) {
    throw "MBSPC konnte keine AAS erstellen: $aasLog"
}

Remove-Item -LiteralPath (Join-Path $repoRoot 'baseq3r\maps\q3r_ctf_bot_test.prt') -ErrorAction SilentlyContinue
Remove-Item -LiteralPath (Join-Path $repoRoot 'baseq3r\maps\q3r_ctf_bot_test.srf') -ErrorAction SilentlyContinue
Write-Output "Created BSP: $bspFile"
Write-Output "Created AAS: $aasFile"
