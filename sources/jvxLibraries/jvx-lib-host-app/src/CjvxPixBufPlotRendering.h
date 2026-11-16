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
 * - The external 2D buffer is allocated and installed in the rendering target property when processing
 *   starts (CjvxMyGlWidget::inform_about_to_start) and removed when processing has stopped
 *   (CjvxMyGlWidget::inform_stopped) - the rendering target properties are typically only writable
 *   while processing is stopped.
 * - The buffer is rendered by CjvxGlPlotRenderer (jvxVideoRenderCore in an offscreen OpenGL context),
 *   the images are passed to the texture via the publish callback of the pixbuf extension.
 * - If the texture is released while processing is running, the buffer remains installed ("detached")
 *   until processing stops. A texture created for the same rendering target in the meantime takes
 *   over the buffer.
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
	jvxBool detached = false;

	// Start / stop the renderer of the texture
	jvxErrorType start_renderer();
	void stop_renderer();

	const std::string* arg(const char* key);

	void publish_frame(const uint8_t* rgba, uint32_t width, uint32_t height) override;
};

#endif
