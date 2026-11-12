Text output of the plot overlay in jvx-opengl-cpp (jvxVideoOverlayText.h)
==========================================================================

In the CMake configure step, prepare.cmake downloads and generates (not part of the repository):

- include/stb/stb_truetype.h
  stb_truetype v1.27 by Sean Barrett, https://github.com/nothings/stb
  (pinned commit, SHA256 checked). License: public domain (Unlicense) or MIT, see end of file.

- include/fonts/jvxFontDejaVuSans.h
  Subset of DejaVu Sans 2.37, https://dejavu-fonts.github.io/ (SHA256 checked),
  embedded as byte array. Contains Latin-1, Greek and a few math symbols, no hinting.
  License: Bitstream Vera / Arev font license, copied into the generated header.
  The subset is created by gen_font_header.py and requires Python 3 with fontTools:

      pip install fonttools

Downloads are stored in download/. To force regeneration, delete include/ and download/.
