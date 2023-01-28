#include "CjvxAudioCoreAudioDevice.h"
#include "jvx_config.h"

#define VERBOSE_MODE

// =======================================================================
// Some helper functions
// =======================================================================

static
std::string matchDataFormat(UInt32 format)
{
    switch(format)
    {
        case kAudioFormatLinearPCM:
            return("PCM linear");
        case kAudioFormatAC3 :
            return("AC-3");
        case kAudioFormat60958AC3:
            return("CAC3");
        case kAudioFormatAppleIMA4:
            return("IMA4");
        case kAudioFormatMPEG4AAC:
            return("AAC");
        case kAudioFormatMPEG4CELP:
            return("AAC CELP");
        case kAudioFormatMPEG4HVXC:
            return("HVXC");
        case kAudioFormatMPEG4TwinVQ:
            return("TWIN VQ");
        case kAudioFormatMACE3 :
            return("MAC3");
        case kAudioFormatMACE6 :
            return("MAC6");
        case kAudioFormatULaw:
            return("ULAW");
        case kAudioFormatALaw:
            return("ALAW");
        case kAudioFormatQDesign:
            return("QDMC");
        case kAudioFormatQDesign2:
            return("QDMC2");
        case kAudioFormatQUALCOMM:
            return("QCLP");
        case kAudioFormatMPEGLayer1:
            return("MP1");
        case kAudioFormatMPEGLayer2:
            return("MP2");
        case kAudioFormatMPEGLayer3:
            return("MP3");
        case kAudioFormatTimeCode:
            return("TIME");
        case kAudioFormatMIDIStream:
            return("MIDI");
        case kAudioFormatParameterValueStream:
            return("APVS");
        case kAudioFormatAppleLossless:
            return("ALAC");
        case kAudioFormatMPEG4AAC_HE:
            return("AACH");
        case kAudioFormatMPEG4AAC_LD:
            return("AACL");
        case kAudioFormatMPEG4AAC_HE_V2:
            return("AACP");
        case kAudioFormatMPEG4AAC_Spatial:
            return("AACS");
        case kAudioFormatAMR:
            return("AMR");
        case kAudioFormatAudible:
            return("AUDB");
        case kAudioFormatiLBC:
            return("ILBC");
        case kAudioFormatDVIIntelIMA:
            return("DVIINTEL");
        case kAudioFormatMicrosoftGSM:
            return("MSGSM");
        case kAudioFormatAES3:
            return("PAES3");
    }
    return("Unknown");
}

static
std::string matchDataFormatFlag(UInt32 format)
{
    std::string token = "";
    
    // UInt32 tst = kAudioFormatFlagIsBigEndian;
    
    /*
     kAudioFormatFlagIsFloat                  = (1 << 0),    // 0x1
     kAudioFormatFlagIsBigEndian              = (1 << 1),    // 0x2
     kAudioFormatFlagIsSignedInteger          = (1 << 2),    // 0x4
     kAudioFormatFlagIsPacked                 = (1 << 3),    // 0x8
     kAudioFormatFlagIsAlignedHigh            = (1 << 4),    // 0x10
     kAudioFormatFlagIsNonInterleaved         = (1 << 5),    // 0x20
     kAudioFormatFlagIsNonMixable             = (1 << 6),    // 0x40
     kAudioFormatFlagsAreAllClear             = (1 << 31),
     
     kLinearPCMFormatFlagIsFloat              = kAudioFormatFlagIsFloat,
     kLinearPCMFormatFlagIsBigEndian          = kAudioFormatFlagIsBigEndian,
     kLinearPCMFormatFlagIsSignedInteger      = kAudioFormatFlagIsSignedInteger,
     kLinearPCMFormatFlagIsPacked             = kAudioFormatFlagIsPacked,
     kLinearPCMFormatFlagIsAlignedHigh        = kAudioFormatFlagIsAlignedHigh,
     kLinearPCMFormatFlagIsNonInterleaved     = kAudioFormatFlagIsNonInterleaved,
     kLinearPCMFormatFlagIsNonMixable         = kAudioFormatFlagIsNonMixable,
     kLinearPCMFormatFlagsSampleFractionShift = 7,
     kLinearPCMFormatFlagsSampleFractionMask  =
     (0x3F << kLinearPCMFormatFlagsSampleFractionShift ),
     kLinearPCMFormatFlagsAreAllClear         = kAudioFormatFlagsAreAllClear,
     
     kAppleLosslessFormatFlag_16BitSourceData = 1,
     kAppleLosslessFormatFlag_20BitSourceData = 2,
     kAppleLosslessFormatFlag_24BitSourceData = 3,
     kAppleLosslessFormatFlag_32BitSourceData = 4*/
    
    if(format & kAudioFormatFlagIsFloat)
        token += " Float";
    
    if(format & kAudioFormatFlagIsBigEndian)
        token += " Big Endian";
    
    if(format & kAudioFormatFlagIsSignedInteger)
        token = " Signed Integer";
    
    if(format & kAudioFormatFlagIsPacked)
        token += " Packed";
    
    if(format & kAudioFormatFlagIsAlignedHigh)
        token += " Aligned High";
    
    if(format & kAudioFormatFlagIsNonInterleaved)
        token += " Non Interleaved";
    
    if(format & kAudioFormatFlagsAreAllClear)
        token += " All Clear";
    
    if(format & kAudioFormatFlagsNativeFloatPacked)
        token += " Float Packed";
    
    if(format & kAudioFormatFlagsNativeEndian)
        token += " Native Endian";
    
    return(token);
}

/*
 *
 *///==============================================================================
std::string
CjvxAudioCoreAudioDevice::CFString2StdString(CFStringRef& str)
{
	int i;
	UniChar sChar;
	char cChar[2] = {0,0};
	std::string nm;
	UInt32 ll = CFStringGetLength(str);
	nm = "";
	for(i = 0; i < ll; i++)
	{
		sChar = CFStringGetCharacterAtIndex(str, i);
		cChar[0] = (char)sChar;
		nm += cChar;
	}
	return(nm);
}

// =======================================================================
// Audio processing callback
// =======================================================================

/**
 * C function entry for bufferswitch callback
 *///===============================================
static
OSStatus AudioDeviceIOProcDev(AudioDeviceID inDevice, const AudioTimeStamp* inNow,
                              const AudioBufferList* inInputData, const AudioTimeStamp* inInputTime,
                              AudioBufferList * outOutputData, const AudioTimeStamp* inOutputTime,
                              void* inClientData)
{
    if(inClientData)
    {
        CjvxAudioCoreAudioDevice* ptrDevice = (CjvxAudioCoreAudioDevice*)inClientData;
        
        ptrDevice->process(inNow, inInputData, inInputTime, outOutputData, inOutputTime);
    }
    
    return(0);
}

// =======================================================================
// Core audio audio object property functions
// =======================================================================

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyDataSize(AudioObjectID mAudioDeviceID, UInt32 inChannel,
												Boolean inSection, UInt32 inPropertyID,
												UInt32* ioDataSize)
{
	AudioObjectPropertyAddress singlePropGet;

	// requested property
	singlePropGet.mSelector = inPropertyID;

	// kAudioDevicePropertyScopeInput = 'inpt',
	// kAudioDevicePropertyScopeOutput
	if(inSection)
		singlePropGet.mScope = kAudioDevicePropertyScopeInput;
	else
		singlePropGet.mScope = kAudioDevicePropertyScopeOutput;


	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectGetPropertyDataSize(mAudioDeviceID, &singlePropGet, 0, NULL, ioDataSize);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyData(AudioObjectID mAudioDeviceID, UInt32 inChannel,
											Boolean inSection, UInt32 inPropertyID,
											UInt32* ioDataSize, void* outData)
{
	AudioObjectPropertyAddress singlePropGet;

	// requested property
	singlePropGet.mSelector = inPropertyID;

	// kAudioDevicePropertyScopeInput = 'inpt',
    // kAudioDevicePropertyScopeOutput
	if(inSection)
		singlePropGet.mScope = kAudioDevicePropertyScopeInput;
	else
		singlePropGet.mScope = kAudioDevicePropertyScopeOutput;

	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectGetPropertyData(mAudioDeviceID, &singlePropGet, 0, NULL, ioDataSize, outData);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyData_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
                                                       Boolean inSection, UInt32 inPropertyID,
                                                       UInt32* ioDataSize, void* outData)
{
    AudioObjectPropertyAddress singlePropGet;
    
    // requested property
    singlePropGet.mSelector = inPropertyID;
    
    // kAudioDevicePropertyScopeInput = 'inpt',
    // kAudioDevicePropertyScopeOutput
    if(inSection)
        singlePropGet.mScope = kAudioDevicePropertyScopeInput;
    else
        singlePropGet.mScope = kAudioDevicePropertyScopeOutput;
    
    // 0 for master
    singlePropGet.mElement = kAudioObjectPropertyElementMaster;
    
    
    OSStatus theError = AudioObjectGetPropertyData(mAudioDeviceID, &singlePropGet, 0, NULL, ioDataSize, outData);
    if(theError == noErr)
        return(true);
    return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::SetPropertyData_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
                                                       Boolean inSection, UInt32 inPropertyID,
                                                       UInt32 ioDataSize, void* outData)
{
    AudioObjectPropertyAddress singlePropGet;
    
	// requested property
	singlePropGet.mSelector = inPropertyID;

	// kAudioDevicePropertyScopeInput = 'inpt',
    // kAudioDevicePropertyScopeOutput
	if(inSection)
		singlePropGet.mScope = kAudioDevicePropertyScopeInput;
	else
		singlePropGet.mScope = kAudioDevicePropertyScopeOutput;

	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectSetPropertyData(mAudioDeviceID, &singlePropGet, 0, NULL, ioDataSize, outData);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyData_AudioStream(AudioDeviceID mAudioStreamID, UInt32 inPropertyID, UInt32* ioDataSize, void* outData)
{
	AudioObjectPropertyAddress singlePropGet;

	// requested property
	singlePropGet.mSelector = inPropertyID;

	singlePropGet.mScope = kAudioObjectPropertyScopeGlobal;

	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectGetPropertyData(mAudioStreamID, &singlePropGet, 0, NULL, ioDataSize, outData);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyDataSize_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
														 Boolean inSection, UInt32 inPropertyID,
														 UInt32* ioDataSize)
{
	AudioObjectPropertyAddress singlePropGet;

	// requested property
	singlePropGet.mSelector = inPropertyID;

	// kAudioDevicePropertyScopeInput = 'inpt',
	// kAudioDevicePropertyScopeOutput
	if(inSection)
		singlePropGet.mScope = kAudioDevicePropertyScopeInput;
	else
		singlePropGet.mScope = kAudioDevicePropertyScopeOutput;


	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectGetPropertyDataSize(mAudioDeviceID, &singlePropGet, 0, NULL, ioDataSize);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
Boolean
CjvxAudioCoreAudioDevice::GetPropertyDataSize_AudioStream(AudioStreamID mAudioStreamID, UInt32 inPropertyID, UInt32* ioDataSize)
{
	AudioObjectPropertyAddress singlePropGet;

	// requested property
	singlePropGet.mSelector = inPropertyID;

	singlePropGet.mScope = kAudioObjectPropertyScopeGlobal;


	// 0 for master
	singlePropGet.mElement = kAudioObjectPropertyElementMaster;


	OSStatus theError = AudioObjectGetPropertyDataSize(mAudioStreamID, &singlePropGet, 0, NULL, ioDataSize);
	if(theError == noErr)
		return(true);
	return(false);
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::clearProperties()
{
	int i;
	configurationStruct.clockDomain = 0;
	configurationStruct.deviceIsAlive = 0;
	configurationStruct.deviceIsRunningElsewhere = 0;
	configurationStruct.isOverload = 0;
	configurationStruct.hogMode = 0;
    configurationStruct.hogModeSupported = false;
	configurationStruct.deviceLatencyInput = 0;
	configurationStruct.deviceLatencyOutput = 0;
	configurationStruct.framesize = 0;

    memset(&configurationStruct.framesizesAvailable, 0, sizeof(AudioValueRange));

	configurationStruct.useVarFramesize = 0;
    configurationStruct.useVarFramesizeSupported = false;

	if(configurationStruct.streamsInput)
		delete[](configurationStruct.streamsInput);
	configurationStruct.streamsInput = NULL;

    // Input
    
	for(i = 0; i < configurationStruct.numStreamsInput; i++)
	{
		configurationStruct.classStreamsInput[i].direction = 0;
		configurationStruct.classStreamsInput[i].terminalType = 0;
		configurationStruct.classStreamsInput[i].startingChannel = 0;
		configurationStruct.classStreamsInput[i].latency = 0;

        memset(&configurationStruct.classStreamsInput[i].descriptionVFormat, 0, sizeof(AudioStreamBasicDescription));

		if(configurationStruct.classStreamsInput[i].descriptionAvailableVFormats)
			delete[](configurationStruct.classStreamsInput[i].descriptionAvailableVFormats);
		configurationStruct.classStreamsInput[i].descriptionAvailableVFormats = NULL;

		if(configurationStruct.classStreamsInput[i].descriptionAvailablePFormats)
			delete(configurationStruct.classStreamsInput[i].descriptionAvailablePFormats);
		configurationStruct.classStreamsInput[i].descriptionAvailablePFormats = NULL;

        memset(&configurationStruct.classStreamsInput[i].descriptionPFormat, 0, sizeof(AudioStreamBasicDescription));

        
		configurationStruct.classStreamsInput[i].numVFormats = 0;
		configurationStruct.classStreamsInput[i].numPFormats = 0;
	}
	if(configurationStruct.classStreamsInput)
		delete[](configurationStruct.classStreamsInput);
	configurationStruct.classStreamsInput = NULL;
    configurationStruct.numStreamsInput = 0;
    
    // Output

    if(configurationStruct.streamsOutput)
        delete[](configurationStruct.streamsOutput);
    configurationStruct.streamsOutput = NULL;

	for(i = 0; i < configurationStruct.numStreamsOutput; i++)
	{
		configurationStruct.classStreamsOutput[i].direction = 0;
		configurationStruct.classStreamsOutput[i].terminalType = 0;
		configurationStruct.classStreamsOutput[i].startingChannel = 0;
		configurationStruct.classStreamsOutput[i].latency = 0;

        memset(&configurationStruct.classStreamsOutput[i].descriptionVFormat, 0, sizeof(AudioStreamBasicDescription));

		if(configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats)
			delete[](configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats);
		configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats = NULL;

		if(configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats)
			delete(configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats);
		configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats = NULL;

        memset(&configurationStruct.classStreamsOutput[i].descriptionPFormat, 0, sizeof(AudioStreamBasicDescription));

		configurationStruct.classStreamsOutput[i].numVFormats = 0;
		configurationStruct.classStreamsOutput[i].numPFormats = 0;
	}
	if(configurationStruct.classStreamsOutput)
		delete[](configurationStruct.classStreamsOutput);
	configurationStruct.classStreamsOutput = NULL;

	configurationStruct.numStreamsOutput = 0;
	configurationStruct.safetyOffset = 0;
	configurationStruct.cycleUsage = 0.0;

	if(configurationStruct.streamsInputStr)
		delete[](configurationStruct.streamsInputStr);
	configurationStruct.streamsInputStr	= NULL;

	if(configurationStruct.streamsOutputStr)
		delete[](configurationStruct.streamsOutputStr);
	configurationStruct.streamsOutputStr = NULL;

	if(configurationStruct.streamUsageInput)
		delete[](configurationStruct.streamUsageInput);
	configurationStruct.streamUsageInput = NULL;

	if(configurationStruct.streamUsageOutput)
		delete[](configurationStruct.streamUsageOutput);
	configurationStruct.streamUsageOutput = NULL;

    if(configurationStruct.samplerates)
        delete[](configurationStruct.samplerates);
    configurationStruct.samplerates = NULL;

	configurationStruct.numSamplerates = 0;
	configurationStruct.nominalSamplerate = 0;
	configurationStruct.actualSamplerate = 0;
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::clearPropertiesInit()
{
	configurationStruct.clockDomain = 0;
	configurationStruct.deviceIsAlive = 0;
	configurationStruct.deviceIsRunningElsewhere = 0;
	configurationStruct.isOverload = 0;
	configurationStruct.hogMode = 0;
    configurationStruct.hogModeSupported = false;
	configurationStruct.deviceLatencyInput = 0;
	configurationStruct.deviceLatencyOutput = 0;
	configurationStruct.framesize = 0;
    memset(&configurationStruct.framesizesAvailable, 0, sizeof(AudioValueRange));
	configurationStruct.useVarFramesize = 0;
    configurationStruct.useVarFramesizeSupported = false;
    configurationStruct.streamsInput = NULL;
	configurationStruct.streamsOutput = NULL;
	configurationStruct.classStreamsInput = NULL;
	configurationStruct.classStreamsOutput = NULL;
	configurationStruct.numStreamsInput = 0;
	configurationStruct.numStreamsOutput = 0;
	configurationStruct.safetyOffset = 0;
	configurationStruct.cycleUsage = 0.0;
	configurationStruct.streamsInputStr	= NULL;
	configurationStruct.streamsOutputStr = NULL;
	configurationStruct.streamUsageInput = NULL;
	configurationStruct.streamUsageOutput = NULL;
    configurationStruct.samplerates = NULL;
	configurationStruct.numSamplerates = 0;
	configurationStruct.nominalSamplerate = 0;
	configurationStruct.actualSamplerate = 0;
}

// =======================================================================
// Some get functions for audio object properties
// =======================================================================

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getClockDomain()
{
	UInt32 sz = 0;
    Boolean locRes = true;
    
	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, 	kAudioDevicePropertyClockDomain, &sz);

    // Check the previous function call
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyClockDomain, &sz, &configurationStruct.clockDomain);
    // Check the previous function call
    assert(locRes == true);
    

#ifdef VERBOSE_MODE
    std::cout << "Initialize Device: Clock Domain id = " << configurationStruct.clockDomain << std::endl;
	//	std::cout << "kAudioDevicePropertyClockDomain failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceAlive()
{
    UInt32 sz = 0;
    Boolean locRes = true;
    
    // =======================================
    // Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyDeviceIsAlive, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));
    
    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyDeviceIsAlive, &sz, &configurationStruct.deviceIsAlive);
    assert(locRes == true);
    
#ifdef VERBOSE_MODE
    std::cout << "Initialize Device: Device is alive = " << configurationStruct.deviceIsAlive << std::endl;
 //       std::cout << "kAudioDevicePropertyDeviceIsAlive failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceRunningElsewhere()
{
	UInt32 sz = 0;
   Boolean locRes = true;
    
	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyDeviceIsRunningSomewhere, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyDeviceIsRunningSomewhere, &sz, &configurationStruct.deviceIsRunningElsewhere);
    assert(locRes == true);

#ifdef VERBOSE_MODE
    std::cout << "Initialize Device: Device is running elsewhere = " << configurationStruct.deviceIsRunningElsewhere << std::endl;
	//	std::cout << "kAudioDevicePropertyDeviceIsRunningSomewhere failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceOverload()
{
	UInt32 sz = 0;
    Boolean locRes = true;
    
	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDeviceProcessorOverload, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDeviceProcessorOverload, &sz, &configurationStruct.isOverload);
    assert(locRes == true);
    
#ifdef VERBOSE_MODE
    std::cout << "Initialize Device: Processor overload = " << configurationStruct.isOverload << std::endl;
	//	std::cout << "kAudioDevicePropertyProcessorOverload failed." << std::endl;
#endif
}
        
/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceHogMode()
{
    UInt32 sz = 0;
    Boolean locRes = true;
    
    // We may get a negative result here - whatever that means
    configurationStruct.hogModeSupported = false;
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyHogMode, &sz);
    if(locRes == true)
    {
        assert(sz == sizeof(UInt32));
        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyHogMode, &sz, &configurationStruct.hogMode);
        assert(locRes == true);
        configurationStruct.hogModeSupported = true;
    }
#ifdef VERBOSE_MODE
    if(configurationStruct.hogModeSupported)
    {
        std::cout << "Initialize Device: Hog mode id = " << configurationStruct.deviceIsRunningElsewhere << std::endl;
    }
    else
    {
        std::cout << "kAudioDevicePropertyHogMode not supported." << std::endl;
    }
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceLatencies()
{
    UInt32 sz = 0;
    Boolean locRes = true;
    
    // =======================================
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyLatency, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyLatency, &sz, &configurationStruct.deviceLatencyInput);
    assert(locRes == true);

#ifdef VERBOSE_MODE
    std::cout << "Initialize Device: Latency input = " << configurationStruct.deviceLatencyInput << std::endl;
	//	std::cout << "kAudioDevicePropertyLatency (input) failed." << std::endl;
#endif

	sz = 0;
	// =======================================
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyLatency, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyLatency, &sz, &configurationStruct.deviceLatencyOutput);
    assert(locRes == true);

#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Latency output = " << configurationStruct.deviceLatencyOutput << std::endl;
 //       std::cout << "kAudioDevicePropertyLatency (output) failed." << std::endl;
#endif
}

        /*
         *
         *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceFramesize()
{
	UInt32 sz = 0;
    Boolean locRes = true;

	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSize, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSize, &sz, &configurationStruct.framesize);
    assert(locRes == true);

	// Set the framesize used in the base class
	/* TODO
	this->_buffersizeProcessing = configurationStruct.framesize;
	*/

#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Frame size = " << configurationStruct.framesize << std::endl;
	//	std::cout << "kAudioPropertyDeviceBufferFrameSize failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
jvxErrorType
CjvxAudioCoreAudioDevice::setDeviceFramesize(UInt32 fSize)
{
    UInt32 sz = 0;
    Boolean locRes = true;
    
    if((fSize >= this->configurationStruct.framesizesAvailable.mMinimum) && (fSize <= this->configurationStruct.framesizesAvailable.mMaximum))
    {
        locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSize, &sz);
        assert(locRes == true);
        assert(sz == sizeof(UInt32));
        
        locRes = this->SetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSize, sz, &fSize);
        assert(locRes == true);
        
#ifdef VERBOSE_MODE
            std::cout << "Modify Property Device: Frame size = " << fSize << std::endl;
        //    std::cout << "kAudioPropertyDeviceBufferFrameSize failed." << std::endl;
#endif
        return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_SETTING);
}

        /*
         *
         *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceFramesizesAvailable()
{
	UInt32 sz = 0;
    Boolean locRes = true;


	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSizeRange, &sz);
    assert(locRes == true);
    assert(sz == sizeof(AudioValueRange));

    configurationStruct.framesizesAvailable.mMinimum = 0;
    configurationStruct.framesizesAvailable.mMaximum = 0;

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyBufferFrameSizeRange, &sz, &configurationStruct.framesizesAvailable);
    assert(locRes == true);
    
#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Frame sizes available, min = " << configurationStruct.framesizesAvailable.mMinimum
    << ", max  = " << configurationStruct.framesizesAvailable.mMaximum << std::endl;
//std::cout << "kAudioDevicePropertyBufferFrameSizeRange failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceUseVarFramesize()
{
	UInt32 sz = 0;
    Boolean locRes = true;

    configurationStruct.useVarFramesizeSupported = false;
    
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyUsesVariableBufferFrameSizes, &sz);
    if(locRes == true)
    {
        configurationStruct.useVarFramesizeSupported = true;
        assert(sz == sizeof(UInt32));

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyUsesVariableBufferFrameSizes, &sz, &configurationStruct.useVarFramesize);
        assert(locRes == true);
    }
#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Use variable framesize = " << configurationStruct.useVarFramesize << std::endl;
	//std::cout << "kAudioDevicePropertyUsesVariableBufferFrameSizes failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceStreams()
{
	int i;
	UInt32 sz = 0;
    Boolean locRes = true;

	configurationStruct.numStreamsInput = 0;
	configurationStruct.streamsInput = NULL;
	configurationStruct.classStreamsInput = NULL;

	configurationStruct.numStreamsOutput = 0;
	configurationStruct.streamsOutput = NULL;
	configurationStruct.classStreamsOutput = NULL;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // First: Input streams
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyStreams, &sz);
    assert(locRes == true);
    
    configurationStruct.numStreamsInput = 0;
    configurationStruct.streamsInput = NULL;
    configurationStruct.classStreamsInput = NULL;

	if(sz != 0)
	{
		configurationStruct.numStreamsInput = sz/sizeof(AudioStreamID);
		if(configurationStruct.numStreamsInput)
		{
			configurationStruct.streamsInput = new AudioStreamID[configurationStruct.numStreamsInput];
			configurationStruct.classStreamsInput = new streamFormats[configurationStruct.numStreamsInput];

            locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyStreams, &sz, configurationStruct.streamsInput);
            assert(locRes == true);

#ifdef VERBOSE_MODE
			std::cout << "Initialize Device: Number input streams = " << configurationStruct.numStreamsInput << std::endl;
            //std::cout << "kAudioDevicePropertyStreams failed." << std::endl;
#endif

				/*
				kAudioStreamPropertyDirection = 'sdir',
				kAudioStreamPropertyTerminalType = 'term',
				kAudioStreamPropertyStartingChannel = 'schn',
				kAudioStreamPropertyLatency = kAudioDevicePropertyLatency,
				kAudioStreamPropertyVirtualFormat = 'sfmt',
				kAudioStreamPropertyAvailableVirtualFormats = 'sfma',
				kAudioStreamPropertyPhysicalFormat = 'pft ',
				kAudioStreamPropertyAvailablePhysicalFormats = 'pfta'
				*/
        }
	}

    // Obtain properties input streams
	for(i = 0; i < configurationStruct.numStreamsInput; i++)
	{
		// Direction
		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyDirection, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyDirection, &sz, &configurationStruct.classStreamsInput[i].direction);
        assert(locRes == true);

        // Terminal type
		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyTerminalType, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyTerminalType, &sz, &configurationStruct.classStreamsInput[i].terminalType);
        assert(locRes == true);

        // Starting channel
		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyStartingChannel, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyStartingChannel, &sz, &configurationStruct.classStreamsInput[i].startingChannel);
        assert(locRes == true);

        // Latency
		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyLatency, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i],kAudioStreamPropertyLatency, &sz, &configurationStruct.classStreamsInput[i].latency);
        assert(locRes == true);

        //
		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyVirtualFormat, &sz);
        assert(locRes == true);
        assert(sz == sizeof(AudioStreamBasicDescription));

        memset(&configurationStruct.classStreamsInput[i].descriptionVFormat, 0, sizeof(AudioStreamBasicDescription));

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i],kAudioStreamPropertyVirtualFormat, &sz, &configurationStruct.classStreamsInput[i].descriptionVFormat);
        assert(locRes == true);

        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyAvailableVirtualFormats, &sz);
        assert(locRes == true);

        configurationStruct.classStreamsInput[i].descriptionAvailableVFormats = NULL;
        configurationStruct.classStreamsInput[i].numVFormats = sz/sizeof(AudioStreamRangedDescription);

        if(configurationStruct.classStreamsInput[i].numVFormats)
        {
            configurationStruct.classStreamsInput[i].descriptionAvailableVFormats = new AudioStreamRangedDescription[configurationStruct.classStreamsInput[i].numVFormats];
            
            locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i],kAudioStreamPropertyAvailableVirtualFormats, &sz, configurationStruct.classStreamsInput[i].descriptionAvailableVFormats);
            assert(locRes == true);
        }

		sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyPhysicalFormat, &sz);
        assert(locRes == true);
        assert(sz == sizeof(AudioStreamBasicDescription));

        memset(&configurationStruct.classStreamsInput[i].descriptionPFormat, 0, sizeof(AudioStreamBasicDescription));

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i],kAudioStreamPropertyPhysicalFormat, &sz, &configurationStruct.classStreamsInput[i].descriptionPFormat);
        assert(locRes == true);
        
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsInput[i], kAudioStreamPropertyAvailablePhysicalFormats, &sz);
        assert(locRes == true);

        configurationStruct.classStreamsInput[i].descriptionAvailablePFormats = NULL;
        configurationStruct.classStreamsInput[i].numPFormats = sz/sizeof(AudioStreamRangedDescription);
        
		if(configurationStruct.classStreamsInput[i].numPFormats)
		{
            configurationStruct.classStreamsInput[i].descriptionAvailablePFormats = new AudioStreamRangedDescription[configurationStruct.classStreamsInput[i].numPFormats];

            locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsInput[i],kAudioStreamPropertyAvailablePhysicalFormats, &sz, configurationStruct.classStreamsInput[i].descriptionAvailablePFormats);
            assert(locRes == true);
		}
	}

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Next: Output streams
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyStreams, &sz);
    assert(locRes == true);
 
    configurationStruct.numStreamsOutput = 0;
    configurationStruct.streamsOutput = NULL;
    configurationStruct.classStreamsOutput = NULL;
    
    if(sz != 0)
	{
		configurationStruct.numStreamsOutput = sz/sizeof(AudioStreamID);
		if(configurationStruct.numStreamsOutput)
		{
			configurationStruct.streamsOutput = new AudioStreamID[configurationStruct.numStreamsOutput];
			configurationStruct.classStreamsOutput = new streamFormats[configurationStruct.numStreamsOutput];

            locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyStreams, &sz, configurationStruct.streamsOutput);
            assert(locRes == true);

#ifdef VERBOSE_MODE
			std::cout << "Initialize Device: Number output streams = " << configurationStruct.numStreamsOutput << std::endl;
#endif
        }
    }

	for(i = 0; i < configurationStruct.numStreamsOutput; i++)
    {
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyDirection, &sz);
        assert(locRes == true);
        
        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyDirection, &sz, &configurationStruct.classStreamsOutput[i].direction);
        assert(locRes == true);

        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyTerminalType, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyTerminalType, &sz, &configurationStruct.classStreamsOutput[i].terminalType);
        assert(locRes == true);

        // Starting channel
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyStartingChannel, &sz);
        assert(locRes == true);
        
        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyStartingChannel, &sz, &configurationStruct.classStreamsOutput[i].startingChannel);
        assert(locRes == true);

        // Latency
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyLatency, &sz);
        assert(locRes == true);

        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i],kAudioStreamPropertyLatency, &sz, &configurationStruct.classStreamsOutput[i].latency);
        assert(locRes == true);

        //
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyVirtualFormat, &sz);
        assert(locRes == true);
        assert(sz == sizeof(AudioStreamBasicDescription));

        memset(&configurationStruct.classStreamsOutput[i].descriptionVFormat, 0, sizeof(AudioStreamBasicDescription));
        
        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i],kAudioStreamPropertyVirtualFormat, &sz, &configurationStruct.classStreamsOutput[i].descriptionVFormat);
        assert(locRes == true);
        
        //
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyAvailableVirtualFormats, &sz);
        assert(locRes == true);
        
        configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats  = NULL;
        configurationStruct.classStreamsOutput[i].numVFormats = sz/sizeof(AudioStreamRangedDescription);
        
        if(configurationStruct.classStreamsOutput[i].numVFormats)
        {
            configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats = new AudioStreamRangedDescription[configurationStruct.classStreamsOutput[i].numVFormats];
            
            locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i],kAudioStreamPropertyAvailableVirtualFormats, &sz,
                                                       configurationStruct.classStreamsOutput[i].descriptionAvailableVFormats);
            assert(locRes == true);
        }
        
        //
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyPhysicalFormat, &sz);
        assert(locRes == true);
        
        assert(sz == sizeof(AudioStreamBasicDescription));
        
        memset(&configurationStruct.classStreamsOutput[i].descriptionPFormat, 0, sizeof(AudioStreamBasicDescription));
        
        locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i],kAudioStreamPropertyPhysicalFormat, &sz,
                                                   &configurationStruct.classStreamsOutput[i].descriptionPFormat);
        assert(locRes == true);
        
        //
        sz = 0;
        locRes = this->GetPropertyDataSize_AudioStream(configurationStruct.streamsOutput[i], kAudioStreamPropertyAvailablePhysicalFormats, &sz);
        assert(locRes == true);
        configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats = NULL;
        configurationStruct.classStreamsOutput[i].numPFormats = sz/sizeof(AudioStreamRangedDescription);
        
        if(configurationStruct.classStreamsOutput[i].numPFormats)
        {
            configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats = new AudioStreamRangedDescription[configurationStruct.classStreamsOutput[i].numPFormats];
            
            locRes = this->GetPropertyData_AudioStream(configurationStruct.streamsOutput[i],kAudioStreamPropertyAvailablePhysicalFormats, &sz, configurationStruct.classStreamsOutput[i].descriptionAvailablePFormats);
            assert(locRes == true);
        }
	}
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceSafetyOffset()
{
	UInt32 sz = 0;
    Boolean locRes = true;

	// =======================================
	// Get the clock domain
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertySafetyOffset, &sz);
    assert(locRes == true);
    assert(sz == sizeof(UInt32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertySafetyOffset, &sz, &configurationStruct.safetyOffset);
    assert(locRes);
    
#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Safety Offset = " << configurationStruct.safetyOffset << std::endl;
	//	std::cout << "kAudioDevicePropertySafetyOffset failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceCycleUsage()
{
	UInt32 sz = 0;
    Boolean locRes = true;

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyIOCycleUsage, &sz);
    assert(locRes == true);
    assert(sz == sizeof(Float32));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyIOCycleUsage, &sz, &configurationStruct.cycleUsage);
    assert(locRes == true);

#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: IO cycle usage = " << configurationStruct.cycleUsage << std::endl;
	//	std::cout << "kAudioDevicePropertyIOCycleUsage failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceAudioStreams()
{
	UInt32 sz = 0;
    Boolean locRes = true;

	configurationStruct.streamsInputStr = NULL;
	configurationStruct.streamsOutputStr = NULL;

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &sz);
    assert(locRes == true);

    if(sz)
	{
		configurationStruct.streamsInputStr = (AudioBufferList*)new char[sz];

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &sz, configurationStruct.streamsInputStr);
        assert(locRes == true);
    
	}

#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Stream Configuration (input) " << configurationStruct.cycleUsage << std::endl;
	//	std::cout << "kAudioDevicePropertyStreamConfiguration failed." << std::endl;
#endif

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyStreamConfiguration, &sz);
    assert(locRes == true);

    if(sz)
	{
		configurationStruct.streamsOutputStr = (AudioBufferList*)new char[sz];

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyStreamConfiguration, &sz, configurationStruct.streamsOutputStr);
        assert(locRes == true);

#ifdef VERBOSE_MODE
		std::cout << "Initialize Device: Stream Configuration (output) " << configurationStruct.cycleUsage << std::endl;
		//	std::cout << "kAudioDevicePropertyStreamConfiguration failed." << std::endl;
#endif

    }
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceAudioStreamUsage()
{
	UInt32 sz = 0;
    Boolean locRes = true;

    configurationStruct.streamUsageInput = NULL;
    configurationStruct.streamUsageOutput = NULL;

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &sz);
    assert(locRes == true);

    configurationStruct.streamUsageInputSupported = false;
    if(sz)
	{
		configurationStruct.streamUsageInput = (AudioHardwareIOProcStreamUsage*)new char[sz];

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &sz, configurationStruct.streamUsageInput);
        if(locRes == true)
        {
            configurationStruct.streamUsageInputSupported = true;
        }
#ifdef VERBOSE_MODE

        std::cout << "Initialize Device: Stream Usage " << configurationStruct.cycleUsage << std::endl;
		//	std::cout << "kAudioDevicePropertyIOProcStreamUsage failed." << std::endl;
#endif
	}

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &sz);
    assert(locRes == true);

    configurationStruct.streamUsageOutputSupported = false;

	if(sz)
	{
		configurationStruct.streamUsageOutput = (AudioHardwareIOProcStreamUsage*)new char[sz];

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &sz, configurationStruct.streamUsageOutput);
        if(locRes == true)
        {
            configurationStruct.streamUsageOutputSupported = true;
        }
#ifdef VERBOSE_MODE
		std::cout << "Initialize Device: Stream Usage " << configurationStruct.cycleUsage << std::endl;
//				std::cout << "kAudioDevicePropertyIOProcStreamUsage failed." << std::endl;
#endif
	}
}


/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getDeviceSamplerates()
{
	UInt32 sz = 0;
    Boolean locRes = true;

    configurationStruct.numSamplerates = 0;
	configurationStruct.samplerates = NULL;

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyAvailableNominalSampleRates, &sz);
    assert(locRes == true);

    configurationStruct.numSamplerates = sz/sizeof(AudioValueRange);
    if(configurationStruct.numSamplerates)
    {
        configurationStruct.samplerates = new AudioValueRange[configurationStruct.numSamplerates];

        locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyAvailableNominalSampleRates, &sz, configurationStruct.samplerates);
        assert(locRes == true);
        
#ifdef VERBOSE_MODE
		std::cout << "Initialize Device: Number samperates = " << configurationStruct.numSamplerates << std::endl;
		//		std::cout << "kAudioDevicePropertyAvailableNominalSampleRates failed." << std::endl;
#endif
	}
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getNominalSamplerate()
{
	UInt32 sz = 0;
    Boolean locRes = true;

    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyNominalSampleRate, &sz);
    assert(locRes == true);
    assert(sz == sizeof(Float64));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyNominalSampleRate, &sz, &configurationStruct.nominalSamplerate);
    assert(locRes == true);


#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Samperate = " << configurationStruct.nominalSamplerate << std::endl;
	//	std::cout << "kAudioDevicePropertyNominalSampleRate failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
jvxErrorType
CjvxAudioCoreAudioDevice::setNominalSamplerate(Float64 sRate)
{
	int i;
	UInt32 sz = 0;
    Boolean locRes = true;

	bool isInList = false;
	for(i = 0; i < configurationStruct.numSamplerates; i++)
	{
		if((sRate >= configurationStruct.samplerates[i].mMinimum)&&(sRate <= configurationStruct.samplerates[i].mMaximum))
		{
			isInList = true;
			break;
		}
	}

	if(isInList)
	{
        locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyNominalSampleRate, &sz);
        assert(locRes == true);
        
        assert(sz == sizeof(Float64));

        locRes = this->SetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyNominalSampleRate, sz, &sRate);
        assert(locRes == true);

#ifdef VERBOSE_MODE
		std::cout << "Modify Properties Device: Samperate = " << sRate << std::endl;
		//	std::cout << "kAudioDevicePropertyNominalSampleRate failed." << std::endl;
#endif
        return(JVX_NO_ERROR);
    }
	return(JVX_ERROR_INVALID_SETTING);
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::getActualSamplerate()
{
	UInt32 sz = 0;
    Boolean locRes = true;

	// =======================================
	//
    locRes = this->GetPropertyDataSize_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyActualSampleRate, &sz);
    assert(locRes == true);
    assert(sz == sizeof(Float64));

    locRes = this->GetPropertyData_AudioDeviceN(runtime.deviceProps.hdlDevice, 0, true, kAudioDevicePropertyActualSampleRate, &sz, &configurationStruct.actualSamplerate);
    assert(locRes == true);

#ifdef VERBOSE_MODE
	std::cout << "Initialize Device: Act. Samperate = " << configurationStruct.actualSamplerate << std::endl;
	//	std::cout << "kAudioDevicePropertyActualSampleRate failed." << std::endl;
#endif
}

/*
 *
 *///==============================================================================
void
CjvxAudioCoreAudioDevice::evaluateProperties()
{
	int i,ii;
    Boolean locRes = true;

#ifdef VERBOSE_MODE
	std::cout << "=======================================" << std::endl;
	std::cout << "Name Device: " << _common_set_min.theDescription << std::endl;
	std::cout << "Framesize = " << this->configurationStruct.framesize << std::endl;
	std::cout << "Framesize min = " << configurationStruct.framesizesAvailable.mMinimum << std::endl;
	std::cout << "Framesize max = " << configurationStruct.framesizesAvailable.mMaximum << std::endl;
#endif
	UInt32 inChans = 0;
	UInt32 outChans = 0;

    CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
    CjvxAudioDevice::properties_active.inputchannelselection.value.selection = 0;

    CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
    CjvxAudioDevice::properties_active.outputchannelselection.value.selection = 0;

    int idChannels = 0;

#ifdef VERBOSE_MODE
	std::cout << "Input streams:=============" << std::endl;
#endif
	for(i = 0; i < configurationStruct.streamsInputStr->mNumberBuffers; i++)
	{
#ifdef VERBOSE_MODE
		std::cout << "\t Buffer in #" << i << ": " << configurationStruct.streamsInputStr->mBuffers[i].mNumberChannels << " : " << configurationStruct.streamsInputStr->mBuffers[i].mDataByteSize << std::endl;
#endif
		inChans += configurationStruct.streamsInputStr->mBuffers[i].mNumberChannels;

		for(ii = 0; ii < configurationStruct.streamsInputStr->mBuffers[i].mNumberChannels; ii++)
		{
            CFStringRef str;
            UInt32 sz = sizeof(str);
            
            AudioObjectPropertyAddress theAddress = {
                kAudioObjectPropertyElementName,
                kAudioObjectPropertyScopeInput,
                (UInt32)(1+ii)};
            OSStatus errLoc = AudioObjectGetPropertyData(runtime.deviceProps.hdlDevice, &theAddress, 0, NULL, &sz, &str);
            std::string nm = CFString2StdString(str);
            CFRelease(str);

            if(nm.empty())
            {
                CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(
                        ("In #" + jvx_int2String(idChannels) + ":" + _common_set_min.theDescription).c_str());
            }
            else
            {
                CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(nm.c_str());
            }
            CjvxAudioDevice::properties_active.inputchannelselection.value.selection |= ((jvxUInt64) 1 << idChannels);
            idChannels++;
        }
	}

	idChannels = 0;

#ifdef VERBOSE_MODE
	std::cout << "Output streams:=============" << std::endl;
#endif
	for(i = 0; i < configurationStruct.streamsOutputStr->mNumberBuffers; i++)
	{
#ifdef VERBOSE_MODE
		std::cout << "\t Buffer out #" << i << ": " << configurationStruct.streamsOutputStr->mBuffers[i].mNumberChannels << " : " << configurationStruct.streamsOutputStr->mBuffers[i].mDataByteSize << std::endl;
#endif
		outChans += configurationStruct.streamsOutputStr->mBuffers[i].mNumberChannels;

		for(ii = 0; ii < configurationStruct.streamsOutputStr->mBuffers[i].mNumberChannels; ii++)
		{
            CFStringRef str;
            UInt32 sz = sizeof(str);
            
            AudioObjectPropertyAddress theAddress = {
                kAudioObjectPropertyElementName,
                kAudioObjectPropertyScopeOutput,
                (UInt32)(1+ii)};
            OSStatus errLoc = AudioObjectGetPropertyData(runtime.deviceProps.hdlDevice, &theAddress, 0, NULL, &sz, &str);
            std::string nm = CFString2StdString(str);
            CFRelease(str);
            
            if(nm.empty())
            {
                CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(
                ("Out #" + jvx_int2String(idChannels) + ":" + _common_set_min.theDescription).c_str());
            }
            else
            {
                CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(nm.c_str());

            }
            CjvxAudioDevice::properties_active.outputchannelselection.value.selection |= ((jvxUInt64) 1 << idChannels);
            idChannels++;
		}
	}

    genCoreAudio_device::properties_active.ratesselection.value.entries.clear();
    genCoreAudio_device::properties_active.ratesselection.value.selection = 0;
    
#ifdef VERBOSE_MODE
	std::cout << "Number samplerates: " << configurationStruct.numSamplerates << std::endl;
#endif
	for(i = 0; i < configurationStruct.numSamplerates; i++)
	{
#ifdef VERBOSE_MODE
		std::cout << "\t Rate #" << i << ": " << configurationStruct.samplerates[i].mMinimum << "::" << configurationStruct.samplerates[i].mMaximum << std::endl;
#endif
        this->runtime.samplerates.push_back(configurationStruct.samplerates[i].mMinimum);
        genCoreAudio_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(configurationStruct.samplerates[i].mMinimum));
        
        if(configurationStruct.samplerates[i].mMinimum == configurationStruct.nominalSamplerate)
        {
            genCoreAudio_device::properties_active.ratesselection.value.selection = ((jvxBitField)1 << i);
        }
    }
#ifdef VERBOSE_MODE
	std::cout << "Nominal samplerate: " << configurationStruct.nominalSamplerate << std::endl;
	std::cout << "Actual samplerate: " << configurationStruct.actualSamplerate << std::endl;
#endif
    
    jvxInt32 lastValid = -1;
    runtime.sizesBuffers.clear();
    genCoreAudio_device::properties_active.sizesselection.value.entries.clear();
    genCoreAudio_device::properties_active.sizesselection.value.selection = 0;
    
    for (i = 0; i < runtime.usefulBSizes.size(); i++)
    {
        if (
            (runtime.usefulBSizes[i] >= configurationStruct.framesizesAvailable.mMinimum) &&
            (runtime.usefulBSizes[i] <= configurationStruct.framesizesAvailable.mMaximum))
        {
            lastValid = runtime.usefulBSizes[i];
            runtime.sizesBuffers.push_back(runtime.usefulBSizes[i]);
            genCoreAudio_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(runtime.usefulBSizes[i]));
        }
    }
    
    assert(lastValid >= 0);
    this->setDeviceFramesize(lastValid);
    genCoreAudio_device::properties_active.sizesselection.value.selection = (jvxBitField)1 << (runtime.sizesBuffers.size() -1);
    

#ifdef VERBOSE_MODE

    std::cout << "==========Streams input============" << std::endl;
#endif
    for(i = 0; i < configurationStruct.numStreamsInput; i++)
    {
#ifdef VERBOSE_MODE
        std::cout << "\t Input stream #" << i << std::endl;
        std::cout << "\t Direction: " << configurationStruct.classStreamsInput[i].direction << std::endl;
        std::cout << "\t Terminal Type: " << configurationStruct.classStreamsInput[i].terminalType << std::endl;
        std::cout << "\t Starting channel: " << configurationStruct.classStreamsInput[i].startingChannel << std::endl;
        std::cout << "\t Latency: " << configurationStruct.classStreamsInput[i].latency << std::endl;
        std::cout << "\t Virtual Format: " << std::endl;
#endif
        
#ifdef VERBOSE_MODE
        std::cout << "\t\t Samplerate: " << configurationStruct.classStreamsInput[i].descriptionVFormat.mSampleRate << std::endl;
        std::cout << "\t\t Format: " << matchDataFormat(configurationStruct.classStreamsInput[i].descriptionVFormat.mFormatID) << std::endl;
        std::cout << "\t\t Format Flag: " << matchDataFormatFlag(configurationStruct.classStreamsInput[i].descriptionVFormat.mFormatFlags) << std::endl;
        std::cout << "\t\t Bytes per packet: " << configurationStruct.classStreamsInput[i].descriptionVFormat.mBytesPerPacket << std::endl;
        std::cout << "\t\t Frames per Packet: " << configurationStruct.classStreamsInput[i].descriptionVFormat.mFramesPerPacket << std::endl;
        std::cout << "\t\t Channels per frame: " << configurationStruct.classStreamsInput[i].descriptionVFormat.mChannelsPerFrame << std::endl;
        std::cout << "\t\t Bits per channel: " << configurationStruct.classStreamsInput[i].descriptionVFormat.mBitsPerChannel << std::endl;
#endif
#ifdef VERBOSE_MODE
        std::cout << "\t Physical Format: " << std::endl;
#endif
#ifdef VERBOSE_MODE
        std::cout << "\t\t Samplerate: " << configurationStruct.classStreamsInput[i].descriptionPFormat.mSampleRate << std::endl;
        std::cout << "\t\t Format: " << matchDataFormat(configurationStruct.classStreamsInput[i].descriptionPFormat.mFormatID) << std::endl;
        std::cout << "\t\t Format Flag: " << matchDataFormatFlag(configurationStruct.classStreamsInput[i].descriptionPFormat.mFormatFlags) << std::endl;
        std::cout << "\t\t Bytes per packet: " << configurationStruct.classStreamsInput[i].descriptionPFormat.mBytesPerPacket << std::endl;
        std::cout << "\t\t Frames per Packet: " << configurationStruct.classStreamsInput[i].descriptionPFormat.mFramesPerPacket << std::endl;
        std::cout << "\t\t Channels per frame: " << configurationStruct.classStreamsInput[i].descriptionPFormat.mChannelsPerFrame << std::endl;
        std::cout << "\t\t Bits per channel: " << configurationStruct.classStreamsInput[i].descriptionPFormat.mBitsPerChannel << std::endl;
#endif
        
        /*
         TODO
         if(running.bitsInternally == -1)
         {
         running.bitsInternally = configurationStruct.classStreamsInput[i].descriptionPFormat->mBitsPerChannel;
         }
         else
         {
         if(configurationStruct.classStreamsInput[i].descriptionPFormat->mBitsPerChannel < running.bitsInternally)
         {
         running.bitsInternally = configurationStruct.classStreamsInput[i].descriptionPFormat->mBitsPerChannel;
         }
         }
         */
    }

#ifdef VERBOSE_MODE
	std::cout << "==========Streams output============" << std::endl;
#endif
    for(i = 0; i < configurationStruct.numStreamsOutput; i++)
    {
#ifdef VERBOSE_MODE
        std::cout << "\t Output stream #" << i << std::endl;
        std::cout << "\t Direction: " << configurationStruct.classStreamsOutput[i].direction << std::endl;
        std::cout << "\t Terminal Type: " << configurationStruct.classStreamsOutput[i].terminalType << std::endl;
        std::cout << "\t Starting channel: " << configurationStruct.classStreamsOutput[i].startingChannel << std::endl;
        std::cout << "\t Latency: " << configurationStruct.classStreamsOutput[i].latency << std::endl;
        std::cout << "\t Virtual Format: " << std::endl;
#endif
#ifdef VERBOSE_MODE
        std::cout << "\t\t Samplerate: " << configurationStruct.classStreamsOutput[i].descriptionVFormat.mSampleRate << std::endl;
        std::cout << "\t\t Format: " << matchDataFormat(configurationStruct.classStreamsOutput[i].descriptionVFormat.mFormatID) << std::endl;
        std::cout << "\t\t Format Flag: " << matchDataFormatFlag(configurationStruct.classStreamsOutput[i].descriptionVFormat.mFormatFlags) << std::endl;
        std::cout << "\t\t Bytes per packet: " << configurationStruct.classStreamsOutput[i].descriptionVFormat.mBytesPerPacket << std::endl;
        std::cout << "\t\t Frames per Packet: " << configurationStruct.classStreamsOutput[i].descriptionVFormat.mFramesPerPacket << std::endl;
        std::cout << "\t\t Channels per frame: " << configurationStruct.classStreamsOutput[i].descriptionVFormat.mChannelsPerFrame << std::endl;
        std::cout << "\t\t Bits per channel: " << configurationStruct.classStreamsOutput[i].descriptionVFormat.mBitsPerChannel << std::endl;
#endif
        
#ifdef VERBOSE_MODE
        std::cout << "\t Physical Format: " << std::endl;
#endif
#ifdef VERBOSE_MODE
        std::cout << "\t\t Samplerate: " << configurationStruct.classStreamsOutput[i].descriptionPFormat.mSampleRate << std::endl;
        std::cout << "\t\t Format: " << matchDataFormat(configurationStruct.classStreamsOutput[i].descriptionPFormat.mFormatID) << std::endl;
        std::cout << "\t\t Format Flag: " << matchDataFormatFlag(configurationStruct.classStreamsOutput[i].descriptionPFormat.mFormatFlags) << std::endl;
        std::cout << "\t\t Bytes per packet: " << configurationStruct.classStreamsOutput[i].descriptionPFormat.mBytesPerPacket << std::endl;
        std::cout << "\t\t Frames per Packet: " << configurationStruct.classStreamsOutput[i].descriptionPFormat.mFramesPerPacket << std::endl;
        std::cout << "\t\t Channels per frame: " << configurationStruct.classStreamsOutput[i].descriptionPFormat.mChannelsPerFrame << std::endl;
        std::cout << "\t\t Bits per channel: " << configurationStruct.classStreamsOutput[i].descriptionPFormat.mBitsPerChannel << std::endl;
#endif
        /* TODO
         if(running.bitsInternally == -1)
         {
         running.bitsInternally = configurationStruct.classStreamsOutput[i].descriptionPFormat->mBitsPerChannel;
         }
         else
         {
         if(configurationStruct.classStreamsOutput[i].descriptionPFormat->mBitsPerChannel < running.bitsInternally)
         {
         running.bitsInternally = configurationStruct.classStreamsOutput[i].descriptionPFormat->mBitsPerChannel;
         }
         }
         */
    }
}

// =============================================================================================================
// System switch state callbacks
// =============================================================================================================

/*
 *
 *///===========================================================
jvxErrorType
CjvxAudioCoreAudioDevice::activateCoreAudioProperties()
{
	jvxErrorType res = JVX_NO_ERROR;

	clearProperties();
    getClockDomain();
	getDeviceAlive();
	getDeviceRunningElsewhere();
	getDeviceOverload();
	getDeviceHogMode();
	getDeviceLatencies();
	getDeviceFramesize();
	getDeviceFramesizesAvailable();
	getDeviceUseVarFramesize();
    getDeviceStreams();
	getDeviceAudioStreams();
	getDeviceSafetyOffset();
	getDeviceCycleUsage();
	getDeviceAudioStreamUsage();
	getDeviceSamplerates();
    getNominalSamplerate();
    getActualSamplerate();
    evaluateProperties();

    
	_lock_properties_local();
	_unlock_properties_local();

	return (res);
}

/*
 * 
 *///===============================================================
jvxErrorType
CjvxAudioCoreAudioDevice::deactivateCoreAudioProperties()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Todo: remove all property descriptors
	this->clearProperties();

	return (res);
}

/*
 *
 *///==============================================================================
jvxErrorType
CjvxAudioCoreAudioDevice::prepareCoreAudio()
{
    jvxSize i;
    jvxSize cnt;
    JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
    
    // Allocate bufferswitch callback
    if(AudioDeviceCreateIOProcID(runtime.deviceProps.hdlDevice, AudioDeviceIOProcDev, (void*)this, &runtime.references.procID) == noErr)
    {
        
        this->setNominalSamplerate(_inproc.samplerate);
        this->setDeviceFramesize(_inproc.buffersize);
        
        cnt = 0;
        runtime.references.chan_map_input.clear();
        for(i = 0; i < CjvxAudioDevice::properties_active.inputchannelselection.value.entries.size(); i++)
        {
	  if(jvx_bitTest(CjvxAudioDevice::properties_active.inputchannelselection.value.selection, i))
            {
                runtime.references.chan_map_input.push_back(cnt);
                cnt++;
            }
            else
            {
                runtime.references.chan_map_input.push_back(-1);
            }
        }
        //assert(runtime.references.chan_map_input.size() == _inproc.numInputs);
        
        cnt = 0;
        runtime.references.chan_map_output.clear();
        for(i = 0; i < CjvxAudioDevice::properties_active.outputchannelselection.value.entries.size(); i++)
        {
	  if(jvx_bitTest(CjvxAudioDevice::properties_active.outputchannelselection.value.selection, i))
            {
                runtime.references.chan_map_output.push_back(cnt);
                cnt++;
            }
            else
            {
                runtime.references.chan_map_output.push_back(-1);
            }
        }
        //assert(runtime.references.chan_map_output.size() == _inproc.numOutputs);
        
        return(JVX_NO_ERROR);
    }
    
    return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
}

jvxErrorType
CjvxAudioCoreAudioDevice::postprocessCoreAudio()
{
    OSStatus err = AudioDeviceDestroyIOProcID ( runtime.deviceProps.hdlDevice, runtime.references.procID);
    assert(err == noErr);
    return(JVX_NO_ERROR);
}

/**
 *
 *///=========================================================
jvxErrorType
CjvxAudioCoreAudioDevice::startCoreAudio()
{
    if(AudioDeviceStart(runtime.deviceProps.hdlDevice, AudioDeviceIOProcDev) != noErr)
    {
        return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
    }
    return(JVX_NO_ERROR);
}

/**
 *
 *///=========================================================
jvxErrorType
CjvxAudioCoreAudioDevice::stopCoreAudio()
{
    OSStatus err = AudioDeviceStop(runtime.deviceProps.hdlDevice, AudioDeviceIOProcDev);
    assert(err == noErr);
    return(JVX_NO_ERROR);}

//==============================================================================
// Processing callback
//==============================================================================

void
CjvxAudioCoreAudioDevice::process(const AudioTimeStamp* inNow, const AudioBufferList* inInputData,
                                  const AudioTimeStamp* inInputTime, AudioBufferList * outOutputData,
                                  const AudioTimeStamp* inOutputTime)
{
    /*
     // Most simple talkthrough
     char* ptrFieldHardIn = (char*)inInputData->mBuffers[0].mData;
     char* ptrFieldHardOut = (char*)outOutputData->mBuffers[0].mData;
     memcpy(ptrFieldHardOut, ptrFieldHardIn, outOutputData->mBuffers[0].mDataByteSize);
     return;
     */
#if 0
    static jvxData phase = 0.0;
#endif
    int i, ii, iii;
    int cntIn = 0;
    int cntInValid = 0;
    jvxData tmp;
    
    jvxData* ptrOut_dbl = NULL;
    jvxData* ptrOut_flt = NULL;
    jvxInt8* ptrOut_int8 = NULL;
    jvxInt16* ptrOut_int16 = NULL;
    jvxInt32* ptrOut_int32 = NULL;
    jvxInt64* ptrOut_int64 = NULL;
    
    jvxData* ptrIn_dbl = NULL;
    jvxData* ptrIn_flt = NULL;
    jvxInt8* ptrIn_int8 = NULL;
    jvxInt16* ptrIn_int16 = NULL;
    jvxInt32* ptrIn_int32 = NULL;
    jvxInt64* ptrIn_int64 = NULL;
    
    if (_common_set_ldslave.theData_out.con_link.connect_to)
      {
	_common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
      }
				
    jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
    jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
				
    if(inInputData)
    {
        for(i = 0; i < inInputData->mNumberBuffers; i++)
        {
            jvxSize szBytes = inInputData->mBuffers[i].mDataByteSize / _inproc.buffersize / inInputData->mBuffers[i].mNumberChannels;
            assert(szBytes == sizeof(float));
            jvxSize increment = inInputData->mBuffers[i].mNumberChannels;
            
            for(ii = 0; ii < inInputData->mBuffers[i].mNumberChannels; ii++)
            {
                float* inPtrs_interleaved = reinterpret_cast<float*>(inInputData->mBuffers[i].mData);
                inPtrs_interleaved += ii;
                
                jvxInt32 chanMap = runtime.references.chan_map_input[ii];
                if(chanMap >= 0)
                {
                    switch(_common_set_ldslave.theData_out.con_params.format)
                    {
                        case JVX_DATAFORMAT_DATA:
                            ptrOut_dbl = reinterpret_cast<jvxData*>(
								    _common_set_ldslave.theData_out.con_data.buffers[idxToProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_FLOAT_2_DATA(*inPtrs_interleaved);
                                *ptrOut_dbl = JVX_DATA_2_DATA(tmp);
                                inPtrs_interleaved += increment;
                                ptrOut_dbl++;
                            }
                            break;
 
                        case JVX_DATAFORMAT_8BIT:
                            ptrOut_int8 = reinterpret_cast<jvxInt8*>(_common_set_ldslave.theData_out.con_data.buffers[idxToProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_FLOAT_2_DATA(*inPtrs_interleaved);
                                *ptrOut_int8 = JVX_DATA_2_INT8(tmp);
                                inPtrs_interleaved += increment;
                                ptrOut_int8++;
                            }
                            break;
                        case JVX_DATAFORMAT_16BIT_LE:
                            ptrOut_int16 = reinterpret_cast<jvxInt16*>(_common_set_ldslave.theData_out.con_data.buffers[idxToProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_FLOAT_2_DATA(*inPtrs_interleaved);
                                *ptrOut_int16 = JVX_DATA_2_INT16(tmp);
                                inPtrs_interleaved += increment;
                                ptrOut_int16++;
                            }
                            break;
                        case JVX_DATAFORMAT_32BIT_LE:
                            ptrOut_int32 = reinterpret_cast<jvxInt32*>(_common_set_ldslave.theData_out.con_data.buffers[idxToProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_FLOAT_2_DATA(*inPtrs_interleaved);
                                *ptrOut_int32 = JVX_DATA_2_INT32(tmp);
                                inPtrs_interleaved += increment;
                                ptrOut_int32++;
                            }
                            break;
                        case JVX_DATAFORMAT_64BIT_LE:
                            ptrOut_int64 = reinterpret_cast<jvxInt64*>(_common_set_ldslave.theData_out.con_data.buffers[idxToProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_FLOAT_2_DATA(*inPtrs_interleaved);
                                *ptrOut_int64 = JVX_DATA_2_INT64(tmp);
                                inPtrs_interleaved += increment;
                                ptrOut_int64++;
                            }
                            break;
                    }
                }
            }
        }
    }
    
    if (_common_set_ldslave.theData_out.con_link.connect_to)
      {
	_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
      }
    /*
    else
    {
        int maxChans = JVX_MAX(inProcessing.theData.con_params.number_channels,inProcessing.theData.con_compat.number_channels);
        for(ii = 0; ii < maxChans; ii++)
        {
            int idxIn = ii % inProcessing.theData.con_params.number_channels;
            int idxOut = ii % inProcessing.theData.con_compat.number_channels;
            
            assert(inProcessing.theData.con_params.format == inProcessing.theData.con_compat.format);
            jvx_convertSamples_memcpy(
                                      inProcessing.theData.con_data.buffers[0][idxIn],
                                      inProcessing.theData.con_compat.from_receiver_buffer_allocated_by_sender[0][idxOut],
                                      jvxDataFormat_size[inProcessing.theData.con_params.format],
                                      inProcessing.theData.con_params.buffersize);
        }
    }
    */
    
    if(outOutputData)
    {
        for(i = 0; i < outOutputData->mNumberBuffers; i++)
        {
            jvxSize szBytes = outOutputData->mBuffers[i].mDataByteSize / _inproc.buffersize / outOutputData->mBuffers[i].mNumberChannels;
            assert(szBytes == sizeof(float));
            jvxSize increment = outOutputData->mBuffers[i].mNumberChannels;
            
            for(ii = 0; ii < outOutputData->mBuffers[i].mNumberChannels; ii++)
            {
                float* outPtrs_interleaved = reinterpret_cast<float*>(outOutputData->mBuffers[i].mData);
                outPtrs_interleaved += ii;
                
                jvxInt32 chanMap = runtime.references.chan_map_output[ii];
                if(chanMap >= 0)
                {
                    switch(_common_set_ldslave.theData_in->con_params.format)
                    {
                        case JVX_DATAFORMAT_DATA:
                            ptrIn_dbl = reinterpret_cast<jvxData*>(_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_DATA_2_DATA(*ptrIn_dbl);
                                *outPtrs_interleaved = JVX_DATA_2_FLOAT(tmp);
                                outPtrs_interleaved += increment;
                                ptrIn_dbl++;
                            }
                            break;
                                                    
                        case JVX_DATAFORMAT_8BIT:
                            ptrIn_int8 = reinterpret_cast<jvxInt8*>(_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_INT8_2_DATA(*ptrIn_flt);
                                *outPtrs_interleaved = JVX_DATA_2_FLOAT(tmp);
                                outPtrs_interleaved += increment;
                                ptrIn_int8++;
                            }
                            break;
                        case JVX_DATAFORMAT_16BIT_LE:
                            ptrIn_int16 = reinterpret_cast<jvxInt16*>(_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_INT16_2_DATA(*ptrIn_flt);
                                *outPtrs_interleaved = JVX_DATA_2_FLOAT(tmp);
                                outPtrs_interleaved += increment;
                                ptrIn_int16++;
                            }
                            break;
                        case JVX_DATAFORMAT_32BIT_LE:
                            ptrIn_int32 = reinterpret_cast<jvxInt32*>(_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_INT32_2_DATA(*ptrIn_flt);
                                *outPtrs_interleaved = JVX_DATA_2_FLOAT(tmp);
                                outPtrs_interleaved += increment;
                                ptrIn_int32++;
                            }
                            break;
                        case JVX_DATAFORMAT_64BIT_LE:
                            ptrIn_int64 = reinterpret_cast<jvxInt64*>(_common_set_ldslave.theData_in->con_data.buffers[idxFromProc][chanMap]);
                            for(iii = 0; iii < _inproc.buffersize; iii++)
                            {
                                tmp = JVX_INT64_2_DATA(*ptrIn_flt);
                                *outPtrs_interleaved = JVX_DATA_2_FLOAT(tmp);
                                outPtrs_interleaved += increment;
                                ptrIn_int64++;
                            }
                            break;
                    }
                }
            }
        }
    }

    if (_common_set_ldslave.theData_out.con_link.connect_to)
      {
	_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
      }
}

