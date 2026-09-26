param(
    [string]$OutputPath = ""
)

Add-Type -AssemblyName System.Drawing

if (-not $OutputPath) {
    $OutputPath = Join-Path $PSScriptRoot "..\baseq3r\gfx\ui\ingame_charset.png"
}

$outputDirectory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

$size = 512
$cell = 32
$bitmap = New-Object System.Drawing.Bitmap $size, $size,
    ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.Clear([System.Drawing.Color]::Transparent)
$graphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
$graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality

# The game HUD gets its own, denser technical face.  The atlas stays separate
# from the frontend atlas so menu typography can evolve independently.
$font = New-Object System.Drawing.Font(
    "Bahnschrift",
    29,
    [System.Drawing.FontStyle]::Bold,
    [System.Drawing.GraphicsUnit]::Pixel
)
$brush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
$format = New-Object System.Drawing.StringFormat
$format.Alignment = [System.Drawing.StringAlignment]::Center
$format.LineAlignment = [System.Drawing.StringAlignment]::Center
$format.Trimming = [System.Drawing.StringTrimming]::None

for ($code = 32; $code -le 126; $code++) {
    $char = [char]$code
    $index = $code
    $row = [math]::Floor($index / 16)
    $column = $index % 16
    $rect = New-Object System.Drawing.RectangleF(
        ($column * $cell),
        ($row * $cell),
        $cell,
        $cell
    )
    $graphics.DrawString($char.ToString(), $font, $brush, $rect, $format)
}

$bitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)

$format.Dispose()
$brush.Dispose()
$font.Dispose()
$graphics.Dispose()
$bitmap.Dispose()

Write-Host "Generated $OutputPath"
