#include "jvx.h"
#include "CjvxBareNode1ioMex.h"

CjvxBareNode1ioMex::CjvxBareNode1ioMex(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theComponentSubTypeDescriptor = "signal_processing_node/audio_node";
	
	_theExtCallObjectNameBase = "";

	_common_set_ldslave.num_min_buffers_in = 1;
	_common_set_ldslave.zeroCopyBuffering_cfg = true; // <- works only with this option
	_common_set_ldslave.num_additional_pipleline_stages = 0;
}

CjvxBareNode1ioMex::~CjvxBareNode1ioMex()
{
}

jvxErrorType
CjvxBareNode1ioMex::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename,
	jvxInt32 lineno)
{
	std::vector<std::string> warnings;
	jvxErrorType res = CjvxBareNode1io::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	if (res != JVX_NO_ERROR)
		return res;

	res = CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno);
	this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	return res;
}

jvxErrorType
CjvxBareNode1ioMex::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res != JVX_NO_ERROR)
		return res;

	res = CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}

jvxErrorType
CjvxBareNode1ioMex::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
	jvxErrorType resF = JVX_NO_ERROR;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		// Accepts and modify some parameters
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = _common_set_ldslave.num_additional_pipleline_stages;
		_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_ldslave.theData_in->con_data.number_buffers + _common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages,
			_common_set_ldslave.num_min_buffers_in);

		// Do the processing checks and allocate buffers etc
		// Setup has been verified in the test chain functions before - this is only for simpler access during processing
		node_inout._common_set_node_params_a_1io.buffersize = _common_set_ldslave.theData_in->con_params.buffersize;
		node_inout._common_set_node_params_a_1io.number_channels = _common_set_ldslave.theData_in->con_params.number_channels;
		node_output._common_set_node_params_a_1io.number_channels = _common_set_ldslave.theData_out.con_params.number_channels;
		node_inout._common_set_node_params_a_1io.samplerate = _common_set_ldslave.theData_in->con_params.rate;
		node_inout._common_set_node_params_a_1io.format = _common_set_ldslave.theData_in->con_params.format;

		// Prepare next processing stage processing
		// The only deviation from the input side may be the number of output channels - which is taken from the node parameter set
		//assert(_common_set_ldslave.theData_out.con_params.buffersize == _common_set_node_params_1io.processing.buffersize);
		//assert(_common_set_ldslave.theData_out.con_params.format == _common_set_node_params_1io.processing.format);
		assert(_common_set_ldslave.theData_out.con_data.buffers == NULL);
		//assert(_common_set_ldslave.theData_out.con_params.number_channels == _common_set_node_params_1io.processing.number_output_channels);
		//assert(_common_set_ldslave.theData_out.con_params.rate == _common_set_node_params_1io.processing.samplerate);
#if 1
		zeroCopyBuffering_rt = _common_set_ldslave.zeroCopyBuffering_cfg;
		if (
			(jvx_bitTest(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)) ||
			(jvx_bitTest(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT)))
		{
			zeroCopyBuffering_rt = false;
		}

		// Constraints fulfilled for zerocopy?
		if (!
			(
			(_common_set_ldslave.theData_out.con_params.number_channels == _common_set_ldslave.theData_in->con_params.number_channels) &&
			(_common_set_ldslave.theData_out.con_params.rate == _common_set_ldslave.theData_in->con_params.rate) &&
			(_common_set_ldslave.theData_out.con_params.buffersize == _common_set_ldslave.theData_in->con_params.buffersize) &&
			(_common_set_ldslave.theData_out.con_params.format == _common_set_ldslave.theData_in->con_params.format)))
		{
			zeroCopyBuffering_rt = false;
		}

		if (zeroCopyBuffering_rt)
		{
			// Set the number of buffers on the output identical to the input side
			_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.theData_in->con_data.number_buffers;
			_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
			///jvx_bitClear(_common_set_ldslave.theData_out.con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT)
		}
		else
		{
			// Set the number of buffers as desired
			_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
			_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
		}

		// Only little to do: Attach a user hint here
		jvxErrorType resL = CjvxMexCalls::prepare_sender_to_receiver(_common_set_ldslave.theData_in);

		// We might attach user hints here!!!
		// _common_set_ldslave.theData_out.con_data.user_hints = _common_set_ldslave.theData_in->con_data.user_hints;

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			if (zeroCopyBuffering_rt)
			{
				// Link the buffers directly to output
				res = allocate_pipeline_and_buffers_prepare_to_zerocopy();
				assert(res == JVX_NO_ERROR);
				if (jvx_bitTest(_common_set_ldslave.theData_out.con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT))
				{
					jvx_bitSet(_common_set_ldslave.theData_in->con_data.alloc_flags, 
						(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
				}
			}
			else
			{
				res = allocate_pipeline_and_buffers_prepare_to();
				assert(res == JVX_NO_ERROR);
				jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, 
					(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
			}

			// This is for convenience for the MeX interface: support old type of processing principle
			_common_set_ldslave.theData_in->con_compat.buffersize =
				_common_set_ldslave.theData_out.con_params.buffersize;
			_common_set_ldslave.theData_in->con_compat.format =
				_common_set_ldslave.theData_out.con_params.format;
			_common_set_ldslave.theData_in->con_compat.from_receiver_buffer_allocated_by_sender =
				_common_set_ldslave.theData_out.con_data.buffers;
			_common_set_ldslave.theData_in->con_compat.number_buffers =
				_common_set_ldslave.theData_out.con_data.number_buffers;
			_common_set_ldslave.theData_in->con_compat.number_channels =
				_common_set_ldslave.theData_out.con_params.number_channels;
			_common_set_ldslave.theData_in->con_compat.rate =
				_common_set_ldslave.theData_out.con_params.rate;

			_common_set_ldslave.theData_in->con_compat.ext.segmentation_x =
				_common_set_ldslave.theData_out.con_params.segmentation_x;
			_common_set_ldslave.theData_in->con_compat.ext.segmentation_y =
				_common_set_ldslave.theData_out.con_params.segmentation_y;
			_common_set_ldslave.theData_in->con_compat.ext.subformat =
				_common_set_ldslave.theData_out.con_params.format_group;
			_common_set_ldslave.theData_in->con_compat.ext.hints =
				_common_set_ldslave.theData_out.con_params.hints;

			// Link the fully allocated buffers
			resL = CjvxMexCalls::prepare_complete_receiver_to_sender(_common_set_ldslave.theData_in, &_common_set_ldslave.theData_out);
		}
		else
		{
			resL = CjvxMexCalls::postprocess_sender_to_receiver(_common_set_ldslave.theData_in);
		}

#else
		// !!! No zero copy allowed !!!
			
		// Set the number of buffers as desired
		_common_set_ldslave.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
		_common_set_ldslave.theData_out.con_data.alloc_flags = _common_set_ldslave.theData_in->con_data.alloc_flags;
		jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
		jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);

		// Only little to do: Attach a user hint here
		jvxErrorType resL = CjvxMexCalls::prepare_sender_to_receiver(_common_set_ldslave.theData_in);

		// We might attach user hints here!!!
		_common_set_ldslave.theData_out.con_data.user_hints = _common_set_ldslave.theData_in->con_data.user_hints;

		res = _prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			res = allocate_pipeline_and_buffers_prepare_to();
			assert(res == JVX_NO_ERROR);
			jvx_bitClear(_common_set_ldslave.theData_in->con_data.alloc_flags, JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

			// This is for convenience for the MeX interface: support old type of processing principle
			_common_set_ldslave.theData_in->con_compat.buffersize =
				_common_set_ldslave.theData_out.con_params.buffersize;
			_common_set_ldslave.theData_in->con_compat.format =
				_common_set_ldslave.theData_out.con_params.format;
			_common_set_ldslave.theData_in->con_compat.from_receiver_buffer_allocated_by_sender =
				_common_set_ldslave.theData_out.con_data.buffers;
			_common_set_ldslave.theData_in->con_compat.number_buffers =
				_common_set_ldslave.theData_out.con_data.number_buffers;
			_common_set_ldslave.theData_in->con_compat.number_channels =
				_common_set_ldslave.theData_out.con_params.number_channels;
			_common_set_ldslave.theData_in->con_compat.rate =
				_common_set_ldslave.theData_out.con_params.rate;
			_common_set_ldslave.theData_in->con_compat.ext =
				_common_set_ldslave.theData_out.con_data.ext;

			// Linke the fully allocated buffers
			resL = CjvxMexCalls::prepare_complete_receiver_to_sender(_common_set_ldslave.theData_in, &_common_set_ldslave.theData_out);
		}
		else
		{
			resL = CjvxMexCalls::pos(_common_set_ldslave.theData_in, &_common_set_ldslave.theData_out);
		}


#endif

	}

	return res;
};

jvxErrorType
CjvxBareNode1ioMex::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool forward = true;
	if (_common_set_min.theState == JVX_STATE_PREPARED)
	{
		jvxErrorType resL = CjvxMexCalls::before_postprocess_receiver_to_sender(_common_set_ldslave.theData_in);

		// Remove all simplifying shortcuts
		_common_set_ldslave.theData_in->con_compat.buffersize = 0;
		_common_set_ldslave.theData_in->con_compat.format = JVX_DATAFORMAT_NONE;
		_common_set_ldslave.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
		_common_set_ldslave.theData_in->con_compat.number_buffers = 0;
		_common_set_ldslave.theData_in->con_compat.number_channels = 0;
		_common_set_ldslave.theData_in->con_compat.rate = 0;
		_common_set_ldslave.theData_in->con_compat.ext.hints = 0;
		_common_set_ldslave.theData_in->con_compat.ext.segmentation_x = 0;
		_common_set_ldslave.theData_in->con_compat.ext.segmentation_y = 0;
		_common_set_ldslave.theData_in->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;

		res = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		// Remove the user hints
		resL = CjvxMexCalls::postprocess_sender_to_receiver(_common_set_ldslave.theData_in);

		// The only deviation may be the number of output channels - which is taken from the node parameter set
		_common_set_ldslave.theData_out.con_data.number_buffers = 0;

		if (zeroCopyBuffering_rt)
		{
			res = deallocate_pipeline_and_buffers_postprocess_to_zerocopy();
		}
		else
		{
			res = deallocate_pipeline_and_buffers_postprocess_to();
		}

		_common_set_ldslave.theData_in->con_data.number_buffers = 0;
		_common_set_ldslave.theData_in->con_pipeline.num_additional_pipleline_stages = 0;

		zeroCopyBuffering_rt = false;
	}

	return res;
}


jvxErrorType
CjvxBareNode1ioMex::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool extCallPres = false;
	jvxBool matlabEngaged = false;

	res = CjvxMexCalls::is_extcall_reference_present(&extCallPres);
	if (extCallPres)
	{
		CjvxMexCalls::is_matlab_processing_engaged(&matlabEngaged);
	}

	if (matlabEngaged)
	{
		// Pass control to Matlab processing
		res = CjvxMexCalls::process_st(_common_set_ldslave.theData_in, 
			*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr, 
			*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr);
	}
	else
	{
		if (!zeroCopyBuffering_rt)
		{
			res = process_oplace(_common_set_ldslave.theData_in, &_common_set_ldslave.theData_out);
		}
		else
		{
			res = process_iplace(&_common_set_ldslave.theData_out);
		}

		if (res != JVX_NO_ERROR)
		{
			return res;
		}

	}

	// Forward to next chain
	return _process_buffers_icon( mt_mask,  idx_stage);
}

jvxErrorType 
CjvxBareNode1ioMex::select(IjvxObject* theOwner)
{ 
	jvxErrorType res = JVX_NO_ERROR; 
	res = CjvxBareNode1io::select(theOwner);
	
	if (res == JVX_NO_ERROR) 
	{ 
		IjvxToolsHost* theTools = NULL;
		IjvxObject* theObj = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theTools));
		if ((res == JVX_NO_ERROR) && theTools)
		{
			res = theTools->reference_tool(JVX_COMPONENT_EXTERNAL_CALL, &theObj, 0, NULL);
			if ((res == JVX_NO_ERROR) && theObj)
			{
				res = theObj->request_specialization(reinterpret_cast<jvxHandle**>(&_theExtCallHandlerBase), NULL, NULL);
			}
			res = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theTools));
		}

		/*
		 * This function calls the member function of the derived class which then registers all
		 * local functions. Also, It sets the name of the component which is then forwarded to the
		 * registration of the "basic" matlab entry points of class CjvxMexCall.
		 */
		init_external_call_local(_theExtCallObjectNameBase); // initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), compNameStr);
		

		// Call the following in the initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), compNameStr); 
		res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this), "", _theExtCallObjectNameBase.c_str()); 
	}
	return(res);
}

jvxErrorType
CjvxBareNode1ioMex::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxBareNode1io::unselect();
	if (res == JVX_NO_ERROR)
	{
		// unregister all basic member functions first
		res = CjvxMexCalls::unselect();

		// Pass control to the derived class
		terminate_external_call_local();

		IjvxToolsHost* theTools = NULL;
		IjvxObject* theObj = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		res = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theTools));
		if ((res == JVX_NO_ERROR) && theTools)
		{
			res = theTools->return_reference_tool(JVX_COMPONENT_EXTERNAL_CALL, static_cast<IjvxObject*>(_theExtCallHandlerBase));
			_theExtCallHandlerBase = NULL;
			_common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theTools));
		}
		_theExtCallObjectNameBase = "";
		return(res);
	} 
	return(res); 
}

// Some default implementations
JVX_MEXCALL_ACTIVATE(CjvxBareNode1ioMex, CjvxBareNode1io)
JVX_MEXCALL_DEACTIVATE(CjvxBareNode1ioMex, CjvxBareNode1io)
JVX_MEXCALL_PREPARE(CjvxBareNode1ioMex, CjvxBareNode1io)
JVX_MEXCALL_POSTPROCESS(CjvxBareNode1ioMex, CjvxBareNode1io)

// =====================================================================
// Local implementation of processing callbacks
// =====================================================================

void
CjvxBareNode1ioMex::init_external_call_local(std::string& _theExtCallObjectNameBase)
{
	// Call <initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), compNameStr);> in derived class
}

void
CjvxBareNode1ioMex::terminate_external_call_local()
{
	// Call <terminateExternalCallhandler(_theExtCallHandler);> in the derived class 
}

jvxErrorType
CjvxBareNode1ioMex::process_oplace(jvxLinkDataDescriptor* data_in, jvxLinkDataDescriptor* data_out)
{
	// Talkthrough
	jvxSize ii;
	jvxSize maxChans = JVX_MAX(data_in->con_params.number_channels, data_out->con_params.number_channels);
	jvxSize minChans = JVX_MIN(data_in->con_params.number_channels, data_out->con_params.number_channels);

	// This default function does not tolerate a lot of unexpected settings
	assert(data_in->con_params.format == data_out->con_params.format);
	assert(data_in->con_params.buffersize == data_out->con_params.buffersize);

	if (minChans)
	{
		for (ii = 0; ii < maxChans; ii++)
		{
			jvxSize idxIn = ii % data_in->con_params.number_channels;
			jvxSize idxOut = ii % data_out->con_params.number_channels;

			assert(data_in->con_params.format == data_out->con_params.format);
			jvx_convertSamples_memcpy(
				data_in->con_data.buffers[
					*data_in->con_pipeline.idx_stage_ptr][idxIn],
				data_out->con_data.buffers[
					*data_out->con_pipeline.idx_stage_ptr][idxOut],
				jvxDataFormat_size[data_in->con_params.format],
				data_in->con_params.buffersize);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareNode1ioMex::process_iplace(jvxLinkDataDescriptor* data_out)
{
	return JVX_NO_ERROR;
}
