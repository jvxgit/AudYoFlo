"""Convert black-on-white icons to black icons with an alpha channel.

Flutter only uses the alpha channel of these icons (the color is applied
by the widget), so white must become transparent. The brightness of each
pixel is mapped to transparency:

    white -> fully transparent
    black -> fully opaque
    gray  -> partially transparent

The color channels are set to black. Files are overwritten in place.

Usage:
    python make_transparent.py [--force] <file.png | directory> [...]

Directories are processed non-recursively (all *.png in them). Images that
already have an alpha channel are skipped unless --force is given.

Requires Pillow (pip install pillow).
"""

import glob
import os
import sys

from PIL import Image, ImageOps


def convert(path, force):
    im = Image.open(path)
    if ("A" in im.getbands() or "transparency" in im.info) and not force:
        print(f"skip (has alpha): {path}")
        return
    alpha = ImageOps.invert(im.convert("L"))
    out = Image.new("RGBA", im.size, (0, 0, 0, 0))
    out.putalpha(alpha)
    out.save(path)
    print(f"converted: {path}")


def main(argv):
    force = "--force" in argv
    targets = [a for a in argv if a != "--force"]
    if not targets:
        print(__doc__)
        return 1
    for t in targets:
        files = sorted(glob.glob(os.path.join(t, "*.png"))) if os.path.isdir(t) else [t]
        for f in files:
            convert(f, force)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
