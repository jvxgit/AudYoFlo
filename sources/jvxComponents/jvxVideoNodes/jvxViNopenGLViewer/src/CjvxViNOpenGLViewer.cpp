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
	_common_set_ldslave.zeroCopyBuffering_cfg = false;
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

	// This is the return from the link list

	nativeGl.bufs = NULL;
	nativeGl.szFld = 0;

	if (_common_set_icon.theData_in->con_params.number_channels >= 1)
	{
		if (jvx_bitTest(genOpenGl_node::configuration_mf.rendering_mode.value.selection(), 1))
		{
			nativeGl.renderingTarget = JVX_GL_RENDER_EXTERNAL;
		}
#ifdef JVX_USE_GLEW_GLUT
		else if (jvx_bitTest(genOpenGl_node::configuration_mf.rendering_mode.value.selection(), 2))
		{
			nativeGl.renderingTarget = JVX_GL_RENDER_NATIVE;
		}
#endif
		else
		{
			nativeGl.renderingTarget = JVX_GL_DO_NOT_RENDER;
		}

#ifdef JVX_USE_GLEW_GLUT
		if (
			(nativeGl.renderingTarget == JVX_GL_RENDER_EXTERNAL) &&
			(genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr == NULL))
		{
			_report_text_message("External Render Buffer not present, activating native rendering.", JVX_REPORT_PRIORITY_WARNING);
			nativeGl.renderingTarget = JVX_GL_RENDER_NATIVE;
		}
#endif
		nativeGl.idxBuf_Read = 0;
		nativeGl.idxBuf_Fheight = 0;

		switch (nativeGl.renderingTarget)
		{
#ifdef JVX_USE_GLEW_GLUT
		case JVX_GL_RENDER_NATIVE:
			nativeGl.threadIsInitialized = false;

			nativeGl.form = _common_set_icon.theData_in->con_params.format;
			nativeGl.subform = _common_set_icon.theData_in->con_params.format_group;
			nativeGl.height = _common_set_icon.theData_in->con_params.segmentation.y;
			nativeGl.width = _common_set_icon.theData_in->con_params.segmentation.x;
			nativeGl.numBufs = _common_set_icon.theData_in->con_data.number_buffers;
			nativeGl.numChannels = _common_set_icon.theData_in->con_params.number_channels;
			nativeGl.hdlThread = JVX_INVALID_HANDLE_VALUE;
			nativeGl.requestStop = false;


			JVX_CREATE_THREAD(nativeGl.hdlThread, my_local_thread_entry, reinterpret_cast<jvxHandle*>(this), nativeGl.idThread);

			while (!nativeGl.threadIsInitialized)
			{
				JVX_SLEEP_MS(100);
			}

			assert(nativeGl.bufs);
			assert(nativeGl.szFld == _common_set_icon.theData_in->con_params.buffersize);

			_common_set_icon.theData_in->con_data.buffers = nativeGl.bufs;
			break;
#endif
		case JVX_GL_RENDER_EXTERNAL:
			ptrExt = genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr;
			ptr = ptrExt->ptrFld;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(_common_set_icon.theData_in->con_data.buffers,
				jvxHandle**, _common_set_icon.theData_in->con_data.number_buffers);
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(_common_set_icon.theData_in->con_data.buffers[i],
					jvxHandle*, _common_set_icon.theData_in->con_params.number_channels);
				for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
				{
					_common_set_icon.theData_in->con_data.buffers[i][j] = ptr;
					ptr += JVX_DATA2SIZE(ptrExt->numElmFldOneChanOneBuf * ptrExt->szElmFld);
				}
			}
			break;
		default:
			// No render but allocate buffers for data flow
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(_common_set_icon.theData_in->con_data.buffers, jvxHandle**,
				_common_set_icon.theData_in->con_data.number_buffers);
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(_common_set_icon.theData_in->con_data.buffers[i], jvxHandle*,
					_common_set_icon.theData_in->con_params.number_channels);
				for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
				{

					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(_common_set_icon.theData_in->con_data.buffers[i][j], jvxByte,
						_common_set_icon.theData_in->con_params.buffersize);
				}
			}
			break;
		}

		jvx_allocateDataLinkPipelineControl(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
			, _common_set.theModuleName.c_str()
#endif
		);
		*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;
		_common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[
			*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr].idProcess =
			_common_set_io_common.myRuntimeId;
	}

	
	_common_set_ocon.theData_out.con_data.number_buffers = 1;

	res = CjvxVideoNodeTpl<CjvxBareNode1io>::_prepare_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		
	return res;
}

jvxErrorType
CjvxViNOpenGLViewer::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	JVX_WAIT_RESULT resW = JVX_WAIT_SUCCESS;
	if (_common_set_icon.theData_in->con_params.number_channels >= 1)
	{
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


		jvx_deallocateDataLinkPipelineControl(_common_set_icon.theData_in);

		switch (nativeGl.renderingTarget)
		{
#ifdef JVX_USE_GLEW_GLUT
		case JVX_GL_RENDER_NATIVE:
			// Deallocation handled in stopped thread.
			break;
#endif
		case JVX_GL_RENDER_EXTERNAL:
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
				{
					_common_set_icon.theData_in->con_data.buffers[i][j] = NULL;
				}
				JVX_DSP_SAFE_DELETE_FIELD(_common_set_icon.theData_in->con_data.buffers[i]);
			}
			JVX_DSP_SAFE_DELETE_FIELD(_common_set_icon.theData_in->con_data.buffers);

			break;
		case JVX_GL_DO_NOT_RENDER:
			for (i = 0; i < _common_set_icon.theData_in->con_data.number_buffers; i++)
			{
				for (j = 0; j < _common_set_icon.theData_in->con_params.number_channels; j++)
				{
					JVX_DSP_SAFE_DELETE_FIELD(_common_set_icon.theData_in->con_data.buffers[i][j]);
				}
				JVX_DSP_SAFE_DELETE_FIELD(_common_set_icon.theData_in->con_data.buffers[i]);
			}
			JVX_DSP_SAFE_DELETE_FIELD(_common_set_icon.theData_in->con_data.buffers);
			break;
		}
		_common_set_icon.theData_in->con_data.buffers = NULL;
	}
	
	res = CjvxVideoNodeTpl<CjvxBareNode1io>::_postprocess_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxViNOpenGLViewer::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	// no idea what mightbe done here. The more important part is the stop function
	return  CjvxVideoNodeTpl<CjvxBareNode1io>::process_buffers_icon(mt_mask, idx_stage);
}

jvxErrorType
CjvxViNOpenGLViewer::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool lostBuffer = false;

	res = CjvxVideoNodeTpl<CjvxBareNode1io>::_process_stop_icon(idx_stage, shift_fwd,
		tobeAccessedByStage, clbk, priv_ptr);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_icon.theData_in->con_params.number_channels >= 1)
		{
			// Input an incoming buffer into the buffer pipeline.
			switch (nativeGl.renderingTarget)
			{
			case JVX_GL_RENDER_EXTERNAL:

				// Hint: _common_set_icon.theData_in->con_data.number_buffers and 
				// genMf_device::expose_visual_if.visual_data_video.rendering_target.ptr->numElmFld /
				//	genMf_device::expose_visual_if.visual_data_video.rendering_target.ptr->numElmFldOneBuf
				// should match!

				// Not in protected area since we only read
				if (genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->fill_height <
					_common_set_icon.theData_in->con_data.number_buffers -
					(_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages + 1))
				{
					// Ok, there is some space within the output buffer!

					// Lock the external viewer buffer
					genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->safe_access.lockf(
						genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->safe_access.priv);

					// Lock the pipeline access buffer
					_common_set_icon.theData_in->con_pipeline.do_lock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
					_common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr].idProcess =
						JVX_SIZE_UNSELECTED;

						// Shift the buffer forward
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr =
							(*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr + 1) %
							_common_set_icon.theData_in->con_data.number_buffers;

						_common_set_icon.theData_in->con_pipeline.reserve_buffer_pipeline_stage[
							*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr].idProcess =
							_common_set_io_common.myRuntimeId;

							// Indicate new buffer available
							genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->fill_height++;

							// Unlock both
							_common_set_icon.theData_in->con_pipeline.do_unlock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
							genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->safe_access.unlockf(
								genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->safe_access.priv);

							// Inform viewer OUTSIDE the critical sections!
							assert(genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->specific.the2DFieldBuffer_full.report_triggerf);
							genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->specific.the2DFieldBuffer_full.report_triggerf(
								genOpenGl_node::expose_visual_if.visual_data_video.rendering_target.ptr->specific.the2DFieldBuffer_full.report_trigger_priv);
				}
				else
				{
					lostBuffer = true;
				}
				break;
			default:

				// Take the incomingbuffer
				if (nativeGl.idxBuf_Fheight < nativeGl.numBufs - 1)
				{
					JVX_LOCK_MUTEX(nativeGl.safeAccessBuffer);
					nativeGl.idxBuf_Fheight++;				
					JVX_UNLOCK_MUTEX(nativeGl.safeAccessBuffer);
				}
				else
				{
					lostBuffer = true;
				}
				break;
			}
			/*
			if (lostBuffer)
			{
				CjvxVideoDevice::rt_info_vd.number_lost_frames.value++;
				// Buffer not yet available, reuse the lastbuffer
			}
			*/
		}
	}
	return res;
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

	nativeGl.bufs = NULL;
	nativeGl.szFld = 0;
	myGlobalNativeGlContext = NULL;
}

void
CjvxViNOpenGLViewer::do_rendering_gl()
{
	if (nativeGl.idxBuf_Fheight > 0)
	{
		cfgRenderStraight cfg;
		cfg.invert_y = 1;
		jvxVideoRenderCore_nobc::prepare_render((const jvxByte**)_common_set_icon.theData_in->con_data.buffers[nativeGl.idxBuf_Read], &cfg);
		JVX_LOCK_MUTEX(nativeGl.safeAccessBuffer);
		nativeGl.idxBuf_Read = (nativeGl.idxBuf_Read + 1) % nativeGl.numBufs;
		nativeGl.idxBuf_Fheight--;
		JVX_UNLOCK_MUTEX(nativeGl.safeAccessBuffer);
		jvxVideoRenderCore_nobc::finalize_render();
	}
}

void
CjvxViNOpenGLViewer::do_idle_gl()
{
	if (nativeGl.idxBuf_Fheight > 0)
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
