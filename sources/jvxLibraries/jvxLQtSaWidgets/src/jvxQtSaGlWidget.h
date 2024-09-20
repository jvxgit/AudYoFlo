#ifndef __CJVXMYGLWIDGET_H__
#define __CJVXMYGLWIDGET_H__

#include "jvx.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "interfaces/properties_hosts/IjvxAccessProperties.h"

/* On RPI, this define is not set somehow */
#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

// Default open gl functionality:
#define JVX_OPEN_GL_DEFAULT
//#define JVX_OPEN_GL_4_3

#define JVX_OPEN_GL_DEPTH_BUFFER_SIZE 24
#define JVX_OPEN_GL_STENCIL_BUFFER_SIZE 8

#if defined(JVX_OPEN_GL_DEFAULT)
#define JVX_OPN_GL_BASE_CLASS QOpenGLFunctions
#elif defined(JVX_OPEN_GL_4_3)
#include <QOpenGLFunctions_4_3_Core>
#define JVX_OPN_GL_BASE_CLASS QOpenGLFunctions_4_3_Core
#else
#error No valid open GLversion specified
#endif

#define JVX_VIDEO_RENDER_CORE_CLASS JVX_OPN_GL_BASE_CLASS
#include "jvx.h"
#include "jvxVideoRenderCore.h"
//class CjvxGlVideoClass : protected JVX_OPN_GL_BASE_CLASS
//{
//
//};
class CjvxMyGlWidget_config
{
public:
	const char* prop_segx = nullptr; // "/system/segmentsizex"
	const char* prop_segy = nullptr; // "/system/segmentsizey"
	const char* prop_frmt = nullptr; // "/system/dataformat"
	const char* prop_sfrmt = nullptr; //  "/system/datasubformat"
	const char* prop_omode = nullptr; // "/expose_visual_if/operation_mode"
	const char* prop_nbufs = nullptr;// "/expose_visual_if/number_buffers"
	const char* prop_rtrgt = nullptr;// "/expose_visual_if/rendering_target"

	const char* prop_cp_prefix = nullptr;
};

class CjvxMyGlWidget: public QOpenGLWidget, protected jvxVideoRenderCore_bc
{
	QOpenGLContext *m_context;
	Q_OBJECT


	struct gl_struct
	{
		jvxSize width;
		jvxSize height;
		jvxHandle*** bufs;
		jvxSize szFld;
		jvxSize numChannels;
		jvxSize numBufs;
		jvxDataFormat form;
		jvxDataFormatGroup subform;

		jvxBool threadIsInitialized;
		jvxSize idxBuf_Read;
		jvxSize idxBuf_Fheight;

		jvxBool requestStop;
		jvxBool renderingOn;

		//jvxBool initialized;
	};
	
	gl_struct gl;

	struct
	{
		gl_struct gl;
		jvxByte* ptrBuf;
		jvxSize szBuf;
		jvxOpenGlRendering renderOperation;
		jvxHandle* cfgRender;
	} postpone;

	struct
	{
		jvxExternalBuffer* mySwitchBuffer;
		jvxBool mySwitchBufferIsValid;
	} external_link;

	//struct
	//{
	//	jvxExternalBuffer* mySwitchBuffer;
	//} internal_copy;

	IjvxAccessProperties* propRef;
	
	jvx::externalBufferSubType operation_mode;
	//jvxBool glInitializeWasCalled;
	jvxCBitField status_gl;

	QOpenGLShaderProgram* m_program;

	jPAD ident;
	jPD trans;

	cfgRenderStraight myVar_straightPaint;

	CjvxMyGlWidget_config* cfg = nullptr;

public:

	CjvxMyGlWidget(QWidget* parent);
	~CjvxMyGlWidget();

	void getWidgetReferences(QWidget** myWidgetRef);

	void setConnectLinks(IjvxAccessProperties* propRefIn, CjvxMyGlWidget_config* cfgArg, jvx::externalBufferSubType my_operation);
	void clearConnectLinks();

	jvxErrorType inform_about_to_start();
	jvxErrorType inform_stopped();
	virtual void inform_update_window_periodic();

	virtual void init_manually();
private:

	//virtual void resizeEvent(QResizeEvent *event)override;

	void initializeGL() override;
	// void resizeGL(int w, int h) override;
	void paintGL() override;

	jvxErrorType create_programm_assign(
		const std::string& vertex_shader_code,
		const std::string& fragment_shader_code,
		jvxOpenGlRendering myRenderOperation,
		GLint* position, GLint* texture,
		GLint* border_x, GLint* border_y, 
		GLint* offset_x, GLint* invert_y) override;

	jvxErrorType release_programm_unassign() override;

	jvxErrorType start_rendering_program() override;
	jvxErrorType stop_rendering_program() override;

	virtual jvxErrorType system_initialize(const char* txtToShow)override
	{
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType system_passcontrol()override
	{
		return JVX_NO_ERROR;
	};

	static jvxErrorType static_report_bufferswitch_trigger(jvxHandle* priv);
	jvxErrorType ic_report_bufferswitch_trigger();

	void reinitGL(jvxSize width, jvxSize height, jvxSize numBuffers, jvxSize numChannels, jvxDataFormat form, 
		jvxDataFormatGroup subform, jvxByte* ptrBuf, jvxSize szBufjvxOpenGlRendering,
		jvxOpenGlRendering renderOperation = JVX_GL_RENDER_STRAIGHT, jvxHandle* cfgRender = NULL);
	void deinitGL();
	//void terminateGL();

signals:
	void emit_update_video_frame();

	public slots:
	void report_bufferswitch_trigger_inThread();
};

#endif
