#ifndef __CJVXVINOPENGLVIEWER_H__
#define __CJVXVINOPENGLVIEWER_H__

#include "jvxNodes/CjvxBareNode1io.h"
#include "jvxVideoNodes/CjvxVideoNodeTpl.h"

#ifdef JVX_USE_GLEW_GLUT
#include "jvxVideoRenderCore.h"
#endif

#include "pcg_exports_node.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxViNOpenGLViewer: public CjvxVideoNodeTpl<CjvxBareNode1io>, 
#ifdef JVX_USE_GLEW_GLUT
	public jvxVideoRenderCore_nobc,
#endif
	public genOpenGl_node
{
private:
protected:
public:

	typedef enum
	{
		JVX_GL_DO_NOT_RENDER = 0,
		JVX_GL_RENDER_EXTERNAL = 1
#ifdef JVX_USE_GLEW_GLUT
		, JVX_GL_RENDER_NATIVE = 2
#endif

	}jvxOpenGlRenderTarget;

	class bufStatus
	{
	public:
		jvxSize idxBuf_Read = 0;
		jvxSize idxBuf_Fheight = 0;
	};

	class renderNative
	{
	public:

		JVX_THREAD_HANDLE hdlThread = JVX_INVALID_HANDLE_VALUE;
		JVX_THREAD_ID idThread = JVX_THREAD_ID_INVALID;
		jvxSize width = 0;
		jvxSize height = 0;
		jvxHandle*** bufs = nullptr;
		jvxSize szFld = 0;
		jvxSize numChannels = 0;
		jvxSize numBufs = 0;
		jvxDataFormat form = jvxDataFormat::JVX_DATAFORMAT_NONE;
		jvxDataFormatGroup subform = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
		jvxBool threadIsInitialized = false;
		jvxLockWithVariable< bufStatus> safeAccess;
		jvxBool requestStop = false;
		jvxOpenGlRenderTarget renderingTarget = jvxOpenGlRenderTarget::JVX_GL_DO_NOT_RENDER;

		renderNative()
		{
			reset();
		}

		void reset()
		{
			hdlThread = JVX_NULLTHREAD;
			idThread = JVX_THREAD_ID_INVALID;
			width = 0;
			height = 0;
			bufs = nullptr;
			szFld = 0;
			numChannels = 0;
			numBufs = 0;
			form = jvxDataFormat::JVX_DATAFORMAT_NONE;
			subform = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
			threadIsInitialized = false;
			requestStop = false;
			renderingTarget = jvxOpenGlRenderTarget::JVX_GL_DO_NOT_RENDER;
		}
	};

	renderNative nativeGl;


	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNOpenGLViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxViNOpenGLViewer();

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage);

#ifdef JVX_USE_GLEW_GLUT
	void run_native_gl();
	void do_rendering_gl();
	void do_idle_gl();
#endif
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
