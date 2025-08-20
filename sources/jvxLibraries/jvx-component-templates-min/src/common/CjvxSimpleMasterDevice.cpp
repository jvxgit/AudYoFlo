#include "common/CjvxSimpleMasterDevice.h"

CjvxSimpleMasterDevice::CjvxSimpleMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxSimpleMasterDevice::~CjvxSimpleMasterDevice()
{
}

jvxErrorType
CjvxSimpleMasterDevice::prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (_common_set_min.theState)
	{
	case JVX_STATE_ACTIVE:

		assert(_common_set.stateChecks.prepare_flags == 0);

		// Automatically switch state of device
		res = this->prepare();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit prepare failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}
		jvx_bitSet(_common_set.stateChecks.prepare_flags, 0);
		break;
	case JVX_STATE_PREPARED:
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxSimpleMasterDevice::start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (_common_set_min.theState)
	{
	case JVX_STATE_PREPARED:

		assert(_common_set.stateChecks.start_flags == 0);

		// Automatically switch state of device
		res = this->start();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit start failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}
		jvx_bitSet(_common_set.stateChecks.start_flags, 0);
		break;
	case JVX_STATE_PROCESSING:
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxSimpleMasterDevice::postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set.stateChecks.prepare_flags)
	{
		res = this->postprocess();
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Implicit postprocess failed on object " << _common_set_min.theDescription << std::endl;
		}
		jvx_bitClear(_common_set.stateChecks.prepare_flags, 0);
	}
	return res;
}

jvxErrorType
CjvxSimpleMasterDevice::stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set.stateChecks.start_flags)
	{
		res = this->stop();
		if (res != JVX_NO_ERROR)
		{
			std::cout << "Implicit stop failed on object " << _common_set_min.theDescription << std::endl;
		}
		jvx_bitClear(_common_set.stateChecks.start_flags, 0);
	}
	return res;
}

void 
CjvxSimpleMasterDevice::test_set_output_parameters()
{
	// Do nothing here
}

jvxErrorType 
CjvxSimpleMasterDevice::set_location_info(const jvxComponentIdentification& tpMe) 
{
	jvxErrorType res = CjvxObject::_set_location_info(tpMe);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(this);
		}
	}
	return res;
};

/*

jvxErrorType
CjvxSimpleMasterDevice::filterOptionsDevice(
	jvxBool* avail,
	jvxSize* numDataStreams,
	jvxSize* assSlot )
{
	if (avail)
	{
		*avail = _common_set_simple_master_device.avail;
	}
	if (numDataStreams)
	{
		*numDataStreams = 1;
	}
	if (assSlot)
	{
		*assSlot = _common_set.theComponentType.slotsubid;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSimpleMasterDevice::filterOptionsDataStream(
	jvxSize idx,
	jvxDeviceCapabilities* caps ,
	jvxDataFormatGroup* dataGroup ,
	jvxApiString* astr )
{
	if (idx == 0)
	{
		if (caps)
		{
			*caps = _common_set_simple_master_device.devCaps;
		}
		if (dataGroup)
		{
			*dataGroup = _common_set_simple_master_device.dGroup;
		}
		if (astr)
		{
			astr->assign(_common_set_simple_master_device.nmStream);
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}
*/
