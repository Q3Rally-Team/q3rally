param(
    [string]$OutputPath = "",
    # 0.5, 1 and 2 generate 256, 512 and 1024 square atlases respectively.
    [ValidateSet('0.5', '1', '2')]
    [string]$Scale = '1'
)

Add-Type -AssemblyName System.Drawing

if (-not $OutputPath) {
    $OutputPath = Join-Path $PSScriptRoot "..\baseq3r\gfx\ui\ingame_charset.png"
}

$outputDirectory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

$scaleFactor = [double]::Parse($Scale, [System.Globalization.CultureInfo]::InvariantCulture)
$size = [int](512 * $scaleFactor)
$cell = [int](32 * $scaleFactor)
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
    (29 * $scaleFactor),
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
    # Bahnschrift's M and W have shorter ink bounds; align their cap baseline
    # with the rest of the uppercase glyphs rather than centering them higher.
    if ($char -eq 'M' -or $char -eq 'W') {
        $rect.Y += 3 * $scaleFactor
    }
    $graphics.DrawString($char.ToString(), $font, $brush, $rect, $format)
}

$bitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)

$format.Dispose()
$brush.Dispose()
$font.Dispose()
$graphics.Dispose()
$bitmap.Dispose()

Write-Host "Generated $OutputPath"
