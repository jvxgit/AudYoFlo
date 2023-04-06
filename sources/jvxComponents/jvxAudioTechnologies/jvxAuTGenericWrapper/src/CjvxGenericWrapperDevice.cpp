#include <cassert>
#include <cmath>
#include <iostream>

#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapperDevice.h"
#include "CjvxCircularBuffer.h"

#include "configConfiguration.h"
#include "configSetup.h"

CjvxGenericWrapperDevice::CjvxGenericWrapperDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	JVX_MY_BASE_CLASS_D(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)

{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_DEVICE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	//_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	onInit.connectedDevice = NULL;
	onInit.parentRef = NULL;
	onInit.theFilesRef = NULL;
	onInit.theExtRef = NULL;

	runtime.theProps = NULL;
	runtime.userSelection.files.idInput = -1;
	runtime.userSelection.files.idOutput = -1;

	runtime.props_hw.buffersize.entry = -1;
	runtime.props_hw.buffersize.selection_id = -1;
	runtime.props_hw.buffersize.sellist_exists = false;

	runtime.props_hw.samplerate.entry = -1;
	runtime.props_hw.samplerate.selection_id = -1;
	runtime.props_hw.samplerate.sellist_exists = false;

	runtime.props_hw.format.selection_id = -1;

	runtime.lockParams.allParamsLocked = false;

	last_user_interaction.last_selection_user_input_channels = JVX_SIZE_UNSELECTED;
	last_user_interaction.last_selection_user_output_channels = JVX_SIZE_UNSELECTED;

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);

	// no, file writer/reader does not support following
	//_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	runtime.props_user.autosetup.forceSyncMode = false;
	runtime.props_user.autosetup.autofs = false;
	runtime.props_user.autosetup.numLoopsMax = 4;
	runtime.props_user.autosetup.deviationFsMax = 0.1;
	runtime.props_user.autosetup.disadvantageLoops = 0.99;
	runtime.props_user.autosetup.samplerateMaxAuto = 48000;

	processingControl.operationDevice = OPERATION_MODE_1;
	processingControl.timebaseDevice = TIMEBASE_CHANGE;
	processingControl.clockDevice = TRIGGER_DEVICE;
	processingControl.computedParameters.timebasechange_required = false;
	processingControl.computedParameters.resampler_required = false;
	processingControl.computedParameters.bSize_hw = -1;
	processingControl.computedParameters.bSize_hw_ds = -1;
	processingControl.computedParameters.bSize_sw = -1;
	processingControl.computedParameters.dacon_required = false;
	processingControl.computedParameters.downsamplingFactor = 1;
	processingControl.computedParameters.downSamplingFactor_real = 1;
	processingControl.computedParameters.fillHeightInput = -1;
	processingControl.computedParameters.fillHeightOutput = -1;
	processingControl.computedParameters.delayFilterResamplingOutput = -1;
	processingControl.computedParameters.delayFilterResamplingInput = -1;

	processingControl.computedParameters.form_hw = JVX_DATAFORMAT_NONE;
	processingControl.resampler_proto.theHdl = NULL;
	processingControl.resampler_proto.theObj = NULL;

	processingControl.levels.input_avrg = NULL;
	processingControl.levels.input_max = NULL;
	processingControl.levels.input_gain = NULL;
	processingControl.levels.input_clip = NULL;
	processingControl.levels.direct = NULL;
	processingControl.levels.num_entries_input = 0;

	processingControl.levels.output_avrg = NULL;
	processingControl.levels.output_max = NULL;
	processingControl.levels.output_gain = NULL;
	processingControl.levels.output_clip = NULL;
	processingControl.levels.num_entries_input = 0;

	//runtime.props_user.resampler.loadEstimate = 100;
	runtime.props_user.resampler.orderFilterResamplingOutput = 64;
	runtime.props_user.resampler.orderFilterResamplingInput = 64;

	processingControl.measureLoad.deltaT_avrg = 0;
	processingControl.measureLoad.deltaT_theory = 0;
	processingControl.measureLoad.deltaT_max = 0;

	//_common_spec_proc_set.thisisme = _common_set.thisisme;

	proc_fields.threads.thread_handler.callback_thread_startup = NULL;
	proc_fields.threads.thread_handler.callback_thread_stopped = NULL;
	proc_fields.threads.thread_handler.callback_thread_timer_expired = static_process_async;
	proc_fields.threads.thread_handler.callback_thread_wokeup = static_process_async;

	runtime.threadCtrl.coreHdl.hdl = NULL;
	runtime.threadCtrl.coreHdl.object = NULL;

	onInit.theFilesRef = NULL;
	onInit.theExtRef = NULL;
	onInit.theDummyRef = NULL;
	
	runtime.channelMappings.modifyWeightCnt = 1;

	JVX_INITIALIZE_MUTEX(safeSettings);
	//JVX_INITIALIZE_MUTEX(saveAccessCircBuffers);
}

CjvxGenericWrapperDevice::~CjvxGenericWrapperDevice()
{
	// CjvxDeviceCaps_genpcg::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

	//JVX_TERMINATE_MUTEX(saveAccessCircBuffers);
	JVX_TERMINATE_MUTEX(safeSettings);
}

jvxErrorType
CjvxGenericWrapperDevice::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch(tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
		break;

	case JVX_INTERFACE_SEQUENCERCONTROL:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxSequencerControl*>(this));
		break;

	case JVX_INTERFACE_CONFIGURATION:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		break;

	case JVX_INTERFACE_CONNECTOR_FACTORY:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(this));
		break;

	default:
		res = JVX_MY_BASE_CLASS_D::request_hidden_interface(tp, hdl);
	}
	return(res);
};

jvxErrorType
CjvxGenericWrapperDevice::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch(tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	case JVX_INTERFACE_SEQUENCERCONTROL:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxSequencerControl*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	case JVX_INTERFACE_CONFIGURATION:
		if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	case JVX_INTERFACE_CONNECTOR_FACTORY:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		res = JVX_MY_BASE_CLASS_D::return_hidden_interface(tp, hdl);
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = JVX_MY_BASE_CLASS_D::select(theOwner);
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		theRelocator.setReference(this);
		if(onInit.connectedDevice)
		{
			res = onInit.connectedDevice->initialize(static_cast<IjvxHiddenInterface*>(&theRelocator)); //_common_set_min.theHostRef);
			if(res == JVX_NO_ERROR)
			{
				// Select the referenced device
				if(onInit.parentRef)
				{
					{
						res = onInit.connectedDevice->select();
					}
				}
				else
				{
					res = onInit.connectedDevice->select();
				}
			}
		}
		if (res == JVX_NO_ERROR)
		{
			_common_set_device.caps.selectable = false;
			onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
		else
		{
			JVX_MY_BASE_CLASS_D::unselect();
		}
	}
	return(res);
}

jvxErrorType 
CjvxGenericWrapperDevice::set_location_info(const jvxComponentIdentification& tp) 
{
	jvxErrorType res = CjvxObject::_set_location_info(tp);
	if (res == JVX_NO_ERROR)
	{
		if (onInit.connectedDevice)
		{
			res = onInit.connectedDevice->set_location_info(tp);
		}
		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
	}
	return res;
};

jvxErrorType
CjvxGenericWrapperDevice::unselect()
{
	jvxErrorType res = JVX_MY_BASE_CLASS_D::unselect();
	jvxErrorType resL = JVX_NO_ERROR;

	if(res == JVX_NO_ERROR)
	{
		if(onInit.connectedDevice)
		{
			if(onInit.parentRef)
			{
				{
					res = onInit.connectedDevice->unselect();
				}
			}
			else
			{
				res = onInit.connectedDevice->unselect();
			}
			onInit.connectedDevice->terminate();
		}

		theRelocator.resetReference();
		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));

		_common_set_device.caps.selectable = true;
	}
	return(res);
}

// ==========================================================================
// ==========================================================================

jvxErrorType
CjvxGenericWrapperDevice::activate()
{
	jvxErrorType res = JVX_MY_BASE_CLASS_D::activate_lock();
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	jvxBool isValid = false;
	jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;

	if(res == JVX_NO_ERROR)
	{
		// Start secondary link structures
		theRelocator.activate();
		if (_common_set.theComponentType.slotsubid == 0)
		{
			onInit.theFilesRef = &onInit.parentRef->theFiles;
			onInit.theExtRef = &onInit.parentRef->theExtReferences;
			onInit.theDummyRef = &onInit.parentRef->theDummyChannels;;
		}
		runtime.theProps = NULL;

		// Expose properties
		genGenericWrapper_device::init__properties_active();
		genGenericWrapper_device::allocate__properties_active();
		genGenericWrapper_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genGenericWrapper_device::init__properties_active_higher();
		genGenericWrapper_device::allocate__properties_active_higher();
		genGenericWrapper_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));

		genGenericWrapper_device::register_callbacks(
			static_cast<CjvxProperties*>(this), 
			set_channel_gain,
			set_channel_index,
			get_channel_gain,
			reinterpret_cast<jvxHandle*>(this), NULL);

		runtime.props_hw.latIn = -1;
		runtime.props_hw.latOut = -1;

		// Request and receive resampler prototype from tools host
		if(this->onInit.parentRef->runtime.theToolsHost)
		{
			res = this->onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER, &this->processingControl.resampler_proto.theObj, 0, NULL);
			if((res == JVX_NO_ERROR) && this->processingControl.resampler_proto.theObj)
			{
				res =  this->processingControl.resampler_proto.theObj->request_specialization(reinterpret_cast<jvxHandle**>(& this->processingControl.resampler_proto.theHdl), NULL, NULL);
			}
			else
			{
				this->_common_set.theErrordescr = "Failed to obtain resampler tool";
			}
		}

		// Do not allow activation in case we can not get a resampler
		if(this->processingControl.resampler_proto.theHdl == NULL)
		{
			res = JVX_ERROR_NOT_READY;
		}

		// In case a device is wrapped, start that device at first
		if(onInit.connectedDevice)
		{
			// Activate the subdevice
			res = onInit.connectedDevice->activate();

			if(res == JVX_NO_ERROR)
			{
				// Check channel setting for subdevice and add all channels to the channel mapper
				res = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&runtime.theProps));
				if((res == JVX_NO_ERROR) && runtime.theProps)
				{
					lock_settings();
					this->buildPropertiesSubDevice_noLock();
					unlock_settings();
				}
			}
		}

		if(res == JVX_NO_ERROR)
		{
			lock_settings();
			this->onInit.parentRef->lock_files();

			// Make sure that all files present are taken over into the channel mappers
			updateChannelMapperWithFiles_nolock();

			// Derive current processing properties for the given setting
			updateSWSamplerateAndBuffersize_nolock(NULL, NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL); // Default parameters at first

			// Update the property to expose all channels INCLUDING the fils
			updateChannelExposure_nolock();

			this->onInit.parentRef->unlock_files();
			unlock_settings();
		}

		if(res == JVX_NO_ERROR)
		{
			lock_settings();

			// Create a struct to expose channels as properties to allow selection of channels
			updateChannelExposure_nolock();
			unlock_settings();

			// Modify property exposure type for all channels to allow reordering and saving of channels orders

			// Change the standard decoder type to allow channel reordering
			_lock_properties_local();
			CjvxAudioDevice_genpcg::properties_active.inputchannelselection.onlySelectionToFile = false;
			CjvxAudioDevice_genpcg::properties_active.outputchannelselection.onlySelectionToFile = false;

			// Modify decoder type such that reordering is going to be allowed
			this->_update_property_decoder_type(JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER,
				CjvxAudioDevice_genpcg::properties_active.inputchannelselection);
			this->_update_property_decoder_type(JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER,
				CjvxAudioDevice_genpcg::properties_active.outputchannelselection);
			_unlock_properties_local();


			// Update all dependent properties in base class
			this->updateDependentVariables_lock(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		}

		unlock_state();

		
		// If activation has failed, immediately deactivate
		if(res != JVX_NO_ERROR)
		{
			this->deactivate();
		}
		else
		{
			onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}

	}
	return(res);

}

jvxErrorType
CjvxGenericWrapperDevice::deactivate()
{
	jvxErrorType res = JVX_MY_BASE_CLASS_D::deactivate_lock();
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	if(res == JVX_NO_ERROR)
	{

		if(onInit.connectedDevice)
		{

			onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(runtime.theProps));
			runtime.theProps = NULL;

			runtime.channelMappings.inputChannelMapper.clear();
			runtime.channelMappings.outputChannelMapper.clear();
			onInit.connectedDevice->deactivate();

			if(this->onInit.parentRef->runtime.theToolsHost)
			{
				 this->onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, this->processingControl.resampler_proto.theObj, 0, NULL);
				 this->processingControl.resampler_proto.theObj = NULL;
				 this->processingControl.resampler_proto.theHdl = NULL;
			}
			/* only unregistered in base class
			this->unregister__properties_active(static_cast<CjvxProperties*>(this));
			this->deallocate__properties_active();
			*/
		}

		onInit.theFilesRef = &dummyRefs.theFiles;
		onInit.theExtRef = &dummyRefs.theExt;
		onInit.theDummyRef = &dummyRefs.theDummy;

		genGenericWrapper_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genGenericWrapper_device::deallocate__properties_active_higher();

		genGenericWrapper_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genGenericWrapper_device::deallocate__properties_active();
		unlock_state();

		// Stop secondary link structures
		theRelocator.deactivate();

		onInit.parentRef->on_device_caps_changed(static_cast<IjvxDevice*>(this));
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::setDeviceReferences(IjvxDevice* theDevice, CjvxGenericWrapperTechnology* theParent, jvxDeviceCapabilities caps, const std::string& name)
{
	onInit.connectedDevice = theDevice;
	onInit.parentRef = theParent;
	onInit.theFilesRef = &dummyRefs.theFiles;
	onInit.theExtRef = &dummyRefs.theExt;
	onInit.theDummyRef = &dummyRefs.theDummy;
	_common_set_device.caps = caps;
	_common_set.theName = name;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::deviceReference(IjvxDevice** theDevice)
{
	if (theDevice)
	{
		*theDevice = onInit.connectedDevice;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::updateChannelMapper_ActiveSelection_noLock()
{
	jvxSize i;

	int cnt0 = 0;
	int cnt1 = 0;
	int cnt = 0;

	int chanCount = 0;
	jvxBool continueLoop = true;

	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles = -1;
		runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_exclFiles = -1;
		runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice = -1;

		if(runtime.channelMappings.inputChannelMapper[i].isSelected)
		{
			runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_inclFiles = cnt0;
			cnt0 ++;
			if(runtime.channelMappings.inputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				runtime.channelMappings.inputChannelMapper[i].toSw.pointsToId_exclFiles = cnt1;
				cnt1++;
			}
		}
	}

	cnt = 0;

	while(continueLoop)
	{
		jvxBool foundit = false;
		for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			if(runtime.channelMappings.inputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				if(runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice == chanCount)
				{
					foundit = true;
					if(runtime.channelMappings.inputChannelMapper[i].isSelected)
					{
						runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice = cnt;
						cnt ++;
					}
					chanCount++;
					break;
				}
			}
		}
		if(foundit == false)
		{
			continueLoop = false;
		}
	}

	cnt0 = 0;
	cnt1 = 0;

	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles = -1;
		runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_exclFiles = -1;
		runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice = -1;

		if(runtime.channelMappings.outputChannelMapper[i].isSelected)
		{
			runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_inclFiles = cnt0;
			cnt0 ++;
			if(runtime.channelMappings.outputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				runtime.channelMappings.outputChannelMapper[i].toSw.pointsToId_exclFiles = cnt1;
				cnt1++;
			}
		}
	}

	cnt = 0;
	chanCount = 0;
	continueLoop = true;

	while(continueLoop)
	{
		jvxBool foundit = false;
		for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			if(runtime.channelMappings.outputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
			{
				if(runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice == chanCount)
				{
					foundit = true;
					if(runtime.channelMappings.outputChannelMapper[i].isSelected)
					{
						runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdSelectionThisDevice = cnt;
						cnt ++;
					}
					chanCount++;
					break;
				}
			}
		}
		if(foundit == false)
		{
			continueLoop = false;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::feature_class(jvxBitField* ft)
{
	jvxBitField ftLoc = 0;
	if(onInit.connectedDevice)
	{
		onInit.connectedDevice->feature_class(&ftLoc);
	}
	if(ft)
	{
		*ft = ftLoc;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = CjvxAudioDevice::is_ready(suc, reasonIfNot);
	if(res == JVX_NO_ERROR)
	{
		assert(suc);
		if(*suc)
		{
			if(onInit.connectedDevice)
			{
				res = onInit.connectedDevice->is_ready(suc, reasonIfNot);
			}
		}
	}
	return(res);
}


jvxErrorType
CjvxGenericWrapperDevice::rebuildPropertiesSubDevice_noLock()
{
	jvxSize i,j;
	std::vector<oneEntryChannelMapper> oldChansInput = runtime.channelMappings.inputChannelMapper;
	std::vector<oneEntryChannelMapper> oldChansOutput = runtime.channelMappings.outputChannelMapper;

	runtime.channelMappings.inputChannelMapper.clear();
	runtime.channelMappings.outputChannelMapper.clear();

	buildPropertiesSubDevice_noLock();

	// Make sure that all files present are taken over into the channel mappers
	updateChannelMapperWithFiles_nolock(); 

	// HERE: Try to reconstruct info from before considering the order - even if the number of channels has changed

	// ============================================================== 
	// INPUT
	// ============================================================== 
	std::vector<oneEntryChannelMapper> reorderedChansInput;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		runtime.channelMappings.inputChannelMapper[i].tmp.check = false;
	}

	for(i = 0; i < oldChansInput.size(); i++)
	{
		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if(oldChansInput[i].theChannelType == runtime.channelMappings.inputChannelMapper[j].theChannelType)
			{
				if(
					(oldChansInput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) ||
					(oldChansInput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) ||
					(oldChansInput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY))
				{
					if(oldChansInput[i].refHint == runtime.channelMappings.inputChannelMapper[j].refHint)
					{
						if(oldChansInput[i].toHwFile.pointsToIdAllThisDevice == runtime.channelMappings.inputChannelMapper[j].toHwFile.pointsToIdAllThisDevice)
						{
							runtime.channelMappings.inputChannelMapper[j].isSelected = oldChansInput[i].isSelected; // Need to update file channels
							runtime.channelMappings.inputChannelMapper[j].tmp.check = true;
							reorderedChansInput.push_back(runtime.channelMappings.inputChannelMapper[j]);
							break;
						}
					}
				}
				if(oldChansInput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
				{
					if(runtime.channelMappings.inputChannelMapper[j].tmp.check == false)
					//if(oldChansInput[i].refHint == runtime.channelMappings.inputChannelMapper[j].refHint)
					{
						runtime.channelMappings.inputChannelMapper[j].tmp.check = true;
						reorderedChansInput.push_back(runtime.channelMappings.inputChannelMapper[j]);
						break;
					}
				}
			}
		}
	}
	for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
	{
		if(runtime.channelMappings.inputChannelMapper[j].tmp.check == false)
		{
			reorderedChansInput.push_back(runtime.channelMappings.inputChannelMapper[j]);
		}
	}
	runtime.channelMappings.inputChannelMapper = reorderedChansInput;

	// ============================================================== 
	// OUTPUT
	// ============================================================== 
	std::vector<oneEntryChannelMapper> reorderedChansOutput;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		runtime.channelMappings.outputChannelMapper[i].tmp.check = false;
	}

	for(i = 0; i < oldChansOutput.size(); i++)
	{
		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if(oldChansOutput[i].theChannelType == runtime.channelMappings.outputChannelMapper[j].theChannelType)
			{
				if(
					(oldChansOutput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) ||
					(oldChansOutput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) ||
					(oldChansOutput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY))
				{
					if(oldChansOutput[i].refHint == runtime.channelMappings.outputChannelMapper[j].refHint)
					{
						if(oldChansOutput[i].toHwFile.pointsToIdAllThisDevice == runtime.channelMappings.outputChannelMapper[j].toHwFile.pointsToIdAllThisDevice)
						{
							runtime.channelMappings.outputChannelMapper[j].isSelected = oldChansOutput[i].isSelected;
							runtime.channelMappings.outputChannelMapper[j].tmp.check = true;
							reorderedChansOutput.push_back(runtime.channelMappings.outputChannelMapper[j]);
							break;
						}
					}
				}
				if(oldChansOutput[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
				{
					if(runtime.channelMappings.outputChannelMapper[j].tmp.check == false)
					//if(oldChansInput[i].refHint == runtime.channelMappings.inputChannelMapper[j].refHint)
					{
						runtime.channelMappings.outputChannelMapper[j].tmp.check = true;
						reorderedChansOutput.push_back(runtime.channelMappings.outputChannelMapper[j]);
						break;
					}
				}
			}
		}
	}
	for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
	{
		if(runtime.channelMappings.outputChannelMapper[j].tmp.check == false)
		{
			reorderedChansOutput.push_back(runtime.channelMappings.outputChannelMapper[j]);
		}
	}
	runtime.channelMappings.outputChannelMapper = reorderedChansOutput;



	// Derive current processing properties for the given setting
	updateSWSamplerateAndBuffersize_nolock(NULL, NULL JVX_CONNECTION_FEEDBACK_CALL_A_NULL); // Use the default parameters at first

	// Update the property to expose all channels INCLUDING the fils
	updateChannelExposure_nolock();

	return(JVX_NO_ERROR);
}




jvxErrorType
CjvxGenericWrapperDevice::buildPropertiesSubDevice_noLock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	jvxCallManagerProperties callGate;
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident;
	jvx::propertyDetail::CjvxTranferDetail trans;
	idx = 0;
	format = JVX_DATAFORMAT_NONE;
	num = 0;
	decHtTp = JVX_PROPERTY_DECODER_NONE;

	if(runtime.theProps)
	{
		if(jvx_findPropertyDescriptor("jvx_selection_input_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(res == JVX_NO_ERROR)
			{
				for(i = 0; i < fldSelList.strList.ll(); i++)
				{
					oneEntryChannelMapper oneElm;
					oneElm.description = fldSelList.strList.std_str_at(i);
					oneElm.friendlyName = fldSelList.strList.std_str_at(i);
					oneElm.refHint = (jvxInt32)i;
					oneElm.theChannelType = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE;
					oneElm.toHwFile.pointsToIdAllThisDevice = i;
					if(jvx_bitTest(fldSelList.bitFieldSelected(),i))
					{
						oneElm.isSelected = true;
					}
					else
					{
						oneElm.isSelected = false;
					}
					runtime.channelMappings.inputChannelMapper.push_back(oneElm);
				}
			}
		}
		else
		{
			assert(0);
		}

		idx = 0;
		format = JVX_DATAFORMAT_NONE;
		num = 0;
		decHtTp = JVX_PROPERTY_DECODER_NONE;

		if(jvx_findPropertyDescriptor("jvx_selection_output_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(res == JVX_NO_ERROR)
			{
				for(i = 0; i < fldSelList.strList.ll(); i++)
				{
					oneEntryChannelMapper oneElm;
					oneElm.description = fldSelList.strList.std_str_at(i);
					oneElm.friendlyName = fldSelList.strList.std_str_at(i);
					oneElm.refHint = (jvxInt32)i;
					oneElm.theChannelType = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE;
					oneElm.toHwFile.pointsToIdAllThisDevice = i;
					if(jvx_bitTest(fldSelList.bitFieldSelected(), i))
					{
						oneElm.isSelected = true;
					}
					else
					{
						oneElm.isSelected = false;
					}
					runtime.channelMappings.outputChannelMapper.push_back(oneElm);
				}
			}
		}
		else
		{
			assert(0);
		}

		// Receive input and output latency
		if(jvx_findPropertyDescriptor("jvx_input_delay_int32", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&runtime.props_hw.latIn, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
			if(res != JVX_NO_ERROR)
			{
				res = JVX_NO_ERROR;
			}
		}

		if(jvx_findPropertyDescriptor("jvx_output_delay_int32", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&runtime.props_hw.latOut, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
			if(res != JVX_NO_ERROR)
			{
				res = JVX_NO_ERROR;
			}
		}

		// Obtain all processing properties from sub component
		pullProperty_buffersize_nolock();
		pullProperty_samplerate_nolock();
		pullProperty_format_nolock();
	}
	return(res);
}

/*
void
CjvxGenericWrapperDevice::reconstructExposedParameters()
{
	runtime.channelMappings.inputChannelMapper.clear();
	runtime.channelMappings.outputChannelMapper.clear();
	buildPropertiesSubDevice();
}
*/

jvxErrorType
CjvxGenericWrapperDevice::select_master(IjvxDataConnectionProcess* ref)
{
	jvxErrorType res = CjvxConnectionMaster::_select_master(ref);

	if (onInit.connectedDevice)
	{
		res = theRelocator.x_select_master();
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::unselect_master()
{
	// This command starts the secondary chain to the connected device.
	// If no device is attached, this function will return UNSUPPORTED
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		res = theRelocator.x_unselect_master();
		assert(res == JVX_NO_ERROR);
	}
	CjvxConnectionMaster::_unselect_master();
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::associated_process(IjvxDataConnectionProcess** ref)
{
	return _associated_process(ref);
}

jvxErrorType
CjvxGenericWrapperDevice::connect_chain_master(const jvxChainConnectArguments& args,
	const jvxLinkDataDescriptor_con_params* i_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		res = CjvxConnectionMaster::_activate_master(NULL, NULL);
		if (res != JVX_NO_ERROR)
		{
			goto leave_error;
		}

		uIdMain = args.uIdConnection;

		res = theRelocator.x_connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			res = theRelocator.x_disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			// Set the connectionflag manually since no default implmentation is used
			_common_set_ld_master.isConnected = true;
		}
	}
	else
	{
		// If return value is UNSUPPORTED, no hw device is involved.
		// We need to start all the connections ourself and not via the 
		// connected device
		res = CjvxConnectionMaster::_activate_master(static_cast<IjvxOutputConnector*>(this), static_cast<IjvxConnectionMaster*>(this));
		if (res != JVX_NO_ERROR)
		{
			goto leave_error;
		}
		res = CjvxConnectionMaster::_connect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			res = CjvxConnectionMaster::_disconnect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			assert(res == JVX_NO_ERROR);
		}
	}
	return res;

leave_error:
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::disconnect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		res = theRelocator.x_disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
		uIdMain = JVX_SIZE_UNSELECTED;
		res = CjvxConnectionMaster::_deactivate_master(NULL, NULL);
	}
	else
	{		
		res = CjvxConnectionMaster::_disconnect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		assert(res == JVX_NO_ERROR);
		res = CjvxConnectionMaster::_deactivate_master(static_cast<IjvxOutputConnector*>(this), static_cast<IjvxConnectionMaster*>(this));
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		switch (tp)
		{
		case JVX_MASTER_UPDATE_EVENT_STARTED:
			res = theRelocator.x_connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			break;
		case JVX_MASTER_UPDATE_EVENT_ABOUT_TO_STOP:
			res = theRelocator.x_disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			break;
		case JVX_MASTER_UPDATE_EVENT_LAYOUT_CHANGED:
			res = theRelocator.x_disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			res = theRelocator.x_connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			break;
		default:
			assert(0);
		}
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		CjvxConnectionMaster::_activate_master(static_cast<IjvxOutputConnector*>(this), static_cast<IjvxConnectionMaster*>(this));
		res = CjvxConnectionMaster::_inform_changed_master(tp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType 
CjvxGenericWrapperDevice::transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		res = theRelocator.x_transfer_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		
		res = CjvxConnectionMaster::_transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}

	if (res == JVX_NO_ERROR)
	{
		switch (tp) 
		{
		case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:
			break;
		}
	}

	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	if (onInit.connectedDevice)
	{
		res = theRelocator.x_test_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		updateSWSamplerateAndBuffersize_nolock(NULL, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType 
CjvxGenericWrapperDevice::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Terminate local processing chain
	res = CjvxAudioDevice::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (onInit.connectedDevice)
	{
		// Forward to the x side
		res = theRelocator.connect_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType 
CjvxGenericWrapperDevice::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Forward to the x side
	if (onInit.connectedDevice)
	{
		res = theRelocator.disconnect_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	// Disconnect local chaning side
	res = CjvxAudioDevice::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::test_connect_icon_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string txt;
	local_proc_params params;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	populate_countchannels_datatype(params);
	if (
		(_common_set_ldslave.theData_in->con_params.buffersize != params.buffersize) ||
		(_common_set_ldslave.theData_in->con_params.rate != params.samplerate) ||
		(_common_set_ldslave.theData_in->con_params.format != params.format) ||
		(_common_set_ldslave.theData_in->con_params.number_channels != params.chans_out))
	{
		jvxLinkDataDescriptor ld_loc;
		ld_loc.con_params.number_channels = params.chans_out;
		ld_loc.con_params.buffersize = params.buffersize;
		ld_loc.con_params.rate = params.samplerate;
		ld_loc.con_params.format = params.format;
		ld_loc.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
		ld_loc.con_params.segmentation.x = ld_loc.con_params.buffersize;
		ld_loc.con_params.segmentation.y = 1;
		res = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
			&ld_loc JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		switch (res)
		{
		case JVX_ERROR_COMPROMISE:

			// If this complain went all the way back to the source, these parameters may have changed!!
			// Therefore we need to read it once more!
			release_countchannels_datatype(params);
			populate_countchannels_datatype(params);

			if (
				(_common_set_ldslave.theData_in->con_params.buffersize != params.buffersize) ||
				(_common_set_ldslave.theData_in->con_params.rate != params.samplerate) ||
				(_common_set_ldslave.theData_in->con_params.format != params.format))
			{
				std::string err = "";
				if (_common_set_ldslave.theData_in->con_params.buffersize != params.buffersize)
				{
					err = "Buffersize: " + jvx_size2String(_common_set_ldslave.theData_in->con_params.buffersize) + " vs " +
						jvx_size2String(params.buffersize);
				}
				if (_common_set_ldslave.theData_in->con_params.rate != params.samplerate)
				{
					err = "Samplerate: " + jvx_size2String(_common_set_ldslave.theData_in->con_params.rate) + " vs " +
						jvx_size2String(params.samplerate);
				}
				if (_common_set_ldslave.theData_in->con_params.format != params.format)
				{
					err = "Format: ";
					err += jvxDataFormat_txt(_common_set_ldslave.theData_in->con_params.format);
					err += " vs ";
					err += jvxDataFormat_txt(params.format);
				}
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, 
					("Could not find compromise with previous audio node: " + err).c_str(), JVX_ERROR_INVALID_SETTING);
				res = JVX_ERROR_INVALID_SETTING;

			}
			else
			{
			  if (_common_set_ldslave.theData_in->con_params.number_channels <= JVX_SIZE_INT32(runtime.channelMappings.outputChannelMapper.size()))
				{
				  int delta = JVX_SIZE_INT32(_common_set_ldslave.theData_in->con_params.number_channels) - JVX_SIZE_INT32(params.chans_out);
					
					while(delta > 0)
					{
						jvxSize minCnt = JVX_SIZE_UNSELECTED;
						// We need to deactivate one channel
						if (genGenericWrapper_device::properties_active.fill_channels_from_last_out.value)
						{
							std::vector< oneEntryChannelMapper>::reverse_iterator elmr = 
								runtime.channelMappings.outputChannelMapper.rbegin();
							std::vector< oneEntryChannelMapper>::reverse_iterator elmrs = 
								runtime.channelMappings.outputChannelMapper.rend();
							for (; elmr != runtime.channelMappings.outputChannelMapper.rend(); elmr++)
							{
								if (elmr->lastModCnt < minCnt)
								{
									if (!elmr->isSelected)
									{
										elmrs = elmr;
										minCnt = elmr->lastModCnt;
									}
								}
							}
							if (elmrs != runtime.channelMappings.outputChannelMapper.rend())
							{
								elmrs->isSelected = true;
								delta--;
								elmrs->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
							}
							else
							{
								assert(0);
							}
						}
						else
						{
							std::vector< oneEntryChannelMapper>::iterator elm = runtime.channelMappings.outputChannelMapper.begin();
							std::vector< oneEntryChannelMapper>::iterator elms = runtime.channelMappings.outputChannelMapper.end();
							for (; elm != runtime.channelMappings.outputChannelMapper.end(); elm++)
							{
								if (elm->lastModCnt < minCnt)
								{
									if (!elm->isSelected)
									{
										elms = elm;
										minCnt = elm->lastModCnt;
									}
								}
							}
							if (elms != runtime.channelMappings.outputChannelMapper.end())
							{
								elms->isSelected = true;
								delta--;
								elms->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
							}
							else
							{
								assert(0);
							}
						}
					}

					// ===============================================================

					while (delta < 0)
					{
						jvxSize minCnt = JVX_SIZE_UNSELECTED;
						if (genGenericWrapper_device::properties_active.fill_channels_from_last_out.value)
						{
							std::vector< oneEntryChannelMapper>::iterator elm =
								runtime.channelMappings.outputChannelMapper.begin();
							std::vector< oneEntryChannelMapper>::iterator elms = 
								runtime.channelMappings.outputChannelMapper.end();
							for (; elm != runtime.channelMappings.outputChannelMapper.end(); elm++)
							{
								if (elm->lastModCnt < minCnt)
								{
									if (elm->isSelected)
									{
										elms = elm;
										minCnt = elm->lastModCnt;
									}
								}
							}
							if (elms != runtime.channelMappings.outputChannelMapper.end())
							{
								elms->isSelected = false;
								delta++;
								elms->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
							}
							else
							{
								assert(0);
							}
						}
						else
						{
							std::vector< oneEntryChannelMapper>::reverse_iterator elmr = runtime.channelMappings.outputChannelMapper.rbegin();
							std::vector< oneEntryChannelMapper>::reverse_iterator elmrs = runtime.channelMappings.outputChannelMapper.rend();
							for (; elmr != runtime.channelMappings.outputChannelMapper.rend(); elmr++)
							{
								if (elmr->lastModCnt < minCnt)
								{
									if (elmr->isSelected)
									{
										elmrs = elmr;
										minCnt = elmr->lastModCnt;
									}
								}
							}
							if (elmrs != runtime.channelMappings.outputChannelMapper.rend())
							{
								elmrs->isSelected = false;
								delta++;
								elmrs->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
							}
							else
							{
								assert(0);
							}
						}
					}

					assert(delta == 0);

					// Show channels as newly selected
					updateChannelExposure_nolock();
					updateDependentVariables_lock(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.globalIdx,
						CjvxAudioDevice_genpcg::properties_active.outputchannelselection.category, false,
						JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS);
					res = JVX_NO_ERROR;
				}
				else
				{
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, 
						"Could not find compromise with previous audio node - not sufficient channels available.",
						JVX_ERROR_INVALID_SETTING);
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
			break;

		case JVX_NO_ERROR:
			break;

		default:
			// Error case. Prepare error message and return error
			txt = __FUNCTION__;
			txt += ": Failed to negotiate processing properties with output connector.";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
		}
	}
	// release_countchannels_datatype(params); <- not required since local variable in use
	return res;
}
jvxErrorType
CjvxGenericWrapperDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str(), 
		"Entering CjvxGenericWrapperDevice default input connector");
	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, _common_set_ldslave.theData_in);

	res = test_connect_icon_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (res != JVX_NO_ERROR)
		return res;

	// Terminate local processing chain
	CjvxAudioDevice::_test_connect_icon(false, JVX_CONNECTION_FEEDBACK_CALL(fdb));

	if (onInit.connectedDevice)
	{
		// Forward to the x side
		res = theRelocator.test_connect_icon_x(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Terminate local processing chain
	res = CjvxAudioDevice::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	switch (tp)
	{
	case jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:

		// This type call ends here. Maybe that should be done also for other requests
		return res;
	}

	if (onInit.connectedDevice)
	{
		// Forward to the x side
		res = theRelocator.transfer_forward_icon_x(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		/*
		if (tp != JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
		{
			assert(res == JVX_NO_ERROR);
		}
		*/
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* theLocDescr = NULL;
	jvx::propertyCallCompactList* propCallCompact = nullptr;
	jvxSize idx = 0;
	local_proc_params params;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		theLocDescr = (jvxLinkDataDescriptor*)data;
		populate_countchannels_datatype(params);

		if (
			(CjvxAudioDevice_genpcg::properties_active.buffersize.value != JVX_SIZE_INT32(theLocDescr->con_params.buffersize)) ||
			(CjvxAudioDevice_genpcg::properties_active.samplerate.value != JVX_SIZE_INT32(theLocDescr->con_params.rate)) ||
			(CjvxAudioDevice_genpcg::properties_active.format.value != theLocDescr->con_params.format))
		{
			// If there is a mismatch between the algo and the generic wrapper, we need to propagate this info further

			// First we modify the internal setting
			CjvxAudioDevice_genpcg::properties_active.buffersize.value = JVX_SIZE_INT32(theLocDescr->con_params.buffersize);
			CjvxAudioDevice_genpcg::properties_active.samplerate.value = JVX_SIZE_INT32(theLocDescr->con_params.rate);
			jvxSize idSelForm = JVX_SIZE_UNSELECTED;
			jvxSize i;
			for (i = 0; i < CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries.size(); i++)
			{
				if (theLocDescr->con_params.format == jvxDataFormat_decode(CjvxAudioDevice_genpcg::properties_active.formatselection.value.entries[i].c_str()))
				{
					idSelForm = i;
					break;
				}
			}
			if (JVX_CHECK_SIZE_SELECTED(idSelForm))
			{
				CjvxAudioDevice_genpcg::properties_active.format.value = theLocDescr->con_params.format;
				jvx_bitZSet(CjvxAudioDevice_genpcg::properties_active.formatselection.value.selection(), idSelForm);
			}

			// Then we propagate it further OR
			if (theRelocator.runtime.refDevice)
			{
				// Try to propagate it further. Pass theLocDescr only for control
				res = theRelocator.transfer_backward_x(theLocDescr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res != JVX_NO_ERROR)
				{
					// Of this failed, we return an error

					// We need to propagate it through
					std::string txt = __FUNCTION__;
					txt += ": ";
					txt += ": Unable to propagate parameters any further.";
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
					return JVX_ERROR_UNSUPPORTED;
				}
			}
			else
			{
				// We update the data structs
				updateSWSamplerateAndBuffersize_nolock(NULL, NULL JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				// Only use the settings as specified
			}
		}
		
		// Consider channels separately
		if (theLocDescr->con_params.number_channels <= runtime.channelMappings.inputChannelMapper.size())
		{
		  int delta = JVX_SIZE_INT32(params.chans_in) - JVX_SIZE_INT32(theLocDescr->con_params.number_channels);

			while (delta < 0)
			{
				jvxSize minCnt = JVX_SIZE_UNSELECTED;
				// We need to deactivate one channel
				if (genGenericWrapper_device::properties_active.fill_channels_from_last_in.value)
				{
					std::vector< oneEntryChannelMapper>::reverse_iterator elmr = 
						runtime.channelMappings.inputChannelMapper.rbegin();
					std::vector< oneEntryChannelMapper>::reverse_iterator elmrs = 
						runtime.channelMappings.inputChannelMapper.rend();
					for (; elmr != runtime.channelMappings.inputChannelMapper.rend(); elmr++)
					{
						if (elmr->lastModCnt < minCnt)
						{
							if (!elmr->isSelected)
							{
								elmrs = elmr;
								minCnt = elmr->lastModCnt;
							}
						}
					}
					if (elmrs != runtime.channelMappings.inputChannelMapper.rend())
					{
						elmrs->isSelected = true;
						delta++;
						elmrs->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
					else
					{
						assert(0);
					}
				}
				else
				{
					std::vector< oneEntryChannelMapper>::iterator elm = runtime.channelMappings.inputChannelMapper.begin();
					std::vector< oneEntryChannelMapper>::iterator elms = runtime.channelMappings.inputChannelMapper.end();
					for (; elm != runtime.channelMappings.inputChannelMapper.end(); elm++)
					{
						if (elm->lastModCnt < minCnt)
						{
							if (!elm->isSelected)
							{
								elms = elm;
								minCnt = elm->lastModCnt;
							}
						}
					}
					if (elms != runtime.channelMappings.inputChannelMapper.end())
					{
						elms->isSelected = true;
						delta++;
						elms->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
					else
					{
						assert(0);
					}
				}
			}

			// ===============================================================

			while(delta > 0)
			{
				jvxSize minCnt = JVX_SIZE_UNSELECTED;
				if (genGenericWrapper_device::properties_active.fill_channels_from_last_in.value)
				{
					std::vector< oneEntryChannelMapper>::iterator elm = 
						runtime.channelMappings.inputChannelMapper.begin();
					std::vector< oneEntryChannelMapper>::iterator elms = 
						runtime.channelMappings.inputChannelMapper.end();
					for (; elm != runtime.channelMappings.inputChannelMapper.end(); elm++)
					{
						if (elm->lastModCnt < minCnt)
						{
							if (elm->isSelected)
							{
								elms = elm;
								minCnt = elm->lastModCnt;
							}
						}
					}
					if (elms != runtime.channelMappings.inputChannelMapper.end())
					{
						elms->isSelected = false;
						delta--;
						elms->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
					else
					{
						assert(0);
					}

				}
				else
				{
					std::vector< oneEntryChannelMapper>::reverse_iterator elmr = runtime.channelMappings.inputChannelMapper.rbegin();
					std::vector< oneEntryChannelMapper>::reverse_iterator elmrs = runtime.channelMappings.inputChannelMapper.rend();
					for (; elmr != runtime.channelMappings.inputChannelMapper.rend(); elmr++)
					{
						if (elmr->lastModCnt < minCnt)
						{
							if (elmr->isSelected)
							{
								elmrs = elmr;
								minCnt = elmr->lastModCnt;
							}
						}
					}
					if (elmrs != runtime.channelMappings.inputChannelMapper.rend())
					{
						elmrs->isSelected = false;
						delta--;
						elmrs->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
					else
					{
						assert(0);
					}
				}
			}

			assert(delta == 0);

			// Show channels as newly selected
			updateChannelExposure_nolock();
			updateDependentVariables_lock(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.globalIdx,
				CjvxAudioDevice_genpcg::properties_active.inputchannelselection.category, false,
				JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS);
		}
		else
		{
			std::string txt = __FUNCTION__;
			txt += ": ";
			txt += ": Unable to allow ";
			txt += jvx_size2String(theLocDescr->con_params.number_channels);
			txt += " input channels. Only ";
			txt += jvx_size2String(runtime.channelMappings.inputChannelMapper.size());
			txt += " channels are available.";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), JVX_ERROR_INVALID_SETTING);
			return JVX_ERROR_UNSUPPORTED;
		}
		//updateSWSamplerateAndBuffersize_nolock();
		// release_countchannels_datatype(params); <- not required since local variable
		
		_common_set_ldslave.theData_out.con_params.buffersize = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
		_common_set_ldslave.theData_out.con_params.rate = CjvxAudioDevice_genpcg::properties_active.samplerate.value;
		_common_set_ldslave.theData_out.con_params.format = (jvxDataFormat)CjvxAudioDevice_genpcg::properties_active.format.value;
		_common_set_ldslave.theData_out.con_params.number_channels = CjvxAudioDevice_genpcg::properties_active.numberinputchannels.value;

		res = JVX_NO_ERROR;
		break;

	case JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT:
		if (_common_set_ldslave.theData_out.con_user.chain_spec_user_hints)
		{
			jvxDataProcessorHintDescriptorSearch* ptr = (jvxDataProcessorHintDescriptorSearch*)data;
			if (data)
			{
				jvxErrorType res = jvx_hintDesriptor_find(_common_set_ldslave.theData_out.con_user.chain_spec_user_hints,
					&ptr->reference, ptr->hintDescriptorId, ptr->assocHint);
				if (res == JVX_NO_ERROR)
				{
					return res;
				}
			}
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
		break;
	default:
		// Terminate local processing chain
		res = CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

		switch (tp)
		{
		case jvxLinkDataTransferType::JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES:

			// This type call ends here. Maybe that should be done also for other requests
			return res;
		}

		if (onInit.connectedDevice)
		{
			// Forward to the x side
			res = theRelocator.transfer_backward_ocon_x(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			assert(res == JVX_NO_ERROR);
		}
		break;
	}
	return res;
}
// ===================================================================================

jvxErrorType 
CjvxGenericWrapperDevice::iterator_chain(IjvxConnectionIterator** it)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		// Forward to the first eement in chain - which is the embedded device
		res = theRelocator.iterator_chain_x(it);
	}
	else
	{
		res = JVX_MY_BASE_CLASS_D::iterator_chain(it);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::number_next(jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		// Forward to the first eement in chain - which is the embedded device
		res = theRelocator.number_next_x(num);
	}
	else
	{
		res = JVX_MY_BASE_CLASS_D::number_next(num);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::reference_next(jvxSize idx, IjvxConnectionIterator** next)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		// Forward to the first eement in chain - which is the embedded device
		res = theRelocator.reference_next_x(idx, next);
	}
	else
	{
		res = JVX_MY_BASE_CLASS_D::reference_next(idx, next);
	}
	return res;
}
/*
jvxErrorType
CjvxGenericWrapperDevice::number_next(jvxSize* num)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (onInit.connectedDevice)
	{
		res = theRelocator.number_next_x(num);
	}
	else
	{
		res = JVX_MY_BASE_CLASS_D::_number_next(num);
	}
	return res;
}
*/

jvxErrorType 
CjvxGenericWrapperDevice::number_next_x(jvxSize* num)
{
	return _number_next(num);
}

jvxErrorType
CjvxGenericWrapperDevice::reference_next_x(jvxSize idx, IjvxConnectionIterator** next)
{
	return _reference_next(idx, next);
}

