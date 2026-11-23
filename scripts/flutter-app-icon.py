#!/usr/bin/env python3
"""
Creates a multi-resolution Windows .ico file (e.g. for a Flutter Windows runner) from an image.
Python counterpart of flutter-app-icon.ps1 - same options, same output layout.

- Optionally crops a region from the source image (--crop "x,y,w,h", in source pixels).
- Trims the empty border automatically: transparent pixels, or - if the image has no
  transparency - pixels matching the top-left corner color.
- Centers the result in a square with a relative padding.
- Writes an .ico with sizes 16, 24, 32, 48, 64, 128 (32-bit BMP entries) and 256 (PNG entry).

In a Flutter app the icon is referenced in windows/runner/Runner.rc (IDI_APP_ICON) and is
used for the .exe/desktop shortcut and at runtime for the window and taskbar
(windows/runner/win32_window.cpp). Rebuild the app afterwards; Windows caches icons, so
re-create an existing desktop shortcut (or run "ie4uinit.exe -show").

Requires Pillow (pip install pillow).

Example (ayfstarter / startern_app: ring symbol from the left part of the binaurics logo):
  python scripts/flutter-app-icon.py \
     --source sources/sub-projects/ayfstarter/flutter/startern_app/images/ayfstartern_icons/ayfstartern-logo.png \
     --crop   "0,0,1800,1195" \
     --output sources/sub-projects/ayfstarter/flutter/startern_app/example/windows/runner/resources/app_icon.ico
"""

import argparse
import io
import math
import os
import struct
import sys

try:
    from PIL import Image, ImageChops
except ImportError:
    sys.exit("Pillow is required: pip install pillow")

SIZES = (16, 24, 32, 48, 64, 128, 256)


def parse_crop(text, img_w, img_h):
    parts = [int(p.strip()) for p in text.split(",")]
    if len(parts) != 4:
        raise SystemExit("--crop expects 'x,y,w,h'")
    x, y = max(0, parts[0]), max(0, parts[1])
    w, h = min(parts[2], img_w - x), min(parts[3], img_h - y)
    return x, y, x + w, y + h


def content_bbox(img, alpha_threshold):
    """Bounding box of the non-empty content, plus background color (None if transparent)."""
    alpha = img.getchannel("A")
    if alpha.getextrema()[0] <= alpha_threshold:
        mask = alpha.point(lambda a: 255 if a > alpha_threshold else 0)
        return mask.getbbox(), None

    bg = img.getpixel((0, 0))
    print(f"No transparency found - trimming against corner color R={bg[0]} G={bg[1]} B={bg[2]}")
    rgb = img.convert("RGB")
    diff = ImageChops.difference(rgb, Image.new("RGB", rgb.size, bg[:3]))
    r, g, b = diff.split()
    total = ImageChops.add(ImageChops.add(r, g), b)  # saturating sum of channel differences
    mask = total.point(lambda v: 255 if v > 24 else 0)
    return mask.getbbox(), (bg[0], bg[1], bg[2], 255)


def bmp_entry(im):
    """Classic 32-bit DIB icon entry: BITMAPINFOHEADER + bottom-up BGRA + AND mask."""
    z = im.width
    mask_stride = int(math.ceil(z / 32.0)) * 4
    out = io.BytesIO()
    out.write(struct.pack("<IiiHHIIiiII", 40, z, 2 * z, 1, 32, 0,
                          z * z * 4 + mask_stride * z, 0, 0, 0, 0))
    r, g, b, a = im.split()
    bgra = Image.merge("RGBA", (b, g, r, a)).tobytes()
    row = 4 * z
    for y in range(z - 1, -1, -1):
        out.write(bgra[y * row:(y + 1) * row])
    alpha = a.tobytes()
    for y in range(z - 1, -1, -1):
        mask = bytearray(mask_stride)
        for x in range(z):
            if alpha[y * z + x] == 0:
                mask[x // 8] |= 0x80 >> (x % 8)
        out.write(mask)
    return out.getvalue()


def main():
    ap = argparse.ArgumentParser(description="Create a multi-resolution Windows .ico from an image.")
    ap.add_argument("--source", required=True, help="source image (PNG, ...)")
    ap.add_argument("--output", required=True, help="target .ico file")
    ap.add_argument("--crop", default="", help="optional crop rectangle 'x,y,w,h' in source pixels")
    ap.add_argument("--padding", type=float, default=0.04,
                    help="free border around the symbol, relative to icon size (default 0.04)")
    ap.add_argument("--alpha-threshold", type=int, default=8,
                    help="alpha value at or below which a pixel counts as empty (default 8)")
    ap.add_argument("--preview-dir", default="", help="optional folder for PNG previews of all sizes")
    args = ap.parse_args()

    img = Image.open(args.source).convert("RGBA")

    # --- Crop ---------------------------------------------------------------------------
    if args.crop:
        img = img.crop(parse_crop(args.crop, img.width, img.height))

    # --- Trim empty border ----------------------------------------------------------------
    bbox, bg = content_bbox(img, args.alpha_threshold)
    if bbox is None:
        raise SystemExit("Image (region) is empty.")
    img = img.crop(bbox)
    tw, th = img.size
    print(f"Content bounds in crop: x={bbox[0]} y={bbox[1]} w={tw} h={th}")

    # --- Square master with padding --------------------------------------------------------
    side = int(math.ceil(max(tw, th) / (1.0 - 2.0 * args.padding)))
    master = Image.new("RGBA", (side, side), bg if bg else (0, 0, 0, 0))
    master.paste(img, ((side - tw) // 2, (side - th) // 2), img)

    # --- Render sizes -------------------------------------------------------------------
    entries = []
    for z in SIZES:
        im = master.resize((z, z), Image.LANCZOS)  # Pillow resizes RGBA premultiplied
        if args.preview_dir:
            os.makedirs(args.preview_dir, exist_ok=True)
            im.save(os.path.join(args.preview_dir, f"icon_{z}.png"))
        if z >= 256:
            buf = io.BytesIO()
            im.save(buf, format="PNG")  # PNG-compressed entry (Vista+) for the large size
            entries.append(buf.getvalue())
        else:
            entries.append(bmp_entry(im))

    # --- Write ICO ----------------------------------------------------------------------
    data = io.BytesIO()
    data.write(struct.pack("<HHH", 0, 1, len(SIZES)))
    offset = 6 + 16 * len(SIZES)
    for z, e in zip(SIZES, entries):
        d = 0 if z >= 256 else z
        data.write(struct.pack("<BBBBHHII", d, d, 0, 0, 1, 32, len(e), offset))
        offset += len(e)
    for e in entries:
        data.write(e)

    with open(args.output, "wb") as f:
        f.write(data.getvalue())
    print(f"Written {os.path.abspath(args.output)} ({data.tell()} bytes, sizes: "
          f"{', '.join(str(s) for s in SIZES)})")


if __name__ == "__main__":
    main()
