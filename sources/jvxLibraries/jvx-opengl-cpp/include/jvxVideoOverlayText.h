#ifndef __JVXVIDEOOVERLAYTEXT_H__
#define __JVXVIDEOOVERLAYTEXT_H__

/*
 * Platform independent text rasterizer for the plot overlay (jvxVideoOverlay.h).
 * Based on stb_truetype with an embedded subset of DejaVu Sans - no dependency to
 * any UI toolkit. Header-only: stb_truetype is compiled with internal linkage
 * (STBTT_STATIC) into every translation unit that includes this file.
 *
 * stb/stb_truetype.h and fonts/jvxFontDejaVuSans.h are not part of the repository: they
 * are downloaded / generated in the CMake configure step of jvx-opengl-cpp into
 * sources/jvxLibraries/third_party/web/stb-dejavu/include - add this folder to the include
 * path of every project that includes jvxVideoRenderCore.h.
 *
 * Text is UTF-8. Characters not contained in the font subset (Latin-1, Greek and a
 * few math symbols) are rendered as the font's "missing glyph".
 *
 * Coordinates are image coordinates: origin top-left, y downwards.
 */

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstddef>

// Silence warnings from the third party code. clang on Windows also defines _MSC_VER,
// hence check for gcc / clang first
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#elif defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#ifndef STBTT_STATIC
#define STBTT_STATIC
#define JVX_STBTT_STATIC_LOCAL
#endif
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define JVX_STBTT_IMPL_LOCAL
#endif

#include "stb/stb_truetype.h"

#ifdef JVX_STBTT_IMPL_LOCAL
#undef STB_TRUETYPE_IMPLEMENTATION
#undef JVX_STBTT_IMPL_LOCAL
#endif
#ifdef JVX_STBTT_STATIC_LOCAL
#undef STBTT_STATIC
#undef JVX_STBTT_STATIC_LOCAL
#endif

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "fonts/jvxFontDejaVuSans.h"

struct jvxVideoOverlayColor
{
	std::uint8_t r = 0;
	std::uint8_t g = 0;
	std::uint8_t b = 0;
	std::uint8_t a = 255;
};

enum class jvxVideoOverlayAlign
{
	start,	// left / top
	center,
	end		// right / bottom
};

class jvxVideoOverlayText
{
public:

	// Shared instance: the font is parsed once, all methods are const and re-entrant
	static const jvxVideoOverlayText& instance()
	{
		static jvxVideoOverlayText inst;
		return inst;
	}

	bool valid() const
	{
		return ok;
	}

	// Size of the (unrotated) bounding box of the text. pixel_size is the em size in pixels.
	// The height covers ascent and descent of the font, independent of the actual characters.
	void measure(const std::string& txt, int pixel_size, int& width, int& height) const
	{
		width = 0;
		height = 0;
		if (!ok || (pixel_size <= 0))
		{
			return;
		}
		const float scale = stbtt_ScaleForMappingEmToPixels(&info, (float)pixel_size);
		height = (int)std::ceil((float)(ascent - descent) * scale);
		width = (int)std::ceil(text_advance(decode_utf8(txt), scale));
	}

	// Draw text into buf (width * height * 4 bytes, RGBA non-premultiplied, top row first),
	// alpha-blended over the existing content. The bounding box (see measure) is placed
	// relative to the anchor (x, y) according to halign / valign. If rotate_ccw is set, the
	// text is rotated by 90 degrees counter-clockwise (reading bottom to top); alignment then
	// refers to the rotated box.
	void draw(std::vector<std::uint8_t>& buf, int width, int height,
		const std::string& txt, int pixel_size, int x, int y,
		jvxVideoOverlayAlign halign, jvxVideoOverlayAlign valign,
		bool rotate_ccw, const jvxVideoOverlayColor& col) const
	{
		if (!ok || txt.empty() || (pixel_size <= 0) || (width <= 0) || (height <= 0) ||
			(buf.size() < (size_t)width * (size_t)height * 4))
		{
			return;
		}

		const float scale = stbtt_ScaleForMappingEmToPixels(&info, (float)pixel_size);
		const std::vector<int> cps = decode_utf8(txt);
		const int tw = (int)std::ceil(text_advance(cps, scale));
		const int th = (int)std::ceil((float)(ascent - descent) * scale);
		const int baseline = (int)std::floor((float)ascent * scale + 0.5f);

		// Bounding box in image coordinates (rotated text: width and height swapped)
		const int bw = rotate_ccw ? th : tw;
		const int bh = rotate_ccw ? tw : th;
		int bx = x;
		int by = y;
		switch (halign)
		{
		case jvxVideoOverlayAlign::center: bx -= bw / 2; break;
		case jvxVideoOverlayAlign::end: bx -= bw; break;
		default: break;
		}
		switch (valign)
		{
		case jvxVideoOverlayAlign::center: by -= bh / 2; break;
		case jvxVideoOverlayAlign::end: by -= bh; break;
		default: break;
		}

		std::vector<unsigned char> glyph;
		float pen = 0.0f;
		for (size_t i = 0; i < cps.size(); i++)
		{
			const int cp = cps[i];
			const int gi = stbtt_FindGlyphIndex(&info, cp);
			if (i > 0)
			{
				pen += scale * (float)stbtt_GetGlyphKernAdvance(&info, stbtt_FindGlyphIndex(&info, cps[i - 1]), gi);
			}

			const int penInt = (int)std::floor(pen);
			const float shiftX = pen - (float)penInt;
			int ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
			stbtt_GetGlyphBitmapBoxSubpixel(&info, gi, scale, scale, shiftX, 0.0f, &ix0, &iy0, &ix1, &iy1);
			const int gw = ix1 - ix0;
			const int gh = iy1 - iy0;
			if ((gw > 0) && (gh > 0))
			{
				glyph.assign((size_t)gw * (size_t)gh, 0);
				stbtt_MakeGlyphBitmapSubpixel(&info, glyph.data(), gw, gh, gw, scale, scale, shiftX, 0.0f, gi);
				for (int gy = 0; gy < gh; gy++)
				{
					for (int gx = 0; gx < gw; gx++)
					{
						const unsigned char cov = glyph[(size_t)gy * (size_t)gw + (size_t)gx];
						if (cov == 0)
						{
							continue;
						}

						// Position in the unrotated text box, then in the image
						const int tx = penInt + ix0 + gx;
						const int ty = baseline + iy0 + gy;
						int px = bx + tx;
						int py = by + ty;
						if (rotate_ccw)
						{
							px = bx + ty;
							py = by + bh - 1 - tx;
						}
						blend(buf, width, height, px, py, col, cov);
					}
				}
			}

			int adv = 0, lsb = 0;
			stbtt_GetGlyphHMetrics(&info, gi, &adv, &lsb);
			pen += scale * (float)adv;
		}
	}

private:

	stbtt_fontinfo info;
	bool ok = false;
	int ascent = 0;
	int descent = 0;

	jvxVideoOverlayText()
	{
		std::size_t sz = 0;
		const unsigned char* data = jvxFontDejaVuSans_data(&sz);
		const int offset = stbtt_GetFontOffsetForIndex(data, 0);
		ok = (offset >= 0) && (stbtt_InitFont(&info, data, offset) != 0);
		if (ok)
		{
			int lineGap = 0;
			stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
		}
	}

	float text_advance(const std::vector<int>& cps, float scale) const
	{
		float pen = 0.0f;
		int giPrev = 0;
		for (size_t i = 0; i < cps.size(); i++)
		{
			const int gi = stbtt_FindGlyphIndex(&info, cps[i]);
			if (i > 0)
			{
				pen += scale * (float)stbtt_GetGlyphKernAdvance(&info, giPrev, gi);
			}
			int adv = 0, lsb = 0;
			stbtt_GetGlyphHMetrics(&info, gi, &adv, &lsb);
			pen += scale * (float)adv;
			giPrev = gi;
		}
		return pen;
	}

	// Alpha blend color with coverage over the (non-premultiplied) destination pixel
	static void blend(std::vector<std::uint8_t>& buf, int width, int height, int x, int y,
		const jvxVideoOverlayColor& col, unsigned char cov)
	{
		if ((x < 0) || (y < 0) || (x >= width) || (y >= height))
		{
			return;
		}
		std::uint8_t* px = buf.data() + ((size_t)y * (size_t)width + (size_t)x) * 4;
		const float sa = ((float)cov / 255.0f) * ((float)col.a / 255.0f);
		const float da = (float)px[3] / 255.0f;
		const float oa = sa + da * (1.0f - sa);
		if (oa <= 0.0f)
		{
			return;
		}
		const float wd = da * (1.0f - sa);
		px[0] = (std::uint8_t)(((float)col.r * sa + (float)px[0] * wd) / oa + 0.5f);
		px[1] = (std::uint8_t)(((float)col.g * sa + (float)px[1] * wd) / oa + 0.5f);
		px[2] = (std::uint8_t)(((float)col.b * sa + (float)px[2] * wd) / oa + 0.5f);
		px[3] = (std::uint8_t)(oa * 255.0f + 0.5f);
	}

	// Minimal UTF-8 decoder, invalid sequences are mapped to '?'
	static std::vector<int> decode_utf8(const std::string& s)
	{
		std::vector<int> out;
		out.reserve(s.size());
		size_t i = 0;
		while (i < s.size())
		{
			const unsigned char c = (unsigned char)s[i];
			int cp = '?';
			int len = 1;
			if (c < 0x80)
			{
				cp = c;
			}
			else if ((c & 0xE0) == 0xC0)
			{
				cp = c & 0x1F;
				len = 2;
			}
			else if ((c & 0xF0) == 0xE0)
			{
				cp = c & 0x0F;
				len = 3;
			}
			else if ((c & 0xF8) == 0xF0)
			{
				cp = c & 0x07;
				len = 4;
			}
			else
			{
				out.push_back('?');
				i++;
				continue;
			}

			if (i + (size_t)len > s.size())
			{
				out.push_back('?');
				break;
			}
			bool valid = true;
			for (int k = 1; k < len; k++)
			{
				const unsigned char cc = (unsigned char)s[i + (size_t)k];
				if ((cc & 0xC0) != 0x80)
				{
					valid = false;
					break;
				}
				cp = (cp << 6) | (cc & 0x3F);
			}
			if (!valid)
			{
				out.push_back('?');
				i++;
				continue;
			}
			out.push_back(cp);
			i += (size_t)len;
		}
		return out;
	}
};

#endif
