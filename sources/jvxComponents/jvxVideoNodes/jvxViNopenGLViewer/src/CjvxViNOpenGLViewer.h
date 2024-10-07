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

	struct
	{
		JVX_THREAD_HANDLE hdlThread;
		JVX_THREAD_ID idThread;

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
		JVX_MUTEX_HANDLE safeAccessBuffer;

		jvxBool requestStop;
		jvxOpenGlRenderTarget renderingTarget;
	} nativeGl;


	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNOpenGLViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxViNOpenGLViewer();

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage);
	jvxErrorType process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock clbk,
			jvxHandle* priv_ptr)override;

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
