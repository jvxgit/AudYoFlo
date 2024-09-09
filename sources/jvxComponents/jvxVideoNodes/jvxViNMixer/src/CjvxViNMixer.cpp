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

#ifdef RW_MUTEX
		JVX_INITIALIZE_RW_MUTEX(safeCall);
#else
		JVX_INITIALIZE_MUTEX(safeCall);
#endif

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
	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::cbStartStop = this;
	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::theUniqueId = _common_set.theUniqueId;
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
	CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::cbStartStop = this; 
	CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::theUniqueId = _common_set.theUniqueId;
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
		oneConnectorPlusName<CjvxSingleInputConnector> addme;

		oneConnectorPrivateData* newPrivateData = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newPrivateData, oneConnectorPrivateData);

		jvxSize cnt = 0;
		jvxBool foundId = true;
		while(foundId)
		{ 
			foundId = false;
			for (auto& elm : CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors)
			{
				oneConnectorPrivateData* dat = (oneConnectorPrivateData*)elm.second.privData;
				if (dat->uId == cnt)
				{
					foundId = true;
					cnt++;
					break;
				}
			}
		}
		newPrivateData->uId = cnt;
		
		// Add another entry in list of selected connectors
		addme.ioconn = iconn;
		addme.nmUnique = _common_set_min.theDescription + "-" +
			CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName + "_" + jvx_size2String(uIdGen++);
		addme.privData = newPrivateData;

		CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors[iconn] = addme;
		if (_common_set_icon.theData_in)
		{
			// All parameters as desired
			// jvxLinkDataDescriptor_con_params params = _common_set_icon.theData_in->con_params;

			// Pass it to the connector but do not request a test of the chain - it is in build status anyway
			// iconn->updateFixedProcessingArgs(params);
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		// Directly run the connector test function here!!
		video_input.node.prepare_negotiate(iconn->neg_input);
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_unselected_connector(CjvxSingleInputConnector* iconn)
	{
		auto elm = CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.find(iconn);
		if (elm != CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.end())
		{
			oneConnectorPrivateData* newPrivateData = reinterpret_cast<oneConnectorPrivateData*>(elm->second.privData);
			if (newPrivateData)
			{
				JVX_SAFE_DELETE_OBJ(newPrivateData);
			}
			CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.erase(elm);
			return JVX_NO_ERROR;
		}

		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	void
	CjvxViNMixer::report_started_connector_in_lock(CjvxSingleInputConnector* iconnPlus, jvxHandle* priv)
	{
		oneConnectorPrivateData* myPrivateData = reinterpret_cast<oneConnectorPrivateData*>(priv);
		if(myPrivateData)
		{
			myPrivateData->szFldRgba32 = (video_input.node._common_set_node_params_a_1io.segmentation.x *
			video_input.node._common_set_node_params_a_1io.segmentation.y) *
			jvxDataFormat_getsize(video_input.node._common_set_node_params_a_1io.format) *
			jvxDataFormatGroup_getsize(video_input.node._common_set_node_params_a_1io.subformat);
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(myPrivateData->fldRgba32, jvxByte, myPrivateData->szFldRgba32);
		}
	}

	void
	CjvxViNMixer::report_before_stop_connector_in_lock(CjvxSingleInputConnector* iconnPlus, jvxHandle* priv)
	{
		oneConnectorPrivateData* myPrivateData = reinterpret_cast<oneConnectorPrivateData*>(priv);
		if (myPrivateData)
		{
			JVX_SAFE_DELETE_FIELD(myPrivateData->fldRgba32);
			myPrivateData->szFldRgba32 = 0;			
		}
	}

	void
	CjvxViNMixer::report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
	{
		// Here, we can receive the input data
		jvxByte** imageDataFrom = jvx_process_icon_extract_output_buffers<jvxByte>(datThisConnector);

		// Lock the list of active connections
		
		// this->lock(false, CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::lockId);
		/* <-we must not hold the lock here!If we come along here, the mixer has triggered the data within the lock!! */
		
		auto elm = CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.find(iconn);
		if (elm != CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::selectedConnectors.end())
		{
			oneConnectorPrivateData* localData = (oneConnectorPrivateData*)elm->second.privData;
			if (localData)
			{
				if (localData->szFldRgba32 == datThisConnector.con_params.buffersize)
				{
					memcpy(localData->fldRgba32, imageDataFrom[0], localData->szFldRgba32);
				}
			}
		}

		// this->unlock(false, CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::lockId);
		/* <-we must not hold the lock here!If we come along here, the mixer has triggered the data within the lock!! */

		if (_common_set_ocon.theData_out.con_params.buffersize == datThisConnector.con_params.buffersize)
		{
			jvxByte** imageDataTo = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);
			memcpy(imageDataTo[0], imageDataFrom[0], _common_set_ocon.theData_out.con_params.buffersize);
		}
	}

	// ===========================================================================================

	jvxErrorType
	CjvxViNMixer::report_selected_connector(CjvxSingleOutputConnector* oconn)
	{
		oneConnectorPlusName<CjvxSingleOutputConnector> addme;

		oneConnectorPrivateData* newPrivateData = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(newPrivateData, oneConnectorPrivateData);

		// Add another entry in list of selected connectors
		addme.ioconn = oconn;
		addme.nmUnique = _common_set_min.theDescription + "-" +
			CjvxConnectorCollection < CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName + "_" + jvx_size2String(uIdGen++);
		addme.privData = newPrivateData;

		CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::selectedConnectors[oconn] = addme;
		if (_common_set_icon.theData_in)
		{
			// All parameters as desired
			// jvxLinkDataDescriptor_con_params params = _common_set_icon.theData_in->con_params;

			// Pass it to the connector but do not request a test of the chain - it is in build status anyway
			// iconn->updateFixedProcessingArgs(params);
		}		
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		video_output.node.prepare_negotiate(oconn->neg_output);
		return JVX_NO_ERROR;
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
		
		jvxByte** bufsIn = jvx_process_icon_extract_input_buffers<jvxByte>(_common_set_icon.theData_in, idx_stage);

		jvxByte** bufsOut = jvx_process_icon_extract_output_buffers<jvxByte>(_common_set_ocon.theData_out);

#ifdef JVX_OPEN_BMP_FOR_TEXT
		if (CjvxViNMixer_genpcg::config.replace_video_image.value)
		{
			jvxSize minCpy = _common_set_ocon.theData_out.con_params.buffersize * jvxDataFormat_getsize(_common_set_ocon.theData_out.con_params.format);
			minCpy = JVX_MIN(test.szBufRGBA32, minCpy);

			memcpy(bufsOut[0], test.bufRGBA32, minCpy);

		}
#endif
		this->lock(false, CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::lockId);
		for (auto& elm : CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::processingConnectors)
		{
			// All connected outputs should have been "buffer started"
			if (elm.second.ioconn->_common_set_ocon.theData_out.con_params.buffersize == _common_set_icon.theData_in->con_params.buffersize)
			{
				jvxByte** bufsOutLoc = jvx_process_icon_extract_output_buffers<jvxByte>(elm.second.ioconn->_common_set_ocon.theData_out);
				memcpy(bufsOutLoc[0], bufsIn[0], elm.second.ioconn->_common_set_ocon.theData_out.con_params.buffersize);
			}
		}
		this->unlock(false, CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::lockId);

		this->lock(false, CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::lockId);
		for (auto& elm : CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::processingConnectors)
		{
			elm.second.ioconn->transfer_backward_icon(jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_DATA, nullptr JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
		}
		this->unlock(false, CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::lockId);

		// Forward the input video to the output connections
		this->lock(false, CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::lockId);
		for (auto& elm : CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::processingConnectors)
		{
			if (elm.second.ioconn->_common_set_ocon.theData_out.con_link.connect_to)
			{
				elm.second.ioconn->_common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
			}
		}
		this->unlock(false, CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::lockId);

		// Forward to next chain
		return fwd_process_buffers_icon(mt_mask, idx_stage);
};

	void 
	CjvxViNMixer::lock(jvxBool rwExclusive, jvxSize idLock)
	{
		if (rwExclusive)
		{
#ifdef RW_MUTEX
			JVX_LOCK_RW_MUTEX_EXCLUSIVE(safeCall);
			tIdEx = JVX_GET_CURRENT_THREAD_ID();
#else
			JVX_LOCK_MUTEX(safeCall);
#endif

		}
		else
		{
#ifdef RW_MUTEX
			JVX_LOCK_RW_MUTEX_SHARED(safeCall);
			tIdSh = JVX_GET_CURRENT_THREAD_ID();
#else
			JVX_LOCK_MUTEX(safeCall);
#endif
		}
		
	}
	
	void 
		CjvxViNMixer::unlock(jvxBool rwExclusive, jvxSize idLock)
	{

		if (rwExclusive)
		{
#ifdef RW_MUTEX
			tIdEx = 0;
			JVX_UNLOCK_RW_MUTEX_EXCLUSIVE(safeCall);
#else
			JVX_UNLOCK_MUTEX(safeCall);
#endif
		}
		else
		{
#ifdef RW_MUTEX
			tIdSh = 0;
			JVX_UNLOCK_RW_MUTEX_SHARED(safeCall);
#else
			JVX_UNLOCK_MUTEX(safeCall);
#endif
		}
	}

	

	void 
	CjvxViNMixer::report_started_connector_in_lock(CjvxSingleOutputConnector* oconnPlus, jvxHandle* priv)
	{
	}

	void 
		CjvxViNMixer::report_before_stop_connector_in_lock(CjvxSingleOutputConnector* oconnPlus, jvxHandle* priv)
	{
	}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
