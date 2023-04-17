#include "CjvxAudioAsioDevice.h"
#include "jvx_config.h"

#define RESTRICT_BUFFERSIZE

typedef struct
{
	CjvxAudioAsioDevice* associatedWithClass;
	ASIOCallbacks theCallbacks;
} oneFunctionCallHandler;

oneFunctionCallHandler* allHandlerPtrs = NULL;

// ================================================================================================

void bufferSwitch_0(long doubleBufferIndex, ASIOBool directProcess)
{
	allHandlerPtrs[0].associatedWithClass->bufferSwitch(doubleBufferIndex, directProcess);
}
ASIOTime* bufferSwitchTimeInfo_0(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return(allHandlerPtrs[0].associatedWithClass->bufferSwitchTimeInfo(params, doubleBufferIndex, directProcess));
}
void samplerateDidChange_0(ASIOSampleRate sRate)
{
	allHandlerPtrs[0].associatedWithClass->samplerateDidChange(sRate);
}
long asioMessage_0(long selector, long value, void* message, double* opt)
{
	return(allHandlerPtrs[0].associatedWithClass->asioMessage(selector, value, message, opt));
}

// ================================================================================================

void bufferSwitch_1(long doubleBufferIndex, ASIOBool directProcess)
{
	allHandlerPtrs[1].associatedWithClass->bufferSwitch(doubleBufferIndex, directProcess);
}
ASIOTime* bufferSwitchTimeInfo_1(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return(allHandlerPtrs[1].associatedWithClass->bufferSwitchTimeInfo(params, doubleBufferIndex, directProcess));
}
void samplerateDidChange_1(ASIOSampleRate sRate)
{
	allHandlerPtrs[1].associatedWithClass->samplerateDidChange(sRate);
}
long asioMessage_1(long selector, long value, void* message, double* opt)
{
	return(allHandlerPtrs[1].associatedWithClass->asioMessage(selector, value, message, opt));
}

// ================================================================================================

void bufferSwitch_2(long doubleBufferIndex, ASIOBool directProcess)
{
	allHandlerPtrs[2].associatedWithClass->bufferSwitch(doubleBufferIndex, directProcess);
}
ASIOTime* bufferSwitchTimeInfo_2(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return(allHandlerPtrs[2].associatedWithClass->bufferSwitchTimeInfo(params, doubleBufferIndex, directProcess));
}
void samplerateDidChange_2(ASIOSampleRate sRate)
{
	allHandlerPtrs[2].associatedWithClass->samplerateDidChange(sRate);
}
long asioMessage_2(long selector, long value, void* message, double* opt)
{
	return(allHandlerPtrs[2].associatedWithClass->asioMessage(selector, value, message, opt));
}

// ================================================================================================

void bufferSwitch_3(long doubleBufferIndex, ASIOBool directProcess)
{
	allHandlerPtrs[3].associatedWithClass->bufferSwitch(doubleBufferIndex, directProcess);
}
ASIOTime* bufferSwitchTimeInfo_3(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return(allHandlerPtrs[3].associatedWithClass->bufferSwitchTimeInfo(params, doubleBufferIndex, directProcess));
}
void samplerateDidChange_3(ASIOSampleRate sRate)
{
	allHandlerPtrs[3].associatedWithClass->samplerateDidChange(sRate);
}
long asioMessage_3(long selector, long value, void* message, double* opt)
{
	return(allHandlerPtrs[3].associatedWithClass->asioMessage(selector, value, message, opt));
}

// ================================================================================================

#define NUM_BUFFERSWITCH_CB_HANDLERS 4
static oneFunctionCallHandler allHandlers[NUM_BUFFERSWITCH_CB_HANDLERS] =
{
	{
		NULL, {bufferSwitch_0, samplerateDidChange_0, asioMessage_0, bufferSwitchTimeInfo_0}
	},
	{
		NULL, {bufferSwitch_1, samplerateDidChange_1, asioMessage_1, bufferSwitchTimeInfo_1}
	},
	{
		NULL, {bufferSwitch_2, samplerateDidChange_2, asioMessage_2, bufferSwitchTimeInfo_2}
	},
	{
		NULL, {bufferSwitch_3, samplerateDidChange_3, asioMessage_3, bufferSwitchTimeInfo_3}
	}
};

static
HWND GetConsoleHwnd()
{
#define BUFSTRINGSIZE 1024

	HWND hwndfound = NULL;

	char pszNewWindowTitle[BUFSTRINGSIZE];
	char pszOldWindowTitle[BUFSTRINGSIZE];

	// Fetch current window title

	GetConsoleTitle(pszOldWindowTitle, BUFSTRINGSIZE);

	// Format a unique window title

	wsprintf(pszNewWindowTitle, "%d%d", GetTickCount(), GetCurrentProcessId());

	// Change current window title

	SetConsoleTitle(pszNewWindowTitle);

	// Ensure Window title has been updated

	Sleep(40);

	// Look for console handle

	hwndfound = FindWindow(NULL, pszNewWindowTitle);

	// Restore Original title

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndfound);
}


CjvxAudioAsioDevice::CjvxAudioAsioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	resetAsio();
	allHandlerPtrs = allHandlers;
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);

	runtime.threads.coreHdl.hdl = NULL;
	runtime.threads.coreHdl.object = NULL;
	runtime.threads.theToolsHost = NULL;
	runtime.threads.syncAudioThreadAssoc = false;
	runtime.threads.syncAudioThreadId = 0;

	runtime.usefulBSizes.clear();
	jvxSize cnt = 0;
	while(1)
	{
		int oneBSize = jvxUsefulBSizes[cnt++];
		if(oneBSize > 0)
		{
			runtime.usefulBSizes.push_back(oneBSize);
		}
		else
		{
			break;
		}
	}
}


CjvxAudioAsioDevice::~CjvxAudioAsioDevice()
{
}

jvxErrorType
CjvxAudioAsioDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = CjvxAudioDevice::select(theOwner);
	if(res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if(res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::activate()
{
	jvxErrorType res = CjvxAudioDevice::activate();
	if(res == JVX_NO_ERROR)
	{
		genAsio_device::init__properties_active();
		genAsio_device::allocate__properties_active();
		genAsio_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genAsio_device::init__properties_active_higher();
		genAsio_device::allocate__properties_active_higher();
		genAsio_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));
		genAsio_device::properties_active_higher.loadpercent.isValid = false;

		res = activateAsioProperties();
		if(res == JVX_NO_ERROR)
		{
			CjvxAudioDevice::updateDependentVariables_lock(1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
			updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
		}
		else
		{
			jvxErrorType res2 = this->deactivate();
			assert(res2 == JVX_NO_ERROR);
		}
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::prepare()
{
	jvxErrorType res = CjvxAudioDevice::prepare();
	

	if(res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.ratesselection);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.sizesselection);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.openasiocontrolpanel);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.clocksourceselection);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.latencyin);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.latencyout);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.supportstimeinfo);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genAsio_device::properties_active.controlThreads);

		CjvxProperties::_unlock_properties_local();

	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	if(res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.ratesselection);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.sizesselection);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.openasiocontrolpanel);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.clocksourceselection);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.latencyin);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.latencyout);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.supportstimeinfo);

		CjvxProperties::_undo_update_property_access_type(
			genAsio_device::properties_active.controlThreads);

		CjvxProperties::_unlock_properties_local();
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::start()
{
	jvxErrorType res = CjvxAudioDevice::start();
	if(res == JVX_NO_ERROR)
	{
		//res = start_master(&inProcessing.theData_to_queue);
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::stop()
{
	jvxErrorType res = CjvxAudioDevice::stop();
	if(res == JVX_NO_ERROR)
	{
		//res = stop_master(&inProcessing.theData_to_queue);
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::deactivate();
	if(res == JVX_NO_ERROR)
	{
		this->deactivateAsioProperties();

		updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genAsio_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genAsio_device::deallocate__properties_active_higher();

		genAsio_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genAsio_device::deallocate__properties_active();
	}
	return(res);
}

// ====================================================================================
// ASIO SPECIFIC FUNCTIONALITIES
// ====================================================================================

jvxErrorType
CjvxAudioAsioDevice::activateAsioProperties()
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;
	ASIOError resAsio;
	ASIOChannelInfo infoChannels;

	jvxInt32 delta = JVX_MAX_INT_32_INT32;
	jvxInt16 idxDeltaMin = 0;

//	asioWave::activateStruct structIntern;
	HWND winHandle;
	winHandle = GetConsoleHwnd();
	//bool localopenControlPanel = false;

	long bufCnt;

	runtime.ptrToDriver = NULL;
	runtime.available = true;

	_lock_properties_local();

	genAsio_device::properties_active.supportstimeinfo.value.selection() = ((jvxBitField)1<<1); // Expect no timeInfo support

	auto hr = CoCreateInstance(this->my_props.clsid, 0, CLSCTX_INPROC_SERVER, this->my_props.clsid, (LPVOID*)&runtime.ptrToDriver);
	if(hr != S_OK)
	{

		_common_set.theErrordescr = "Failed in CoCreateInstance, <CjvxAudioAsioDevice::activateAsioProperties>";
		runtime.available = false;
	}

	// First: Init the driver struct
	if(runtime.ptrToDriver->init(winHandle) != ASIOTrue)
	{
		char errASIO[256] = {0};
		runtime.ptrToDriver->getErrorMessage(errASIO);
		_common_set.theErrordescr = "Failed in init, <CjvxAudioAsioDevice::activateAsioProperties>, message: " + (std::string)errASIO;
		res = JVX_ERROR_SYSTEMCALL_FAILED;
	}

	char nm[32]; //Size due to ASIO SDK
	runtime.ptrToDriver->getDriverName(nm);
	runtime.nameDriver = nm;
	runtime.versionDriver = runtime.ptrToDriver->getDriverVersion();


	// Be aware that this control panel is NOT blocking for most of the devices, that is why the
	// application must always be restarted before the changes take effect!
	if(this->my_props.selectionOpenControlPanel == true)
	{
		// Open the control panel
		runtime.ptrToDriver->controlPanel();
	}

	long num_chans_in = 0;
	long num_chans_out = 0;

	// Get the maximum number of channels available
	if(runtime.ptrToDriver->getChannels(&num_chans_in, &num_chans_out) != ASE_OK)
	{
		res = JVX_ERROR_SYSTEMCALL_FAILED;
		_common_set.theErrordescr = "Failed to query number channels, <CjvxAudioAsioDevice::activateAsioProperties>";
	}

	if(res == JVX_NO_ERROR)
	{

		// Set the sampletypes, assuming type is the same for all channels!!!
		runtime.typeSamples = ASIOSTInt16LSB;
		bool foundFirstSampleType = false;

		// Determine available input channels!
		infoChannels.isInput = ASIOTrue;

		CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;

		for(i = 0; i < (int)num_chans_in; i++)
		{
			infoChannels.channel = i;
			if((resAsio = runtime.ptrToDriver->getChannelInfo(&infoChannels)) == ASE_OK)
			{
				// CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(("In #" + jvx_int2String(i) + ":" + infoChannels.name).c_str());
				CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(infoChannels.name);
				jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i);

				if(foundFirstSampleType == false)
				{
					runtime.typeSamples = infoChannels.type;
					foundFirstSampleType = true;
				}
				else
				{
					if(runtime.typeSamples != infoChannels.type)
					{
						res = JVX_ERROR_UNEXPECTED;
						_common_set.theErrordescr = "Unexpected different types found for audio samples in input channels, <CjvxAudioAsioDevice::activateAsioProperties>";
					}
				}
			}
			else
			{
				res = JVX_ERROR_SYSTEMCALL_FAILED;
				_common_set.theErrordescr = "Unexpected error while querying input channels, <CjvxAudioAsioDevice::activateAsioProperties>";
			}
		}

		// Prepare for output channels
		infoChannels.isInput = ASIOFalse;

		CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
		CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

		for(i=0; i < (int)num_chans_out; i++)
		{
			infoChannels.channel = i;
			if(resAsio = runtime.ptrToDriver->getChannelInfo(&infoChannels) == ASE_OK)
			{
				// CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(("Out #" + jvx_int2String(i) + ":" + infoChannels.name).c_str());
				CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(infoChannels.name);
				jvx_bitSet(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i);

				if(foundFirstSampleType == false)
				{
					runtime.typeSamples = infoChannels.type;
					foundFirstSampleType = true;
				}
				else
				{
					if(runtime.typeSamples != infoChannels.type)
					{
						res = JVX_ERROR_UNEXPECTED;
						_common_set.theErrordescr = "Unexpected different types found for audio samples in output channels, <CjvxAudioAsioDevice::activateAsioProperties>";
					}
				}
			}
			else
			{
				res = JVX_ERROR_SYSTEMCALL_FAILED;
				_common_set.theErrordescr = "Unexpected error while querying output channels, <CjvxAudioAsioDevice::activateAsioProperties>";
			}
		}
	}

	if(res == JVX_NO_ERROR)
	{
		runtime.number_bits_sample = -1;
		switch(runtime.typeSamples)
		{
		case ASIOSTInt16MSB:
		case ASIOSTInt24MSB:
		case ASIOSTInt32MSB:
		case ASIOSTFloat32MSB:
		case ASIOSTFloat64MSB:
		case ASIOSTInt32MSB16:
		case ASIOSTInt32MSB18:
		case ASIOSTInt32MSB20:
		case ASIOSTInt32MSB24:
		case ASIOSTFloat64LSB:
			res = JVX_ERROR_UNSUPPORTED;
			_common_set.theErrordescr = "Asio device with unsupported sample type, <CjvxAudioAsioDevice::activateAsioProperties>";
			break;

		case ASIOSTInt16LSB:
			runtime.number_bits_sample = 16;
			runtime.bytes_space_sample = 2;
			break;
		case ASIOSTInt24LSB:
			runtime.number_bits_sample = 24;
			runtime.bytes_space_sample = 3;
			break;
		case ASIOSTInt32LSB:
			runtime.number_bits_sample = 32;
			runtime.bytes_space_sample = 4;
			break;
		case ASIOSTInt32LSB16:
			runtime.number_bits_sample = 16;
			runtime.bytes_space_sample = 4;
			break;
		case ASIOSTInt32LSB18:
			runtime.number_bits_sample = 18;
			runtime.bytes_space_sample = 4;
			break;
		case ASIOSTInt32LSB20:
			runtime.number_bits_sample = 20;
			runtime.bytes_space_sample = 4;
			break;
		case ASIOSTInt32LSB24:
			runtime.number_bits_sample = 24;
			runtime.bytes_space_sample = 4;
			break;
		case ASIOSTFloat32LSB:
			runtime.number_bits_sample = 32;
			runtime.bytes_space_sample = 4;
			break;
		}
	}

	if(res == JVX_NO_ERROR)
	{
		// Now obtain the buffersizes available
		if(runtime.ptrToDriver->getBufferSize(&runtime.minSizeBuffers, &runtime.maxSizeBuffers, &runtime.preferredSizeBuffers, &runtime.granBuffers) == ASE_OK)
		{
			genAsio_device::properties_active.sizesselection.value.selection() = 0;
			if(runtime.granBuffers > 0)
			{
				// List up all available buffersizes
				bufCnt = runtime.minSizeBuffers;
				while(bufCnt <= runtime.maxSizeBuffers)
				{
					jvxBool isUseful = false;
					for(i = 0; i < runtime.usefulBSizes.size(); i++)
					{
						if(runtime.usefulBSizes[i] == bufCnt)
						{
							isUseful = true;
							break;
						}
					}

					if(isUseful)
					{
						runtime.sizesBuffers.push_back(bufCnt);
						genAsio_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(bufCnt));
					}
					bufCnt += runtime.granBuffers;
				}
			}
			else
			{
				// Buffersizes are power of two inbetween min and max
				bufCnt = runtime.minSizeBuffers;
				while(bufCnt <= runtime.maxSizeBuffers)
				{
					runtime.sizesBuffers.push_back(bufCnt);
					genAsio_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(bufCnt));
					bufCnt *= 2;
				}
			}

			CjvxAudioDevice::properties_active.buffersize.value = runtime.preferredSizeBuffers;

			if(runtime.sizesBuffers.size() == 0)
			{
				res = JVX_ERROR_NOT_READY;
				_common_set.theErrordescr = "Failed to query a single valid buffersize, <CjvxAudioAsioDevice::activateAsioProperties>";
			}
			else
			{
				delta = JVX_MAX_INT_32_INT32;
				idxDeltaMin = 0;

				for(i = 0; i < (int)runtime.sizesBuffers.size(); i++)
				{
					if(abs(runtime.sizesBuffers[i] - runtime.preferredSizeBuffers) < delta)
					{
						delta = abs(runtime.sizesBuffers[i] - runtime.preferredSizeBuffers);
						idxDeltaMin = i;
					}
				}

				genAsio_device::properties_active.sizesselection.value.selection() = (jvxBitField)1 << idxDeltaMin;
			}
			this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxAudioDevice::properties_active.buffersize);
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			_common_set.theErrordescr = "Failed to query buffersize parameters from ASIO device, <CjvxAudioAsioDevice::activateAsioProperties>";
		}
	}

	if(res == JVX_NO_ERROR)
	{
		// Determine the samplerates that function: Test from a list of available rates
		i = 0;
		while(1)
		{
			if(jvxUsefulSRates[i] > 0)
			{
				if(resAsio = runtime.ptrToDriver->canSampleRate((ASIOSampleRate)jvxUsefulSRates[i]) == ASE_OK)
				{
					this->runtime.samplerates.push_back(jvxUsefulSRates[i]);
					genAsio_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(jvxUsefulSRates[i]));
				}
				i++;
			}
			else
			{
				break;
			}
		}

		if(runtime.samplerates.size() > 0)
		{
			CjvxAudioDevice::properties_active.samplerate.value = runtime.samplerates[0];
			genAsio_device::properties_active.ratesselection.value.selection() = (1 << 0);
			this->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,  CjvxAudioDevice::properties_active.samplerate);
		}
		else
		{
			res = JVX_ERROR_NOT_READY;
			_common_set.theErrordescr = "Failed to query a single valid samplerate, <CjvxAudioAsioDevice::activateAsioProperties>";
		}
	}

	// Set the default rate to the lowest rate available
	//if(sampleratesAvailable.size())
	//	setupRuntime.samplerateSelected = sampleratesAvailable[0];
	if(res == JVX_NO_ERROR)
	{
		ASIOClockSource sources[100];
		long num_sources = 100;

		if(runtime.ptrToDriver->getClockSources(sources, &num_sources) == ASE_OK)
		{
			assert(num_sources < 100);
			jvxSize sel = 0;
			for (i = 0; i < num_sources; i++)
			{
				runtime.theClockSources.push_back(sources[i]);
				genAsio_device::properties_active.clocksourceselection.value.entries.push_back(sources[i].name);
				if(sources[i].isCurrentSource)
				{
					sel = i;
				}
			}
			genAsio_device::properties_active.clocksourceselection.value.selection() = (jvxBitField)1 << (jvxUInt32)sel;
			genAsio_device::properties_active.clocksourceselection.value.exclusive = (jvxBitField)-1;
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			_common_set.theErrordescr = "Failed to query clock sources, <CjvxAudioAsioDevice::activateAsioProperties>";
		}
	}

	if(res == JVX_NO_ERROR)
	{
		long latin = 0;
		long latout = 0;

		if(runtime.ptrToDriver->getLatencies(&latin, &latout) == ASE_OK)
		{
			genAsio_device::properties_active.latencyin.value = latin;
			genAsio_device::properties_active.latencyout.value = latout;
		}
		else
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			_common_set.theErrordescr = "Failed to query device latencies, <CjvxAudioAsioDevice::activateAsioProperties>";
		}
	}

	if(res == JVX_NO_ERROR)
	{
		runtime.requiresOutputReady = false;
		if(runtime.ptrToDriver->outputReady() == ASE_OK)
		{
			runtime.requiresOutputReady = true;
		}
		else
		{
			runtime.requiresOutputReady = false;
		}
	}

	_unlock_properties_local();

	// setupRuntime.formatSelected = defaultSetupForDevices.defaultFormat;
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::deactivateAsioProperties()
{
	jvxErrorType res = JVX_NO_ERROR;
	_lock_properties_local();

	runtime.requiresOutputReady = false;

	if(runtime.ptrToDriver)
	{
		runtime.ptrToDriver->Release();
	}

	resetAsio();
	_unlock_properties_local();

	return(res);
}

void
CjvxAudioAsioDevice::resetAsio()
{
	runtime.ptrToDriver = NULL;

	genAsio_device::properties_active.clocksourceselection.value.entries.clear();
	genAsio_device::properties_active.clocksourceselection.value.set_all(0);
	genAsio_device::properties_active.clocksourceselection.value.exclusive = 0;
	runtime.theClockSources.clear();

	CjvxAudioDevice::properties_active.samplerate.value = -1;
	genAsio_device::properties_active.ratesselection.value.entries.clear();
	genAsio_device::properties_active.ratesselection.value.set_all(0);
	genAsio_device::properties_active.ratesselection.value.exclusive = 0;
	this->runtime.samplerates.clear();

	CjvxAudioDevice::properties_active.buffersize.value = -1;
	genAsio_device::properties_active.sizesselection.value.entries.clear();
	genAsio_device::properties_active.sizesselection.value.set_all(0);
	genAsio_device::properties_active.sizesselection.value.exclusive = 0;
	this->runtime.sizesBuffers.clear();

	runtime.minSizeBuffers = 0;
	runtime.maxSizeBuffers= 0;
	runtime.preferredSizeBuffers = 0;
	runtime.granBuffers = 0;

	runtime.number_bits_sample = 0;
	runtime.bytes_space_sample = 0;

	runtime.typeSamples = ASIOSTInt16MSB;
	runtime.requiresOutputReady = false;
	runtime.available = false;

	runtime.nameDriver = "unknown";
	runtime.versionDriver = 0;

	runtime.samplerates.clear();
	runtime.sizesBuffers.clear();

}

jvxErrorType
CjvxAudioAsioDevice::prepareAsio()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt16 cnt = 0;
	this->inProcessing.bSwitchId = -1;
	this->inProcessing.input.numberChannelsUsed = 0;
	this->inProcessing.output.numberChannelsUsed = 0;


	for(i = 0; i < NUM_BUFFERSWITCH_CB_HANDLERS; i++)
	{
		if(allHandlers[i].associatedWithClass == NULL)
		{
			this->inProcessing.bSwitchId = (jvxInt16)i;
			break;
		}
	}

	if(inProcessing.bSwitchId>= 0)
	{
		allHandlers[inProcessing.bSwitchId].associatedWithClass = this;
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
	}

	if (res == JVX_NO_ERROR)
	{
		_lock_properties_local();
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,  CjvxAudioDevice::properties_active.inputchannelselection);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,  CjvxAudioDevice::properties_active.outputchannelselection);
		_unlock_properties_local();


		for (i = 0; i < CjvxAudioDevice::properties_active.inputchannelselection.value.entries.size(); i++)
		{
			if (jvx_bitTest(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i))
			{
				this->inProcessing.input.numberChannelsUsed++;
			}
		}

		for (i = 0; i < CjvxAudioDevice::properties_active.outputchannelselection.value.entries.size(); i++)
		{
			if (jvx_bitTest(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i))
			{
				this->inProcessing.output.numberChannelsUsed++;
			}
		}

		// ==================================================================================================
		if (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed)
		{

			inProcessing.asioBufferField = new ASIOBufferInfo[(this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed)];

			for (i = 0; i < CjvxAudioDevice::properties_active.inputchannelselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i))
				{
					assert(cnt < (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed));
					inProcessing.asioBufferField[cnt].channelNum = (long)i;
					inProcessing.asioBufferField[cnt].isInput = true;
					cnt++;
				}
			}

			for (i = 0; i < CjvxAudioDevice::properties_active.outputchannelselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i))
				{
					assert(cnt < (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed));
					inProcessing.asioBufferField[cnt].channelNum = (long)i;
					inProcessing.asioBufferField[cnt].isInput = false;
					cnt++;
				}
			}

			assert(cnt == (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed));

			if (runtime.ptrToDriver->setSampleRate(CjvxAudioDevice::properties_active.samplerate.value) != ASE_OK)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}

			if (runtime.ptrToDriver->createBuffers(inProcessing.asioBufferField, (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed),
				CjvxAudioDevice::properties_active.buffersize.value, &allHandlers[inProcessing.bSwitchId].theCallbacks) != ASE_OK)
			{
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
			// Next: Allocate software buffers
		}
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::startAsio()
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.ptrToDriver);

	inProcessing.ssilence.active = CjvxAudioDevice_genpcg::properties_active.activateSilenceStop.value;
	inProcessing.ssilence.period = CjvxAudioDevice_genpcg::properties_active.periodSilenceStop.value;
	inProcessing.ssilence.stateEnd = JVX_STATE_COMPLETE;
	inProcessing.ssilence.countEnd = 0;
	if (inProcessing.ssilence.active)
	{
		inProcessing.ssilence.stateEnd = JVX_STATE_ACTIVE;
	}

	if (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed)
	{
		ASIOError resAsio = runtime.ptrToDriver->start();
		assert(resAsio == ASE_OK);
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::stopAsio()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	assert(runtime.ptrToDriver);

	if (inProcessing.ssilence.active)
	{
		if (inProcessing.ssilence.stateEnd == JVX_STATE_PREPARED)
		{
			inProcessing.ssilence.stateEnd = JVX_STATE_PROCESSING;
		}
		for (i = 0; i < ceil(inProcessing.ssilence.period*10*2); i++) // double the time of the silence period
		{
			JVX_SLEEP_MS(100);
			if (inProcessing.ssilence.stateEnd == JVX_STATE_COMPLETE)
			{
				break;
			}
		}
	}
	else
	{
		inProcessing.ssilence.stateEnd = JVX_STATE_COMPLETE;
	}
	if (inProcessing.ssilence.stateEnd != JVX_STATE_COMPLETE)
	{
		_report_text_message("Failed to insert stop silence within target time period.", JVX_REPORT_PRIORITY_WARNING);
	}

	if (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed)
	{
		ASIOError resAsio = runtime.ptrToDriver->stop();
		assert(resAsio == ASE_OK);
	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::postProcessAsio()
{
	// Deallocate the software buffers
	if (this->inProcessing.input.numberChannelsUsed + this->inProcessing.output.numberChannelsUsed)
	{
		if (runtime.ptrToDriver)
		{
			runtime.ptrToDriver->disposeBuffers();
		}
		delete[](inProcessing.asioBufferField);
	}
	allHandlers[inProcessing.bSwitchId].associatedWithClass = NULL;
	this->inProcessing.bSwitchId = -1;

	this->inProcessing.input.numberChannelsUsed = 0;
	this->inProcessing.output.numberChannelsUsed = 0;

	_lock_properties_local();
	_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT,  CjvxAudioDevice::properties_active.inputchannelselection);
	_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT,  CjvxAudioDevice::properties_active.outputchannelselection);
	_unlock_properties_local();
	return(JVX_NO_ERROR); // <- what would we do in case of an error?
}

// ====================================================================================

void
CjvxAudioAsioDevice::bufferSwitch(long doubleBufferIndex, ASIOBool directProcess)
{
	jvxData* ptrDoubleDest = NULL, *ptrDoubleSrc = NULL;
	signed short* ptr16BitSrc = NULL, *ptr16BitDest = NULL;
	char* ptr8BitSrc = NULL, *ptr8BitDest = NULL;
	int* ptr32BitSrc = NULL, *ptr32BitDest = NULL;
	jvxInt32 ii;
	jvxErrorType res = JVX_NO_ERROR;

	jvxTick tStamp_start = JVX_GET_TICKCOUNT_US_GET_CURRENT(&inProcessing.theTimestamp);
	//jvxSize locIndex = 0;

	if (inProcessing.ssilence.stateEnd == JVX_STATE_ACTIVE)
	{
		inProcessing.ssilence.stateEnd = JVX_STATE_PREPARED;
	}

	if(runtime.threads.coreHdl.hdl)
	{
		if(!runtime.threads.syncAudioThreadAssoc)
		{
			runtime.threads.coreHdl.hdl->associate_tc_thread(GetCurrentThreadId(), runtime.threads.syncAudioThreadId);
			runtime.threads.syncAudioThreadAssoc = true;
		}
	}

	// In this function, we get the desired buffer target index
	res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
	assert(res == JVX_NO_ERROR);

	// At this point, we might overwrite the output buffer index in the range of valid buffer indices - but we do not do so
	//_common_set_ocon.theData_out.con_link.address_flags = JVX_LINKDATA_ADDRESS_FLAGS_NONE;
	
	// =====================================================================================================
	// From input
	// =====================================================================================================
	switch(_common_set_ocon.theData_out.con_params.format)
	{
		case JVX_DATAFORMAT_DATA:
			switch(runtime.typeSamples)
			{
				case ASIOSTInt16LSB:
					for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
							(jvxInt16*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
							(jvxData*)_common_set_ocon.theData_out.con_data.buffers[
								*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
							_common_set_ocon.theData_out.con_params.buffersize,
							JVX_MAX_INT_16_DIV);
					}
					break;
				case ASIOSTInt32LSB:
					for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
							(jvxInt32*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
							(jvxData*)_common_set_ocon.theData_out.con_data.buffers[
								*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
							_common_set_ocon.theData_out.con_params.buffersize,
							JVX_MAX_INT_32_DIV);
					}
					break;

					// =================================================================

				case ASIOSTInt24LSB:
					for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_bytes_shift_norm_to_flp<jvxInt32, jvxData>(
							(char*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
							(jvxData*)_common_set_ocon.theData_out.con_data.buffers[
								*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
							_common_set_ocon.theData_out.con_params.buffersize,
							8, JVX_MAX_INT_32_DIV, 3);
					}
					break;
				default:
					assert(0);
			}
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			switch(runtime.typeSamples)
			{
			case ASIOSTInt16LSB:
				for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
				{
					jvx_convertSamples_memcpy(
						inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
						_common_set_ocon.theData_out.con_data.buffers[
							*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
						jvxDataFormat_size[CjvxAudioDevice::properties_active.format.value],
						_common_set_ocon.theData_out.con_params.buffersize);
				}
				break;
			case ASIOSTInt24LSB:

				for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
				{
					jvx_convertSamples_from_bytes_shiftright_to_fxp<jvxInt16, jvxInt32>(
						(char*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
						(jvxInt16*)_common_set_ocon.theData_out.con_data.buffers[
							*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
						3, _common_set_ocon.theData_out.con_params.buffersize, 8);
				}
				break;

		case ASIOSTInt32LSB:

			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
					(jvxInt32*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt16*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 16);
			}
			break;

		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		switch(runtime.typeSamples)
		{
		case ASIOSTInt16LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
					(jvxInt16*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt32*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 16);
			}
			break;
		case ASIOSTInt24LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_bytes_shiftleft_to_fxp<jvxInt32, jvxInt32>(
					(char*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt32*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 3, 8);
			}
			break;
		case ASIOSTInt32LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_memcpy(
					(jvxInt8*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt8*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					jvxDataFormat_size[CjvxAudioDevice::properties_active.format.value], _common_set_ocon.theData_out.con_params.buffersize);
			}
			break;
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		switch(runtime.typeSamples)
		{
		case ASIOSTInt16LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt64, jvxInt64>(
					(jvxInt16*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt64*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 48);
			}
			break;
		case ASIOSTInt24LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_bytes_shiftleft_to_fxp<jvxInt64, jvxInt64>(
					(char*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt64*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 3, 40);
			}
			break;
		case ASIOSTInt32LSB:
			for(ii = 0; ii < inProcessing.input.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt32, jvxInt64, jvxInt64>(
					(jvxInt32*)inProcessing.asioBufferField[ii].buffers[doubleBufferIndex],
					(jvxInt64*)_common_set_ocon.theData_out.con_data.buffers[
						*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr][ii],
					_common_set_ocon.theData_out.con_params.buffersize, 32);
			}
			break;
		}
	}

	// =====================================================================================================
	// CORE PROCESSING
	// =====================================================================================================
	_common_set_ocon.theData_out.admin.tStamp_us = tStamp_start;

	// No info about prpeviously lost buffers in ASIO 
	_common_set_ocon.theData_out.admin.frames_lost_since_last_time = 0;

	if (inProcessing.ssilence.active)
	{
		if (inProcessing.ssilence.stateEnd >= JVX_STATE_PROCESSING)
		{
			for (ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				for (ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
				{
					jvx_samples_clear(_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
							jvxDataFormat_size[CjvxAudioDevice::properties_active.format.value], _common_set_icon.theData_in->con_params.buffersize);
				}
			}

			if (inProcessing.ssilence.stateEnd == JVX_STATE_PROCESSING)
			{
				inProcessing.ssilence.countEnd += _common_set_icon.theData_in->con_params.buffersize;
				if (inProcessing.ssilence.countEnd >= JVX_DATA2SIZE(_common_set_icon.theData_in->con_params.rate * inProcessing.ssilence.period))
				{
					inProcessing.ssilence.stateEnd = JVX_STATE_COMPLETE;
				}
			}
		}
		else
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
			assert(res == JVX_NO_ERROR);
		}
	}
	else
	{
#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "===========================" << std::endl;
		std::cout << "ASIO input, bidx = " << _common_set_ocon.theData_out.pipeline.idx_stage << "/" 
			<< _common_set_ocon.theData_out.con_data.number_buffers << std::endl;
#endif

		res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();

#ifdef VERBOSE_BUFFERING_REPORT
		std::cout << "ASIO output, bidx = " << _common_set_icon.theData_in->pipeline.idx_stage << "/" 
			<< _common_set_icon.theData_in->con_data.number_buffers << std::endl;
		std::cout << "===========================" << std::endl;
#endif

		assert(res == JVX_NO_ERROR);
	}

	// =====================================================================================================
	// To output
	// =====================================================================================================
	switch(_common_set_ocon.theData_out.con_params.format)
	{
		case JVX_DATAFORMAT_DATA:
			switch(runtime.typeSamples)
			{
				case ASIOSTInt16LSB:
					for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
							(jvxData*)_common_set_icon.theData_in->con_data.buffers[
								*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
							(jvxInt16*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
							_common_set_icon.theData_in->con_params.buffersize,
							JVX_MAX_INT_16_DATA);
					}
					break;
				case ASIOSTInt32LSB:
					for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
							(jvxData*)_common_set_icon.theData_in->con_data.buffers[
								*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
							(jvxInt32*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
									_common_set_icon.theData_in->con_params.buffersize,
							JVX_MAX_INT_32_DATA);
					}
					break;

					// =================================================================

				case ASIOSTInt24LSB:
					for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
					{
						jvx_convertSamples_from_flp_norm_to_bytes<jvxData, jvxInt32>(
							(jvxData*)_common_set_icon.theData_in->con_data.buffers[
								*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
							(char*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
									_common_set_icon.theData_in->con_params.buffersize,
							JVX_MAX_INT_24_DATA, 3);
					}
					break;
				default:
					assert(0);
			}
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			switch(runtime.typeSamples)
			{
			case ASIOSTInt16LSB:
				for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
				{
					jvx_convertSamples_memcpy(
						_common_set_icon.theData_in->con_data.buffers[
							*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
						inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
						jvxDataFormat_size[CjvxAudioDevice::properties_active.format.value], _common_set_icon.theData_in->con_params.buffersize);
				}
				break;
			case ASIOSTInt24LSB:

				for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
				{
					jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
						(jvxInt16*)_common_set_icon.theData_in->con_data.buffers[
							*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
						(char*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
						3, _common_set_icon.theData_in->con_params.buffersize, 8);
				}
				break;

		case ASIOSTInt32LSB:

			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
					(jvxInt16*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(jvxInt32*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
							_common_set_icon.theData_in->con_params.buffersize, 16);
			}
			break;

		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		switch(runtime.typeSamples)
		{
		case ASIOSTInt16LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
					(jvxInt32*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(jvxInt16*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
							_common_set_icon.theData_in->con_params.buffersize, 16);
			}
			break;
		case ASIOSTInt24LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
					(jvxInt32*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(char*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
					3, _common_set_icon.theData_in->con_params.buffersize, 8);
			}
			break;
		case ASIOSTInt32LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_memcpy(
					(jvxInt32*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(jvxInt32*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
					jvxDataFormat_size[CjvxAudioDevice::properties_active.format.value], _common_set_icon.theData_in->con_params.buffersize);
			}
			break;

			}
			break;
	case JVX_DATAFORMAT_64BIT_LE:
		switch(runtime.typeSamples)
		{
		case ASIOSTInt16LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
					(jvxInt64*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(jvxInt16*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
							_common_set_icon.theData_in->con_params.buffersize, 48);
			}
			break;
		case ASIOSTInt24LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
					(jvxInt32*)_common_set_icon.theData_in->con_data.buffers[
						*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(char*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
					3, _common_set_icon.theData_in->con_params.buffersize, 40);
			}
			break;
		case ASIOSTInt32LSB:
			for(ii = 0; ii < inProcessing.output.numberChannelsUsed; ii++)
			{
				jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
					(jvxInt64*)_common_set_icon.theData_in->con_data.buffers
							[*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr][ii],
					(jvxInt32*)inProcessing.asioBufferField[ii + inProcessing.input.numberChannelsUsed].buffers[doubleBufferIndex],
					_common_set_icon.theData_in->con_params.buffersize, 32);
			}
			break;
		}
	}

	//static double ds[1000];
	//static int cnt = 0;
	// Circular forwarding of buffer

	// ==============================================================================================
	// Browse through chain to terminate this buffer processing
	// ==============================================================================================
	res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	assert(res == JVX_NO_ERROR);


	jvxTick tStamp_stop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&inProcessing.theTimestamp);
	jvxData deltaT = (jvxData)(tStamp_stop - tStamp_start);

	/*
	ds[cnt] = deltaT;
	cnt++;
	if(cnt >= 1000)
		cnt = 0;
	*/

	inProcessing.deltaT_average_us = (1-ALPHA_SMOOTH_DELTA_T) * deltaT + ALPHA_SMOOTH_DELTA_T *inProcessing.deltaT_average_us;
	jvxData load = inProcessing.deltaT_average_us / inProcessing.deltaT_theory_us * 100.0;
	load = JVX_MIN(load, 100.0);
	genAsio_device::properties_active_higher.loadpercent.value = load;

}

ASIOTime*
CjvxAudioAsioDevice::bufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	bufferSwitch(doubleBufferIndex, directProcess);
	return(params);
}

void
CjvxAudioAsioDevice::samplerateDidChange(ASIOSampleRate sRate)
{
	assert(0);// <- I have never seen a message
}

long
CjvxAudioAsioDevice::asioMessage(long selector, long value, void* message, double* opt)
{
	std::string mess = "";
	switch(selector)
	{
	case kAsioSelectorSupported:
		mess = "SELECTOR SUPPORT REQUEST";
		return(1L);
	case kAsioEngineVersion:
		mess = "ENGINE VERSION REQUEST";
		return(2L);
	case kAsioResetRequest:
		mess = "ASIO RESET REQUEST";
		return(1L);
	case kAsioSupportsTimeInfo:
		genAsio_device::properties_active.supportstimeinfo.value.selection() = ((jvxBitField)1 << 0);
		mess = "SUPPORT TIMEINFO REQUEST";
		return(1L);
	case kAsioSupportsTimeCode:
		mess = "SUPPORT TIMECODE REQUEST";
		return(1L);
	case kAsioBufferSizeChange:
		mess = "SUPPORT BUFFERSIZE CHANGED REQUEST";
		return(0L);
	case kAsioResyncRequest:
		mess = "SUPPORT RESYNC REQUEST";
		return(0L);
	case kAsioLatenciesChanged:
		mess = "SUPPORT LATENCIES CHANGED REQUEST";
		return(0L);
	}
	return(0L);
}

jvxErrorType
CjvxAudioAsioDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool report_update = false;

	jvxErrorType res = CjvxAudioDevice::set_property(callGate, rawPtr, ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if(
			(category == genAsio_device::properties_active.openasiocontrolpanel.category) &&
			(propId== genAsio_device::properties_active.openasiocontrolpanel.globalIdx))
		{
			if(runtime.ptrToDriver)
			{
				runtime.ptrToDriver->controlPanel();
			}
			genAsio_device::properties_active.openasiocontrolpanel.value.selection() = 0;
		}

		updateDependentVariables(propId, category, false, callGate.call_purpose);
	}
	return(res);
}

/*
void
CjvxAudioAsioDevice::updateDependentVariables()
{
	jvxSize i;

	CjvxAudioDevice::updateDependentVariables();

	assert(	runtime.samplerates.size() == genAsio_device::properties_active.ratesselection.value.entries.size());

	for(i = 0; i < genAsio_device::properties_active.ratesselection.value.entries.size(); i++)
	{
		if(genAsio_device::properties_active.ratesselection.value.selection() & ((jvxUInt64)1 << i))
		{
			CjvxAudioDevice::properties_active.samplerate.value = runtime.samplerates[i];
			break;
		}
	}

	assert(	runtime.sizesBuffers.size() == genAsio_device::properties_active.sizesselection.value.entries.size());

	for(i = 0; i < genAsio_device::properties_active.sizesselection.value.entries.size(); i++)
	{
		if(genAsio_device::properties_active.sizesselection.value.selection() & ((jvxUInt64)1 << i))
		{
			CjvxAudioDevice::properties_active.buffersize.value = runtime.sizesBuffers[i];
			break;
		}
	}


}*/

jvxErrorType
CjvxAudioAsioDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno );

	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		genAsio_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if(!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for(i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

	}
	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
	CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		// Write all parameters from this class but only if device is active
		genAsio_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(JVX_NO_ERROR);
}

void
CjvxAudioAsioDevice::updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxInt32 newValue = 0;

	// Update the variables in base class
	//CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll); <- this happens already in the base class

	if(category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if(
			(propId == genAsio_device::properties_active.ratesselection.globalIdx) ||
			(propId == CjvxAudioDevice::properties_active.samplerate.globalIdx) ||
			updateAll)
		{
			for(i = 0; i < (int)genAsio_device::properties_active.ratesselection.value.entries.size(); i++)
			{
				if(jvx_bitTest(genAsio_device::properties_active.ratesselection.value.selection(), i))
				{
					newValue = runtime.samplerates[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.samplerate.value = newValue;
			report_update = true;

		}


		// =================================================================================================
		if(
			(propId == genAsio_device::properties_active.sizesselection.globalIdx) ||
			(propId == CjvxAudioDevice::properties_active.buffersize.globalIdx) ||
			updateAll)
		{
			for(i = 0; i < (int)genAsio_device::properties_active.sizesselection.value.entries.size(); i++)
			{
				if(jvx_bitTest(genAsio_device::properties_active.sizesselection.value.selection(), i))
				{
					newValue = runtime.sizesBuffers[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.buffersize.value = newValue;
			report_update = true;

		}
	}

	if (report_update)
	{
		if (callPurp != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
		{
			if (_common_set_ld_master.refProc)
			{
				//_report_property_has_changed(category, propId, true, ( JVX_COMPONENT_UNKNOWN), callPurpose);
				jvxSize uId = JVX_SIZE_UNSELECTED;
				_common_set_ld_master.refProc->unique_id_connections(&uId);
				this->_request_test_chain_master(uId);
			}
		}
	}
}

jvxErrorType
CjvxAudioAsioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// If the chain is prepared, the object itself should also be prepared if not done so before
	resL = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	// Start to set all parameters for chaining at first
	_common_set_ocon.theData_out.con_params.buffersize = _inproc.buffersize;
	_common_set_ocon.theData_out.con_params.format = _inproc.format;
	_common_set_ocon.theData_out.con_data.buffers = NULL;
#if defined(DOUBLE_BUFFERING_PIPELINE_INPUT)
	_common_set_ocon.theData_out.con_data.number_buffers = DOUBLE_BUFFERING_PIPELINE_INPUT;
#else
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
#endif
	_common_set_ocon.theData_out.con_params.number_channels = _inproc.numInputs;
	_common_set_ocon.theData_out.con_params.rate = _inproc.samplerate;
	_common_set_ocon.theData_out.con_user.chain_spec_user_hints = NULL;
	_common_set_ocon.theData_out.con_data.alloc_flags = 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NONE;
	res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		goto leave_error;
	}

	// ============================================================================
	// ============================================================================

	runtime.threads.coreHdl.hdl = NULL;
	runtime.threads.coreHdl.object = NULL;
	runtime.threads.theToolsHost = NULL;
	runtime.threads.syncAudioThreadAssoc = false;
	runtime.threads.syncAudioThreadId = 0;
	runtime.buffer_idx.idxInput = 0;

	if (JVX_EVALUATE_BITFIELD(genAsio_device::properties_active.controlThreads.value.selection() & 0x2))
	{
		if (_common_set_min.theHostRef)
		{
			resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&runtime.threads.theToolsHost));
			if ((resL == JVX_NO_ERROR) && runtime.threads.theToolsHost)
			{
				resL = runtime.threads.theToolsHost->reference_tool(JVX_COMPONENT_THREADCONTROLLER, &runtime.threads.coreHdl.object, 0, NULL);
				if ((resL == JVX_NO_ERROR) && (runtime.threads.coreHdl.object))
				{
					resL = runtime.threads.coreHdl.object->request_specialization(reinterpret_cast<jvxHandle**>(&runtime.threads.coreHdl.hdl), NULL, NULL);
					if ((resL == JVX_NO_ERROR) && runtime.threads.coreHdl.hdl)
					{
						jvxState stat = JVX_STATE_NONE;
						runtime.threads.coreHdl.hdl->state(&stat);
						if (stat < JVX_STATE_ACTIVE)
						{
							runtime.threads.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threads.coreHdl.object);
							runtime.threads.coreHdl.hdl = NULL;
							runtime.threads.coreHdl.object = NULL;
						}
					}
				}
			}
		}
	}

	if (runtime.threads.coreHdl.hdl)
	{
		runtime.threads.syncAudioThreadAssoc = false;
		resL = runtime.threads.coreHdl.hdl->register_tc_thread(&runtime.threads.syncAudioThreadId);
		if (resL != JVX_NO_ERROR)
		{
			this->_report_text_message("Failed to control audio thread, no thread control used.", JVX_REPORT_PRIORITY_WARNING);
		}
	}

	// ============================================================================
	// Here, do the soundcard preparation
	// ============================================================================

	res = prepareAsio();

	if (res != JVX_NO_ERROR)
	{
		resL = postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

	JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
	inProcessing.timestamp_previous = -1;
	inProcessing.deltaT_theory_us = (jvxData)_common_set_ocon.theData_out.con_params.buffersize /
		(jvxData)_common_set_ocon.theData_out.con_params.rate * 1000.0 * 1000.0;
	inProcessing.deltaT_average_us = 0;

	return res;

leave_error:

	return res;
}

jvxErrorType
CjvxAudioAsioDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

	jvxBool checkme = _check_postprocess_chain_master();
	assert(checkme);

	// Stop device first
	postProcessAsio();
	if (runtime.threads.coreHdl.hdl)
	{
		if (runtime.threads.syncAudioThreadId >= 0)
		{
			resL = runtime.threads.coreHdl.hdl->unregister_tc_thread(runtime.threads.syncAudioThreadId);
			runtime.threads.syncAudioThreadAssoc = false;
			runtime.threads.syncAudioThreadId = -1;
		}

		resL = runtime.threads.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.threads.coreHdl.object);
		runtime.threads.coreHdl.object = NULL;
		runtime.threads.coreHdl.hdl = NULL;
	}
	if (runtime.threads.theToolsHost)
	{
		resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(runtime.threads.theToolsHost));
		runtime.threads.theToolsHost = NULL;
	}

	// Contact through
	res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(res == JVX_NO_ERROR);

	// If the chain is postprocessed, the object itself should also be postprocessed if not done so before
	resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return(res);
}

jvxErrorType
CjvxAudioAsioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL;

	// If the chain is started, the object itself should also be started if not done so before
	resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = CjvxConnectionMaster::_start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		resL = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(resL == JVX_NO_ERROR);
		goto leave_error;
	}

	runtime.buffer_idx.idxInput = 0;

	genAsio_device::properties_active_higher.loadpercent.value = 0;
	genAsio_device::properties_active_higher.loadpercent.isValid = true;

	res = startAsio();
	if (res != JVX_NO_ERROR)
	{
		jvxErrorType res2 = stopAsio();
		assert(res2 == JVX_NO_ERROR);
	}

	return res;

leave_error:
	return res;
}

jvxErrorType
CjvxAudioAsioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool resL = true;
	jvxErrorType resLL = JVX_NO_ERROR;

	resL = _check_stop_chain_master();
	assert(resL);

	// Stop device at first
	res = stopAsio();
	genAsio_device::properties_active_higher.loadpercent.value = 0;
	genAsio_device::properties_active_higher.loadpercent.isValid = false;

	res = CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		std::cout << "Stopping chain failed." << std::endl;
	}

	// If the chain is stopped, the object itself should also be stopped if not done so before
	resLL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resLL == JVX_NO_ERROR);

	return res;
}

// =========================================================================

jvxErrorType
CjvxAudioAsioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
#if defined(DOUBLE_BUFFERING_PIPELINE_OUTPUT)
	_common_set_icon.theData_in->con_data.number_buffers = 
		JVX_MAX(_common_set_icon.theData_in->con_data.number_buffers, DOUBLE_BUFFERING_PIPELINE_OUTPUT);
#else
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
		_common_set_icon.theData_in->con_data.number_buffers, 1);
#endif
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	jvx_bitSet(_common_set_icon.theData_in->con_data.alloc_flags, 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();


	return res;
}

jvxErrorType
CjvxAudioAsioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioAsioDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	return res;
}

jvxErrorType
CjvxAudioAsioDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = shift_buffer_pipeline_idx_on_start( pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	return res;
}


jvxErrorType
CjvxAudioAsioDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( 
		idx_stage,  
		shift_fwd,
		tobeAccessedByStage,
		clbk,
		priv_ptr);

	return res;
}

jvxErrorType
CjvxAudioAsioDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// Call a specific version of this function to find a parameter setting
	if (tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS)
	{
		res = CjvxAudioDevice::transfer_backward_ocon_match_setting(tp, data, &genAsio_device::properties_active.ratesselection, &genAsio_device::properties_active.sizesselection JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

