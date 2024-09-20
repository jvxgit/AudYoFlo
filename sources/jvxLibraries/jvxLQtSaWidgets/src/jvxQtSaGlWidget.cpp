#include "jvxQtSaGlWidget.h"
#include <iostream>

// =====================================================================
// Handling of stuff BEFORE main enters Qt Main Loop overriding a weak symbol
// =====================================================================

// Discussion of rendering to a open gl widget that is not visible:
// https://bugreports.qt.io/browse/QTBUG-47185

void print_opengl_context_format(QSurfaceFormat surfme, std::string context)
{
	std::string out_text;
	out_text += "Open GL properties <" + context + ": Version: ";
	switch (surfme.renderableType())
	{
	case QSurfaceFormat::DefaultRenderableType:
		out_text += "Default";
		break;
	case QSurfaceFormat::OpenGL:
		out_text += "Open GL";
		break;
	case QSurfaceFormat::OpenGLES:
		out_text += "Open GL ES";
		break;
	case QSurfaceFormat::OpenVG:
		out_text += "Open VG" ;
		break;
	}

	out_text += "; Version " + jvx_int2String(surfme.majorVersion()) + "." + jvx_int2String(surfme.minorVersion());
	std::cout << out_text << std::endl;
/*
	std::cout << "Major:" << surfme.majorVersion() << std::endl;
	std::cout << "Minor: " << surfme.minorVersion() << std::endl;
	switch (surfme.renderableType())
	{
	case QSurfaceFormat::DefaultRenderableType:
		std::cout << "Render Type: " << "Default" << std::endl;
		break;
	case QSurfaceFormat::OpenGL:
		std::cout << "Render Type: " << "Open GL" << std::endl;
		break;
	case QSurfaceFormat::OpenGLES:
		std::cout << "Render Type: " << "Open GL ES" << std::endl;
		break;
	case QSurfaceFormat::OpenVG:
		std::cout << "Render Type: " << "Open VG" << std::endl;
		break;
	}*/
	out_text = "Profile: ";
	switch (surfme.profile())
	{
	case QSurfaceFormat::NoProfile:
		out_text += "No Profile";
		break;
	case QSurfaceFormat::CoreProfile:
		out_text += "Core Profile";
		break;
	case QSurfaceFormat::CompatibilityProfile:
		out_text += "Compatibility Profile";
		break;
	}

	out_text += "; Depth Buffer Size: " + jvx_int2String( surfme.depthBufferSize() );
	out_text += "; Stencil Buffer Size: " + jvx_int2String(surfme.stencilBufferSize());

	/*
	switch (surfme.profile())
	{
	case QSurfaceFormat::NoProfile:
		std::cout << "Profile: " << "No Profile" << std::endl;
		break;
	case QSurfaceFormat::CoreProfile:
		std::cout << "Profile: " << "Core Profile" << std::endl;
		break;
	case QSurfaceFormat::CompatibilityProfile:
		std::cout << "Profile: " << "Compatibility Profile" << std::endl;
		break;
	}

	std::cout << "Depth Buffer Size: " << surfme.depthBufferSize() << std::endl;
	std::cout << "Stencil Buffer Size: " << surfme.stencilBufferSize() << std::endl;*/
}

extern "C"
{
	void jvx_init_before_start()
	{
		// auto hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		QSurfaceFormat surfme;

		surfme = QSurfaceFormat::defaultFormat();

		//std::cout << "Open GL format on init" << std::endl;
		print_opengl_context_format(surfme, "init");

		//QOpenGLContext::OpenGLModuleType mod = QOpenGLContext::openGLModuleType();

#if defined(JVX_OPEN_GL_DEFAULT)
		// Set the appropriate opengl version and parameters
		//surfme.setMajorVersion(2); //whatever version
		//surfme.setMinorVersion(0); //
		//surfme.setProfile(QSurfaceFormat::NoProfile);
		//surfme.setRenderableType(QSurfaceFormat::OpenGLES);
		//surfme.setDepthBufferSize(JVX_OPEN_GL_DEPTH_BUFFER_SIZE);
		//surfme.setStencilBufferSize(JVX_OPEN_GL_STENCIL_BUFFER_SIZE);

#elif defined(JVX_OPEN_GL_4_3)
		// Set the appropriate opengl version and parameters
		surfme.setMajorVersion(4); //whatever version
		surfme.setMinorVersion(3); //
		surfme.setProfile(QSurfaceFormat::CompatibilityProfile);
		surfme.setRenderableType(QSurfaceFormat::OpenGL);
		surfme.setDepthBufferSize(JVX_OPEN_GL_DEPTH_BUFFER_SIZE);
		surfme.setStencilBufferSize(JVX_OPEN_GL_STENCIL_BUFFER_SIZE);
#else
#error No valid open GLversion specified
#endif

		// Specify default format
		QSurfaceFormat::setDefaultFormat(surfme);

		// Print for control
		surfme = QSurfaceFormat::defaultFormat();
		//std::cout << "Open GL format after initialization" << std::endl;
		print_opengl_context_format(surfme, "on start");
	}
}

// =====================================================================
// =====================================================================

CjvxMyGlWidget::CjvxMyGlWidget(QWidget* parent): QOpenGLWidget(parent)
{
	//glInitializeWasCalled = false;
	propRef = NULL;
	external_link.mySwitchBuffer = NULL;
	external_link.mySwitchBufferIsValid = false;
	//gl.initialized = false;
	status_gl = 0;
	postpone.ptrBuf = NULL;
	postpone.szBuf = 0;
	postpone.renderOperation = JVX_GL_RENDER_STRAIGHT;
	postpone.cfgRender = NULL;

	myVar_straightPaint.invert_y = c_true;
	m_program = NULL;
}

CjvxMyGlWidget::~CjvxMyGlWidget()
{
}

void 
CjvxMyGlWidget::getWidgetReferences(QWidget** myWidgetRef)
{
	if (myWidgetRef)
	{
		*myWidgetRef = static_cast<QWidget*>(this);
	}
}

void 
CjvxMyGlWidget::setConnectLinks(IjvxAccessProperties* propRefIn, CjvxMyGlWidget_config* cfgArg, jvx::externalBufferSubType myOperationMode)
{
	propRef = propRefIn;
	cfg = cfgArg;
	operation_mode = myOperationMode;
}

void
CjvxMyGlWidget::clearConnectLinks()
{
	propRef = NULL;
	cfg = nullptr;
}


jvxErrorType
CjvxMyGlWidget::inform_about_to_start()
{
	// Activate the GL widget
	std::string propStr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize fsize = 0;
	jvxSize seg_x = 0;
	jvxSize seg_y = 0;
	jvxInt16 form = JVX_DATAFORMAT_NONE;
	jvxInt16 subform = JVX_DATAFORMAT_GROUP_NONE;
	jvxSize numBuffers = 0;
	jvxSize szBuf = 0;
	jvxSelectionList selLst;
	jvxCallManagerProperties callGate;	

	assert(cfg);
	assert(cfg->prop_frmt);
	assert(cfg->prop_nbufs);
	assert(cfg->prop_omode);
	assert(cfg->prop_rtrgt);
	assert(cfg->prop_segx);
	assert(cfg->prop_segy);
	assert(cfg->prop_sfrmt);

	propStr = cfg->prop_segx;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	ident.reset(propStr.c_str());
	trans.reset(true, 0);
	res = propRef->get_property(callGate, jPRIO<jvxSize>(seg_x), ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	propStr = cfg->prop_segy;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	ident.reset(propStr.c_str());
	trans.reset(true, 0);
	res = propRef->get_property(callGate, jPRIO<jvxSize>(seg_y), ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	fsize = seg_x * seg_y;

	propStr = cfg->prop_frmt;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	ident.reset(propStr.c_str());
	trans.reset(true, 0);
	res = propRef->get_property(callGate, jPRG(&form, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	propStr = cfg->prop_sfrmt;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	ident.reset(propStr.c_str());
	trans.reset(true, 0);
	res = propRef->get_property(callGate, jPRG(&subform, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	propStr = cfg->prop_omode;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	ident.reset(propStr.c_str());
	trans.reset(true, 0);
	res = propRef->get_property(callGate, jPRG(&selLst, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	numBuffers = 1;

	switch (operation_mode)
	{
	case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL:
		if (jvx_bitTest(selLst.bitFieldSelected(), 0))
		{
			// Triggered Multibuffer Mode
			propStr = cfg->prop_nbufs;
			if (cfg->prop_cp_prefix)
			{
				propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
			}
			ident.reset(propStr.c_str());
			trans.reset(true, 0);
			res = propRef->get_property(callGate, jPRG(&numBuffers, 1, JVX_DATAFORMAT_SIZE), ident, trans);
			JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

			// Check settings!
			external_link.mySwitchBuffer = jvx_allocate2DFieldExternalBuffer_full(fsize, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform,
				jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
				&szBuf, numBuffers, seg_x, seg_y);
			reinitGL(seg_x, seg_y, numBuffers, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform,
				external_link.mySwitchBuffer->ptrFld, szBuf);

			external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.report_triggerf = static_report_bufferswitch_trigger;
			external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.report_trigger_priv = reinterpret_cast<jvxHandle*>(this);
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC:
		if (jvx_bitTest(selLst.bitFieldSelected(), 1))
		{
			// Check settings!
			numBuffers = 1;
			external_link.mySwitchBuffer = jvx_allocate2DFieldExternalBuffer_inc(fsize, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform, jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
				&szBuf, numBuffers, seg_x, seg_y);
			reinitGL(seg_x, seg_y, numBuffers, 1, (jvxDataFormat)form, (jvxDataFormatGroup)subform,
				external_link.mySwitchBuffer->ptrFld, szBuf, JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW, NULL);

			// This type of buffer is painted in a period timer basis
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		break;
	default:
		assert(0);
	}

	propStr = cfg->prop_rtrgt;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}
	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(external_link.mySwitchBuffer, &external_link.mySwitchBufferIsValid,
		external_link.mySwitchBuffer->formFld);
	ident.reset(propStr.c_str());

	res = propRef->install_referene_property(callGate, ptrRaw, ident);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	return res;
}

void
CjvxMyGlWidget::inform_update_window_periodic()
{
	if (operation_mode == jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC)
	{
		report_bufferswitch_trigger_inThread();
	}
}

jvxErrorType
CjvxMyGlWidget::inform_stopped()
{
	std::string propStr;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	propStr = cfg->prop_rtrgt;
	if (cfg->prop_cp_prefix)
	{
		propStr = jvx_makePathExpr(cfg->prop_cp_prefix, propStr);
	}

	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(
		external_link.mySwitchBuffer, 
		&external_link.mySwitchBufferIsValid,
		external_link.mySwitchBuffer->formFld);
	ident.reset(propStr.c_str());

	res = propRef->uninstall_referene_property(callGate, ptrRaw, ident);
	JVX_ASSERT_PROPERTY_ACCESS_RETURN(res, propStr);

	if (external_link.mySwitchBuffer)
	{
		deinitGL();

		//terminateGL(); //don't call if reinitGL was not called!

		// Check settings!
		jvx_deallocateExternalBuffer(external_link.mySwitchBuffer);
	}
	external_link.mySwitchBuffer = NULL;
	external_link.mySwitchBufferIsValid = false;
	return res;
}

jvxErrorType
CjvxMyGlWidget::create_programm_assign(
	const std::string& vertex_shader_code, 
	const std::string& fragment_shader_code,
	jvxOpenGlRendering myRenderOperation, 
	GLint* position, GLint* texture,
	GLint* border_x, GLint* border_y, 
	GLint* offset_x, GLint* invert_y)
{
	QString log;
	m_program = new QOpenGLShaderProgram(this);
	
	bool cSuc = m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_code.c_str());
	if (!cSuc)
	{
		log = m_program->log();
		std::cout << "Function <addShaderFromSourceCode> failed, reason: " << log.toLatin1().data() << std::endl;
		assert(0);
	}

	cSuc = m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_code.c_str());
	if (!cSuc)
	{
		log = m_program->log();
		std::cout << "Function <addShaderFromSourceCode> failed, reason: " << log.toLatin1().data() << std::endl;
		assert(0);
	}

	cSuc = m_program->link();
	if (!cSuc)
	{
		log = m_program->log();
		std::cout << "Function <link> failed, reason: " << log.toLatin1().data() << std::endl;
		assert(0);
	}

	switch (myRenderOperation)
	{
	case JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW:
		*border_x = m_program->uniformLocation("border_x");
		*border_y = m_program->uniformLocation("border_y");
		*offset_x = m_program->uniformLocation("offset_x");
		break;
	case JVX_GL_RENDER_STRAIGHT:
		*invert_y = m_program->uniformLocation("invert_y");
		break;
	}
	*texture = m_program->uniformLocation("texture");
	*position = m_program->attributeLocation("position");

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMyGlWidget::release_programm_unassign()
{
	delete m_program;
	m_program = NULL;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMyGlWidget::start_rendering_program()
{
	m_program->bind();
	return JVX_NO_ERROR;
}
jvxErrorType 
CjvxMyGlWidget::stop_rendering_program()
{
	m_program->release();
	return JVX_NO_ERROR;
}

void
CjvxMyGlWidget::init_manually()
{
	//makeCurrent();
	this->initializeGL();
}

// ==================================================================================================
// ==================================================================================================

void 
CjvxMyGlWidget::initializeGL()
{
	if (jvx_bitTest(status_gl, 0))
	{
	}
	else
	{
		jvx_bitSet(status_gl, 0);

		connect(this, SIGNAL(emit_update_video_frame()), this, SLOT(report_bufferswitch_trigger_inThread()), Qt::QueuedConnection);

		// Obtain all opengl function
		initializeOpenGLFunctions();

		// Clear screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (jvx_bitTest(status_gl, 1))
		{
			gl = postpone.gl;

			jvxVideoRenderCore_bc::initialize(gl.width, gl.height, (jvxByte****)(&gl.bufs),
				&gl.szFld, gl.numChannels, gl.numBufs, gl.form, gl.subform,
				postpone.renderOperation, postpone.cfgRender,
				"Init GL Native Display", postpone.ptrBuf, postpone.szBuf);
			status_gl |= 0x2;
		}
		//gl.initialized = false;
		//glInitializeWasCalled = true;
	}
}

/*
void 
CjvxMyGlWidget::resizeGL(int w, int h)
{
	//makeCurrent();
	// Update projection matrix and other size related settings:
	//m_projection.setToIdentity();
	//m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}
*/

void 
CjvxMyGlWidget::paintGL()
{
	//makeCurrent();
	// Draw the scene:
	//glClear(GL_COLOR_BUFFER_BIT);
	/*
	QRect geom = this->geometry();
	std::cout << "Geom: " << geom.width() << ":" << geom.height() << std::endl;
	*/

	jvxSize fH = 0;
	jvxSize iR = 0;
	jvxSize iW = 0;
	jvxSize ll1 = 0;
	jvxSize ll2 = 0;
	cfgRotateAngleOffsetFollow myVar_rotatePaint;
	
	//std::cout << "Paint" << std::endl;
	if (jvx_bitTest(status_gl, 0))
	{
		if (jvx_bitTest(status_gl, 1))
		{
			switch (operation_mode)
			{
			case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_FULL:
				if (external_link.mySwitchBuffer->fill_height > 0)
				{					
					// ## std::cout << "Start render buffer " << external_link.mySwitchBuffer->idx_read << std::endl;
					prepare_render((const jvxByte**)gl.bufs[external_link.mySwitchBuffer->idx_read], &myVar_straightPaint);
					// ## std::cout << "Stop render buffer " << external_link.mySwitchBuffer->idx_read << std::endl;

					external_link.mySwitchBuffer->safe_access.lockf(external_link.mySwitchBuffer->safe_access.priv);
					external_link.mySwitchBuffer->idx_read = (external_link.mySwitchBuffer->idx_read + 1) %
						external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.common.number_buffers;
					external_link.mySwitchBuffer->fill_height--;
					external_link.mySwitchBuffer->safe_access.unlockf(external_link.mySwitchBuffer->safe_access.priv);
					
					// ## std::cout << "Fill height " << external_link.mySwitchBuffer->fill_height << std::endl;
					
					if (external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.report_completef)
					{
						external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.report_completef(external_link.mySwitchBuffer->specific.the2DFieldBuffer_full.report_complete_priv);
					}
				}
				else
				{
					std::cout << "Fill heigth is zero" << std::endl;
				}
				break;
			case jvx::JVX_EXTERNAL_BUFFER_SUB_2D_INC:

				if (external_link.mySwitchBuffer->fill_height > 0)
				{
					external_link.mySwitchBuffer->safe_access.lockf(external_link.mySwitchBuffer->safe_access.priv);
					fH = external_link.mySwitchBuffer->fill_height;
					iR = external_link.mySwitchBuffer->idx_read;
					external_link.mySwitchBuffer->safe_access.unlockf(external_link.mySwitchBuffer->safe_access.priv);
					ll1 = JVX_MIN(fH, external_link.mySwitchBuffer->specific.the2DFieldBuffer_inc.common.seg_y - iR);
					ll2 = fH - ll1;
					iW = (iR + fH) % external_link.mySwitchBuffer->specific.the2DFieldBuffer_inc.common.seg_y;
					myVar_rotatePaint.border_x = 0.05;
					myVar_rotatePaint.border_y = 0.1;
					iW = iR;
					if (ll1)
					{
						iW += ll1;
						myVar_rotatePaint.offset_x = (jvxData)iW / (jvxData)external_link.mySwitchBuffer->specific.the2DFieldBuffer_inc.common.seg_y;
						myVar_rotatePaint.update_offset_y = iR;
						myVar_rotatePaint.update_num_y = ll1;
						prepare_render((const jvxByte**)gl.bufs[0], &myVar_rotatePaint);
						iR += ll1;
					}
					if (ll2)
					{
						iR = 0;
						iW += ll2;
						myVar_rotatePaint.offset_x = (jvxData)iW / (jvxData)external_link.mySwitchBuffer->specific.the2DFieldBuffer_inc.common.seg_y;
						myVar_rotatePaint.update_offset_y = iR;
						myVar_rotatePaint.update_num_y = ll2;
						prepare_render((const jvxByte**)gl.bufs[0], &myVar_rotatePaint);
					}

					external_link.mySwitchBuffer->safe_access.lockf(external_link.mySwitchBuffer->safe_access.priv);
					external_link.mySwitchBuffer->fill_height -= (ll1 + ll2);
					external_link.mySwitchBuffer->idx_read = (external_link.mySwitchBuffer->idx_read + ll1 + ll2) % external_link.mySwitchBuffer->specific.the2DFieldBuffer_inc.common.seg_y;
					external_link.mySwitchBuffer->safe_access.unlockf(external_link.mySwitchBuffer->safe_access.priv);
				}
				break;
			}
		}
		else
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
}

/*
void 
CjvxMyGlWidget::resizeEvent(QResizeEvent *event)
{
	resizeGL(width(), height());
}
*/

void
CjvxMyGlWidget::reinitGL(jvxSize width, jvxSize height, 
	jvxSize numBuffers, jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	jvxByte* ptrBuf, jvxSize szBuf, jvxOpenGlRendering renderOperation, jvxHandle* cfgRender)
{
	//if (!glInitializeWasCalled)
	if(jvx_bitTest(status_gl, 0)) 
	{
		status_gl |= 0x2;

		//gl.initialized = true;
		gl.width = width;
		gl.height = height;
		gl.numBufs = numBuffers;
		gl.numChannels = numChannels;
		gl.form = form;
		gl.subform = subform;
		gl.bufs = NULL;
		gl.szFld = 0;


		jvxVideoRenderCore_bc::initialize(gl.width, gl.height, (jvxByte****)(&gl.bufs),
			&gl.szFld, gl.numChannels, gl.numBufs, gl.form, gl.subform,
			renderOperation, cfgRender,
			"Init GL Native Display", ptrBuf, szBuf);
	}
	else
	{
		postpone.gl.width = width;
		postpone.gl.height = height;
		postpone.gl.numBufs = numBuffers;
		postpone.gl.numChannels = numChannels;
		postpone.gl.form = form;
		postpone.gl.subform = subform;
		postpone.gl.bufs = NULL;
		postpone.gl.szFld = 0;

		postpone.ptrBuf = ptrBuf;
		postpone.szBuf = szBuf;
		postpone.renderOperation = renderOperation;
		postpone.cfgRender = cfgRender;

		status_gl |= 0x2;
	}
}

void
CjvxMyGlWidget::deinitGL()
{
	if (jvx_bitTest(status_gl, 1))
	{
		jvxVideoRenderCore_bc::terminate();
		jvx_bitClear(status_gl, 1);

		postpone.ptrBuf = NULL;
		postpone.szBuf = 0;
		postpone.renderOperation = JVX_GL_RENDER_STRAIGHT;
		postpone.cfgRender = NULL;
	}
}

jvxErrorType
CjvxMyGlWidget::static_report_bufferswitch_trigger(jvxHandle* priv)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (priv)
	{
		CjvxMyGlWidget* this_pointer = (CjvxMyGlWidget*)priv;
		res = this_pointer->ic_report_bufferswitch_trigger();
	}
	return res;
}

jvxErrorType
CjvxMyGlWidget::ic_report_bufferswitch_trigger()
{
	/*
	static int cnt = 0;
	std::cout << "Report buffer switch ext: " << cnt << std::endl;
	cnt++;*/
	emit emit_update_video_frame();
	return JVX_NO_ERROR;
}

void
CjvxMyGlWidget::report_bufferswitch_trigger_inThread()
{
	/*
	static int cnt = 0;
	std::cout << "Report buffer switch int: " << cnt << std::endl;
	cnt++;
	*/
	// Note: the update function does not run immediately!
	this->update();
}