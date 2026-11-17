#ifndef __CJVXGLPLOTRENDERER_H__
#define __CJVXGLPLOTRENDERER_H__

#include <cstdint>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>

// jvx.h must be included before any other header that pulls in windows.h (winsock vs. winsock2).
#include "jvx.h"

#include "jvxGlFunctions.h"
#include "IjvxGlContext.h"

// Subset of jvxVideoOverlayConfig - used to pass the configuration from the UI to the render thread
// without pulling in the render core headers.
struct CjvxGlPlotOverlayLegend
{
	uint8_t r = 255, g = 255, b = 255, a = 255;
	std::string text;
};

struct CjvxGlPlotOverlay
{
	bool active = true;
	int num_div_x = 8;
	int num_div_y = 4;
	bool show_grid = true;
	std::list<std::string> labels_x;
	std::list<std::string> labels_y;
	std::string axis_label_x;
	std::string axis_label_y;
	std::string title;
	int font_size = 11;
	int font_size_title = 13;
	std::list<CjvxGlPlotOverlayLegend> legend;
	int legend_pos = 2; // jvxRenderLegendPosition, JVX_RENDER_LEGEND_RIGHT_TOP
};

// Output of the renderer: an RGBA image, top row first
class IjvxGlPlotSink
{
public:
	virtual ~IjvxGlPlotSink() {};
	virtual void publish_frame(const uint8_t* rgba, uint32_t width, uint32_t height) = 0;
};

class CjvxGlPlotRendererCore;

/*
 * Counterpart of CjvxMyGlWidget (jvxLQtSaWidgets) without Qt:
 * - owns an offscreen OpenGL context in a dedicated render thread
 * - renders the content of a jvxExternalBuffer (2D incremental or 2D full) with jvxVideoRenderCore,
 *   including the overlay (frame, axes, grid, labels, title, legend), into a framebuffer object
 * - reads back the image and passes it to the sink (e.g. a Flutter pixel buffer texture)
 *
 * All public functions are thread safe.
 */
class CjvxGlPlotRenderer
{
public:
	CjvxGlPlotRenderer(int64_t texture_id, uint32_t width, uint32_t height, double scale,
		jvxSize period_ms, IjvxGlPlotSink* sink);
	~CjvxGlPlotRenderer();

	bool start(std::string& errDescr);
	void stop();

	void resize(uint32_t width, uint32_t height, double scale);
	void set_overlay(const CjvxGlPlotOverlay& ovl);

	// Publishing of images to the sink. If disabled, the renderer keeps consuming the buffer and
	// updating its texture (no history is lost) but does not read back and publish images. When this
	// function returns, no image is being published, hence, the sink may be modified. Enabling
	// publishes an image right away.
	void set_publishing(bool on);

	// Decode the overlay description as passed by the UI, see TpjvxPixBuffer.h (key "config")
	static void decode_overlay(const std::string& txt, CjvxGlPlotOverlay& ovl);

	// Backend callback: buffer installed (buf != nullptr) or about to be removed (nullptr).
	// When this function returns with nullptr, the buffer is no longer accessed.
	void set_external_buffer(jvxExternalBuffer* buf);

	int64_t texture_id() const { return texId; };

	// Trigger callback of the external buffer in 2D full mode
	static jvxErrorType static_report_bufferswitch_trigger(jvxHandle* priv);

private:
	void render_loop();
	void wake();

	int64_t texId = 0;
	IjvxGlPlotSink* sink = nullptr;
	jvxSize periodMs = 30;

	std::thread renderThread;
	std::mutex safeAccess;
	std::condition_variable cond;
	bool stopRequest = false;
	bool triggered = false;

	// Startup handshake
	bool startupDone = false;
	bool startupSuccess = false;
	std::string startupErr;

	// State shared with the render thread, protected by safeAccess
	struct
	{
		uint32_t width = 0;
		uint32_t height = 0;
		double scale = 1.0;
		bool sizeChanged = true;

		CjvxGlPlotOverlay overlay;
		bool overlayChanged = true;

		jvxExternalBuffer* buf = nullptr;
		bool bufChanged = false;

		bool publish = true;
		bool forceRepaint = false;
	} shared;

	// Only accessed in the render thread
	CjvxGlPlotRendererCore* core = nullptr;
};

#endif
