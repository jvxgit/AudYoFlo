<#
.SYNOPSIS
  Creates a multi-resolution Windows .ico file (e.g. for a Flutter Windows runner) from an image.

.DESCRIPTION
  - Optionally crops a region from the source image (-Crop "x,y,w,h", in source pixels).
  - Trims the empty border automatically: transparent pixels, or - if the image has no
    transparency - pixels matching the top-left corner color.
  - Centers the result in a square with a relative padding.
  - Writes an .ico with sizes 16, 24, 32, 48, 64, 128 (32-bit BMP entries) and 256 (PNG entry).

  In a Flutter app the icon is referenced in windows/runner/Runner.rc (IDI_APP_ICON) and is
  used for the .exe/desktop shortcut and at runtime for the window and taskbar
  (windows/runner/win32_window.cpp). Rebuild the app afterwards; Windows caches icons, so
  re-create an existing desktop shortcut (or run "ie4uinit.exe -show").

.EXAMPLE
  # ayfstarter / startern_app: ring symbol from the left part of the binaurics logo
  powershell -ExecutionPolicy Bypass -File scripts\flutter-app-icon.ps1 `
     -Source  sources\sub-projects\ayfstarter\flutter\startern_app\images\ayfstartern_icons\ayfstartern-logo.png `
     -Crop    "0,0,1800,1195" `
     -Output  sources\sub-projects\ayfstarter\flutter\startern_app\example\windows\runner\resources\app_icon.ico
#>
param(
    [Parameter(Mandatory = $true)][string]$Source,
    [Parameter(Mandatory = $true)][string]$Output,
    # Optional crop rectangle "x,y,w,h" in source pixels, applied before trimming
    [string]$Crop = "",
    # Free border around the symbol, relative to the icon size
    [double]$Padding = 0.04,
    # Alpha value at or below which a pixel counts as empty
    [int]$AlphaThreshold = 8,
    # Optional folder for PNG previews of all generated sizes
    [string]$PreviewDir = ""
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

function Get-ArgbBytes([System.Drawing.Bitmap]$bmp) {
    $rect = New-Object System.Drawing.Rectangle 0, 0, $bmp.Width, $bmp.Height
    $data = $bmp.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly,
        [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    try {
        $bytes = New-Object byte[] ($data.Stride * $bmp.Height)
        [System.Runtime.InteropServices.Marshal]::Copy($data.Scan0, $bytes, 0, $bytes.Length)
        return @{ Bytes = $bytes; Stride = $data.Stride }
    } finally { $bmp.UnlockBits($data) }
}

$srcPath = (Resolve-Path $Source).Path
$img = [System.Drawing.Image]::FromFile($srcPath)

# --- Crop -------------------------------------------------------------------------------
$cx = 0; $cy = 0; $cw = $img.Width; $ch = $img.Height
if ($Crop) {
    $p = $Crop.Split(',') | ForEach-Object { [int]$_.Trim() }
    if ($p.Count -ne 4) { throw "-Crop expects 'x,y,w,h'" }
    $cx = [Math]::Max(0, $p[0]); $cy = [Math]::Max(0, $p[1])
    $cw = [Math]::Min($p[2], $img.Width - $cx); $ch = [Math]::Min($p[3], $img.Height - $cy)
}
$work = New-Object System.Drawing.Bitmap $cw, $ch, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($work)
$g.DrawImage($img, (New-Object System.Drawing.Rectangle 0, 0, $cw, $ch), $cx, $cy, $cw, $ch, [System.Drawing.GraphicsUnit]::Pixel)
$g.Dispose(); $img.Dispose()

# --- Trim empty border -------------------------------------------------------------------
$px = Get-ArgbBytes $work
$b = $px.Bytes; $stride = $px.Stride
$hasAlpha = $false
for ($i = 3; $i -lt $b.Length; $i += 4) { if ($b[$i] -le $AlphaThreshold) { $hasAlpha = $true; break } }
$bgB = $b[0]; $bgG = $b[1]; $bgR = $b[2]
if (-not $hasAlpha) { Write-Host "No transparency found - trimming against corner color R=$bgR G=$bgG B=$bgB" }

$minX = $cw; $minY = $ch; $maxX = -1; $maxY = -1
for ($y = 0; $y -lt $ch; $y++) {
    $row = $y * $stride
    for ($x = 0; $x -lt $cw; $x++) {
        $o = $row + 4 * $x
        if ($hasAlpha) { $used = $b[$o + 3] -gt $AlphaThreshold }
        else {
            $used = ([Math]::Abs($b[$o] - $bgB) + [Math]::Abs($b[$o + 1] - $bgG) + [Math]::Abs($b[$o + 2] - $bgR)) -gt 24
        }
        if ($used) {
            if ($x -lt $minX) { $minX = $x }; if ($x -gt $maxX) { $maxX = $x }
            if ($y -lt $minY) { $minY = $y }; if ($y -gt $maxY) { $maxY = $y }
        }
    }
}
if ($maxX -lt 0) { throw "Image (region) is empty." }
$tw = $maxX - $minX + 1; $th = $maxY - $minY + 1
Write-Host "Content bounds in crop: x=$minX y=$minY w=$tw h=$th"

# --- Square master with padding ---------------------------------------------------------
$side = [int][Math]::Ceiling([Math]::Max($tw, $th) / (1.0 - 2.0 * $Padding))
$master = New-Object System.Drawing.Bitmap $side, $side, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g = [System.Drawing.Graphics]::FromImage($master)
if (-not $hasAlpha) { $g.Clear([System.Drawing.Color]::FromArgb(255, $bgR, $bgG, $bgB)) } else { $g.Clear([System.Drawing.Color]::Transparent) }
$dx = [int](($side - $tw) / 2); $dy = [int](($side - $th) / 2)
$g.DrawImage($work, (New-Object System.Drawing.Rectangle $dx, $dy, $tw, $th), $minX, $minY, $tw, $th, [System.Drawing.GraphicsUnit]::Pixel)
$g.Dispose(); $work.Dispose()

# --- Render sizes -----------------------------------------------------------------------
$sizes = 16, 24, 32, 48, 64, 128, 256
$entries = @()
foreach ($z in $sizes) {
    $bmp = New-Object System.Drawing.Bitmap $z, $z, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.InterpolationMode = 'HighQualityBicubic'; $g.SmoothingMode = 'HighQuality'
    $g.PixelOffsetMode = 'HighQuality'; $g.CompositingQuality = 'HighQuality'
    $g.DrawImage($master, 0, 0, $z, $z)
    $g.Dispose()

    if ($PreviewDir) {
        New-Item -ItemType Directory -Force $PreviewDir | Out-Null
        $bmp.Save((Join-Path $PreviewDir "icon_$z.png"), [System.Drawing.Imaging.ImageFormat]::Png)
    }

    $ms = New-Object IO.MemoryStream
    if ($z -ge 256) {
        # PNG-compressed entry (Vista+) for the large size
        $bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
    } else {
        # Classic 32-bit DIB entry: BITMAPINFOHEADER + bottom-up BGRA + AND mask
        $bw = New-Object IO.BinaryWriter $ms
        $pz = Get-ArgbBytes $bmp
        $maskStride = [int]([Math]::Ceiling($z / 32.0)) * 4
        $bw.Write([UInt32]40); $bw.Write([Int32]$z); $bw.Write([Int32](2 * $z))
        $bw.Write([UInt16]1); $bw.Write([UInt16]32); $bw.Write([UInt32]0)
        $bw.Write([UInt32]($z * $z * 4 + $maskStride * $z))
        $bw.Write([Int32]0); $bw.Write([Int32]0); $bw.Write([UInt32]0); $bw.Write([UInt32]0)
        for ($y = $z - 1; $y -ge 0; $y--) { $bw.Write($pz.Bytes, $y * $pz.Stride, 4 * $z) }
        for ($y = $z - 1; $y -ge 0; $y--) {
            $mask = New-Object byte[] $maskStride
            for ($x = 0; $x -lt $z; $x++) {
                if ($pz.Bytes[$y * $pz.Stride + 4 * $x + 3] -eq 0) {
                    $mask[[int][Math]::Floor($x / 8)] = $mask[[int][Math]::Floor($x / 8)] -bor (0x80 -shr ($x % 8))
                }
            }
            $bw.Write($mask)
        }
        $bw.Flush()
    }
    $entries += , ($ms.ToArray())
    $bmp.Dispose()
}
$master.Dispose()

# --- Write ICO --------------------------------------------------------------------------
$out = New-Object IO.MemoryStream
$w = New-Object IO.BinaryWriter $out
$w.Write([UInt16]0); $w.Write([UInt16]1); $w.Write([UInt16]$sizes.Count)
$offset = 6 + 16 * $sizes.Count
for ($k = 0; $k -lt $sizes.Count; $k++) {
    $d = if ($sizes[$k] -ge 256) { 0 } else { $sizes[$k] }
    $w.Write([byte]$d); $w.Write([byte]$d); $w.Write([byte]0); $w.Write([byte]0)
    $w.Write([UInt16]1); $w.Write([UInt16]32)
    $w.Write([UInt32]$entries[$k].Length); $w.Write([UInt32]$offset)
    $offset += $entries[$k].Length
}
foreach ($e in $entries) { $w.Write($e) }
$w.Flush()

$outPath = [IO.Path]::GetFullPath((Join-Path (Get-Location) $Output))
if ([IO.Path]::IsPathRooted($Output)) { $outPath = $Output }
[IO.File]::WriteAllBytes($outPath, $out.ToArray())
Write-Host "Written $outPath ($($out.Length) bytes, sizes: $($sizes -join ', '))"
