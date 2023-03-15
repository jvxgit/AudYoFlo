#include "CjvxAuNMeasureIr2Dev.h"

CjvxAuNMeasureIr2Dev::CjvxAuNMeasureIr2Dev(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAuNTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	theObjDeployHdl = NULL;
	theObjDeployId = JVX_COMPONENT_UNKNOWN;
	theObjDeployDev = NULL;
	theObjDeployProps = NULL;
	theObjDeployIf = NULL;

	inProcessing.bufs_received_fld = NULL;
	inProcessing.bufs_received_num = 0;
	inProcessing.bufs_to_emit_fld = NULL;
	inProcessing.bufs_to_emit_num = 0;
	inProcessing.bsize = 0;
	inProcessing.srate = 0;
	inProcessing.form = JVX_DATAFORMAT_NONE;
	inProcessing.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
}

CjvxAuNMeasureIr2Dev::~CjvxAuNMeasureIr2Dev()
{
}

jvxErrorType 
CjvxAuNMeasureIr2Dev::activate()
{
	jvxErrorType res = CjvxAuNTasks::activate();
	if (res == JVX_NO_ERROR)
	{
	}
	return res;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::system_ready()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theHostObj = NULL;
	IjvxHost* theHostHdl = NULL;
	jvxComponentIdentification cpHost;
	jvxComponentIdentification cpDev;
	_common_set_min.theHostRef->object_hidden_interface(&theHostObj);
	if (theHostObj)
	{
		theHostObj->request_specialization(reinterpret_cast<jvxHandle**>(&theHostHdl), &cpHost, NULL);
		assert(cpHost.tp == JVX_COMPONENT_HOST);
		if (theHostHdl)
		{
			res = jvx_activateComponentAsOwnerDevice(theHostHdl,
				JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY,
				"signal_processing_technology/CjvxSignalProcessingTechnologyDeploy",
				theObjDeployHdl, theObjDeployId, static_cast<IjvxObject*>(this));
			assert(res == JVX_NO_ERROR);

			theObjDeployHdl->request_specialization(reinterpret_cast<jvxHandle**>(&theObjDeployDev), &cpDev, NULL);
			if (theObjDeployDev && cpDev.tp == JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE)
			{
				theObjDeployDev->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theObjDeployProps));
				if (theObjDeployProps)
				{
					jvxCallManagerProperties callGt;
					jvx_get_property(theObjDeployProps, 
						reinterpret_cast<jvxHandle*>(&theObjDeployIf),
						0, 1, JVX_DATAFORMAT_HANDLE, true,
						"/ext_access",
						callGt);
					if (theObjDeployIf)
					{
						res = theObjDeployIf->setDataInteractReference(static_cast<IjvxSignalProcessingDeploy_interact*>(this));
						assert(res == JVX_NO_ERROR);

						passParametersToDeployModule();
					}
				}

			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::system_about_to_shutdown()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theHostObj = NULL;
	IjvxHost* theHostHdl = NULL;
	jvxComponentIdentification cpHost;
	_common_set_min.theHostRef->object_hidden_interface(&theHostObj);
	if (theHostObj)
	{
		theHostObj->request_specialization(reinterpret_cast<jvxHandle**>(&theHostHdl), &cpHost, NULL);
		assert(cpHost.tp == JVX_COMPONENT_HOST);
		if (theHostHdl)
		{
			res = jvx_deactivateComponentAsOwnerDevice(theHostHdl,
				theObjDeployId);
			assert(res == JVX_NO_ERROR);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNMeasureIr2Dev::deactivate()
{
	return CjvxAuNTasks::deactivate();
}

jvxErrorType
CjvxAuNMeasureIr2Dev::process_request_data_start(jvxLinkDataDescriptor* dataOut)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNMeasureIr2Dev::process_request_data_stop(jvxLinkDataDescriptor* dataIn) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
	CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxAuNTasks::test_connect_icon_ntask(theData_in,
		theData_out, idCtxt,
		refto JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		passParametersToDeployModule();
	}
	return res;
}

#define JVX_MATCH_DATA_DESCRIPTOR_BSIZE_SHIFT 0
#define JVX_MATCH_DATA_DESCRIPTOR_SRATE_SHIFT 1
#define JVX_MATCH_DATA_DESCRIPTOR_FORMAT_SHIFT 2
#define JVX_MATCH_DATA_DESCRIPTOR_SUBFORMAT_SHIFT 3
#define JVX_MATCH_DATA_DESCRIPTOR_SEGX_SHIFT 4
#define JVX_MATCH_DATA_DESCRIPTOR_SEGY_SHIFT 5
#define JVX_MATCH_DATA_DESCRIPTOR_NCHANS_SHIFT 6

jvxBool
jvx_check_match_params(jvxLinkDataDescriptor_con_params& one,
	jvxLinkDataDescriptor_con_params& theother,
	jvxCBitField what)
{
	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_BSIZE_SHIFT))
	{
		if (one.buffersize != theother.buffersize)
		{
			return false;
		}
	}

	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_SRATE_SHIFT))
	{
		if (one.rate != theother.rate)
		{
			return false;
		}
	}

	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_FORMAT_SHIFT))
	{
		if (one.format != theother.format)
		{
			return false;
		}
	}

	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_SUBFORMAT_SHIFT))
	{
		if (one.format_group != theother.format_group)
		{
			return false;
		}
	}

	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_SEGX_SHIFT))
	{
		if (one.segmentation.x != theother.segmentation.x)
		{
			return false;
		}
	}

	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_SEGY_SHIFT))
	{
		if (one.segmentation.y != theother.segmentation.y)
		{
			return false;
		}
	}
	if (jvx_bitTest(what, JVX_MATCH_DATA_DESCRIPTOR_NCHANS_SHIFT))
	{
		if (one.number_channels != theother.number_channels)
		{
			return false;
		}
	}
	return true;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::passParametersToDeployModule()
{
	jvxSize i;
	jvxSize nchans_out = 0;
	jvxSize nchans_in = 0;
	jvxBool oneConnected = false;
	for (i = 0; i < _common_set_node_ntask.fixedTasks.size(); i++)
	{
		if (_common_set_node_ntask.fixedTasks[i].icon->_common_set_icon_nvtask.theData_in)
		{
			nchans_out += _common_set_node_ntask.fixedTasks[i].icon->_common_set_icon_nvtask.theData_in->con_params.number_channels;
			nchans_in += _common_set_node_ntask.fixedTasks[i].ocon->_common_set_ocon_nvtask.theData_out.con_params.number_channels;

			if (!oneConnected)
			{
				params_receive = _common_set_node_ntask.fixedTasks[i].icon->_common_set_icon_nvtask.theData_in->con_params;
				params_to_emit = _common_set_node_ntask.fixedTasks[i].ocon->_common_set_ocon_nvtask.theData_out.con_params;
				oneConnected = true;
			}
			else
			{
				if (!jvx_check_match_params(params_receive, 
					_common_set_node_ntask.fixedTasks[i].icon->_common_set_icon_nvtask.theData_in->con_params,
					(JVX_MATCH_DATA_DESCRIPTOR_BSIZE_SHIFT |
					 JVX_MATCH_DATA_DESCRIPTOR_SRATE_SHIFT |
					 JVX_MATCH_DATA_DESCRIPTOR_FORMAT_SHIFT |
					 JVX_MATCH_DATA_DESCRIPTOR_SUBFORMAT_SHIFT |
					 JVX_MATCH_DATA_DESCRIPTOR_SEGX_SHIFT |
					 JVX_MATCH_DATA_DESCRIPTOR_SEGY_SHIFT )))
				{
					// Error
				}
			}
		}
	}

	params_receive.number_channels = nchans_out;
	params_to_emit.number_channels = nchans_in;

	if (theObjDeployIf)
	{
		theObjDeployIf->setOutputParameters(&params_receive);
		theObjDeployIf->setInputParameters(&params_to_emit);
		theObjDeployIf->triggerTest();
	}

	/*
	std::cout << "========================================" << std::endl;
	std::cout << "Output Channels = " << nchans_out << std::endl;
	std::cout << "Input Channels = " << nchans_in << std::endl;
	std::cout << "========================================" << std::endl;
	*/
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::process_io(
	jvxLinkDataDescriptor* pri_in, jvxSize idx_stage_pri_in,
	jvxLinkDataDescriptor* sec_in, jvxSize idx_stage_sec_in,
	jvxLinkDataDescriptor* pri_out, jvxLinkDataDescriptor* sec_out)
{
	jvxSize i;
	jvxSize cnt = 0;
	jvxByte** buf = NULL;
	jvxLinkDataAttachedLostFrames lostFrames;
	jvxLinkDataAttachedChain* ptr = NULL;

	jvxSize idx_stage_pri_out = *pri_out->con_pipeline.idx_stage_ptr;
	jvxSize idx_stage_sec_out = *sec_out->con_pipeline.idx_stage_ptr;
	buf = (jvxByte**)pri_in->con_data.buffers[idx_stage_pri_in];
	for (i = 0; i < pri_in->con_params.number_channels; i++)
	{
		if (cnt < inProcessing.bufs_received_num)
		{
			inProcessing.bufs_received_fld[cnt] = buf[i];
		}
		cnt++;
	}
	buf = (jvxByte**)sec_in->con_data.buffers[idx_stage_pri_in];
	for (i = 0; i < sec_in->con_params.number_channels; i++)
	{
		if (cnt < inProcessing.bufs_received_num)
		{
			inProcessing.bufs_received_fld[cnt] = buf[i];
		}
		cnt++;
	}
	
	cnt = 0;
	buf = (jvxByte**)pri_out->con_data.buffers[idx_stage_pri_out];
	for (i = 0; i < pri_out->con_params.number_channels; i++)
	{
		if (cnt < inProcessing.bufs_to_emit_num)
		{
			inProcessing.bufs_to_emit_fld[cnt] = buf[i];
		}
		cnt++;
	}
	buf = (jvxByte**)sec_out->con_data.buffers[idx_stage_sec_out];
	for (i = 0; i < sec_out->con_params.number_channels; i++)
	{
		if (cnt < inProcessing.bufs_to_emit_num)
		{
			inProcessing.bufs_to_emit_fld[cnt] = buf[i];
		}
		cnt++;
	}

	if (in_proc.numGlitchesProcessing > numGlitchesReported)
	{
		// lostFrames.tp = JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES; <- type is set on constructor already
		numGlitchesReported = in_proc.numGlitchesProcessing;
		lostFrames.numLost = numGlitchesReported;
		ptr = &lostFrames;
	}

	// We pass the second argument to the deploy function. In that deploy function, the content is copied
	theObjDeployIf->exchangeData(&inProcessing, ptr);

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNMeasureIr2Dev::prepare() 
{
	jvxErrorType res = CjvxAuNTasks::prepare();
	if (res == JVX_NO_ERROR)
	{
		inProcessing.bufs_received_num = params_receive.number_channels;
		inProcessing.bufs_to_emit_num = params_to_emit.number_channels;

		inProcessing.bsize = params_receive.buffersize;
		inProcessing.srate = params_receive.rate;
		inProcessing.form = params_receive.format;
		inProcessing.format_group = params_receive.format_group;

		inProcessing.bufs_received_fld = NULL;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(
			inProcessing.bufs_received_fld,
			jvxByte*,
			inProcessing.bufs_received_num);

		inProcessing.bufs_to_emit_fld = NULL;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(
			inProcessing.bufs_to_emit_fld,
			jvxByte*,
			inProcessing.bufs_to_emit_num);

		numGlitchesReported = 0;
	}
	return res;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::postprocess()
{
	jvxErrorType res = CjvxAuNTasks::postprocess();
	if (res == JVX_NO_ERROR)
	{
		JVX_DSP_SAFE_DELETE_FIELD(inProcessing.bufs_received_fld);
		JVX_DSP_SAFE_DELETE_FIELD(inProcessing.bufs_to_emit_fld);

		inProcessing.bufs_received_num = 0;
		inProcessing.bufs_to_emit_num = 0;

		inProcessing.bsize = 0;
		inProcessing.srate = 0;
		inProcessing.form = JVX_DATAFORMAT_NONE;
		inProcessing.format_group = JVX_DATAFORMAT_GROUP_NONE;

		numGlitchesReported = 0;
	}
	return res;
}

jvxErrorType
CjvxAuNMeasureIr2Dev::is_ready(jvxBool* isReady, jvxApiString* reason)
{
	return CjvxAuNTasks::is_ready(isReady, reason);
}
