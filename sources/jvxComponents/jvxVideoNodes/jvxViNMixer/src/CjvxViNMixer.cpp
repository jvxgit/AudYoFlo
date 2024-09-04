#include "CjvxViNMixer.h"

#ifdef JVX_OPEN_BMP_FOR_TEXT
#include "cbmp.h"
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#define JVX_LOCK_ID_INPUT 1
#define JVX_LOCK_ID_OUTPUT 2

	CjvxViNMixer::CjvxViNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxVideoNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		// Currently, this is all fixed. We may allow different settngs lateron
		video_input.node._common_set_node_params_a_1io.number_channels = 1;
		video_input.node._common_set_node_params_a_1io.segmentation.x = 640;
		video_input.node._common_set_node_params_a_1io.segmentation.y = 480;
		video_input.node._common_set_node_params_a_1io.format = JVX_DATAFORMAT_BYTE;
		video_input.node._common_set_node_params_a_1io.subformat = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
		video_input.node._common_set_node_params_a_1io.data_flow = JVX_DATAFLOW_PUSH_ON_PULL;
		video_input.node._common_set_node_params_a_1io.samplerate = 30;
		video_input.node.derive_buffersize();

		video_output.node._common_set_node_params_a_1io.number_channels = 1;
		video_output.node._common_set_node_params_a_1io.segmentation.x = 640;
		video_output.node._common_set_node_params_a_1io.segmentation.y = 480;
		video_output.node._common_set_node_params_a_1io.format = JVX_DATAFORMAT_BYTE;
		video_output.node._common_set_node_params_a_1io.subformat = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
		video_output.node._common_set_node_params_a_1io.data_flow = JVX_DATAFLOW_PUSH_ACTIVE;
		video_output.node._common_set_node_params_a_1io.samplerate = 30;
		video_output.node.derive_buffersize();

		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::cbRef = this;
		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::lockId = JVX_LOCK_ID_INPUT;

		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::cbRef = this;
		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::lockId = JVX_LOCK_ID_OUTPUT;

		JVX_INITIALIZE_RW_MUTEX(safeCall);

#ifdef JVX_OPEN_BMP_FOR_TEXT

		test.szBufRGBA32 = (video_output.node._common_set_node_params_a_1io.segmentation.x *
			video_output.node._common_set_node_params_a_1io.segmentation.y) *
			jvxDataFormat_getsize(video_output.node._common_set_node_params_a_1io.format)*
			jvxDataFormatGroup_getsize(video_output.node._common_set_node_params_a_1io.subformat);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(test.bufRGBA32, jvxByte, test.szBufRGBA32);

		BMP* BF = bopen("vimix-demo.bmp");
		jvxByte* ptr = test.bufRGBA32;
		if (BF)
		{
			for (jvxSize iy = 0; iy < video_output.node._common_set_node_params_a_1io.segmentation.y; iy++)
			{
				if (iy < get_height(BF))
				{
					for (jvxSize ix = 0; ix < video_output.node._common_set_node_params_a_1io.segmentation.x; ix++)
					{
						if (ix < get_width(BF))
						{
							unsigned char r, g, b;
							get_pixel_rgb(BF, ix, iy, &r, &g, &b);
							*ptr++ = r; // R
							*ptr++ = g; // G
							*ptr++ = b; // B
							*ptr++ = 255; // A
						}
						else
						{
							ptr += 4;
						}
					}
				}
				else
				{
					for (jvxSize ix = 0; ix < video_output.node._common_set_node_params_a_1io.segmentation.x; ix++)
					{
						ptr += 4;
					}
				}
			}
			bclose(BF);
		}
#endif
	}

CjvxViNMixer::~CjvxViNMixer()
{
#ifdef JVX_OPEN_BMP_FOR_TEXT

	JVX_SAFE_DELETE_FIELD(test.bufRGBA32);
	test.bufRGBA32 = nullptr;
	test.szBufRGBA32 = 0;
#endif
	JVX_TERMINATE_RW_MUTEX(safeCall);

}

//  ===================================================================================================

void
CjvxViNMixer::activate_connectors()
{

	oneInputConnectorElement newElmIn; 

	newElmIn.theConnector = static_cast<IjvxInputConnectorSelect*>(this);
	_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName = "mix-in";
	JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen),
		CjvxSingleInputConnectorMulti(true));
	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->activate(this, this,
		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName, this, 0);
	newElmIn.theConnector = CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen;

	// Functional part "CjvxConnectorFactory"
	_common_set_conn_factory.input_connectors[CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen] = newElmIn;

	// ===================================================================

	oneOutputConnectorElement newElmOut;

	newElmOut.theConnector = static_cast<IjvxOutputConnectorSelect*>(this);
	_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;

	CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName = "mix-out";
	JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen), CjvxSingleOutputConnectorMulti(true));
	CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->activate(this, this,
		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName, this, 0);
	newElmOut.theConnector = CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen;
	// Functional part "CjvxConnectorFactory"
	_common_set_conn_factory.output_connectors[CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen] = newElmOut;
	// ===================================================================

	CjvxInputOutputConnector::lds_activate(nullptr, static_cast<IjvxObject*>(this),
		static_cast<IjvxConnectorFactory*>(this), nullptr,
		"video", static_cast<IjvxInputConnector*>(this),
		static_cast<IjvxOutputConnector*>(this));
	CjvxConnectorFactory::_activate_factory(this);

}

	void
	CjvxViNMixer::deactivate_connectors()
	{
		CjvxConnectorFactory::_deactivate_factory();
		_common_set_conn_factory.input_connectors.clear();
		_common_set_conn_factory.output_connectors.clear();
		CjvxInputOutputConnector::lds_deactivate();

		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->deactivate();
		JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen));

		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->deactivate();
		JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen));
	}

	//  ===================================================================================================
	//  ===================================================================================================
	//  ===================================================================================================

	jvxErrorType
		CjvxViNMixer::activate()
	{
		jvxErrorType res = CjvxVideoNode::activate();
		if (res == JVX_NO_ERROR)
		{
			CjvxViNMixer_genpcg::init_all();
			CjvxViNMixer_genpcg::allocate_all();
			CjvxViNMixer_genpcg::register_all(this);

			video_output.node.initialize(this, "video_out", false);
			video_input.node.initialize(this, "video_in", true);
		}
		return res;
	}

	jvxErrorType
		CjvxViNMixer::deactivate()
	{
		jvxErrorType res = CjvxVideoNode::deactivate();
		if (res == JVX_NO_ERROR)
		{
			video_output.node.terminate();
			video_input.node.terminate();
			CjvxViNMixer_genpcg::unregister_all(this);
			CjvxViNMixer_genpcg::deallocate_all();
		}
		return res;
	}

	//  ===================================================================================================
	//  ===================================================================================================

	jvxErrorType
	CjvxViNMixer::report_selected_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		video_input.node.prepare_negotiate(iconn->neg_input);
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_started_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}


	jvxErrorType
	CjvxViNMixer::report_stopped_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_unselected_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
	{
	}

	// ===========================================================================================

	jvxErrorType
	CjvxViNMixer::report_selected_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		video_output.node.prepare_negotiate(oconn->neg_output);
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_started_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_stopped_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_unselected_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::report_process_buffers(CjvxSingleOutputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
	{
	}

	jvxErrorType
	CjvxViNMixer::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_OPEN_BMP_FOR_TEXT
		if (CjvxViNMixer_genpcg::config.replace_video_image.value)
		{
			jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);
			jvxSize minCpy = _common_set_ocon.theData_out.con_params.buffersize * jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format);
			minCpy = JVX_MIN(test.szBufRGBA32, minCpy);

			memcpy(bufsOut[0], test.bufRGBA32, minCpy);

		}
#endif
		this->lock(false, JVX_LOCK_ID_INPUT);
		for (auto& elm : CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::processingConnectors)
		{
			elm.second.ioconn->transfer_backward_icon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_DATA, nullptr JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		}
		this->unlock(false, JVX_LOCK_ID_INPUT);

		// Forward to next chain
		return fwd_process_buffers_icon(mt_mask, idx_stage);
};

	void 
	CjvxViNMixer::lock(jvxBool rwExclusive, jvxSize idLock)
	{
		if (rwExclusive)
		{
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeCall);
		}
		else
		{
			JVX_LOCK_RW_MUTEX_SHARED(safeCall);
		}
	}
	
	void 
		CjvxViNMixer::unlock(jvxBool rwExclusive, jvxSize idLock)
	{
		if (rwExclusive)
		{
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeCall);
		}
		else
		{
			JVX_UNLOCK_RW_MUTEX_SHARED(safeCall);
		}
	}

	void
	CjvxViNMixer::report_proc_connector_started_in_lock(CjvxSingleInputConnector* iconnPlus)
	{
	}

	void 
	CjvxViNMixer::report_proc_connector_before_stop_in_lock(CjvxSingleInputConnector* iconnPlus)
	{
	}

	void 
	CjvxViNMixer::report_proc_connector_started_in_lock(CjvxSingleOutputConnector* oconnPlus)
	{
	}

	void 
		CjvxViNMixer::report_proc_connector_before_stop_in_lock(CjvxSingleOutputConnector* oconnPlus)
	{
	}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
