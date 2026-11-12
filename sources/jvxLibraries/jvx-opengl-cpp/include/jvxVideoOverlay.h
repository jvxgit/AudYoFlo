#ifndef __JVXVIDEOOVERLAY_H__
#define __JVXVIDEOOVERLAY_H__

/*
 * Platform independent rasterizer for a static plot overlay (frame, axes,
 * ticks, grid, tick labels, axis labels, title and legend). The overlay is rendered
 * into an RGBA buffer on the CPU once per viewport size and composited on top
 * of the plot image by the fragment shader in jvxVideoRenderCore. No dependency
 * to Qt or any other UI toolkit - only standard C++ and the jvx base types.
 *
 * Text is rasterized by jvxVideoOverlayText (stb_truetype + embedded DejaVu Sans),
 * hence also text output works without any UI toolkit.
 *
 * Coordinates in this file are image coordinates: origin top-left, y downwards.
 */

#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstddef>

#include "jvx.h"
#include "jvxVideoOverlayText.h"

// One legend entry: color swatch + description (UTF-8)
struct jvxVideoOverlayLegendEntry
{
	jvxVideoOverlayColor color;
	std::string text;
};

// Corner of the plot area the legend is placed in, or not shown
typedef enum
{
	JVX_RENDER_LEGEND_NOT_SHOWN,
	JVX_RENDER_LEGEND_LEFT_TOP,
	JVX_RENDER_LEGEND_RIGHT_TOP,
	JVX_RENDER_LEGEND_LEFT_BOTTOM,
	JVX_RENDER_LEGEND_RIGHT_BOTTOM
} jvxRenderLegendPosition;

struct jvxVideoOverlayConfig
{
	// Overlay on / off. If off, the plot covers the full viewport as before.
	bool active = false;

	// Minimum margins around the plot area in logical pixels (multiplied by scale).
	// If text is shown, the margins are enlarged automatically to fit the text.
	int margin_left = 50;
	int margin_right = 15;
	int margin_top = 15;
	int margin_bottom = 35;

	// Scale factor logical -> physical pixels (e.g. device pixel ratio on HiDPI)
	double scale = 1.0;

	// Number of divisions along x and y. N divisions lead to N-1 inner grid lines
	// and N+1 ticks. 0 disables grid and ticks for that direction.
	// Not used for an axis with explicit label positions (labels_x_pos / labels_y_pos):
	// then ticks and grid lines are placed at the label positions.
	int num_div_x = 10;
	int num_div_y = 8;

	// Inner grid lines on / off. Frame and ticks are not affected.
	bool show_grid = true;

	// Line widths and tick length in logical pixels
	int axis_width = 1;
	int grid_width = 1;
	int tick_length = 5;

	// Tick labels. x: first entry left, y: first entry at the bottom.
	std::list<std::string> labels_x;
	std::list<std::string> labels_y;

	// Optional label positions, one value per label, relative to the axis from 0.0 to 1.0:
	// x from left to right, y from bottom to top. Values are clipped to 0.0 .. 1.0.
	// Ticks and inner grid lines of that axis follow these positions (num_div_x / num_div_y
	// are not used then).
	// If the list is empty - or the number of entries differs from the number of labels -
	// the labels are distributed evenly: first entry at the start, last at the end of the
	// axis. Hence, with num_div + 1 labels every tick is labeled.
	std::list<jvxData> labels_x_pos;
	std::list<jvxData> labels_y_pos;

	// Axis labels (x below the tick labels, y rotated left of the tick labels) and
	// title centered above the plot area. Empty strings are not shown.
	std::string axis_label_x;
	std::string axis_label_y;
	std::string title;

	// Font sizes (em size) in logical pixels and distance between text and axis/other text.
	// Text is UTF-8; supported are Latin-1, Greek and a few math symbols.
	int font_size = 11;
	int font_size_title = 13;
	int text_gap = 3;

	jvxVideoOverlayColor color_axis = { 200, 200, 200, 255 };
	jvxVideoOverlayColor color_grid = { 255, 255, 255, 70 };
	jvxVideoOverlayColor color_text = { 220, 220, 220, 255 };

	// Legend inside the plot area, one row per entry. Empty list or legend_pos == JVX_RENDER_LEGEND_NOT_SHOWN: no legend.
	// Sizes in logical pixels.
	std::list<jvxVideoOverlayLegendEntry> legend;
	jvxRenderLegendPosition legend_pos = JVX_RENDER_LEGEND_RIGHT_TOP;
	int legend_margin = 8;		// distance legend box <-> plot area border
	int legend_padding = 6;		// distance box border <-> content
	int legend_swatch = 10;		// edge length of the color square
	int legend_row_gap = 2;		// additional distance between rows
	jvxVideoOverlayColor color_legend_bg = { 26, 26, 26, 200 };
	jvxVideoOverlayColor color_legend_border = { 200, 200, 200, 255 };

	// Color of the area outside the plot area (margins)
	jvxVideoOverlayColor color_frame = { 26, 26, 26, 255 };
};

// Plot area in image pixel coordinates (inclusive bounds) plus text metrics
struct jvxVideoOverlayLayout
{
	bool valid = false;
	int width = 0;
	int height = 0;
	int x0 = 0;
	int y0 = 0;
	int x1 = 0;
	int y1 = 0;

	// Text metrics in physical pixels, 0 if the respective text is not shown
	int label_x_h = 0;	// height of the x tick labels
	int label_y_w = 0;	// max width of the y tick labels
	int axis_x_h = 0;	// height of the x axis label
	int axis_y_w = 0;	// width of the (rotated) y axis label
	int title_h = 0;	// height of the title
};

class jvxVideoOverlay
{
public:

	static int scaled(int val, double scale)
	{
		int ret = (int)(val * scale + 0.5);
		return (ret < 0) ? 0 : ret;
	}

	// Plot area and text metrics. Margins are enlarged to fit tick labels, axis labels and title.
	static jvxVideoOverlayLayout compute_layout(const jvxVideoOverlayConfig& cfg, int width, int height)
	{
		const jvxVideoOverlayText& txt = jvxVideoOverlayText::instance();
		jvxVideoOverlayLayout lay;
		lay.width = width;
		lay.height = height;

		int ml = scaled(cfg.margin_left, cfg.scale);
		int mr = scaled(cfg.margin_right, cfg.scale);
		int mt = scaled(cfg.margin_top, cfg.scale);
		int mb = scaled(cfg.margin_bottom, cfg.scale);

		if (cfg.active && txt.valid())
		{
			int aw = 0, gw = 0, tl = 0;
			line_sizes(cfg, aw, gw, tl);
			const int ext = aw + tl;
			const int gap = scaled(cfg.text_gap, cfg.scale);
			const int fs = font_px(cfg.font_size, cfg.scale);
			const int fsT = font_px(cfg.font_size_title, cfg.scale);
			int w = 0, h = 0;

			// Tick labels x: height and overhang of the leftmost / rightmost label
			const std::vector<double> rel_x = label_rel_positions(cfg.labels_x, cfg.labels_x_pos);
			int first_x_w = 0, last_x_w = 0;
			double rel_min = 2.0, rel_max = -1.0;
			size_t cnt = 0;
			for (const auto& s : cfg.labels_x)
			{
				const double r = rel_x[cnt++];
				txt.measure(s, fs, w, h);
				lay.label_x_h = imax(lay.label_x_h, h);
				if (r < rel_min)
				{
					rel_min = r;
					first_x_w = (r < 0.5) ? w : 0;
				}
				if (r > rel_max)
				{
					rel_max = r;
					last_x_w = (r > 0.5) ? w : 0;
				}
			}

			// Tick labels y: width and overhang of the top label
			int label_y_h = 0;
			for (const auto& s : cfg.labels_y)
			{
				txt.measure(s, fs, w, h);
				lay.label_y_w = imax(lay.label_y_w, w);
				label_y_h = imax(label_y_h, h);
			}

			if (!cfg.axis_label_x.empty())
			{
				txt.measure(cfg.axis_label_x, fs, w, h);
				lay.axis_x_h = h;
			}
			if (!cfg.axis_label_y.empty())
			{
				// Rotated: the text height is the horizontal extent
				txt.measure(cfg.axis_label_y, fs, w, h);
				lay.axis_y_w = h;
			}
			if (!cfg.title.empty())
			{
				txt.measure(cfg.title, fsT, w, h);
				lay.title_h = h;
			}

			// Required margins
			int need_b = ext;
			if (lay.label_x_h) need_b += gap + lay.label_x_h;
			if (lay.axis_x_h) need_b += gap + lay.axis_x_h;
			need_b += gap;

			int need_l = ext;
			if (lay.label_y_w) need_l += gap + lay.label_y_w;
			if (lay.axis_y_w) need_l += gap + lay.axis_y_w;
			need_l += gap;
			need_l = imax(need_l, first_x_w / 2 + gap);

			int need_t = label_y_h / 2 + gap;
			if (lay.title_h) need_t = imax(need_t, aw + gap + lay.title_h + gap);

			int need_r = last_x_w / 2 + gap;

			ml = imax(ml, need_l);
			mr = imax(mr, need_r);
			mt = imax(mt, need_t);
			mb = imax(mb, need_b);
		}

		lay.x0 = ml;
		lay.y0 = mt;
		lay.x1 = width - 1 - mr;
		lay.y1 = height - 1 - mb;

		// Need at least a few pixels of plot area, otherwise fall back to "no overlay"
		lay.valid = cfg.active && (width > 0) && (height > 0) &&
			(lay.x1 - lay.x0 >= 2) && (lay.y1 - lay.y0 >= 2);
		if (!lay.valid)
		{
			lay.x0 = 0;
			lay.y0 = 0;
			lay.x1 = width - 1;
			lay.y1 = height - 1;
		}
		return lay;
	}

	// Relative positions (0.0 .. 1.0) of ticks and grid lines of one axis: the label positions
	// if given (and matching the number of labels), otherwise num_div equidistant divisions
	static std::vector<double> tick_rel_positions(int num_div, const std::list<std::string>& labels, const std::list<jvxData>& pos)
	{
		if (!pos.empty() && (pos.size() == labels.size()))
		{
			return label_rel_positions(labels, pos);
		}
		std::vector<double> rel;
		for (int i = 0; i <= num_div && num_div > 0; i++)
		{
			rel.push_back((double)i / (double)num_div);
		}
		return rel;
	}

	// Relative positions (0.0 .. 1.0) of the labels: from the position list if it matches the
	// number of labels, otherwise evenly distributed (a single label is centered)
	static std::vector<double> label_rel_positions(const std::list<std::string>& labels, const std::list<jvxData>& pos)
	{
		const size_t num = labels.size();
		std::vector<double> rel;
		rel.reserve(num);
		if (!pos.empty() && (pos.size() == num))
		{
			for (const auto& p : pos)
			{
				double r = (double)p;
				r = (r < 0.0) ? 0.0 : ((r > 1.0) ? 1.0 : r);
				rel.push_back(r);
			}
		}
		else
		{
			for (size_t i = 0; i < num; i++)
			{
				rel.push_back((num <= 1) ? 0.5 : (double)i / (double)(num - 1));
			}
		}
		return rel;
	}

	// Pixel position of relative position rel (0.0 .. 1.0) from a to b (a > b allowed)
	static int rel_pos(int a, int b, double rel)
	{
		return a + (int)std::floor((double)(b - a) * rel + 0.5);
	}

	// Render the overlay into buf (width * height * 4 bytes, RGBA, top row first).
	// Pixels not covered by the overlay are fully transparent.
	static void render(const jvxVideoOverlayConfig& cfg, const jvxVideoOverlayLayout& lay, std::vector<std::uint8_t>& buf)
	{
		const jvxVideoOverlayText& txt = jvxVideoOverlayText::instance();
		buf.assign((size_t)lay.width * (size_t)lay.height * 4, 0);
		if (!lay.valid)
		{
			return;
		}

		int aw = 0, gw = 0, tl = 0;
		line_sizes(cfg, aw, gw, tl);

		// Ticks / grid lines: x from left to right, y from bottom to top
		const std::vector<double> ticks_x = tick_rel_positions(cfg.num_div_x, cfg.labels_x, cfg.labels_x_pos);
		const std::vector<double> ticks_y = tick_rel_positions(cfg.num_div_y, cfg.labels_y, cfg.labels_y_pos);

		// 1) Inner grid lines - drawn inside the plot area, not on the frame (0.0 and 1.0)
		if (cfg.show_grid && (gw > 0))
		{
			for (double r : ticks_x)
			{
				if ((r > 0.0) && (r < 1.0))
				{
					int x = rel_pos(lay.x0, lay.x1, r) - gw / 2;
					fill_rect(lay, buf, x, lay.y0, x + gw - 1, lay.y1, cfg.color_grid);
				}
			}
			for (double r : ticks_y)
			{
				if ((r > 0.0) && (r < 1.0))
				{
					int y = rel_pos(lay.y1, lay.y0, r) - gw / 2;
					fill_rect(lay, buf, lay.x0, y, lay.x1, y + gw - 1, cfg.color_grid);
				}
			}
		}

		// 2) Frame around the plot area - drawn just outside so that no data is covered
		if (aw > 0)
		{
			fill_rect(lay, buf, lay.x0 - aw, lay.y0 - aw, lay.x1 + aw, lay.y0 - 1, cfg.color_axis); // top
			fill_rect(lay, buf, lay.x0 - aw, lay.y1 + 1, lay.x1 + aw, lay.y1 + aw, cfg.color_axis); // bottom
			fill_rect(lay, buf, lay.x0 - aw, lay.y0, lay.x0 - 1, lay.y1, cfg.color_axis); // left
			fill_rect(lay, buf, lay.x1 + 1, lay.y0, lay.x1 + aw, lay.y1, cfg.color_axis); // right

			// 3) Ticks pointing outwards on the x (bottom) and y (left) axis
			if (tl > 0)
			{
				const int tw = aw;
				for (double r : ticks_x)
				{
					int x = rel_pos(lay.x0, lay.x1, r) - tw / 2;
					fill_rect(lay, buf, x, lay.y1 + aw + 1, x + tw - 1, lay.y1 + aw + tl, cfg.color_axis);
				}
				for (double r : ticks_y)
				{
					int y = rel_pos(lay.y1, lay.y0, r) - tw / 2;
					fill_rect(lay, buf, lay.x0 - aw - tl, y, lay.x0 - aw - 1, y + tw - 1, cfg.color_axis);
				}
			}
		}

		// 4) Text: tick labels, axis labels and title
		if (txt.valid())
		{
			const int ext = aw + tl;
			const int gap = scaled(cfg.text_gap, cfg.scale);
			const int fs = font_px(cfg.font_size, cfg.scale);
			const int fsT = font_px(cfg.font_size_title, cfg.scale);
			size_t i = 0;

			// Tick labels x - relative position 0.0 is left
			const std::vector<double> rel_x = label_rel_positions(cfg.labels_x, cfg.labels_x_pos);
			i = 0;
			for (const auto& s : cfg.labels_x)
			{
				int x = rel_pos(lay.x0, lay.x1, rel_x[i++]);
				txt.draw(buf, lay.width, lay.height, s, fs, x, lay.y1 + ext + gap,
					jvxVideoOverlayAlign::center, jvxVideoOverlayAlign::start, false, cfg.color_text);
			}

			// Tick labels y - relative position 0.0 is at the bottom
			const std::vector<double> rel_y = label_rel_positions(cfg.labels_y, cfg.labels_y_pos);
			i = 0;
			for (const auto& s : cfg.labels_y)
			{
				int y = rel_pos(lay.y1, lay.y0, rel_y[i++]);
				txt.draw(buf, lay.width, lay.height, s, fs, lay.x0 - ext - gap, y,
					jvxVideoOverlayAlign::end, jvxVideoOverlayAlign::center, false, cfg.color_text);
			}

			// Axis label x - below the tick labels
			if (!cfg.axis_label_x.empty())
			{
				int y = lay.y1 + ext + gap;
				if (lay.label_x_h) y += lay.label_x_h + gap;
				txt.draw(buf, lay.width, lay.height, cfg.axis_label_x, fs, (lay.x0 + lay.x1) / 2, y,
					jvxVideoOverlayAlign::center, jvxVideoOverlayAlign::start, false, cfg.color_text);
			}

			// Axis label y - rotated, left of the tick labels
			if (!cfg.axis_label_y.empty())
			{
				int x = lay.x0 - ext - gap;
				if (lay.label_y_w) x -= lay.label_y_w + gap;
				txt.draw(buf, lay.width, lay.height, cfg.axis_label_y, fs, x, (lay.y0 + lay.y1) / 2,
					jvxVideoOverlayAlign::end, jvxVideoOverlayAlign::center, true, cfg.color_text);
			}

			// Title - centered above the plot area
			if (!cfg.title.empty())
			{
				txt.draw(buf, lay.width, lay.height, cfg.title, fsT, (lay.x0 + lay.x1) / 2, lay.y0 - aw - gap,
					jvxVideoOverlayAlign::center, jvxVideoOverlayAlign::end, false, cfg.color_text);
			}

			// 5) Legend - on top of the grid inside the plot area
			render_legend(cfg, lay, buf, txt);
		}
	}

	// Legend box in one corner of the plot area: background, border and one row
	// (color square + description) per entry. Omitted if it does not fit into the plot area.
	static void render_legend(const jvxVideoOverlayConfig& cfg, const jvxVideoOverlayLayout& lay,
		std::vector<std::uint8_t>& buf, const jvxVideoOverlayText& txt)
	{
		if (cfg.legend.empty() || (cfg.legend_pos == JVX_RENDER_LEGEND_NOT_SHOWN))
		{
			return;
		}

		const int fs = font_px(cfg.font_size, cfg.scale);
		const int margin = scaled(cfg.legend_margin, cfg.scale);
		const int pad = scaled(cfg.legend_padding, cfg.scale);
		const int sw = imax(1, scaled(cfg.legend_swatch, cfg.scale));
		const int rgap = scaled(cfg.legend_row_gap, cfg.scale);
		const int tgap = scaled(cfg.text_gap, cfg.scale) * 2;
		const int bw = (cfg.axis_width > 0) ? imax(1, scaled(cfg.axis_width, cfg.scale)) : 0;

		// Row height and text width
		int row_h = sw;
		int text_w = 0;
		int w = 0, h = 0;
		for (const auto& e : cfg.legend)
		{
			txt.measure(e.text, fs, w, h);
			text_w = imax(text_w, w);
			row_h = imax(row_h, h);
		}
		const int n = (int)cfg.legend.size();
		const int box_w = bw + pad + sw + (text_w > 0 ? tgap + text_w : 0) + pad + bw;
		const int box_h = bw + pad + n * row_h + (n - 1) * rgap + pad + bw;

		// Position in the selected corner (default: right top)
		int bx = lay.x1 + 1 - margin - box_w;
		int by = lay.y0 + margin;
		switch (cfg.legend_pos)
		{
		case JVX_RENDER_LEGEND_LEFT_TOP:
			bx = lay.x0 + margin;
			break;
		case JVX_RENDER_LEGEND_RIGHT_BOTTOM:
			by = lay.y1 + 1 - margin - box_h;
			break;
		case JVX_RENDER_LEGEND_LEFT_BOTTOM:
			bx = lay.x0 + margin;
			by = lay.y1 + 1 - margin - box_h;
			break;
		default:
			break;
		}

		// Legend is only shown if it fits completely into the plot area
		if ((bx < lay.x0) || (by < lay.y0) || (bx + box_w - 1 > lay.x1) || (by + box_h - 1 > lay.y1))
		{
			return;
		}

		// Background (replaces grid pixels below) and border
		fill_rect(lay, buf, bx, by, bx + box_w - 1, by + box_h - 1, cfg.color_legend_bg);
		if (bw > 0)
		{
			fill_rect(lay, buf, bx, by, bx + box_w - 1, by + bw - 1, cfg.color_legend_border);
			fill_rect(lay, buf, bx, by + box_h - bw, bx + box_w - 1, by + box_h - 1, cfg.color_legend_border);
			fill_rect(lay, buf, bx, by, bx + bw - 1, by + box_h - 1, cfg.color_legend_border);
			fill_rect(lay, buf, bx + box_w - bw, by, bx + box_w - 1, by + box_h - 1, cfg.color_legend_border);
		}

		// Rows: color square + text, both vertically centered in the row
		int ry = by + bw + pad;
		for (const auto& e : cfg.legend)
		{
			const int sx = bx + bw + pad;
			const int sy = ry + (row_h - sw) / 2;
			fill_rect(lay, buf, sx, sy, sx + sw - 1, sy + sw - 1, e.color);
			if (!e.text.empty())
			{
				txt.draw(buf, lay.width, lay.height, e.text, fs, sx + sw + tgap, ry + row_h / 2,
					jvxVideoOverlayAlign::start, jvxVideoOverlayAlign::center, false, cfg.color_text);
			}
			ry += row_h + rgap;
		}
	}

	// Fill rectangle (inclusive bounds), clipped to the image. Overwrites pixels.
	static void fill_rect(const jvxVideoOverlayLayout& lay, std::vector<std::uint8_t>& buf,
		int xa, int ya, int xb, int yb, const jvxVideoOverlayColor& col)
	{
		if (xa < 0) xa = 0;
		if (ya < 0) ya = 0;
		if (xb > lay.width - 1) xb = lay.width - 1;
		if (yb > lay.height - 1) yb = lay.height - 1;
		for (int y = ya; y <= yb; y++)
		{
			std::uint8_t* row = buf.data() + ((size_t)y * (size_t)lay.width) * 4;
			for (int x = xa; x <= xb; x++)
			{
				std::uint8_t* px = row + (size_t)x * 4;
				px[0] = col.r;
				px[1] = col.g;
				px[2] = col.b;
				px[3] = col.a;
			}
		}
	}

private:
	static int imax(int a, int b) { return (a > b) ? a : b; }
	static int imin(int a, int b) { return (a < b) ? a : b; }

	static int font_px(int sz, double scale)
	{
		return imax(1, scaled(sz, scale));
	}

	// Axis width, grid width and tick length in physical pixels
	static void line_sizes(const jvxVideoOverlayConfig& cfg, int& aw, int& gw, int& tl)
	{
		aw = (cfg.axis_width > 0) ? imax(1, scaled(cfg.axis_width, cfg.scale)) : 0;
		gw = (cfg.grid_width > 0) ? imax(1, scaled(cfg.grid_width, cfg.scale)) : 0;
		tl = scaled(cfg.tick_length, cfg.scale);
	}
};

#endif
