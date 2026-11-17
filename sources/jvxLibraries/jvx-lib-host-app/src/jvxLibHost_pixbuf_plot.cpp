#include "jvxLibHost.h"

#include <mutex>
#include <iostream>
#include <chrono>

/*
 * Plot renderers of the pixbuf connection (mode=plot_inc / mode=plot_full), see TpjvxPixBuffer.h and
 * CjvxPixBufPlotRendering.h.
 */

#ifdef JVX_PIXBUF_WITH_GLPLOT

#include "CjvxPixBufPlotRendering.h"

// Texture callbacks are invoked from the UI thread, the sequencer functions from the thread that runs
// the backend calls. Both may be different, hence, protect the list of plot renderers.
static std::recursive_mutex pixbufPlotSafeAccess;

static int64_t
pixbuf_plot_now_ms()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count();
}

// ==========================================================================================

const std::string*
CjvxPixBufPlotRendering::arg(const char* key)
{
	auto elm = propArgs.find(key);
	if (elm != propArgs.end())
	{
		return &elm->second;
	}
	return nullptr;
}

jvxErrorType
CjvxPixBufPlotRendering::start_renderer()
{
	std::string err;
	jvxSize periodMs = 30;
	const std::string* periodStr = arg("period");
	if (periodStr)
	{
		periodMs = JVX_MAX(atoi(periodStr->c_str()), 1);
	}

	renderer = new CjvxGlPlotRenderer(texture_id, width, height, scale, periodMs, this);
	if (!renderer->start(err))
	{
		std::cout << "[pixbuf plot] Failed to start OpenGL renderer for texture <" << texture_id << ">: " << err << std::endl;
		delete renderer;
		renderer = nullptr;
		return JVX_ERROR_INTERNAL;
	}
	if (buf)
	{
		renderer->set_external_buffer(buf);
	}
	return JVX_NO_ERROR;
}

void
CjvxPixBufPlotRendering::stop_renderer()
{
	if (renderer)
	{
		// Removes the trigger callback from the buffer (2D full mode)
		renderer->set_external_buffer(nullptr);
		renderer->stop();
		delete renderer;
		renderer = nullptr;
	}
}

void
CjvxPixBufPlotRendering::publish_frame(const uint8_t* rgba, uint32_t w, uint32_t h)
{
	// Render thread
	if (publish_cb)
	{
		publish_cb(texture_id, rgba, w, h);
	}
}

// ==========================================================================================

static jvxErrorType
pixbuf_plot_read_size(IjvxProperties* props, const std::string& propName, jvxSize& val)
{
	jvxCallManagerProperties callGate;
	jPAD ident(propName.c_str());
	jPD trans;
	trans.reset(true, 0);
	return props->get_property(callGate, jPRIO<jvxSize>(val), ident, trans);
}

static jvxErrorType
pixbuf_plot_read_int16(IjvxProperties* props, const std::string& propName, jvxInt16& val)
{
	jvxCallManagerProperties callGate;
	jPAD ident(propName.c_str());
	jPD trans;
	trans.reset(true, 0);
	return props->get_property(callGate, jPRG(&val, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
}

static jvxErrorType
pixbuf_plot_read_selection(IjvxProperties* props, const std::string& propName, jvxSelectionList& val)
{
	jvxCallManagerProperties callGate;
	jPAD ident(propName.c_str());
	jPD trans;
	trans.reset(true, 0);
	return props->get_property(callGate, jPRG(&val, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
}

// ==========================================================================================

void
jvxLibHost::pixbuffer_plot_create(int64_t texture_id, uint32_t width, uint32_t height,
	const char* id, const char* ctxt, const std::string& propInstall, const std::map<std::string, std::string>& propArgs)
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	if (extPixBuffer.plots.find(texture_id) != extPixBuffer.plots.end())
	{
		std::cout << __FUNCTION__ << ", error: texture id <" << texture_id << "> already in use." << std::endl;
		return;
	}
	if (extPixBuffer.publish_cb == nullptr)
	{
		std::cout << __FUNCTION__ << ", error: plot rendering requires the publish function of the pixbuf extension, which the "
			"texture plugin does not provide." << std::endl;
		return;
	}

	CjvxPixBufPlotRendering* conn = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(conn, CjvxPixBufPlotRendering);
	conn->texture_id = texture_id;
	conn->width = width;
	conn->height = height;
	conn->publish_cb = extPixBuffer.publish_cb;
	conn->installHint = (ctxt ? ctxt : "");
	conn->propInstall = propInstall;
	conn->propArgs = propArgs;

	const std::string* modeStr = conn->arg("mode");
	if (modeStr && (*modeStr == "plot_full"))
	{
		conn->mode = jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL;
	}

	if (conn->propInstall.empty())
	{
		std::cout << __FUNCTION__ << ", error: plot rendering requires at least an <install target property>." << std::endl;
	}
	if (jvxComponentIdentification_decode(conn->cpId, (id ? id : "")) != JVX_NO_ERROR)
	{
		std::cout << __FUNCTION__ << ", error: plot rendering requires a valid <install target component>, which <" <<
			(id ? id : "") << "> is not!" << std::endl;
	}

	// The plot widget was shown before during this processing run: take over the parked connection -
	// buffer and renderer have continued in the background, the plot history is preserved.
	for (auto elm = extPixBuffer.plots.begin(); elm != extPixBuffer.plots.end(); elm++)
	{
		CjvxPixBufPlotRendering* old = elm->second;
		if (old->parked && (old->renderer != nullptr) &&
			(old->cpId == conn->cpId) && (old->propInstall == conn->propInstall) && (old->mode == conn->mode))
		{
			extPixBuffer.plots.erase(elm);

			// Publishing is disabled, hence, the sink may be modified
			old->texture_id = texture_id;
			old->width = width;
			old->height = height;
			old->publish_cb = conn->publish_cb;
			old->propArgs = conn->propArgs;
			old->parked = false;
			extPixBuffer.plots[texture_id] = old;

			old->renderer->resize(old->width, old->height, old->scale);
			old->renderer->set_publishing(true);

			JVX_SAFE_DELETE_OBJECT(conn);
			pixbuffer_plot_release_buffers(false);
			return;
		}
	}

	// Renders the empty plot area until the buffer is installed
	conn->start_renderer();
	extPixBuffer.plots[texture_id] = conn;

	// The texture is created when the plot widget becomes visible. If processing is running already,
	// install the rendering target right now - the component delivers pixels as soon as the buffer
	// is installed. Otherwise, the buffer is installed with the next start.
	if (extPixBuffer.processingActive)
	{
		pixbuffer_plot_install(conn);
	}
	pixbuffer_plot_release_buffers(false);
}

jvxBool
jvxLibHost::pixbuffer_plot_destroy(int64_t texture_id)
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	auto elm = extPixBuffer.plots.find(texture_id);
	if (elm == extPixBuffer.plots.end())
	{
		return false;
	}

	// No more images for this texture: the plot widget is no longer visible.
	CjvxPixBufPlotRendering* conn = elm->second;
	if (conn->parked)
	{
		// Texture already released
		return true;
	}

	if (extPixBuffer.processingActive && conn->installed && conn->renderer)
	{
		// Processing is running: park the connection. Buffer and renderer continue in the background,
		// such that the plot history is still there when the widget is shown again.
		conn->renderer->set_publishing(false);
		conn->publish_cb = nullptr;
		conn->parked = true;
		return true;
	}

	// Remove the rendering target right away
	conn->stop_renderer();
	conn->publish_cb = nullptr;
	pixbuffer_plot_uninstall(conn);
	extPixBuffer.plots.erase(elm);
	JVX_SAFE_DELETE_OBJECT(conn);

	pixbuffer_plot_release_buffers(false);
	return true;
}

void
jvxLibHost::pixbuffer_local_configure_cb(int64_t texture_id, const char* key, const char* value, void* priv_data)
{
	jvxLibHost* hostRef = reinterpret_cast<jvxLibHost*>(priv_data);
	if ((hostRef == nullptr) || (key == nullptr))
	{
		return;
	}
	std::string val = (value ? value : "");

	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	auto elm = hostRef->extPixBuffer.plots.find(texture_id);
	if ((elm == hostRef->extPixBuffer.plots.end()) || (elm->second->renderer == nullptr))
	{
		// Copy renderers have no runtime configuration
		return;
	}
	CjvxPixBufPlotRendering* conn = elm->second;

	if (strcmp(key, "size") == 0)
	{
		// <width>:<height>:<scale>
		std::vector<std::string> tokens;
		jvx::helper::parseStringListIntoTokens(val, tokens, ':');
		if (tokens.size() >= 2)
		{
			int w = atoi(tokens[0].c_str());
			int h = atoi(tokens[1].c_str());
			double s = (tokens.size() >= 3) ? atof(tokens[2].c_str()) : 1.0;
			if ((w > 0) && (h > 0) && (s > 0))
			{
				conn->width = (uint32_t)w;
				conn->height = (uint32_t)h;
				conn->scale = s;
				conn->renderer->resize(conn->width, conn->height, conn->scale);
			}
		}
	}
	else if (strcmp(key, "config") == 0)
	{
		CjvxGlPlotOverlay ovl;
		CjvxGlPlotRenderer::decode_overlay(val, ovl);
		conn->renderer->set_overlay(ovl);
	}
}

// ==========================================================================================

jvxErrorType
jvxLibHost::pixbuffer_plot_install(CjvxPixBufPlotRendering* conn)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize seg_x = 0;
	jvxSize seg_y = 0;
	jvxInt16 form = JVX_DATAFORMAT_BYTE;
	jvxInt16 subform = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
	jvxSize numBuffers = 1;
	jvxSize szBuf = 0;
	const std::string* propName = nullptr;

	if (conn->installed || conn->propInstall.empty())
	{
		return JVX_NO_ERROR;
	}

	IjvxObject* theObj = nullptr;
	involvedHost.hHost->request_object_selected_component(conn->cpId, &theObj);
	if (theObj == nullptr)
	{
		std::cout << __FUNCTION__ << ", error: component <" << jvxComponentIdentification_txt(conn->cpId) <<
			"> to install plot buffer is not selected." << std::endl;
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	IjvxProperties* props = reqInterfaceObj<IjvxProperties>(theObj);
	if (props == nullptr)
	{
		involvedHost.hHost->return_object_selected_component(conn->cpId, theObj);
		return JVX_ERROR_UNSUPPORTED;
	}

	// Same sequence of property reads as in CjvxMyGlWidget::inform_about_to_start
	propName = conn->arg("sx");
	if (propName)
	{
		if (pixbuf_plot_read_size(props, *propName, seg_x) != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << ", error: could not read property <" << *propName << ">." << std::endl;
		}
	}

	propName = conn->arg("sy");
	if (propName)
	{
		if (pixbuf_plot_read_size(props, *propName, seg_y) != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << ", error: could not read property <" << *propName << ">." << std::endl;
		}
	}

	propName = conn->arg("f");
	if (propName)
	{
		if (pixbuf_plot_read_int16(props, *propName, form) != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << ", error: could not read property <" << *propName << ">." << std::endl;
		}
	}

	propName = conn->arg("fg");
	if (propName)
	{
		if (pixbuf_plot_read_int16(props, *propName, subform) != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << ", error: could not read property <" << *propName << ">." << std::endl;
		}
	}

	// The operation mode property tells which buffer types the component supports:
	// bit 0: 2D full (triggered multibuffer), bit 1: 2D incremental
	propName = conn->arg("om");
	if (propName)
	{
		jvxSelectionList selLst;
		if (pixbuf_plot_read_selection(props, *propName, selLst) == JVX_NO_ERROR)
		{
			jvxSize bitCheck = (conn->mode == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL) ? 0 : 1;
			if (!jvx_bitTest(selLst.bitFieldSelected(), bitCheck))
			{
				std::cout << __FUNCTION__ << ", error: component <" << jvxComponentIdentification_txt(conn->cpId) <<
					"> does not support the requested buffer operation mode." << std::endl;
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
	}

	if (conn->mode == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL)
	{
		propName = conn->arg("nb");
		if (propName)
		{
			if (pixbuf_plot_read_size(props, *propName, numBuffers) != JVX_NO_ERROR)
			{
				std::cout << __FUNCTION__ << ", error: could not read property <" << *propName << ">." << std::endl;
			}
		}
		numBuffers = JVX_MAX(numBuffers, 1);
	}

	if ((seg_x == 0) || (seg_y == 0))
	{
		std::cout << __FUNCTION__ << ", error: invalid segment size <" << seg_x << "x" << seg_y << "> for plot buffer." << std::endl;
		res = JVX_ERROR_INVALID_SETTING;
	}

	if (res == JVX_NO_ERROR)
	{
		jvxSize fsize = seg_x * seg_y;
		switch (conn->mode)
		{
		case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL:
			conn->buf = jvx_allocate2DFieldExternalBuffer_full(fsize, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform,
				jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
				&szBuf, numBuffers, seg_x, seg_y);
			break;
		default:
			conn->buf = jvx_allocate2DFieldExternalBuffer_inc(fsize, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform,
				jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
				&szBuf, 1, seg_x, seg_y);
			break;
		}

		if (conn->buf)
		{
			conn->buf->associationHint = conn->installHint.c_str();
			conn->bufValid = true;

			// Connect the renderer BEFORE the buffer is installed: if processing is running, the component
			// starts writing (and, in 2D full mode, triggering) right after the installation.
			if (conn->renderer)
			{
				conn->renderer->set_external_buffer(conn->buf);
			}

			jvxCallManagerProperties callGate;
			jPAD ident(conn->propInstall.c_str());
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(conn->buf, &conn->bufValid,
				conn->buf->formFld);
			res = props->install_property_reference(callGate, ptrRaw, ident);
			if (res == JVX_NO_ERROR)
			{
				conn->installed = true;
			}
			else
			{
				std::cout << __FUNCTION__ << ", error: failed to install plot buffer in property <" << conn->propInstall <<
					">, reason: " << jvxErrorType_txt(res) << "." << std::endl;
				if (conn->renderer)
				{
					conn->renderer->set_external_buffer(nullptr);
				}
				jvx_deallocateExternalBuffer(conn->buf);
				conn->buf = nullptr;
				conn->bufValid = false;
			}
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}

	retInterfaceObj<IjvxProperties>(theObj, props);
	involvedHost.hHost->return_object_selected_component(conn->cpId, theObj);
	return res;
}

void
jvxLibHost::pixbuffer_plot_uninstall(CjvxPixBufPlotRendering* conn)
{
	if (!conn->installed)
	{
		return;
	}

	if (conn->renderer)
	{
		// The renderer does not access the buffer anymore when this call returns
		conn->renderer->set_external_buffer(nullptr);
	}

	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	IjvxObject* theObj = nullptr;
	involvedHost.hHost->request_object_selected_component(conn->cpId, &theObj);
	if (theObj)
	{
		IjvxProperties* props = reqInterfaceObj<IjvxProperties>(theObj);
		if (props)
		{
			jvxCallManagerProperties callGate;
			jPAD ident(conn->propInstall.c_str());
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(conn->buf, &conn->bufValid,
				conn->buf->formFld);
			res = props->uninstall_property_reference(callGate, ptrRaw, ident);
			retInterfaceObj<IjvxProperties>(theObj, props);
		}
		involvedHost.hHost->return_object_selected_component(conn->cpId, theObj);
	}

	if (res == JVX_NO_ERROR)
	{
		if (extPixBuffer.processingRunning)
		{
			// The processing thread may be writing into the buffer right now: release it later
			extPixBuffer.plotBuffersRelease.push_back(std::make_pair(conn->buf, pixbuf_plot_now_ms()));
		}
		else
		{
			jvx_deallocateExternalBuffer(conn->buf);
		}
	}
	else
	{
		// Better to loose some memory than to release a buffer that might still be in use
		std::cout << __FUNCTION__ << ", error: failed to uninstall plot buffer from property <" << conn->propInstall <<
			">, reason: " << jvxErrorType_txt(res) << ". Buffer will not be released." << std::endl;
	}
	conn->buf = nullptr;
	conn->bufValid = false;
	conn->installed = false;
}

// ==========================================================================================

// Start of processing: install the rendering targets of all existing plot textures. The flag
// processingActive is set here - under the lock - such that a texture created from now on installs
// its rendering target itself (see pixbuffer_plot_create).
void
jvxLibHost::pixbuffer_install_all()
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	extPixBuffer.processingActive = true;
	extPixBuffer.processingRunning = true;
	for (auto& elm : extPixBuffer.plots)
	{
		pixbuffer_plot_install(elm.second);
	}
}

// Stop of processing is about to be triggered: remove all rendering targets BEFORE the stop. The
// processing thread is still running until the stop is acknowledged, hence, the buffers are released
// only then (see pixbuffer_uninstall_all). No textures install their rendering target from now on.
void
jvxLibHost::pixbuffer_uninstall_on_stop()
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	extPixBuffer.processingActive = false;
	for (auto& elm : extPixBuffer.plots)
	{
		pixbuffer_plot_uninstall(elm.second);
	}
	pixbuffer_plot_remove_parked();
}

// Parked connections (no texture attached) are only kept while processing is running
void
jvxLibHost::pixbuffer_plot_remove_parked()
{
	auto elm = extPixBuffer.plots.begin();
	while (elm != extPixBuffer.plots.end())
	{
		CjvxPixBufPlotRendering* conn = elm->second;
		if (conn->parked)
		{
			conn->stop_renderer();
			pixbuffer_plot_uninstall(conn);
			elm = extPixBuffer.plots.erase(elm);
			JVX_SAFE_DELETE_OBJECT(conn);
		}
		else
		{
			elm++;
		}
	}
}

// Stop of processing acknowledged (or failed start): remove all rendering targets still installed and
// release all buffers - the processing thread no longer accesses them.
void
jvxLibHost::pixbuffer_uninstall_all()
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	extPixBuffer.processingActive = false;
	extPixBuffer.processingRunning = false;
	for (auto& elm : extPixBuffer.plots)
	{
		pixbuffer_plot_uninstall(elm.second);
	}
	pixbuffer_plot_remove_parked();
	pixbuffer_plot_release_buffers(true);
}

void
jvxLibHost::pixbuffer_plot_release_all()
{
	std::lock_guard<std::recursive_mutex> lock(pixbufPlotSafeAccess);
	extPixBuffer.processingActive = false;
	extPixBuffer.processingRunning = false;
	for (auto& elm : extPixBuffer.plots)
	{
		elm.second->stop_renderer();
		pixbuffer_plot_uninstall(elm.second);
		JVX_SAFE_DELETE_OBJECT(elm.second);
	}
	extPixBuffer.plots.clear();
	pixbuffer_plot_release_buffers(true);
}

// Release plot buffers uninstalled while processing was running. A buffer is released once processing
// has stopped (all = true) or if it was uninstalled long enough ago - the processing thread reads the
// buffer reference once per frame, hence, it no longer accesses a buffer uninstalled a while ago.
void
jvxLibHost::pixbuffer_plot_release_buffers(jvxBool all)
{
	const int64_t delayMs = 1000;
	int64_t now = pixbuf_plot_now_ms();
	auto elm = extPixBuffer.plotBuffersRelease.begin();
	while (elm != extPixBuffer.plotBuffersRelease.end())
	{
		if (all || ((now - elm->second) > delayMs))
		{
			jvx_deallocateExternalBuffer(elm->first);
			elm = extPixBuffer.plotBuffersRelease.erase(elm);
		}
		else
		{
			elm++;
		}
	}
}

#else

// No offscreen OpenGL context on this platform (yet)

void
jvxLibHost::pixbuffer_plot_create(int64_t texture_id, uint32_t, uint32_t,
	const char*, const char*, const std::string&, const std::map<std::string, std::string>&)
{
	std::cout << __FUNCTION__ << ", error: plot rendering for texture <" << texture_id << "> is not supported on this platform." << std::endl;
}

jvxBool
jvxLibHost::pixbuffer_plot_destroy(int64_t)
{
	return false;
}

void
jvxLibHost::pixbuffer_local_configure_cb(int64_t, const char*, const char*, void*)
{
}

void
jvxLibHost::pixbuffer_install_all()
{
}

void
jvxLibHost::pixbuffer_uninstall_all()
{
}

void
jvxLibHost::pixbuffer_uninstall_on_stop()
{
}

void
jvxLibHost::pixbuffer_plot_remove_parked()
{
}

void
jvxLibHost::pixbuffer_plot_release_all()
{
}

void
jvxLibHost::pixbuffer_plot_release_buffers(jvxBool)
{
}

#endif
