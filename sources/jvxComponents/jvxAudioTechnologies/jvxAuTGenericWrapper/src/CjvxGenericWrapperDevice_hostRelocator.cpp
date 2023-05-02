#include "CjvxGenericWrapperDevice.h"
#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapperDevice_hostRelocator.h"

CjvxGenericWrapperDevice_hostRelocator::CjvxGenericWrapperDevice_hostRelocator()
{
	runtime.refDevice = NULL;
    runtime.refReport = NULL;
    runtime.cntRefReport = 0;

	theConnFac_dev = NULL;
	theMasterFac_dev = NULL;
	theConnFac_me = NULL;

	icon_dev = NULL;
	icon_me = NULL;
	mas_dev = NULL;
	ocon_dev = NULL;
	ocon_me = NULL;
	id_connection_process = JVX_SIZE_UNSELECTED;
	hdl_connection_process = NULL;;
	id_device2me = JVX_SIZE_UNSELECTED;
	id_me2device = JVX_SIZE_UNSELECTED;

	this->_set_unique_descriptor("generic wrapper relocator");
}

CjvxGenericWrapperDevice_hostRelocator::~CjvxGenericWrapperDevice_hostRelocator()
{
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::reference_component(
	jvxComponentIdentification* cpTp,
	jvxApiString* modName,
	jvxApiString* lContext) 
{
	// Re-locate the component information towards generic wrapper
	return runtime.refDevice->_reference_component(cpTp, modName, lContext);
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::supports_connector_class_ocon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (format_group == JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::supports_connector_class_icon(
	jvxDataFormatGroup format_group,
	jvxDataflow data_flow)
{
	if (format_group == JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::activate()
{
	moduleNameLocal = runtime.refDevice->_common_set.theModuleName + "-relocator";
	JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, runtime.refDevice, 
		"default", NULL, moduleNameLocal.c_str());
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::deactivate()
{
	JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	return JVX_NO_ERROR;
}

void
CjvxGenericWrapperDevice_hostRelocator::setReference(CjvxGenericWrapperDevice* refDevice)
{
    jvxErrorType res = JVX_NO_ERROR;
	runtime.refDevice = refDevice;
    runtime.refReport = NULL;
    runtime.cntRefReport = 0;
}

void
CjvxGenericWrapperDevice_hostRelocator::resetReference()
{
    assert(runtime.cntRefReport == 0); // Mmh, could lead to many errors but should be correct..
    runtime.refReport = NULL;
	runtime.refDevice = NULL;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_select_master()
{
	jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.refDevice->onInit.connectedDevice)
	{
		res = runtime.refDevice->onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&theConnFac_dev));
		assert(res == JVX_NO_ERROR); 
		
		res = runtime.refDevice->onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_CONNECTOR_MASTER_FACTORY, reinterpret_cast<jvxHandle**>(&theMasterFac_dev));
		assert(res == JVX_NO_ERROR); 
		
		theConnFac_me = static_cast<IjvxConnectorFactory*>(this);

		assert(theConnFac_dev);
		assert(theMasterFac_dev);
		assert(theConnFac_me);

		res = theConnFac_dev->number_input_connectors(&numi);
		assert(res == JVX_NO_ERROR); 
		assert(numi > 0);
		
		res = theConnFac_dev->number_output_connectors(&numo);
		assert(res == JVX_NO_ERROR); 
		assert(numi > 0);
		
		res = theMasterFac_dev->number_connector_masters(&numm);
		assert(res == JVX_NO_ERROR); 
		assert(numm > 0);

		res = theConnFac_dev->reference_input_connector(0, &icon_dev);
		assert(res == JVX_NO_ERROR); 
		assert(icon_dev);

		res = theConnFac_dev->reference_output_connector(0, &ocon_dev);
		assert(res == JVX_NO_ERROR); 
		assert(ocon_dev);
		
		res = theMasterFac_dev->reference_connector_master(0, &mas_dev);
		assert(res == JVX_NO_ERROR); 
		assert(mas_dev);

		theConnFac_me->number_input_connectors(&numi);
		assert(res == JVX_NO_ERROR); 
		assert(numi > 0);
		
		theConnFac_me->number_output_connectors(&numo);
		assert(res == JVX_NO_ERROR); 
		assert(numi > 0);

		res = theConnFac_me->reference_input_connector(0, &icon_me);
		assert(res == JVX_NO_ERROR);
		assert(icon_dev);

		res = theConnFac_me->reference_output_connector(0, &ocon_me);
		assert(res == JVX_NO_ERROR); 
		assert(ocon_dev);

		res = this->_create_connection_process(NULL, &id_connection_process, false, false, false, false, JVX_SIZE_UNSELECTED);
		assert(res == JVX_NO_ERROR);

		res = this->_reference_connection_process(0, &hdl_connection_process);
		assert(res == JVX_NO_ERROR);

		// Set the context to the connection object
		res = hdl_connection_process->set_connection_context(this);
		assert(res == JVX_NO_ERROR);

		res = hdl_connection_process->associate_master(mas_dev);
		assert(res == JVX_NO_ERROR);

		res = hdl_connection_process->create_bridge(ocon_dev, icon_me, NULL, &id_device2me, false, false);
		assert(res == JVX_NO_ERROR);

		res = hdl_connection_process->create_bridge(ocon_me, icon_dev, NULL, &id_me2device, false, false);
		assert(res == JVX_NO_ERROR);

		// Connect chain master will come later when we call connect_chain from driving component
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.refDevice->onInit.connectedDevice)
	{
		res = hdl_connection_process->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.refDevice->onInit.connectedDevice)
	{
		res = hdl_connection_process->disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);

		hdl_connection_process->unlink_triggers_connection();

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_unselect_master()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.refDevice->onInit.connectedDevice)
	{
		// Disconnect chain has been done before in the slave behavior context
		res = hdl_connection_process->remove_bridge(id_me2device);
		assert(res == JVX_NO_ERROR);

		res = hdl_connection_process->remove_bridge(id_device2me);
		assert(res == JVX_NO_ERROR);

		id_me2device = JVX_SIZE_UNSELECTED;
		id_device2me = JVX_SIZE_UNSELECTED;

		res = hdl_connection_process->deassociate_master();
		assert(res == JVX_NO_ERROR);

		// Reset the context handle
		res = hdl_connection_process->set_connection_context(NULL);
		assert(res == JVX_NO_ERROR);

		res = this->_return_reference_connection_process(hdl_connection_process);
		assert(res == JVX_NO_ERROR); 
		hdl_connection_process = NULL;

		res = this->_remove_connection_process(id_connection_process);
		assert(res == JVX_NO_ERROR);

		res = theConnFac_me->return_reference_output_connector(ocon_me);
		assert(res == JVX_NO_ERROR);

		res = theConnFac_me->return_reference_input_connector(icon_me);
		assert(res == JVX_NO_ERROR);

		res = theMasterFac_dev->return_reference_connector_master(mas_dev);
		mas_dev = NULL;
		assert(res == JVX_NO_ERROR);

		res = theConnFac_dev->return_reference_output_connector(ocon_dev);
		ocon_dev = NULL;
		assert(res == JVX_NO_ERROR);

		res = theConnFac_dev->return_reference_input_connector(icon_dev);
		icon_dev = NULL;
		assert(res == JVX_NO_ERROR);
		
		theConnFac_me = NULL;
		runtime.refDevice->onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle*>(theConnFac_dev));
		runtime.refDevice->onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_CONNECTOR_MASTER_FACTORY, reinterpret_cast<jvxHandle*>(theMasterFac_dev));
		
		theMasterFac_dev = NULL;
		theConnFac_me = NULL;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

// ================================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_prepare_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refDevice->onInit.connectedDevice);

	res = hdl_connection_process->prepare_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_postprocess_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refDevice->onInit.connectedDevice);

	res = hdl_connection_process->postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_start_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refDevice->onInit.connectedDevice);

	res = hdl_connection_process->start_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::x_stop_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	//jvxSize numi, numo, numm;
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refDevice->onInit.connectedDevice);

	res = hdl_connection_process->stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);
	return JVX_NO_ERROR;
}

// ==================================================================================================

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::x_transfer_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxApiString* str = (jvxApiString*)data;
	std::string locStr;
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.refDevice->onInit.connectedDevice)
	{
		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:
			locStr = str->std_str();
			locStr += "master x-chain entry";
			locStr += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			locStr += "-<xcon>->";
			locStr += JVX_TEXT_NEW_SEGMENT_CHAR_START;
			str->assign(locStr);
			break;
		}
		res = hdl_connection_process->transfer_forward_chain(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		if (res == JVX_NO_ERROR)
		{
			switch (tp)
			{
			case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:
				locStr = str->std_str();
				locStr += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locStr += "<-<nocx>-";
				locStr += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				str->assign(locStr);
				break;
			}
		}
		return res;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::x_test_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString strC;
	this->unique_descriptor(&strC);
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_CONTEXT(fdb, static_cast<IjvxObject*>(runtime.refDevice), strC.c_str());
	if (runtime.refDevice->onInit.connectedDevice)
	{
		// Direct access to master
		return mas_dev->test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return JVX_ERROR_UNSUPPORTED;
}

// ================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	CjvxInputOutputConnector::_connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb), false);
	return runtime.refDevice->connect_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = runtime.refDevice->disconnect_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	CjvxInputOutputConnector::_disconnect_connect_icon(theData, false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{	
	CjvxInputOutputConnector::_prepare_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return runtime.refDevice->prepare_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::prepare_connect_icon_x(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// _common_set_ocon.theData_out.con_params.number_channels = ? ;
	res = CjvxInputOutputConnector::_prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	jvx_ccopyDataLinkDescriptor(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
	_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender =
		_common_set_ocon.theData_out.con_data.buffers;
	
	// This user hint link is the short version within the HW loop
	//_common_set_icon.theData_in->con_compat.user_hints =
	//	_common_set_ocon.theData_out.con_data.user_hints;

	return res;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::postprocess_connect_icon_x(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// What to do here??
	// _common_set_ocon.theData_out.con_params.number_channels = ? ;
	res = CjvxInputOutputConnector::_postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	_common_set_icon.theData_in->con_compat.user_hints = NULL;

	_common_set_icon.theData_in->con_compat.buffersize = 0;
	_common_set_icon.theData_in->con_compat.format = JVX_DATAFORMAT_NONE;
	_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
	_common_set_icon.theData_in->con_compat.number_buffers = 0;
	_common_set_icon.theData_in->con_compat.rate = 0;

	return res;
}
// =================================================================================
// =================================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;

	runtime.refDevice->_common_set.theUniqueId->obtain_unique_id(&idRuntime, "GW-HR");

	CjvxInputOutputConnector::_start_connect_icon(false, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	// Initialize output processing index
	*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr = 0;
	_common_set_ocon.theData_out.con_compat.idx_receiver_to_sender = 0;

	return runtime.refDevice->start_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize idRuntime = JVX_SIZE_UNSELECTED;
	CjvxInputOutputConnector::_stop_connect_icon(false, &idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	runtime.refDevice->_common_set.theUniqueId->release_unique_id(idRuntime);

	return runtime.refDevice->stop_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	CjvxInputOutputConnector::_postprocess_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return runtime.refDevice->postprocess_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool acquire = false;

	switch (runtime.refDevice->proc_fields.doSelection)
	{
	case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

		// If we are in async mode, we forward only to involved HW device
		res = process_start_ocon(pipeline_offset,  idx_stage, tobeAccessedByStage,
			clbk, priv_ptr);
		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage, tobeAccessedByStage,
				clbk, priv_ptr);
		}
		break;
	case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:

		// If we are in sync mode, we connect this callback through
		res = runtime.refDevice->process_start_icon_x(pipeline_offset, idx_stage, tobeAccessedByStage);
		if (runtime.refDevice->proc_fields.seq_operation_in == CjvxGenericWrapperDevice::PROC)
		{
			// we may need to take over a modified output index if direct buffer processing
			// The pipeline lock mechanism works on the right lock and buffer anyway.
			/*
			_common_set_icon.theData_in->con_pipeline.do_lock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
			_common_set_icon.theData_in->pipeline.idx_stage =
				runtime.refDevice->_common_set_ocon.theData_out.pipeline.idx_stage;
			_common_set_icon.theData_in->con_pipeline.do_unlock(_common_set_icon.theData_in->con_pipeline.lock_hdl);
			*/
		}
		else
		{
			res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage,  tobeAccessedByStage, clbk, priv_ptr);
		}
		break;
	default:
		assert(0);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (runtime.refDevice->proc_fields.doSelection)
	{
	case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

		res = runtime.refDevice->process_async(mt_mask, idx_stage);

		// If we are in async mode, we forward only to involved HW device
		res = process_buffers_icon_x( mt_mask, idx_stage);
		break;
	case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:

		// If we are in sync mode, we connect this callback through
		res = runtime.refDevice->process_sync( mt_mask,  idx_stage);
		break;
	default:
		assert(0);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (runtime.refDevice->proc_fields.doSelection)
	{
	case CjvxGenericWrapperDevice::SELECTION_ASYNC_MODE:

		res = process_stop_ocon( idx_stage,  shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
		}
		break;
	case CjvxGenericWrapperDevice::SELECTION_SYNC_MODE:

		// If we are in sync mode, we connect this callback through
		res = runtime.refDevice->process_stop_icon_x( idx_stage,  shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);

		// Copy index from connected object for update
		if (runtime.refDevice->proc_fields.seq_operation_in == CjvxGenericWrapperDevice::PROC)
		{
			// Copy the index from following object to this object
			_common_set_icon.theData_in->con_pipeline.do_lock(_common_set_icon.theData_in->con_pipeline.lock_hdl); 
			*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr =
				*runtime.refDevice->_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
			_common_set_icon.theData_in->con_pipeline.do_unlock(_common_set_icon.theData_in->con_pipeline.lock_hdl); 
		}
		else
		{
			res = shift_buffer_pipeline_idx_on_stop(idx_stage, shift_fwd,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
		}
		break;
	default:
		assert(0);
	}
	return res;
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(runtime.refDevice));
	CjvxInputOutputConnector::_test_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, _common_set_icon.theData_in);

	// Setup the parameters
	runtime.refDevice->updateSWSamplerateAndBuffersize_nolock(_common_set_icon.theData_in, &runtime.refDevice->_common_set_ocon.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return runtime.refDevice->test_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxInputOutputConnector::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

// =====================================================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	CjvxInputOutputConnector::_transfer_forward_icon(false, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return runtime.refDevice->transfer_forward_icon_x(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	assert(0);
	return CjvxInputOutputConnector::_transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	assert(0);
	return CjvxInputOutputConnector::_transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	std::string txt;
	jvxErrorType res = JVX_NO_ERROR;
	CjvxInputOutputConnector::_transfer_backward_ocon(false, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS)
	{
		jvxBool errorDetected = false;
		local_proc_params params;

		// There should not be a reason to propagate this any further since the output channels had been updated on the test_connect_icon side
		runtime.refDevice->populate_countchannels_datatype(params);
		txt = "Unexpected transfer backward from device output to generic wrapper input. This should not happen. Reason: "; 
		if (
			(_common_set_ocon.theData_out.con_params.number_channels != params.chanshw_out))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Number of output channels in device is ";
			txt += jvx_size2String(_common_set_ocon.theData_out.con_params.number_channels);
			txt += " whereas the number of active hardware channels in generic wrapper is ";
			txt += jvx_size2String(params.chanshw_out);
			errorDetected = true;
		}
			
		if ((_common_set_ocon.theData_out.con_params.buffersize != runtime.refDevice->processingControl.computedParameters.bSize_hw))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Buffersize in device is ";
			txt += jvx_size2String(_common_set_ocon.theData_out.con_params.buffersize);
			txt += " whereas the hardware buffersize in generic wrapper is ";
			txt += jvx_size2String(runtime.refDevice->processingControl.computedParameters.bSize_hw);
			errorDetected = true;
		}
			
		if ((_common_set_ocon.theData_out.con_params.rate != runtime.refDevice->processingControl.computedParameters.sRate_hw))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Samplerate in device is ";
			txt += jvx_size2String(_common_set_ocon.theData_out.con_params.rate);
			txt += " whereas the hardware samplerate in generic wrapper is ";
			txt += jvx_size2String(runtime.refDevice->processingControl.computedParameters.sRate_hw);
			errorDetected = true;
		}
		
		if ((_common_set_ocon.theData_out.con_params.format != runtime.refDevice->processingControl.computedParameters.form_hw))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Dataformat in device is ";
			txt += jvxDataFormat_txt(_common_set_ocon.theData_out.con_params.format);
			txt += " whereas the hardware dataformat in generic wrapper is ";
			txt += jvxDataFormat_txt(runtime.refDevice->processingControl.computedParameters.form_hw);
			errorDetected = true;
		}

		if ((_common_set_ocon.theData_out.con_params.segmentation.x != runtime.refDevice->processingControl.computedParameters.bSize_hw))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Segmentation x in device is ";
			txt += jvx_size2String(_common_set_ocon.theData_out.con_params.segmentation.x);
			txt += " whereas the hardware buffersize in generic wrapper is ";
			txt += jvx_size2String(runtime.refDevice->processingControl.computedParameters.bSize_hw);
			errorDetected = true;
		}

		if ((_common_set_ocon.theData_out.con_params.segmentation.y != 1))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Segmentation y in device is ";
			txt += jvx_size2String(_common_set_ocon.theData_out.con_params.segmentation.x);
			txt += " whereas the value in generic wrapper is ";
			txt += jvx_size2String(1);
			errorDetected = true;
		}

		if ((_common_set_ocon.theData_out.con_params.format_group != JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED))
		{
			if (errorDetected)
			{
				txt += ", ";
			}
			txt += "Datasubformat in device is ";
			txt += jvxDataFormatGroup_txt(_common_set_ocon.theData_out.con_params.format_group);
			txt += " whereas the hardware datasubformat in generic wrapper is ";
			txt += jvxDataFormatGroup_txt(JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
			errorDetected = true;
		}
		txt += ".";

		if (errorDetected)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
			res = JVX_ERROR_UNSUPPORTED;
		}
		// runtime.refDevice->release_countchannels_datatype(params); <- not required since local variable
		return res;
	}
	return runtime.refDevice->transfer_backward_ocon_x(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

// =====================================================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::request_hidden_interface(jvxInterfaceType tp, jvxHandle** rethdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refDevice);
	if(tp == JVX_INTERFACE_REPORT)
	{
		res = runtime.refDevice->_common_set_min.theHostRef->request_hidden_interface(tp, reinterpret_cast<jvxHandle**>(&runtime.refReport));
		if((res == JVX_NO_ERROR) && runtime.refReport)
		{
			*rethdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxReport*>(this));
            runtime.cntRefReport++;
			return(JVX_NO_ERROR);
		}
		return(res);
	}
	return(runtime.refDevice->_common_set_min.theHostRef->request_hidden_interface(tp, rethdl));
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	assert(runtime.refDevice);
	if(tp == JVX_INTERFACE_REPORT)
	{
		if(runtime.refReport)
		{
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxReport*>(this)))
			{
                runtime.cntRefReport--;
                if(runtime.cntRefReport == 0)
                {
                    runtime.refDevice->_common_set_min.theHostRef->return_hidden_interface(tp, reinterpret_cast<jvxHandle*>(runtime.refReport));
                    runtime.refReport = NULL;
                }
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		return(JVX_ERROR_UNSUPPORTED);
	}
	return(runtime.refDevice->_common_set_min.theHostRef->return_hidden_interface(tp, hdl));
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.refReport);
	res = runtime.refReport->report_simple_text_message(((std::string)"<" + txt + ">").c_str(), prio);
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme, jvxPropertyCategoryType cat, 
	jvxSize propId, bool onlyContent, jvxSize offs, jvxSize numElm, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	jvxBool isValid = false;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	assert(runtime.refReport);
	res = runtime.refReport->report_internals_have_changed(thisismytype, thisisme, cat, propId + JVX_PROPERTIES_OFFSET_SUBCOMPONENT, onlyContent, 
		offs, numElm, tpTo, callpurp);

	if(cat == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if(callpurp != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			// All predefined properties should be passed via test_master entry chaining
			std::cout << "ERROR: " << __FUNCTION__ << " should not be used for passing processing properties, use processing block chaining configuration." << std::endl;	
		}
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification&, jvxPropertyCallPurpose callpurp)
{
	jvxErrorType res = JVX_NO_ERROR;
	propId += JVX_PROPERTIES_OFFSET_SUBCOMPONENT;
	assert(runtime.refReport);
	res = JVX_ERROR_UNSUPPORTED; // This operation is not useful
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::report_command_request(jvxCBitField request, 
	jvxHandle* caseSpecificData,
	jvxSize szSpecificData)
{
	jvxErrorType res = JVX_NO_ERROR;

	/*
	if(request == JVX_REPORT_REQUEST_UPDATE_WINDOW)
	{
		if(runtime.refDevice->_common_set_min.theState <= JVX_STATE_ACTIVE)
		{
			// Only reconfigure everything if not just in states higher than active
		}
	}
	*/
	assert(runtime.refReport);
	if (jvx_bitTest(request, JVX_REPORT_REQUEST_RECONFIGURE_SHIFT))
	{
		//runtime.refDevice->lock_settings();
		//runtime.refDevice->onInit.parentRef->lock_files();
		runtime.refDevice->rebuildPropertiesSubDevice_noLock();
		//runtime.refDevice->onInit.parentRef->unlock_files();
		//runtime.refDevice->unlock_settings();
	}
	if (jvx_bitTest(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT))
	{
		// Reread the channels, everything else is handled by test run
		runtime.refDevice->rereadChannelStatusHw_noLock();
		runtime.refDevice->updateChannelExposure_nolock();

		// Change the id to update and test full chain
		if (runtime.refDevice->_common_set_ld_master.refProc)
		{

			jvxSize uId = JVX_SIZE_UNSELECTED;
			runtime.refDevice->_common_set_ld_master.refProc->unique_id_connections(&uId);
			runtime.refDevice->_request_test_chain_master(uId);

		}
		jvx_bitClear(request, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
	}
	if(request)
	{
		res = runtime.refReport->report_command_request(request);
	}
	return(res);
};

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::request_command(const CjvxReportCommandRequest& request)
{
	jvxReportCommandRequest req = request.request();
	if (req == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY)
	{
		// If we are here, the report_set comes from the submodule
		const CjvxReportCommandRequest_id* reqPtr = castCommandRequest<CjvxReportCommandRequest_id>(request);
		if (reqPtr)
		{
			jvxApiString astr;
			reqPtr->ident(&astr);
			std::list<std::string> lstStr = jvx::helper::properties::stringToCollectedProps(astr.c_str());
			std::list<std::string> lstStrPrefix;
			for (std::string& elm : lstStr)
			{
				lstStrPrefix.push_back(jvx_makePathExpr("JVX_GENW", elm));
			}
			std::string replStr = jvx::helper::properties::collectedPropsToString(lstStrPrefix);
			runtime.refDevice->report_properties_modified(replStr.c_str());
		}

	}
	return runtime.refReport->request_command(request);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::report_os_specific(jvxSize commandId, void* context)
{
    jvxErrorType res = JVX_NO_ERROR;
    assert(runtime.refReport);
    res = runtime.refReport->report_os_specific(commandId, context);
    return(res);

}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::test_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxApiString strC;
	local_proc_params params;
	this->unique_descriptor(&strC);
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_CONTEXT(fdb, static_cast<IjvxObject*>(runtime.refDevice), strC.c_str());

	runtime.refDevice->populate_countchannels_datatype(params);
	/*
	_common_set_ocon.theData_out.con_params.buffersize = runtime.refDevice->processingControl.inProc.params.buffersize;
	_common_set_ocon.theData_out.con_params.rate = runtime.refDevice->processingControl.inProc.params.samplerate;
	_common_set_ocon.theData_out.con_params.format = runtime.refDevice->processingControl.inProc.params.format;
	_common_set_ocon.theData_out.con_data.number_buffers = runtime.refDevice->processingControl.inProc.params.chans_out;
	*/
	_common_set_ocon.theData_out.con_params.buffersize = runtime.refDevice->processingControl.computedParameters.bSize_hw;
	_common_set_ocon.theData_out.con_params.rate = runtime.refDevice->processingControl.computedParameters.sRate_hw;
	_common_set_ocon.theData_out.con_params.format = runtime.refDevice->processingControl.computedParameters.form_hw;
	_common_set_ocon.theData_out.con_params.number_channels = params.chanshw_out;

	_common_set_ocon.theData_out.con_params.segmentation.x = runtime.refDevice->processingControl.computedParameters.bSize_hw;
	_common_set_ocon.theData_out.con_params.segmentation.y = 1;
	_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

	// runtime.refDevice->release_countchannels_datatype(params); <- not required since local variable

	return CjvxInputOutputConnector::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
};


jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::transfer_backward_x(jvxLinkDataDescriptor* descr JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{

	runtime.refDevice->updateSWSamplerateAndBuffersize_nolock(_common_set_icon.theData_in, &runtime.refDevice->_common_set_ocon.theData_out 
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (
		(descr->con_params.format != runtime.refDevice->_common_set_ocon.theData_out.con_params.format) ||
		(descr->con_params.buffersize != runtime.refDevice->_common_set_ocon.theData_out.con_params.buffersize) ||
		(descr->con_params.rate != runtime.refDevice->_common_set_ocon.theData_out.con_params.rate))
	{
		return JVX_ERROR_UNSUPPORTED;
	}
	return JVX_NO_ERROR;
}

// ==================================================================================

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::number_next(jvxSize* num)
{
	return runtime.refDevice->number_next_x(num);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::reference_next(jvxSize idx, IjvxConnectionIterator** next)
{
	return runtime.refDevice->reference_next_x(idx, next);
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::iterator_chain_x(IjvxConnectionIterator** it)
{
	if (runtime.refDevice->onInit.connectedDevice)
	{
		return hdl_connection_process->iterator_chain(it);
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperDevice_hostRelocator::number_next_x(jvxSize* num)
{
	return _number_next(num);
}

jvxErrorType 
CjvxGenericWrapperDevice_hostRelocator::reference_next_x(jvxSize idx, IjvxConnectionIterator** next) 
{
	return _reference_next(idx, next);;
}
