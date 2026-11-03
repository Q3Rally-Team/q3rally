param(
    [string]$Q3Map2 = '',
    [string]$Python = 'python'
)
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
if (-not $Q3Map2) {
    $Q3Map2 = Join-Path (Split-Path $repoRoot -Parent) 'Q3RallyRadiant\install\q3map2.exe'
}
if (-not (Test-Path -LiteralPath $Q3Map2)) {
    throw 'Q3Map2 fehlt. Den Compiler mit -Q3Map2 <Pfad> angeben.'
}
$logRoot = Join-Path $repoRoot 'dist\crossroads-work'
New-Item -ItemType Directory -Force -Path $logRoot | Out-Null
& $Python (Join-Path $PSScriptRoot 'crossroads_textures.py')
if ($LASTEXITCODE -ne 0) { throw 'Shader-Erzeugung fehlgeschlagen.' }
& $Python (Join-Path $PSScriptRoot 'make_crossroads.py')
if ($LASTEXITCODE -ne 0) { throw 'Map-Erzeugung fehlgeschlagen.' }
$mapFile = Join-Path $repoRoot 'baseq3r\maps\q3r_crossroads.map'
# quake3 + current custinfoparms avoids stale built-in Q3Rally surface flags
# in older compilers. The produced IBSP is the standard Q3Rally map format.
$common = @('-game', 'quake3', '-fs_basegame', 'baseq3r', '-fs_basepath', $repoRoot,
            '-threads', '8')
$phases = @(
    @{ Name = 'bsp'; Args = @('-meta', '-custinfoparms') },
    @{ Name = 'vis'; Args = @('-vis', '-saveprt') },
    @{ Name = 'light'; Args = @('-light', '-custinfoparms', '-fast', '-filter', '-super', '2', '-bounce', '1') }
)
foreach ($phase in $phases) {
    $phaseArgs = $phase.Args
    $log = Join-Path $logRoot ('compile-' + $phase.Name + '.log')
    & $Q3Map2 @common @phaseArgs $mapFile 2>&1 | Tee-Object -FilePath $log
    if ($LASTEXITCODE -ne 0) { throw "Q3Map2 $($phase.Name) fehlgeschlagen: $log" }
    if (Select-String -LiteralPath $log -Pattern '\*+ leaked \*+|^ERROR:|^FATAL:|WARNING: Unknown option' -Quiet) {
        throw "Q3Map2 meldet einen Geometriefehler: $log"
    }
}
$validationJson = & $Python (Join-Path $PSScriptRoot 'validate_crossroads.py')
if ($LASTEXITCODE -ne 0) { throw 'BSP-Kollisionspruefung fehlgeschlagen.' }
$validationJson | Set-Content -LiteralPath (Join-Path $repoRoot 'docs\maps\crossroads\bsp_validation.json') -Encoding utf8
Write-Output $validationJson
& $Python (Join-Path $PSScriptRoot 'package_crossroads.py')
if ($LASTEXITCODE -ne 0) { throw 'PK3-Erzeugung fehlgeschlagen.' }
