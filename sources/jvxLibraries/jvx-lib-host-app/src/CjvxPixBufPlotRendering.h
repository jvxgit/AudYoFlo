#ifndef __CJVXPIXBUFPLOTRENDERING_H__
#define __CJVXPIXBUFPLOTRENDERING_H__

#include "jvx.h"
#include "typedefs/TpjvxPixBuffer.h"
#include "glplot/CjvxGlPlotRenderer.h"

#include <map>
#include <string>

/*
 * Pixbuf renderer for plots (mode=plot_inc / mode=plot_full, see TpjvxPixBuffer.h): the counterpart of
 * CjvxMyGlWidget in the Qt hosts.
 *
 * - The texture exists while the plot widget is visible.
 * - The external 2D buffer is allocated and installed in the rendering target property when processing
 *   starts - or, if processing is running already, when the texture is created. It is removed when
 *   processing stops or when the texture is released. The component delivers pixels only while the
 *   buffer is installed.
 * - A buffer removed while processing is running is released with a delay, the processing thread may
 *   still be writing into it.
 * - If the texture is released while processing is running (plot widget no longer visible), the
 *   connection is "parked": the buffer remains installed and the renderer keeps consuming it, it only
 *   stops publishing images. A texture created later for the same rendering target takes over the
 *   connection including the plot history. Parked connections are removed when processing stops.
 * - The buffer is rendered by CjvxGlPlotRenderer (jvxVideoRenderCore in an offscreen OpenGL context),
 *   the images are passed to the texture via the publish callback of the pixbuf extension.
 */
class CjvxPixBufPlotRendering : public IjvxGlPlotSink
{
public:
	int64_t texture_id = 0;
	jvxComponentIdentification cpId;
	std::string installHint;
	std::string propInstall;
	std::map<std::string, std::string> propArgs;
	jvx::externalBufferSubType mode = jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC;

	// Texture side
	uint32_t width = 0;
	uint32_t height = 0;
	double scale = 1.0;
	PixelBufferTexturePluginPublishFrameCallback publish_cb = nullptr;
	CjvxGlPlotRenderer* renderer = nullptr;

	// Rendering target
	jvxExternalBuffer* buf = nullptr;
	jvxBool bufValid = false;
	jvxBool installed = false;

	// No texture attached, see above
	jvxBool parked = false;

	// Start / stop the renderer of the texture
	jvxErrorType start_renderer();
	void stop_renderer();

	const std::string* arg(const char* key);

	void publish_frame(const uint8_t* rgba, uint32_t width, uint32_t height) override;
};

#endif
