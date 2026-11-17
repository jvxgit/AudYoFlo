#include "CjvxGlPlotRenderer.h"

#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>

// Use the render core with our own OpenGL function table as base class - the very same render
// functionality as in CjvxMyGlWidget, where QOpenGLFunctions is the base class.
#define JVX_VIDEO_RENDER_CORE_CLASS CjvxGlFunctions
#include "jvxVideoRenderCore.h"

// =====================================================================================
// Render core: jvxVideoRenderCore_bc plus offscreen framebuffer and the paint logic of
// CjvxMyGlWidget::paintGL. Only used in the render thread.
// =====================================================================================

class CjvxGlPlotRendererCore : public jvxVideoRenderCore_bc
{
public:
	IjvxGlContext* ctx = nullptr;

	// Offscreen target
	GLuint fbo = 0;
	GLuint fboTex = 0;
	uint32_t fboWidth = 0;
	uint32_t fboHeight = 0;
	std::vector<uint8_t> readBuf;
	std::vector<uint8_t> outBuf;

	// External buffer and render core status (CjvxMyGlWidget: external_link, gl, status_gl)
	jvxExternalBuffer* buf = nullptr;
	bool coreInitialized = false;
	jvxHandle*** bufs = nullptr;
	jvxSize szFld = 0;
	cfgRenderStraight straightPaint;
	jvxData lastOffsetX = 0;
	jvxSize lastIdxFull = 0;
	bool frameShownFull = false;

	CjvxGlPlotRendererCore()
	{
		straightPaint.invert_y = c_true;

		jvxVideoOverlayConfig cfgOvl;
		cfgOvl.active = true;
		set_overlay_config(cfgOvl);
	};

	// ===================================================================
	// jvxVideoRenderCore_bc interface - the program is created by the render core itself
	// ===================================================================
	jvxErrorType system_initialize(const char*, void (*)(), void (*)()) override { return JVX_NO_ERROR; };
	jvxErrorType system_passcontrol() override { return JVX_NO_ERROR; };

	jvxErrorType create_programm_assign(
		const std::string&, const std::string&, jvxOpenGlRendering,
		GLint*, GLint*, GLint*, GLint*, GLint*, GLint*) override
	{
		// Let the render core compile and link the shaders with the native OpenGL functions
		return JVX_ERROR_UNSUPPORTED;
	};
	jvxErrorType release_programm_unassign() override { return JVX_NO_ERROR; };
	GLint uniform_location_program(const char*) override { return -1; };
	jvxErrorType start_rendering_program() override { return JVX_NO_ERROR; };
	jvxErrorType stop_rendering_program() override { return JVX_NO_ERROR; };

	// ===================================================================

	bool ensure_framebuffer(uint32_t width, uint32_t height)
	{
		if ((fbo != 0) && (width == fboWidth) && (height == fboHeight))
		{
			return true;
		}
		release_framebuffer();
		if ((width == 0) || (height == 0))
		{
			return false;
		}

		glGenTextures(1, &fboTex);
		glBindTexture(GL_TEXTURE_2D, fboTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "[pixbuf plot] Framebuffer incomplete, status = " << status << std::endl;
			release_framebuffer();
			return false;
		}

		fboWidth = width;
		fboHeight = height;
		readBuf.resize((size_t)width * height * 4);
		outBuf.resize((size_t)width * height * 4);
		return true;
	};

	void release_framebuffer()
	{
		if (fbo)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &fbo);
			fbo = 0;
		}
		if (fboTex)
		{
			glDeleteTextures(1, &fboTex);
			fboTex = 0;
		}
		fboWidth = 0;
		fboHeight = 0;
	};

	// ===================================================================
	// CjvxMyGlWidget::reinitGL / deinitGL
	// ===================================================================
	void reinit()
	{
		if (buf == nullptr)
		{
			return;
		}

		jvxOpenGlRendering renderOperation = JVX_GL_RENDER_STRAIGHT;
		jvx::multichannel2DFieldBufferHeader_common* common = &buf->specific.the2DFieldBuffer_full.common;
		if (buf->subTp == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC)
		{
			common = &buf->specific.the2DFieldBuffer_inc.common;
			renderOperation = JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW;
		}

		bufs = nullptr;
		szFld = 0;
		jvxErrorType res = initialize((int)common->seg_x, (int)common->seg_y, (jvxByte****)(&bufs),
			&szFld, 1, common->number_buffers, buf->formFld, buf->subFormFld,
			renderOperation, nullptr, "Init GL pixbuf plot", buf->ptrFld, buf->szFld);
		if (res == JVX_NO_ERROR)
		{
			coreInitialized = true;
		}
		else
		{
			std::cout << "[pixbuf plot] Failed to initialize render core, error code " << (int)res << "." << std::endl;
			terminate();
		}
		lastOffsetX = 0;
		lastIdxFull = 0;
		frameShownFull = false;
	};

	void deinit()
	{
		if (coreInitialized)
		{
			terminate();
			coreInitialized = false;
		}
		bufs = nullptr;
		szFld = 0;
	};

	// ===================================================================
	// CjvxMyGlWidget::paintGL. Returns true if the framebuffer content was updated.
	// ===================================================================
	bool paint(bool force)
	{
		if (!coreInitialized || (buf == nullptr))
		{
			if (force)
			{
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				return true;
			}
			return false;
		}

		switch (buf->subTp)
		{
		case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL:
			if (buf->fill_height > 0)
			{
				lastIdxFull = buf->idx_read;
				prepare_render((const jvxByte**)bufs[lastIdxFull], &straightPaint);
				frameShownFull = true;

				buf->safe_access.lockf(buf->safe_access.priv);
				buf->idx_read = (buf->idx_read + 1) % buf->specific.the2DFieldBuffer_full.common.number_buffers;
				buf->fill_height--;
				buf->safe_access.unlockf(buf->safe_access.priv);

				if (buf->specific.the2DFieldBuffer_full.report_completef)
				{
					buf->specific.the2DFieldBuffer_full.report_completef(buf->specific.the2DFieldBuffer_full.report_complete_priv);
				}
				return true;
			}
			if (force)
			{
				// Size or overlay changed: repaint the last frame - or the empty plot area
				prepare_render((const jvxByte**)bufs[lastIdxFull], &straightPaint);
				return true;
			}
			return false;

		case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC:
		{
			jvxSize fH = 0;
			jvxSize iR = 0;
			jvxSize iW = 0;
			jvxSize ll1 = 0;
			jvxSize ll2 = 0;
			jvxSize segY = buf->specific.the2DFieldBuffer_inc.common.seg_y;
			cfgRotateAngleOffsetFollow rotatePaint;
			rotatePaint.border_x = 0.05;
			rotatePaint.border_y = 0.1;
			rotatePaint.offset_x = lastOffsetX;
			rotatePaint.update_offset_y = 0;
			rotatePaint.update_num_y = 0;

			buf->safe_access.lockf(buf->safe_access.priv);
			fH = buf->fill_height;
			iR = buf->idx_read;
			buf->safe_access.unlockf(buf->safe_access.priv);

			if (fH > 0)
			{
				// New lines: upload in up to two parts (wrap around of the circular buffer)
				ll1 = JVX_MIN(fH, segY - iR);
				ll2 = fH - ll1;
				iW = iR;
				if (ll1)
				{
					iW += ll1;
					rotatePaint.offset_x = (jvxData)iW / (jvxData)segY;
					rotatePaint.update_offset_y = iR;
					rotatePaint.update_num_y = ll1;
					prepare_render((const jvxByte**)bufs[0], &rotatePaint);
					iR += ll1;
				}
				if (ll2)
				{
					iR = 0;
					iW += ll2;
					rotatePaint.offset_x = (jvxData)iW / (jvxData)segY;
					rotatePaint.update_offset_y = iR;
					rotatePaint.update_num_y = ll2;
					prepare_render((const jvxByte**)bufs[0], &rotatePaint);
				}
				lastOffsetX = rotatePaint.offset_x;

				buf->safe_access.lockf(buf->safe_access.priv);
				buf->fill_height -= (ll1 + ll2);
				buf->idx_read = (buf->idx_read + ll1 + ll2) % segY;
				buf->safe_access.unlockf(buf->safe_access.priv);
				return true;
			}
			if (force)
			{
				// Redraw texture content as is (no upload of new lines)
				prepare_render((const jvxByte**)bufs[0], &rotatePaint);
				return true;
			}
			return false;
		}
		default:
			break;
		}
		return false;
	};

	// Read back the framebuffer, OpenGL delivers the bottom row first
	const uint8_t* read_back()
	{
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, (GLsizei)fboWidth, (GLsizei)fboHeight, GL_RGBA, GL_UNSIGNED_BYTE, readBuf.data());
		size_t rowSz = (size_t)fboWidth * 4;
		for (uint32_t y = 0; y < fboHeight; y++)
		{
			memcpy(outBuf.data() + (size_t)y * rowSz, readBuf.data() + (size_t)(fboHeight - 1 - y) * rowSz, rowSz);
		}
		return outBuf.data();
	};

	void apply_overlay(const CjvxGlPlotOverlay& src, double scale)
	{
		jvxVideoOverlayConfig cfg = overlay_config();
		cfg.active = src.active;
		cfg.num_div_x = src.num_div_x;
		cfg.num_div_y = src.num_div_y;
		cfg.show_grid = src.show_grid;
		cfg.labels_x = src.labels_x;
		cfg.labels_y = src.labels_y;
		cfg.axis_label_x = src.axis_label_x;
		cfg.axis_label_y = src.axis_label_y;
		cfg.title = src.title;
		cfg.font_size = src.font_size;
		cfg.font_size_title = src.font_size_title;
		cfg.legend.clear();
		for (const auto& elm : src.legend)
		{
			jvxVideoOverlayLegendEntry entry;
			entry.color.r = elm.r;
			entry.color.g = elm.g;
			entry.color.b = elm.b;
			entry.color.a = elm.a;
			entry.text = elm.text;
			cfg.legend.push_back(entry);
		}
		cfg.legend_pos = (jvxRenderLegendPosition)src.legend_pos;
		cfg.scale = scale;
		set_overlay_config(cfg);
	};
};

// =====================================================================================
// =====================================================================================

static void*
jvx_gl_texture_get_proc(const char* name, void* priv)
{
	IjvxGlContext* ctx = reinterpret_cast<IjvxGlContext*>(priv);
	return ctx->get_proc_address(name);
}

CjvxGlPlotRenderer::CjvxGlPlotRenderer(int64_t texture_id, uint32_t width, uint32_t height, double scale,
	jvxSize period_ms, IjvxGlPlotSink* sinkArg) :
	texId(texture_id), sink(sinkArg), periodMs(period_ms)
{
	shared.width = width;
	shared.height = height;
	shared.scale = scale;
	if (periodMs == 0)
	{
		periodMs = 30;
	}
}

CjvxGlPlotRenderer::~CjvxGlPlotRenderer()
{
	stop();
}

bool
CjvxGlPlotRenderer::start(std::string& errDescr)
{
	if (renderThread.joinable())
	{
		return true;
	}

	{
		std::lock_guard<std::mutex> lock(safeAccess);
		stopRequest = false;
		startupDone = false;
		startupSuccess = false;
		startupErr.clear();
	}

	renderThread = std::thread(&CjvxGlPlotRenderer::render_loop, this);

	// Wait until the context was created - we want to report errors immediately
	std::unique_lock<std::mutex> lock(safeAccess);
	cond.wait(lock, [this] { return startupDone; });
	if (!startupSuccess)
	{
		errDescr = startupErr;
		lock.unlock();
		renderThread.join();
		return false;
	}
	return true;
}

void
CjvxGlPlotRenderer::stop()
{
	if (renderThread.joinable())
	{
		{
			std::lock_guard<std::mutex> lock(safeAccess);
			stopRequest = true;
		}
		cond.notify_all();
		renderThread.join();
	}
}

void
CjvxGlPlotRenderer::resize(uint32_t width, uint32_t height, double scale)
{
	{
		std::lock_guard<std::mutex> lock(safeAccess);
		if ((width != shared.width) || (height != shared.height) || (scale != shared.scale))
		{
			shared.width = width;
			shared.height = height;
			shared.scale = scale;
			shared.sizeChanged = true;
			shared.overlayChanged = true; // overlay scale
		}
	}
	wake();
}

void
CjvxGlPlotRenderer::set_publishing(bool on)
{
	{
		// The render thread holds this lock while it renders and publishes
		std::lock_guard<std::mutex> lock(safeAccess);
		if (on && !shared.publish)
		{
			shared.forceRepaint = true;
		}
		shared.publish = on;
	}
	wake();
}

void
CjvxGlPlotRenderer::set_overlay(const CjvxGlPlotOverlay& ovl)
{
	{
		std::lock_guard<std::mutex> lock(safeAccess);
		shared.overlay = ovl;
		shared.overlayChanged = true;
	}
	wake();
}

void
CjvxGlPlotRenderer::set_external_buffer(jvxExternalBuffer* buf)
{
	{
		// The render thread holds this lock while it accesses the buffer: once we have it, the
		// buffer is no longer in use.
		std::lock_guard<std::mutex> lock(safeAccess);
		if (shared.buf && (shared.buf->subTp == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL))
		{
			shared.buf->safe_access.lockf(shared.buf->safe_access.priv);
			shared.buf->specific.the2DFieldBuffer_full.report_triggerf = nullptr;
			shared.buf->specific.the2DFieldBuffer_full.report_trigger_priv = nullptr;
			shared.buf->safe_access.unlockf(shared.buf->safe_access.priv);
		}

		shared.buf = buf;
		shared.bufChanged = true;

		if (buf && (buf->subTp == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL))
		{
			// 2D full buffers are rendered once a new frame was reported, 2D incremental buffers periodically
			buf->specific.the2DFieldBuffer_full.report_triggerf = static_report_bufferswitch_trigger;
			buf->specific.the2DFieldBuffer_full.report_trigger_priv = reinterpret_cast<jvxHandle*>(this);
		}
	}
	wake();
}

static std::string
jvx_gl_plot_unescape(const std::string& in)
{
	std::string out;
	for (size_t i = 0; i < in.size(); i++)
	{
		if ((in[i] == '\\') && (i + 1 < in.size()))
		{
			i++;
			out += (in[i] == 'n') ? '\n' : in[i];
		}
		else
		{
			out += in[i];
		}
	}
	return out;
}

void
CjvxGlPlotRenderer::decode_overlay(const std::string& txt, CjvxGlPlotOverlay& ovl)
{
	ovl = CjvxGlPlotOverlay();
	size_t pos = 0;
	while (pos < txt.size())
	{
		size_t end = txt.find('\n', pos);
		if (end == std::string::npos)
		{
			end = txt.size();
		}
		std::string line = txt.substr(pos, end - pos);
		pos = end + 1;

		size_t eq = line.find('=');
		if (eq == std::string::npos)
		{
			continue;
		}
		std::string key = line.substr(0, eq);
		std::string val = jvx_gl_plot_unescape(line.substr(eq + 1));

		if (key == "active") ovl.active = (atoi(val.c_str()) != 0);
		else if (key == "ndx") ovl.num_div_x = atoi(val.c_str());
		else if (key == "ndy") ovl.num_div_y = atoi(val.c_str());
		else if (key == "grid") ovl.show_grid = (atoi(val.c_str()) != 0);
		else if (key == "lx") ovl.labels_x.push_back(val);
		else if (key == "ly") ovl.labels_y.push_back(val);
		else if (key == "alx") ovl.axis_label_x = val;
		else if (key == "aly") ovl.axis_label_y = val;
		else if (key == "title") ovl.title = val;
		else if (key == "fs") ovl.font_size = atoi(val.c_str());
		else if (key == "fst") ovl.font_size_title = atoi(val.c_str());
		else if (key == "legpos") ovl.legend_pos = atoi(val.c_str());
		else if (key == "leg")
		{
			// <AARRGGBB in hex>:<text>
			size_t col = val.find(':');
			if (col != std::string::npos)
			{
				uint32_t argb = (uint32_t)strtoul(val.substr(0, col).c_str(), nullptr, 16);
				CjvxGlPlotOverlayLegend entry;
				entry.a = (uint8_t)((argb >> 24) & 0xFF);
				entry.r = (uint8_t)((argb >> 16) & 0xFF);
				entry.g = (uint8_t)((argb >> 8) & 0xFF);
				entry.b = (uint8_t)(argb & 0xFF);
				entry.text = val.substr(col + 1);
				ovl.legend.push_back(entry);
			}
		}
	}
}

jvxErrorType
CjvxGlPlotRenderer::static_report_bufferswitch_trigger(jvxHandle* priv)
{
	if (priv)
	{
		reinterpret_cast<CjvxGlPlotRenderer*>(priv)->wake();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

void
CjvxGlPlotRenderer::wake()
{
	{
		std::lock_guard<std::mutex> lock(safeAccess);
		triggered = true;
	}
	cond.notify_all();
}

// =====================================================================================

void
CjvxGlPlotRenderer::render_loop()
{
	std::string err;
	const char* missing = nullptr;
	IjvxGlContext* ctx = jvx_gl_context_allocate();
	bool success = false;

	core = new CjvxGlPlotRendererCore;
	core->ctx = ctx;

	if (ctx && ctx->create(err))
	{
		if (core->jvx_gl_load_functions(jvx_gl_texture_get_proc, ctx, &missing))
		{
			success = true;
			const GLubyte* vers = core->glGetString(GL_VERSION);
			const GLubyte* rend = core->glGetString(GL_RENDERER);
			std::cout << "[pixbuf plot] OpenGL context for texture <" << texId << ">: Version <" <<
				(vers ? (const char*)vers : "unknown") << ">, Renderer <" << (rend ? (const char*)rend : "unknown") << ">." << std::endl;
		}
		else
		{
			err = "OpenGL function <";
			err += (missing ? missing : "unknown");
			err += "> not available.";
		}
	}
	else if (ctx == nullptr)
	{
		err = "No OpenGL context available on this platform.";
	}

	{
		std::lock_guard<std::mutex> lock(safeAccess);
		startupDone = true;
		startupSuccess = success;
		startupErr = err;
	}
	cond.notify_all();

	if (success)
	{
		std::unique_lock<std::mutex> lock(safeAccess);
		while (!stopRequest)
		{
			cond.wait_for(lock, std::chrono::milliseconds(periodMs), [this] { return stopRequest || triggered; });
			triggered = false;
			if (stopRequest)
			{
				break;
			}

			ctx->process_events();

			bool force = shared.forceRepaint;
			shared.forceRepaint = false;
			if (shared.bufChanged)
			{
				// CjvxMyGlWidget::inform_about_to_start / inform_stopped
				core->deinit();
				core->buf = shared.buf;
				core->reinit();
				shared.bufChanged = false;
				force = true;
			}
			if (shared.sizeChanged)
			{
				core->ensure_framebuffer(shared.width, shared.height);
				shared.sizeChanged = false;
				force = true;
			}
			if (shared.overlayChanged)
			{
				core->apply_overlay(shared.overlay, shared.scale);
				shared.overlayChanged = false;
				force = true;
			}

			if (core->fbo == 0)
			{
				continue;
			}

			core->glBindFramebuffer(GL_FRAMEBUFFER, core->fbo);
			core->glViewport(0, 0, (GLsizei)core->fboWidth, (GLsizei)core->fboHeight);
			// Paint in any case: in 2D incremental mode, this consumes the new lines of the buffer and
			// keeps the texture up to date also while nobody watches
			if (core->paint(force) && shared.publish)
			{
				const uint8_t* img = core->read_back();
				if (sink)
				{
					sink->publish_frame(img, core->fboWidth, core->fboHeight);
				}
			}
		}

		core->deinit();
		core->release_framebuffer();
	}

	delete core;
	core = nullptr;

	if (ctx)
	{
		ctx->destroy();
		jvx_gl_context_deallocate(ctx);
	}
}
