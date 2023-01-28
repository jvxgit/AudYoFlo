#include "jvxTechnologies/CjvxFullMasterDevice.h"

CjvxFullMasterDevice::CjvxFullMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxSimpleMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(tpComp);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
};

CjvxFullMasterDevice::~CjvxFullMasterDevice()
{
}

jvxErrorType 
CjvxFullMasterDevice::activate()
{
	jvxErrorType res = CjvxSimpleMasterDevice::_activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxFullMasterDevice_genpcg::init_all();
		CjvxFullMasterDevice_genpcg::allocate_all();
		CjvxFullMasterDevice_genpcg::register_all(static_cast<CjvxProperties*>(this));

		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default",
			static_cast<IjvxConnectionMaster*>(this), _common_set.theModuleName.c_str());
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "default");
	}
	return res;
}

jvxErrorType
CjvxFullMasterDevice::deactivate()
{
	jvxErrorType res = CjvxSimpleMasterDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();

		CjvxFullMasterDevice_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxFullMasterDevice_genpcg::deallocate_all();
		CjvxSimpleMasterDevice::_deactivate();
	}
	return res;
}

jvxErrorType
CjvxFullMasterDevice::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// Accept input parameters as specified
	jvxErrorType res = JVX_NO_ERROR;
	
	accept_output_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	res = CjvxSimpleMasterDevice::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{

		CjvxFullMasterDevice_genpcg::proc_params.num_out_channels.value = neg_output._latest_results.number_channels;
		CjvxFullMasterDevice_genpcg::proc_params.bsize.value = neg_output._latest_results.buffersize;
		CjvxFullMasterDevice_genpcg::proc_params.srate.value = neg_output._latest_results.rate;
		CjvxFullMasterDevice_genpcg::proc_params.format.value = neg_output._latest_results.format;
		CjvxFullMasterDevice_genpcg::proc_params.subformat.value = neg_input._latest_results.format_group;
		CjvxFullMasterDevice_genpcg::proc_params.seg_x.value = neg_output._latest_results.segmentation_x;
		CjvxFullMasterDevice_genpcg::proc_params.seg_y.value = neg_output._latest_results.segmentation_y;
	}
	accept_output_parameters_stop(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxFullMasterDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = neg_input._negotiate_connect_icon(_common_set_ldslave.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Accepted
		CjvxFullMasterDevice_genpcg::proc_params.num_in_channels.value = neg_input._latest_results.number_channels;
	}

	return res;
}

jvxErrorType
CjvxFullMasterDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;
	jvxBool thereismismatch = false;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// There is only room to negotiate 
		res = neg_output._negotiate_transfer_backward_ocon(ld,
			&_common_set_ldslave.theData_out, static_cast<IjvxObject*>(this), NULL
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			accept_output_parameters_stop(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			outputParams.number_channels = neg_output._latest_results.number_channels;
			outputParams.buffersize = neg_output._latest_results.buffersize;
			outputParams.rate = neg_output._latest_results.rate;
			outputParams.format = neg_output._latest_results.format;
			outputParams.format_group = neg_output._latest_results.format_group;

			accept_output_parameters_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		break;
	default:

		// This is a master output connector, there is no successor!
		res = CjvxInputOutputConnector::_transfer_backward_ocon(false, tp, data, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxFullMasterDevice::accept_output_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	neg_output._update_parameters_fixed(
		outputParams.number_channels,
		outputParams.buffersize,
		outputParams.rate,
		outputParams.format,
		outputParams.format_group,
		&_common_set_ldslave.theData_out);
	neg_input._push_constraints();

	neg_input._update_parameters_fixed(
		inputParams.number_channels,
		inputParams.buffersize,
		inputParams.rate,
		inputParams.format,
		inputParams.format_group);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullMasterDevice::accept_output_parameters_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	neg_input._pop_constraints();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullMasterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxConnectionMaster::_test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType 
CjvxFullMasterDevice::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullMasterDevice::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	return JVX_NO_ERROR;
}