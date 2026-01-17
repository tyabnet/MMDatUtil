param(
    [string]$OutDir = "Resources",
    [string]$Out = "app.ico"
)

Add-Type -AssemblyName System.Drawing

# Ensure output directory exists
$fullOutDir = Join-Path -Path (Get-Location) -ChildPath $OutDir
if (-not (Test-Path $fullOutDir)) {
    New-Item -ItemType Directory -Path $fullOutDir | Out-Null
}

$sizes = @(16, 32, 48, 256)
$pngs = @()

foreach ($size in $sizes) {
    # typed values to avoid PowerShell conversion issues
    $pixelFormat = [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
    $smoothing = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $fontStyle = [System.Drawing.FontStyle]::Bold
    $graphicsUnit = [System.Drawing.GraphicsUnit]::Pixel

    # create bitmap with typed ArgumentList so types are preserved
    $bmp = New-Object System.Drawing.Bitmap -ArgumentList $size, $size, $pixelFormat
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = $smoothing
    $g.Clear([System.Drawing.Color]::Transparent)

    # background circle with simple gradient
    $rect = New-Object System.Drawing.Rectangle -ArgumentList 0,0,$size,$size
    $brush = New-Object System.Drawing.Drawing2D.LinearGradientBrush -ArgumentList $rect, ([System.Drawing.Color]::FromArgb(255,64,128,192)), ([System.Drawing.Color]::FromArgb(255,32,96,160)), 45
    $g.FillEllipse($brush, 0,0,$size-1,$size-1)

    # draw initials centered using RectangleF and typed Font
    $fontSize = [math]::Max(8, [int]($size * 0.5))
    $font = New-Object System.Drawing.Font -ArgumentList "Segoe UI", $fontSize, $fontStyle, $graphicsUnit
    $rectF = New-Object System.Drawing.RectangleF -ArgumentList 0.0, 0.0, ([float]$size), ([float]$size)
    $sf = New-Object System.Drawing.StringFormat
    $sf.Alignment = [System.Drawing.StringAlignment]::Center
    $sf.LineAlignment = [System.Drawing.StringAlignment]::Center
    $g.DrawString("MU", $font, [System.Drawing.Brushes]::White, $rectF, $sf)

    # dispose GDI+ objects
    $font.Dispose()
    $brush.Dispose()
    $sf.Dispose()
    $g.Dispose()

    $ms = New-Object System.IO.MemoryStream
    $bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
    $pngs += ,@{ size=$size; bytes=$ms.ToArray() }
    $ms.Dispose()
    $bmp.Dispose()
}

# Build ICO (ICONDIR + ICONDIRENTRYs, embed PNG images)
$mem = New-Object System.IO.MemoryStream
$bw = New-Object System.IO.BinaryWriter($mem)

# ICONDIR header
$bw.Write([uint16]0)    # reserved
$bw.Write([uint16]1)    # type = 1 (ICON)
$bw.Write([uint16]$pngs.Count)

$headerSize = 6 + 16 * $pngs.Count
$offset = $headerSize

foreach ($p in $pngs) {
    $bytes = $p.bytes
    $widthByte = if ($p.size -eq 256) { 0 } else { [byte]$p.size }
    $heightByte = $widthByte
    $bw.Write([byte]$widthByte)        # width
    $bw.Write([byte]$heightByte)       # height
    $bw.Write([byte]0)                 # color count
    $bw.Write([byte]0)                 # reserved
    $bw.Write([uint16]1)               # planes (placeholder)
    $bw.Write([uint16]32)              # bit count (placeholder)
    $bw.Write([uint32]$bytes.Length)   # bytes in resource
    $bw.Write([uint32]$offset)         # image offset
    $offset += $bytes.Length
}

foreach ($p in $pngs) {
    $bw.Write($p.bytes)
}

$bw.Flush()
$outPath = Join-Path -Path $fullOutDir -ChildPath $Out
[System.IO.File]::WriteAllBytes($outPath, $mem.ToArray())
$bw.Dispose()
$mem.Dispose()

Write-Host "Wrote $outPath with sizes: $($sizes -join ', ')"