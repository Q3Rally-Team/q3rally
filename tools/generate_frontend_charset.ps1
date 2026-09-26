param(
    [string]$OutputPath = ""
)

Add-Type -AssemblyName System.Drawing

if ( -not $OutputPath ) {
    $OutputPath = Join-Path $PSScriptRoot "..\baseq3r\gfx\ui\frontend_charset.png"
}

$outputDirectory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

$size = 512
$cell = 32
$bitmap = New-Object System.Drawing.Bitmap(
    $size,
    $size,
    [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$font = New-Object System.Drawing.Font(
    "Bahnschrift SemiCondensed",
        26,
    [System.Drawing.FontStyle]::Bold,
    [System.Drawing.GraphicsUnit]::Pixel
)
$brush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
$format = New-Object System.Drawing.StringFormat

try {
    $graphics.Clear([System.Drawing.Color]::Transparent)
    $graphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
    $format.Alignment = [System.Drawing.StringAlignment]::Center
    $format.LineAlignment = [System.Drawing.StringAlignment]::Center
    $format.FormatFlags = [System.Drawing.StringFormatFlags]::NoWrap

    for ( $code = 0; $code -lt 256; $code++ ) {
        if ( $code -lt 32 -or $code -gt 126 ) {
            continue
        }

        $rectangle = New-Object System.Drawing.RectangleF
        $rectangle.X = [float]( ( $code % 16 ) * $cell )
        $rectangle.Y = [float]( [math]::Floor( $code / 16 ) * $cell )
        $rectangle.Width = [float]$cell
        $rectangle.Height = [float]$cell

        $graphics.DrawString( [char]$code, $font, $brush, $rectangle, $format )
    }

    $bitmap.Save( $OutputPath, [System.Drawing.Imaging.ImageFormat]::Png )
}
finally {
    $format.Dispose()
    $brush.Dispose()
    $font.Dispose()
    $graphics.Dispose()
    $bitmap.Dispose()
}
