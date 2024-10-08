#include "CjvxViNOpenGLViewer.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#ifdef JVX_USE_GLEW_GLUT
	JVX_THREAD_ENTRY_FUNCTION(my_local_thread_entry, pp)
	{
		if (pp)
		{
			CjvxViNOpenGLViewer* this_pointer = (CjvxViNOpenGLViewer*)pp;
			assert(this_pointer);
			this_pointer->run_native_gl();
		}
		return 0;
	}

	extern "C"
	{
		static CjvxViNOpenGLViewer* myGlobalNativeGlContext = NULL;		
		static void jvx_ogl_render()
		{
			if (myGlobalNativeGlContext)
			{
				myGlobalNativeGlContext->do_rendering_gl();
			}
		}

		static void jvx_ogl_idle()
		{
			if (myGlobalNativeGlContext)
			{
				myGlobalNativeGlContext->do_idle_gl();
			}
		}		
	};
#endif


CjvxViNOpenGLViewer::CjvxViNOpenGLViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoNodeTpl<CjvxBareNode1io>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxViNOpenGLViewer::~CjvxViNOpenGLViewer()
{
}

jvxErrorType
CjvxViNOpenGLViewer::activate()
{
	jvxSize i;
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1io>::activate();
	if (res == JVX_NO_ERROR)
	{
		genOpenGl_node::init_all();
		genOpenGl_node::allocate_all();
		genOpenGl_node::register_all(static_cast<CjvxProperties*>(this));

		return JVX_NO_ERROR;

	}
	return(res);
}

jvxErrorType
CjvxViNOpenGLViewer::deactivate()
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1io>::deactivate();
	if (res == JVX_NO_ERROR)
	{
		genOpenGl_node::unregister_all(static_cast<CjvxProperties*>(this));
		genOpenGl_node::deallocate_all();		
		return JVX_NO_ERROR;
	}
	return(res);
}


jvxErrorType
CjvxViNOpenGLViewer::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	jvxByte* ptr;
	jvxExternalBuffer* ptrExt = NULL;

	// This will forward the propcessing pipeline. We shpould use a zero-copy here!!
	res = CjvxVideoNodeTpl<CjvxBareNode1io>::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		nativeGl.reset();

		if (_common_set_icon.theData_in->con_params.number_channels >= 1)
		{

			// ========================================================================================
			// Derive rendering target
			// ========================================================================================

			if (jvx_bitTest(genOpenGl_node::config.rendering_mode.value.selection(), 1))
			{
				nativeGl.renderingTarget = JVX_GL_RENDER_EXTERNAL;
			}
#ifdef JVX_USE_GLEW_GLUT
			else if (jvx_bitTest(genOpenGl_node::config.rendering_mode.value.selection(), 2))
			{
				nativeGl.renderingTarget = JVX_GL_RENDER_NATIVE;
			}
#endif
			else
			{
				nativeGl.renderingTarget = JVX_GL_DO_NOT_RENDER;
			}
			// ========================================================================================

			// ========================================================================================
			// Some error checking
			// ========================================================================================
#ifdef JVX_USE_GLEW_GLUT
			if (
				(nativeGl.renderingTarget == JVX_GL_RENDER_EXTERNAL) &&
				(genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr == NULL))
			{
				_report_text_message("External Render Buffer not present, activating native rendering.", JVX_REPORT_PRIORITY_WARNING);
				nativeGl.renderingTarget = JVX_GL_RENDER_NATIVE;
			}
#endif
			// ========================================================================================

			switch (nativeGl.renderingTarget)
			{
#ifdef JVX_USE_GLEW_GLUT
			case JVX_GL_RENDER_NATIVE:
				nativeGl.threadIsInitialized = false;

				nativeGl.form = _common_set_icon.theData_in->con_params.format;
				nativeGl.subform = _common_set_icon.theData_in->con_params.format_group;
				nativeGl.height = _common_set_icon.theData_in->con_params.segmentation.y;
				nativeGl.width = _common_set_icon.theData_in->con_params.segmentation.x;
				nativeGl.numBufs = 2;
				nativeGl.numChannels = _common_set_icon.theData_in->con_params.number_channels;
				nativeGl.hdlThread = JVX_INVALID_HANDLE_VALUE;
				nativeGl.requestStop = false;

				// Create thread and allocate memory within this thread
				JVX_CREATE_THREAD(nativeGl.hdlThread, my_local_thread_entry, reinterpret_cast<jvxHandle*>(this), nativeGl.idThread);

				// Therefore, wait for the allocation to be complete!!
				while (!nativeGl.threadIsInitialized)
				{
					JVX_SLEEP_MS(100);
				}

				assert(nativeGl.bufs);
				assert(nativeGl.szFld == _common_set_icon.theData_in->con_params.buffersize);
				break;
#endif
			case JVX_GL_RENDER_EXTERNAL:
				ptrExt = genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr;
				ptr = ptrExt->ptrFld;
				break;
			default:
				break;
			}
		}
	}
		
	return res;
}

jvxErrorType
CjvxViNOpenGLViewer::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
	
	switch (nativeGl.renderingTarget)
	{

#ifdef JVX_USE_GLEW_GLUT
	case JVX_GL_RENDER_NATIVE:
		nativeGl.requestStop = true;

		resW = JVX_WAIT_FOR_THREAD_TERMINATE_MS(nativeGl.hdlThread, 50000);
		if (resW != JVX_WAIT_SUCCESS)
		{
			JVX_TERMINATE_THREAD(nativeGl.hdlThread, 0);
		}
		break;
#endif
	default:
		break;
	}

	res = CjvxVideoNodeTpl<CjvxBareNode1io>::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		switch (nativeGl.renderingTarget)
		{
#ifdef JVX_USE_GLEW_GLUT
		case JVX_GL_RENDER_NATIVE:
			// Deallocation handled in stopped thread.
			break;
#endif
		case JVX_GL_RENDER_EXTERNAL:
			break;
		case JVX_GL_DO_NOT_RENDER:
			break;
		default:
			assert(0);
			break;
		}
	}

	return res;
}

jvxErrorType
CjvxViNOpenGLViewer::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool lostBuffer = false;

	jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_icon.theData_in, idx_stage);
	
	// Input an incoming buffer into the buffer pipeline.
	switch (nativeGl.renderingTarget)
	{
	case JVX_GL_RENDER_EXTERNAL:
	{
		auto buf = genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr;
		if (buf)
		{
			switch (buf->subTp)
			{
			case jvx::externalBufferSubType::JVX_EXTERNAL_BUFFER_SUB_2D_FULL:
			{
				// Not in protected area since we only read - we have a one-read-one-write constellation
				if (buf->fill_height < buf->specific.the2DFieldBuffer_full.common.number_buffers)
				{
					assert(_common_set_icon.theData_in->con_params.buffersize == buf->numElmFldOneChanOneBuf);
					assert(_common_set_icon.theData_in->con_params.number_channels == buf->number_channels);
					assert(_common_set_icon.theData_in->con_params.format == buf->formFld);
					assert(_common_set_icon.theData_in->con_params.format_group == buf->subFormFld);

					// Check buffer match size!!
					assert(buf->specific.the2DFieldBuffer_full.common.szFldOneBuf == jvx_derive_buffersize(_common_set_icon.theData_in->con_params));

					buf->safe_access.lockf(buf->safe_access.priv);
					jvxSize idx_write = (buf->idx_read + buf->fill_height) % buf->specific.the2DFieldBuffer_full.common.number_buffers;
					buf->safe_access.unlockf(buf->safe_access.priv);

					jvxByte* ptrWrite = buf->ptrFld;
					ptrWrite += idx_write * buf->specific.the2DFieldBuffer_full.common.szFldOneBuf;
					for (jvxSize i = 0; i < buf->number_channels; i++)
					{
						jvxSize copyNum = buf->numElmFldOneChanOneBuf * jvx_derive_elementsize_byte(buf->subFormFld, buf->formFld);
						memcpy(ptrWrite, bufsIn[i], copyNum);
						ptrWrite += copyNum;
					}

					buf->safe_access.lockf(buf->safe_access.priv);
					buf->fill_height++;
					buf->safe_access.unlockf(buf->safe_access.priv);
				}
				else
				{
					lostBuffer = true;
				}

				// Notification to rendering thread
				// Hint: we hold the property lock here. We need to hold it to not remove the callback while calling.
				// In the callback, we will render a buffer and finally report to the UI. The UI protects itself with a log.
				// If UI requests a property and the lock is in the video thread, the UI lock will wait for the propert lock
				// and the property lock will wait for the UI lock -> DEADLOCK
				// Therefore, the buffer callback needs to render the video via another thread!
				if (buf->specific.the2DFieldBuffer_full.report_triggerf)
				{
					buf->specific.the2DFieldBuffer_full.report_triggerf(buf->specific.the2DFieldBuffer_full.report_trigger_priv);
				}
			}
			break;
			default:

				// Do nothing here!!
				break;
			}
		}
		else
		{
			lostBuffer = true;
		}
		break;
	}
	case JVX_GL_DO_NOT_RENDER:
		// Nothing to be done here
		break;

	case JVX_GL_RENDER_NATIVE:

		// Take the incomingbuffer
		if (nativeGl.safeAccess.v.idxBuf_Fheight < nativeGl.numBufs)
		{
			nativeGl.safeAccess.lock();			
			jvxSize idxWrite = (nativeGl.safeAccess.v.idxBuf_Read + nativeGl.safeAccess.v.idxBuf_Fheight) % nativeGl.numBufs;
			nativeGl.safeAccess.unlock();

			jvxByte** bufTo = (jvxByte**)nativeGl.bufs[idxWrite];
			assert(nativeGl.szFld == jvx_derive_buffersize(_common_set_icon.theData_in->con_params));

			for (jvxSize i = 0; i < nativeGl.numChannels; i++)
			{
				memcpy(bufTo[i], bufsIn[i], nativeGl.szFld);
			}

			nativeGl.safeAccess.lock();
			nativeGl.safeAccess.v.idxBuf_Fheight++;
			nativeGl.safeAccess.unlock();
		}
		else
		{
			lostBuffer = true;
		}
		break;
	default:
		assert(0);
		break;
	}

	if (lostBuffer)
	{
		genOpenGl_node::runtime.lost_frames.value++;
	}
	return  fwd_process_buffers_icon(mt_mask, idx_stage);
}

#ifdef JVX_USE_GLEW_GLUT
void
CjvxViNOpenGLViewer::run_native_gl()
{
	jvxVideoRenderCore_nobc::initialize((int)nativeGl.width, (int)nativeGl.height, (jvxByte****)(&nativeGl.bufs),
		&nativeGl.szFld, nativeGl.numChannels, nativeGl.numBufs, nativeGl.form, nativeGl.subform,
		JVX_GL_RENDER_STRAIGHT, NULL,
		"Open GL Native Display", NULL, 0, jvx_ogl_render, jvx_ogl_idle);

	assert(myGlobalNativeGlContext == NULL);
	myGlobalNativeGlContext = this;

	nativeGl.threadIsInitialized = true;

	jvxVideoRenderCore_nobc::system_passcontrol();

	// Here we are about to leave the native GL thread
	jvxVideoRenderCore_nobc::terminate();

	nativeGl.reset();

	myGlobalNativeGlContext = NULL;
}

void
CjvxViNOpenGLViewer::do_rendering_gl()
{
	if (nativeGl.safeAccess.v.idxBuf_Fheight > 0)
	{
		cfgRenderStraight cfg;
		cfg.invert_y = 1;		

		// idxBuf_Read is not to be protected as it will only be touched here!
		jvxVideoRenderCore_nobc::prepare_render((const jvxByte**)nativeGl.bufs[nativeGl.safeAccess.v.idxBuf_Read], &cfg);

		nativeGl.safeAccess.lock();
		nativeGl.safeAccess.v.idxBuf_Read = (nativeGl.safeAccess.v.idxBuf_Read + 1) % nativeGl.numBufs;
		nativeGl.safeAccess.v.idxBuf_Fheight--;
		nativeGl.safeAccess.unlock();

		jvxVideoRenderCore_nobc::finalize_render();
	}
}

void
CjvxViNOpenGLViewer::do_idle_gl()
{
	if (nativeGl.safeAccess.v.idxBuf_Fheight > 0)
	{
		jvxVideoRenderCore_nobc::trigger_render();
	}
	if (nativeGl.requestStop)
	{
		jvxVideoRenderCore_nobc::system_regaincontrol();
	}
}
#endif

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
