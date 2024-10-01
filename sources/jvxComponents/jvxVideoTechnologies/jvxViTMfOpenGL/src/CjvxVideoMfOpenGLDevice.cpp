#include "CjvxVideoMfOpenGLDevice.h"
#include "CjvxVideoMfOpenGLTechnology.h"

#define JVX_ALL_PROPERTY_UPDATE_STRING "mf_device:mode_selection"


CjvxVideoMfOpenGLDevice::CjvxVideoMfOpenGLDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set_properties.reportIfNoChange = true;

	//dataProcessorAndPurpose theSelector;
	//theSelector.theType = JVX_COMPONENT_VIDEO_NODE;
	//_common_set_dataprocessor_selector.entries.push_back(theSelector);

}

CjvxVideoMfOpenGLDevice::~CjvxVideoMfOpenGLDevice()
{
}

jvxErrorType 
CjvxVideoMfOpenGLDevice::init(CjvxVideoMfOpenGLTechnology* parentTech, IMFActivate* inPtr, jvxBool isDefaultDevice)
{
	myParent = parentTech;
	thisisme = inPtr;

	jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT);

	if (isDefaultDevice)
	{
		jvx_bitZSet<jvxCBitField16>(_common_set_device.caps.flags, (int)jvxDeviceCapabilityFlagsShift::JVX_DEVICE_CAPABILITY_FLAGS_DEFAULT_DEVICE_SHIFT);
	}
	_common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVideoMfOpenGLDevice::select(IjvxObject* owner)
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
CjvxVideoMfOpenGLDevice::unselect()
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
CjvxVideoMfOpenGLDevice::activate()
{
	jvxSize i;
	jvxErrorType res = CjvxVideoDevice::activate();
	if(res == JVX_NO_ERROR)
	{
		IMFAttributes *attributes = bootup_camera();

		scanProperties(readPtr);
		attributes->Release();

		genMf_device::init_all();
		genMf_device::allocate_all();

		genMf_device::configuration_mf.mode_selection.value.entries.clear();
		for (i = 0; i < lstModes.size(); i++)
		{
			genMf_device::configuration_mf.mode_selection.value.entries.push_back(lstModes[i].descriptor);
		}

		genMf_device::register_all(static_cast<CjvxProperties*>(this));
		genMf_device::register_callbacks(static_cast<CjvxProperties*>(this), on_mode_selected, this);
		
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


		jvxSize idSel = jvx_bitfieldSelection2Id(genMf_device::configuration_mf.mode_selection.value.selection(), genMf_device::configuration_mf.mode_selection.value.entries.size());
		if (idSel >= genMf_device::configuration_mf.mode_selection.value.entries.size())
		{
			idSel = 0;
		}
		if (JVX_CHECK_SIZE_SELECTED(idSel))
		{
			jvx_bitZSet(genMf_device::configuration_mf.mode_selection.value.selection(), idSel);
		}

		// Activate multi buffering rendering
		jvx_bitZSet(genMf_device::expose_visual_if.visual_data_video.operation_mode.value.selection(), 0);

		this->updateDependentVariables_nolock();

		this->_lock_properties_local();

		return JVX_NO_ERROR;

	}
	return(res);
}

jvxErrorType
CjvxVideoMfOpenGLDevice::deactivate()
{
	jvxErrorType res = CjvxVideoDevice::deactivate();
	if (res == JVX_NO_ERROR)
	{
		genMf_device::unregister_callbacks(static_cast<CjvxProperties*>(this));

		genMf_device::unregister_all(static_cast<CjvxProperties*>(this));
		genMf_device::deallocate_all();

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

		shutdown_camera();

		return JVX_NO_ERROR;
	}
	return(res);
}

IMFAttributes *
CjvxVideoMfOpenGLDevice::bootup_camera()
{
	IMFAttributes * attributes = NULL;

	// Activate the selected device
	thisisme->ActivateObject(__uuidof(IMFMediaSource), (void**)&srcPtr);

	// Allocate attributes for configuration
	HRESULT hr = MFCreateAttributes(&attributes, 3);
	JVX_ASSERT_X("MFCreateAttributes failed", 0, (hr == S_OK));

	// Expect raw data from devices - no implicit conversion
	hr = attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
	JVX_ASSERT_X("attributes->SetUINT32 failed", 0, (hr == S_OK));

	// Provide this class as callback object
	hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
	JVX_ASSERT_X("MFStartup failed", 0, (hr == S_OK));

	// Transform Media source into a source reader
	hr = MFCreateSourceReaderFromMediaSource(srcPtr, attributes, &readPtr);
	JVX_ASSERT_X("MFCreateSourceReaderFromMediaSource failed", 0, (hr == S_OK));

	return attributes;
}

void 
CjvxVideoMfOpenGLDevice::shutdown_camera()
{
	readPtr->Release();
	readPtr = NULL;

	srcPtr->Shutdown();
	srcPtr->Release();
	srcPtr = NULL;

	thisisme->ShutdownObject();
}

jvxErrorType
CjvxVideoMfOpenGLDevice::prepare()
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
CjvxVideoMfOpenGLDevice::prepare_data(jvxBool runPrepare JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxSize idModeSelect = jvx_bitfieldSelection2Id(
		genMf_device::configuration_mf.mode_selection.value.selection(),
		genMf_device::configuration_mf.mode_selection.value.entries.size());

	JVX_ASSERT_X("Check for at least 1 mode", 0, JVX_CHECK_SIZE_SELECTED(idModeSelect));

	IMFMediaType* type = NULL;

	HRESULT hr = readPtr->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, (DWORD)lstModes[idModeSelect].id, &type);
	JVX_ASSERT_X("GetNativeMediaType failed", 0, (hr == S_OK));

	hr = readPtr->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, type);
	JVX_ASSERT_X("SetCurrentMediaType failed", 0, (hr == S_OK));

	// Get native subid to extract FOURCC code (first DWORD of GUID)
	GUID nativeSubGuid = { 0 };
	hr = type->GetGUID(MF_MT_SUBTYPE, &nativeSubGuid);

	_common_set_ocon.theData_out.con_params.format = (jvxDataFormat)lstModes[idModeSelect].form;
	_common_set_ocon.theData_out.con_params.number_channels = 1;
	_common_set_ocon.theData_out.con_params.rate = lstModes[idModeSelect].fps;
	_common_set_ocon.theData_out.con_params.format_group = (jvxDataFormatGroup)lstModes[idModeSelect].subform_sw;
	_common_set_ocon.theData_out.con_params.segmentation.x = lstModes[idModeSelect].width;
	_common_set_ocon.theData_out.con_params.segmentation.y = lstModes[idModeSelect].height;

	// Buffersize is in elements of type as described in <jvxDataFormat>
	jvxSize szElement = jvxDataFormatGroup_getsize_mult(_common_set_ocon.theData_out.con_params.format_group);
	jvxSize szElementsLine = _common_set_ocon.theData_out.con_params.segmentation.x * szElement;
	jvxSize szElementsField = _common_set_ocon.theData_out.con_params.segmentation.y * szElementsLine;
	_common_set_ocon.theData_out.con_params.buffersize = szElementsField / jvxDataFormatGroup_getsize_div(_common_set_ocon.theData_out.con_params.format_group);
	_common_set_ocon.theData_out.con_params.data_flow = jvxDataflow::JVX_DATAFLOW_PUSH_ACTIVE;

	if (runPrepare)
	{
		_common_set_ocon.theData_out.con_data.buffers = NULL;
		_common_set_ocon.theData_out.con_user.chain_spec_user_hints = NULL;
		_common_set_ocon.theData_out.con_data.number_buffers = genMf_device::systemex.number_buffers.value;
		_common_set_ocon.theData_out.con_data.number_buffers = JVX_MAX(_common_set_ocon.theData_out.con_data.number_buffers, 2);
		_common_set_ocon.theData_out.con_params.additional_flags = 0;

		runtime.params_sw.szElement = szElement;
		runtime.params_sw.szElementsLine = szElementsLine;
		runtime.params_sw.szElementsField = szElementsField;

		if (lstModes[idModeSelect].subform_hw != lstModes[idModeSelect].subform_sw)
		{
			runtime.convertOnRead.inConvertBufferInUse = true;

			// Allocate the proper conversion buffer!!
			runtime.convertOnRead.form_hw = lstModes[idModeSelect].subform_hw;
			runtime.convertOnRead.szElement = (jvxData)jvxDataFormat_size[_common_set_ocon.theData_out.con_params.format];
			runtime.convertOnRead.szElement *= (jvxData)jvxDataFormatGroup_size[runtime.convertOnRead.form_hw];
			runtime.convertOnRead.szElement /= (jvxData)jvxDataFormatGroup_size_div[runtime.convertOnRead.form_hw];
			jvxData ll = runtime.convertOnRead.szElement * lstModes[idModeSelect].width * lstModes[idModeSelect].height;
			runtime.convertOnRead.lField = JVX_DATA2SIZE(ll);
			assert(ll == (jvxData)runtime.convertOnRead.lField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(runtime.convertOnRead.bufRead, jvxByte, runtime.convertOnRead.lField);
			runtime.convertOnRead.segWidth = lstModes[idModeSelect].width;
			runtime.convertOnRead.segHeight = lstModes[idModeSelect].height;
			switch (runtime.convertOnRead.form_hw)
			{
			case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
				runtime.convertOnRead.plane1_Sz = runtime.convertOnRead.segWidth * runtime.convertOnRead.segHeight;
				runtime.convertOnRead.plane2_Sz = runtime.convertOnRead.lField - runtime.convertOnRead.plane1_Sz;
				break;
			case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
				break;
			default:
				assert(0);
			}

		}

		res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resL == JVX_NO_ERROR);
			goto leave_error;
		}

		runtime.stride = 0;

		LONG stride = 0;
		hr = MFGetStrideForBitmapInfoHeader(nativeSubGuid.Data1, (DWORD)_common_set_ocon.theData_out.con_params.segmentation.x, &stride);
		JVX_ASSERT_X("MFGetStrideForBitmapInfoHeader failed", 0, (hr == S_OK));
		runtime.stride = stride;
	}
	type->Release();
	return JVX_NO_ERROR;

leave_error:
	return(res);
	
}

jvxErrorType
CjvxVideoMfOpenGLDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return prepare_data(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxVideoMfOpenGLDevice::postprocess()
{
	jvxErrorType res = CjvxVideoDevice::postprocess();
	
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_update_properties_on_stop();
	}
	return res;
}

jvxErrorType
CjvxVideoMfOpenGLDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
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

	// Contact through
	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	runtime.stride = 0;
	runtime.params_sw.szElement = 0;
	runtime.params_sw.szElementsLine = 0;
	runtime.params_sw.szElementsField = 0;

	if (runtime.convertOnRead.inConvertBufferInUse)
	{
		JVX_DSP_SAFE_DELETE_FIELD(runtime.convertOnRead.bufRead);
		runtime.convertOnRead.inConvertBufferInUse = false;
		runtime.convertOnRead.szElement = 0;
		runtime.convertOnRead.lField = 0;
	}
	// This little trick is required to switch the camera LED off after usage ;-)
	shutdown_camera();
	bootup_camera();

	return JVX_NO_ERROR;
}


jvxErrorType
CjvxVideoMfOpenGLDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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
	HRESULT hr = readPtr->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
	JVX_ASSERT_X("readPtr->ReadSample failed", 0, (hr == S_OK));
	return(res);

leave_error:
	return(res);

}

jvxErrorType
CjvxVideoMfOpenGLDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	runtime.streamState = JVX_STATE_COMPLETE;

	// Wait for video to stop
	while (runtime.streamState == JVX_STATE_COMPLETE)
	{
		JVX_SLEEP_MS(100);
	}

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
CjvxVideoMfOpenGLDevice::set_property(jvxCallManagerProperties& callGate, 
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
			(category == genMf_device::configuration_mf.mode_selection.category) &&
			(propId == genMf_device::configuration_mf.mode_selection.globalIdx))
		{
			jvxSize idSel = jvx_bitfieldSelection2Id(genMf_device::configuration_mf.mode_selection.value.selection(), genMf_device::configuration_mf.mode_selection.value.entries.size());
			if (idSel >= genMf_device::configuration_mf.mode_selection.value.entries.size())
			{
				idSel = genMf_device::configuration_mf.mode_selection.value.entries.size() - 1;
			}
			if (JVX_CHECK_SIZE_SELECTED(idSel))
			{
				// Correct invalid selections
				jvx_bitZSet(genMf_device::configuration_mf.mode_selection.value.selection(), idSel);
			}

			this->updateDependentVariables_nolock();
			CjvxVideoDevice::updateDependentVariables_nolock(propId, category, true, callGate.call_purpose);
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}

		if (
			(category == genMf_device::systemex.number_buffers.category) &&
			(propId == genMf_device::systemex.number_buffers.globalIdx))
		{
			this->updateDependentVariables_nolock();
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}
	return(res);
}

jvxErrorType 
CjvxVideoMfOpenGLDevice::install_property_reference(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = CjvxVideoDevice::install_property_reference(callGate, ptrRaw, ident);
	if (res == JVX_NO_ERROR)
	{
		if (genMf_device::expose_visual_if.visual_data_video.rendering_target.ptr)
		{

		}
	}
	return res;
}

jvxErrorType
CjvxVideoMfOpenGLDevice::uninstall_property_reference(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = CjvxVideoDevice::uninstall_property_reference(callGate, ptrRaw, ident);
	if (res == JVX_NO_ERROR)
	{
		if (genMf_device::expose_visual_if.visual_data_video.rendering_target.ptr)
		{

		}
	}
	return res;
}
jvxErrorType
CjvxVideoMfOpenGLDevice::put_configuration(jvxCallManagerConfiguration* callConf,
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
		genMf_device::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		for (i = 0; i < warnings.size(); i++)
		{
			_report_text_message(warnings[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
		if (JVX_EVALUATE_BITFIELD(genMf_device::configuration_mf.mode_selection.value.selection()) == false)
		{
			if (genMf_device::configuration_mf.mode_selection.value.entries.size() > 0)
			{
				jvx_bitZSet(genMf_device::configuration_mf.mode_selection.value.selection(), 0);
			}
		}
		this->updateDependentVariables_nolock();
	}
	return(res);
}

jvxErrorType
CjvxVideoMfOpenGLDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
	CjvxVideoDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	// Write all parameters from this class
	genMf_device::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return(JVX_NO_ERROR);
}

void
CjvxVideoMfOpenGLDevice::updateDependentVariables_nolock()
{
	jvxSize idSel = jvx_bitfieldSelection2Id(genMf_device::configuration_mf.mode_selection.value.selection(),
		genMf_device::configuration_mf.mode_selection.value.entries.size());

	if (JVX_CHECK_SIZE_SELECTED(idSel))
	{
		genMf_device::configuration_mf.rates_min.value = (jvxInt32)lstModes[idSel].fps_min;
		genMf_device::configuration_mf.rates_max.value = (jvxInt32)lstModes[idSel].fps_max;

		CjvxVideoDevice_genpcg::video.framerate.value = (jvxInt32)lstModes[idSel].fps;
		CjvxVideoDevice_genpcg::video.segmentsize_x.value = lstModes[idSel].width;
		CjvxVideoDevice_genpcg::video.segmentsize_y.value = lstModes[idSel].height;
		CjvxVideoDevice_genpcg::video.framesize.value = lstModes[idSel].width * lstModes[idSel].height;
		CjvxVideoDevice_genpcg::video.format.value = lstModes[idSel].form;
		CjvxVideoDevice_genpcg::video.subformat.value = lstModes[idSel].subform_sw;

		// External data visualization interface must be aligned with 
		genMf_device::expose_visual_if.visual_data_video.format.value = CjvxVideoDevice_genpcg::video.format.value;
		genMf_device::expose_visual_if.visual_data_video.number_buffers.value = genMf_device::systemex.number_buffers.value;
		// only on init: genMf_device::expose_visual_if.visual_data_video.operation_mode = ...;
		// only on set_ext_ref: genMf_device::expose_visual_if.visual_data_video.rendering_external = ...;
		genMf_device::expose_visual_if.visual_data_video.segmentsize_x.value = CjvxVideoDevice_genpcg::video.segmentsize_x.value;
		genMf_device::expose_visual_if.visual_data_video.segmentsize_y.value = CjvxVideoDevice_genpcg::video.segmentsize_y.value;
		genMf_device::expose_visual_if.visual_data_video.subformat.value = CjvxVideoDevice_genpcg::video.subformat.value;

	}
	else
	{
		genMf_device::configuration_mf.rates_min.value = 0;
		genMf_device::configuration_mf.rates_max.value = 0;

		CjvxVideoDevice_genpcg::video.framerate.value = 0;
		CjvxVideoDevice_genpcg::video.segmentsize_x.value = 0;
		CjvxVideoDevice_genpcg::video.segmentsize_y.value = 0;
		CjvxVideoDevice_genpcg::video.framesize.value = 0;
		CjvxVideoDevice_genpcg::video.format.value = JVX_DATAFORMAT_NONE;
		CjvxVideoDevice_genpcg::video.subformat.value = JVX_DATAFORMAT_GROUP_GENERIC;
	}
}

jvxErrorType
CjvxVideoMfOpenGLDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

		genMf_device::expose_visual_if.visual_data_video.format.value = (jvxInt16)CjvxVideoDevice_genpcg::video.format.value;
		genMf_device::expose_visual_if.visual_data_video.segmentsize_x.value = CjvxVideoDevice_genpcg::video.segmentsize_x.value;
		genMf_device::expose_visual_if.visual_data_video.segmentsize_y.value = CjvxVideoDevice_genpcg::video.segmentsize_y.value;
		genMf_device::expose_visual_if.visual_data_video.subformat.value = CjvxVideoDevice_genpcg::video.subformat.value;
	}
	return res;
}

jvxErrorType
CjvxVideoMfOpenGLDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	// Actually, we should be able to deal with all settings since input and output are independent.
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVideoMfOpenGLDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxLinkDataDescriptor* ld_con = (jvxLinkDataDescriptor*)data;
	jvxBool foundthis = false;
	jvxSize i;
	jvxData diff, weight = 0;
	jvxData weightmin = JVX_DATA_MAX_POS;
	jvxSize idxmin = JVX_SIZE_UNSELECTED;
	jvxSize idSel = jvx_bitfieldSelection2Id(genMf_device::configuration_mf.mode_selection.value.selection(),
		genMf_device::configuration_mf.mode_selection.value.entries.size());

	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		if (
			(ld_con->con_params.format_group != lstModes[idSel].subform_sw) ||
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

			jvx_bitZSet(genMf_device::configuration_mf.mode_selection.value.selection(), idxmin);
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

