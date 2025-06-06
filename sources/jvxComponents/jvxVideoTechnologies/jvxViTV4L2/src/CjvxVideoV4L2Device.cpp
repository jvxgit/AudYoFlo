#include "CjvxVideoV4L2Device.h"
#include "CjvxVideoV4L2Technology.h"

#include <cerrno>
#include <linux/videodev2.h>

#define JVX_ALL_PROPERTY_UPDATE_STRING "v4l2_device:mode_selection"

#define xioctl CjvxVideoV4L2Device::xioctl


CjvxVideoV4L2Device::CjvxVideoV4L2Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set_properties.reportIfNoChange = true;
}

CjvxVideoV4L2Device::~CjvxVideoV4L2Device()
{
}

jvxErrorType
CjvxVideoV4L2Device::init(CjvxVideoV4L2Technology* parentTech, const char* device, jvxBool isDefaultDevice)
{
	myParent = parentTech;
	this->device = device;

	jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);

	if (isDefaultDevice)
	{
		jvx_bitZSet<jvxCBitField16>(_common_set_device.caps.flags, (int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT);
	}
	_common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVideoV4L2Device::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxVideoDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{
		_common_set_device.caps.selectable = false;
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return res;
}

jvxErrorType
CjvxVideoV4L2Device::unselect()
{
	jvxErrorType res = CjvxVideoDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
		_common_set_device.caps.selectable = true;
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return res;
}

jvxErrorType
CjvxVideoV4L2Device::activate()
{
	jvxSize i;
	jvxErrorType res = CjvxVideoDevice::activate();
	if(res == JVX_NO_ERROR)
	{
		this->dev_fd = open(this->device.c_str(), O_RDWR);
		if (this->dev_fd < 0)
			return JVX_ERROR_OPEN_FILE_FAILED;

		scanProperties();

		genV4L2_device::init_all();
		genV4L2_device::allocate_all();

		genV4L2_device::configuration_mf.mode_selection.value.entries.clear();
		for (i = 0; i < lstModes.size(); i++)
		{
			genV4L2_device::configuration_mf.mode_selection.value.entries.push_back(lstModes[i].descriptor);
		}

		genV4L2_device::register_all(static_cast<CjvxProperties*>(this));
		genV4L2_device::register_callbacks(static_cast<CjvxProperties*>(this), on_mode_selected, this);

		this->_lock_properties_local();

		// The properties in the base class from here on are only for reading, not for editiing
		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			CjvxVideoDevice_genpcg::video.format);

		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			CjvxVideoDevice_genpcg::video.framesize);

		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			CjvxVideoDevice_genpcg::video.segmentsize_x);

		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			CjvxVideoDevice_genpcg::video.segmentsize_y);

		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			CjvxVideoDevice_genpcg::video.subformat);


		jvxSize idSel = jvx_bitfieldSelection2Id(genV4L2_device::configuration_mf.mode_selection.value.selection(), genV4L2_device::configuration_mf.mode_selection.value.entries.size());
		if (idSel >= genV4L2_device::configuration_mf.mode_selection.value.entries.size())
		{
			idSel = 0;
		}
		if (JVX_CHECK_SIZE_SELECTED(idSel))
		{
			jvx_bitZSet(genV4L2_device::configuration_mf.mode_selection.value.selection(), idSel);
		}

		this->updateDependentVariables_nolock();

		this->_lock_properties_local();

		return JVX_NO_ERROR;

	}
	return(res);
}

jvxErrorType
CjvxVideoV4L2Device::deactivate()
{
	jvxErrorType res = CjvxVideoDevice::deactivate();
	if (res == JVX_NO_ERROR)
	{
		genV4L2_device::unregister_callbacks(static_cast<CjvxProperties*>(this));

		genV4L2_device::unregister_all(static_cast<CjvxProperties*>(this));
		genV4L2_device::deallocate_all();

		// The properties in the base class from here on are only for reading, not for editiing
		_undo_update_property_access_type(
			CjvxVideoDevice_genpcg::video.format);

		_undo_update_property_access_type(
			CjvxVideoDevice_genpcg::video.framesize);

		_undo_update_property_access_type(
			CjvxVideoDevice_genpcg::video.segmentsize_x);

		_undo_update_property_access_type(
			CjvxVideoDevice_genpcg::video.segmentsize_y);

		_undo_update_property_access_type(
			CjvxVideoDevice_genpcg::video.subformat);

		lstModes.clear();

		close(this->dev_fd);

		return JVX_NO_ERROR;
	}
	return(res);
}

jvxErrorType
CjvxVideoV4L2Device::prepare()
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxVideoDevice::prepare();
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_start();
	}
	return(res);
}

jvxErrorType
CjvxVideoV4L2Device::prepare_data(jvxBool runPrepare JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxSize idModeSelect = jvx_bitfieldSelection2Id(
		genV4L2_device::configuration_mf.mode_selection.value.selection(),
		genV4L2_device::configuration_mf.mode_selection.value.entries.size());

	JVX_ASSERT_X("Check for at least 1 mode", 0, JVX_CHECK_SIZE_SELECTED(idModeSelect));

	this->set_device_mode(idModeSelect);

	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)lstModes[idModeSelect].form;
	_common_set_ocon.theData_out.con_params.number_channels = 1;
	_common_set_ocon.theData_out.con_params.rate = lstModes[idModeSelect].fps;
	// _common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)lstModes[idModeSelect].subform_sw;
	_common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)lstModes[idModeSelect].subform;
	_common_set_ocon.theData_out.con_params.segmentation.x = lstModes[idModeSelect].width;
	_common_set_ocon.theData_out.con_params.segmentation.y = lstModes[idModeSelect].height;

	jvxSize szElement = jvxDataFormatGroup_getsize_mult(_common_set_ocon.theData_out.con_params.format_group);
	jvxSize szElementsLine = _common_set_ocon.theData_out.con_params.segmentation.x * szElement;
	jvxSize szElementsField = _common_set_ocon.theData_out.con_params.segmentation.y * szElementsLine;
	_common_set_ocon.theData_out.con_params.buffersize = szElementsField / jvxDataFormatGroup_getsize_div(_common_set_ocon.theData_out.con_params.format_group);
	_common_set_ocon.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ACTIVE;

	if (runPrepare)
	{
		_common_set_ocon.theData_out.con_data.buffers = NULL;
		_common_set_ocon.theData_out.con_user.chain_spec_user_hints = NULL;
		_common_set_ocon.theData_out.con_data.number_buffers = 1;
		_common_set_ocon.theData_out.con_params.additional_flags = 0;

		runtime.szElement = szElement;
		runtime.szLine = szElementsLine;
		runtime.szRaw = szElementsField;

		res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resL == JVX_NO_ERROR);
			goto leave_error;
		}

		//runtime.stride = 0;

		//LONG stride = 0;
		//hr = MFGetStrideForBitmapInfoHeader(nativeSubGuid.Data1, (DWORD)_common_set_ocon.theData_out.con_params.segmentation.x, &stride);
		//JVX_ASSERT_X("MFGetStrideForBitmapInfoHeader failed", 0, (hr == S_OK));
		//runtime.stride = stride;
	}
	return JVX_NO_ERROR;

leave_error:
	return(res);

}

jvxErrorType
CjvxVideoV4L2Device::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return prepare_data(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxVideoV4L2Device::postprocess()
{
	jvxErrorType res = CjvxVideoDevice::postprocess();

	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_stop();
	}
	return res;
}

jvxErrorType
CjvxVideoV4L2Device::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Contact through
	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	//runtime.stride = 0;
	runtime.szElement = 0;
	runtime.szLine = 0;
	runtime.szRaw = 0;

	return JVX_NO_ERROR;
}


jvxErrorType
CjvxVideoV4L2Device::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr = 0;
	res = CjvxConnectionMaster::_start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

	runtime.streamState = JVX_STATE_PROCESSING;
	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
	runtime.lastTime = -1;
	runtime.avrgT = 0;
	runtime.inputOverrun = 0;
	runtime.numFrames = 0;

	// Start frame reader thread
	this->read_frame_thread = std::thread(&CjvxVideoV4L2Device::read_frame_thread_main, this);

	return(res);

leave_error:
	return(res);

}

jvxErrorType
CjvxVideoV4L2Device::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	runtime.streamState = JVX_STATE_COMPLETE;

	// Wait for frame reader thread to stop
	this->read_frame_thread.join();

	res = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Stopping chain failed." << std::endl;
	}
	return(res);
}
// ============================================================================================
// ============================================================================================

jvxErrorType
CjvxVideoV4L2Device::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool report_update = false;

	jvxErrorType res = CjvxVideoDevice::set_property(callGate, rawPtr, ident, trans);
	JVX_PROPERTY_CHECK_RESULT(res, callGate)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (
			(category == genV4L2_device::configuration_mf.mode_selection.category) &&
			(propId == genV4L2_device::configuration_mf.mode_selection.globalIdx))
		{
			jvxSize idSel = jvx_bitfieldSelection2Id(genV4L2_device::configuration_mf.mode_selection.value.selection(), genV4L2_device::configuration_mf.mode_selection.value.entries.size());
			if (idSel >= genV4L2_device::configuration_mf.mode_selection.value.entries.size())
			{
				idSel = genV4L2_device::configuration_mf.mode_selection.value.entries.size() - 1;
			}
			if (JVX_CHECK_SIZE_SELECTED(idSel))
			{
				// Correct invalid selections
				jvx_bitZSet(genV4L2_device::configuration_mf.mode_selection.value.selection(), idSel);
			}

			this->updateDependentVariables_nolock();
			CjvxVideoDevice::updateDependentVariables_nolock(propId, category, true, callGate.call_purpose);

			CjvxReportCommandRequest req(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES, _common_set.theComponentType);
			_request_command(req);
		}
	}
	return(res);
}

jvxErrorType
CjvxVideoV4L2Device::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	// Read all parameters for base class
	CjvxVideoDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno );

	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genV4L2_device::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		for (i = 0; i < warnings.size(); i++)
		{
			_report_text_message(warnings[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		if (JVX_EVALUATE_BITFIELD(genV4L2_device::configuration_mf.mode_selection.value.selection()) == false)
		{
			if (genV4L2_device::configuration_mf.mode_selection.value.entries.size() > 0)
			{
				jvx_bitZSet(genV4L2_device::configuration_mf.mode_selection.value.selection(), 0);
			}
		}
		this->updateDependentVariables_nolock();
	}
	return(res);
}

jvxErrorType
CjvxVideoV4L2Device::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
	CjvxVideoDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	// Write all parameters from this class
	genV4L2_device::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return(JVX_NO_ERROR);
}

void
CjvxVideoV4L2Device::updateDependentVariables_nolock()
{
	jvxSize idSel = jvx_bitfieldSelection2Id(genV4L2_device::configuration_mf.mode_selection.value.selection(),
		genV4L2_device::configuration_mf.mode_selection.value.entries.size());

	if (JVX_CHECK_SIZE_SELECTED(idSel))
	{
		genV4L2_device::configuration_mf.rates_min.value = (jvxInt32)lstModes[idSel].fps_min;
		genV4L2_device::configuration_mf.rates_max.value = (jvxInt32)lstModes[idSel].fps_max;

		CjvxVideoDevice_genpcg::video.framerate.value = (jvxInt32)lstModes[idSel].fps;
		CjvxVideoDevice_genpcg::video.segmentsize_x.value = lstModes[idSel].width;
		CjvxVideoDevice_genpcg::video.segmentsize_y.value = lstModes[idSel].height;
		CjvxVideoDevice_genpcg::video.framesize.value = lstModes[idSel].width * lstModes[idSel].height;
		CjvxVideoDevice_genpcg::video.format.value = lstModes[idSel].form;
		CjvxVideoDevice_genpcg::video.subformat.value = lstModes[idSel].subform;
	}
	else
	{
		genV4L2_device::configuration_mf.rates_min.value = 0;
		genV4L2_device::configuration_mf.rates_max.value = 0;

		CjvxVideoDevice_genpcg::video.framerate.value = 0;
		CjvxVideoDevice_genpcg::video.segmentsize_x.value = 0;
		CjvxVideoDevice_genpcg::video.segmentsize_y.value = 0;
		CjvxVideoDevice_genpcg::video.framesize.value = 0;
		CjvxVideoDevice_genpcg::video.format.value = JVX_DATAFORMAT_NONE;
		CjvxVideoDevice_genpcg::video.subformat.value = JVX_DATAFORMAT_GROUP_GENERIC;
	}
}

jvxErrorType
CjvxVideoV4L2Device::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = prepare_data(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	/*
	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)CjvxVideoDevice_genpcg::video.format.value;
	_common_set_ocon.theData_out.con_params.number_channels = 1;
	_common_set_ocon.theData_out.con_params.rate = CjvxVideoDevice_genpcg::video.framerate.value;
	_common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)CjvxVideoDevice_genpcg::video.subformat.value;
	_common_set_ocon.theData_out.con_params.segmentation.x = (jvxDataFormatGroup)CjvxVideoDevice_genpcg::video.segmentsize_x.value;
	_common_set_ocon.theData_out.con_params.segmentation.y = (jvxDataFormatGroup)CjvxVideoDevice_genpcg::video.segmentsize_y.value;
	_common_set_ocon.theData_out.con_params.buffersize = _common_set_ocon.theData_out.con_params.segmentation.x * _common_set_ocon.theData_out.con_params.segmentation.y * jvxDataFormatGroup_getsize()
	*/

	if (res == JVX_NO_ERROR)
	{
		res = CjvxVideoDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		CjvxVideoDevice_genpcg::video.framerate.value = _common_set_ocon.theData_out.con_params.rate;
		CjvxVideoDevice_genpcg::video.segmentsize_x.value = _common_set_ocon.theData_out.con_params.segmentation.x;
		CjvxVideoDevice_genpcg::video.segmentsize_y.value = _common_set_ocon.theData_out.con_params.segmentation.y;
		CjvxVideoDevice_genpcg::video.framesize.value = _common_set_ocon.theData_out.con_params.buffersize;
		CjvxVideoDevice_genpcg::video.format.value = _common_set_ocon.theData_out.con_params.format;
		CjvxVideoDevice_genpcg::video.subformat.value = _common_set_ocon.theData_out.con_params.format_group;

	}
	return res;
}

jvxErrorType
CjvxVideoV4L2Device::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	// Actually, we should be able to deal with all settings since input and output are independent.
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVideoV4L2Device::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_con = (jvxLinkDataDescriptor*)data;
	jvxBool foundthis = false;
	jvxSize i;
	jvxData diff, weight = 0;
	jvxData weightmin = JVX_DATA_MAX_POS;
	jvxSize idxmin = JVX_SIZE_UNSELECTED;
	jvxSize idSel = jvx_bitfieldSelection2Id(genV4L2_device::configuration_mf.mode_selection.value.selection(),
		genV4L2_device::configuration_mf.mode_selection.value.entries.size());

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		if (
			(ld_con->con_params.format_group != lstModes[idSel].subform) ||
			(ld_con->con_params.rate != (jvxInt32)lstModes[idSel].fps) ||
			(ld_con->con_params.format != lstModes[idSel].form))
		{
			return JVX_ERROR_INVALID_SETTING;
		}
		if (
			(ld_con->con_params.segmentation.x != (jvxDataFormatGroup)CjvxVideoDevice_genpcg::video.segmentsize_x.value) ||
			(ld_con->con_params.segmentation.y != (jvxDataFormatGroup)CjvxVideoDevice_genpcg::video.segmentsize_y.value)
			)
		{
			for (i = 0; i < lstModes.size(); i++)
			{
				diff = (ld_con->con_params.segmentation.x - lstModes[i].width);
				weight = diff * diff;
				diff = (ld_con->con_params.segmentation.y - lstModes[i].height);
				weight += diff * diff;
				if (weight < weightmin)
				{
					idxmin = i;
					weightmin = weight;
				}
			}
			if (JVX_CHECK_SIZE_UNSELECTED(idxmin))
			{
				return JVX_ERROR_INVALID_SETTING;
			}

			jvx_bitZSet(genV4L2_device::configuration_mf.mode_selection.value.selection(), idxmin);
			prepare_data(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (weightmin == 0)
			{
				return JVX_NO_ERROR;
			}

			return JVX_ERROR_COMPROMISE;
		}
	}
	return JVX_NO_ERROR;
}
// =========================================================================

int
CjvxVideoV4L2Device::xioctl(int fd, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fd, request, arg);
	}
	while (r == -1 && errno == EINTR);

	return r;
}
