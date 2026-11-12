#!/usr/bin/env python3
"""
Create the embedded overlay font for jvx-opengl-cpp (jvxVideoOverlayText.h):

1) Subset DejaVuSans.ttf to the characters required for plot labels (Latin-1, Greek,
   a few math symbols) and remove hinting (not used by stb_truetype) - fontTools required.
2) Write a C++ header with the subset font as byte array and the font license as comment.

Called by prepare.cmake in the CMake configure step:

    python gen_font_header.py --ttf DejaVuSans.ttf --license LICENSE --out jvxFontDejaVuSans.h
"""

import argparse
import io
import sys

# Characters contained in the subset
UNICODES = (
    list(range(0x0020, 0x007F))      # Basic Latin
    + list(range(0x00A0, 0x0100))    # Latin-1 supplement (incl. degree, micro, plus-minus)
    + list(range(0x0391, 0x03AA))    # Greek capital letters
    + list(range(0x03B1, 0x03CA))    # Greek small letters
    + [0x2013, 0x2014,               # en / em dash
       0x2026,                       # ellipsis
       0x2030,                       # per mille
       0x2212,                       # minus sign
       0x221E,                       # infinity
       0x2248,                       # almost equal
       0x2264, 0x2265]               # less / greater or equal
)


def subset_font(ttf_path):
    try:
        from fontTools import subset
        from fontTools.ttLib import TTFont
    except ImportError:
        sys.stderr.write("Python module fontTools not found, install it with: pip install fonttools\n")
        sys.exit(2)

    options = subset.Options()
    options.layout_features = ["kern"]
    options.hinting = False
    options.desubroutinize = True
    options.name_IDs = ["*"]        # keep copyright / license entries of the name table
    options.notdef_outline = True

    font = TTFont(ttf_path)
    subsetter = subset.Subsetter(options=options)
    subsetter.populate(unicodes=UNICODES)
    subsetter.subset(font)

    out = io.BytesIO()
    font.save(out)
    return out.getvalue()


def write_header(data, license_text, out_path):
    lic = license_text.replace("*/", "* /")
    lines = [
        "#ifndef __JVXFONTDEJAVUSANS_H__",
        "#define __JVXFONTDEJAVUSANS_H__",
        "",
        "/*",
        " * GENERATED FILE - do not edit and do not add to the repository.",
        " * Generator: sources/jvxLibraries/third_party/web/stb-dejavu/gen_font_header.py",
        " *",
        " * Subset of DejaVu Sans (DejaVu fonts 2.37, https://dejavu-fonts.github.io/):",
        " * Latin-1, Greek and a few math symbols, hinting removed. Font license below.",
        " *",
    ]
    lines += [(" * " + l).rstrip() for l in lic.splitlines()]
    lines += [
        " */",
        "",
        "#include <cstddef>",
        "",
        "// Inline function with local static: one instance per program, also if included",
        "// from many translation units (header-only, C++11)",
        "inline const unsigned char* jvxFontDejaVuSans_data(std::size_t* sz)",
        "{",
        "\tstatic const unsigned char data[%d] =" % len(data),
        "\t{",
    ]
    for i in range(0, len(data), 20):
        lines.append("\t\t" + ",".join("0x%02x" % b for b in data[i:i + 20]) + ",")
    lines += [
        "\t};",
        "\tif (sz)",
        "\t{",
        "\t\t*sz = sizeof(data);",
        "\t}",
        "\treturn data;",
        "}",
        "",
        "#endif",
        "",
    ]
    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ttf", required=True, help="DejaVuSans.ttf (full font)")
    ap.add_argument("--license", required=True, help="DejaVu LICENSE file")
    ap.add_argument("--out", required=True, help="Header to write")
    args = ap.parse_args()

    data = subset_font(args.ttf)
    with open(args.license, "r", encoding="utf-8") as f:
        license_text = f.read()
    write_header(data, license_text, args.out)
    print("Written %s (%d bytes font data)" % (args.out, len(data)))


if __name__ == "__main__":
    main()
