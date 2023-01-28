#ifndef __CJVXAUDIOCOREAUDIODEVICE_H__
#define __CJVXAUDIOCOREAUDIODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "pcg_exports_device.h"

#include <CoreAudio/AudioHardware.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>

#define ALPHA_SMOOTH_DELTA_T 0.995

class CjvxAudioCoreAudioDevice:
    public CjvxAudioDevice,
    public genCoreAudio_device
{
    friend class CjvxAudioCoreAudioTechnology;

private:

    //! Properties of the stream associated to the device
    typedef struct
    {
        //! Direction property type
        UInt32 direction;

        //! Terminal type
        UInt32 terminalType;

        //! Starting channel
        UInt32 startingChannel;

        //! Additional stream latency
        UInt32 latency;

        // Descriptor for formats (software)
        AudioStreamBasicDescription descriptionVFormat;

        // Descriptor for formats (software)
        AudioStreamRangedDescription* descriptionAvailableVFormats;
        UInt32 numVFormats;

        // Descriptor for formats (hardware)
        AudioStreamBasicDescription descriptionPFormat;

        // Descriptor for formats (hardware)
        AudioStreamRangedDescription* descriptionAvailablePFormats;
        UInt32 numPFormats;

    } streamFormats;

    //! Struct to hold all devices properties
    typedef struct
    {
        UInt32 clockDomain;

        UInt32 deviceIsAlive;

        UInt32 deviceIsRunningElsewhere;

        UInt32 isOverload;

        UInt32 hogMode;
        jvxBool hogModeSupported;


        UInt32 deviceLatencyInput;
        UInt32 deviceLatencyOutput;

        UInt32 framesize;

        AudioValueRange framesizesAvailable;

        UInt32 useVarFramesize;
        jvxBool useVarFramesizeSupported;

        AudioStreamID* streamsInput;
        AudioStreamID* streamsOutput;

        streamFormats* classStreamsInput;
        streamFormats* classStreamsOutput;

        UInt32 numStreamsInput;
        UInt32 numStreamsOutput;

        UInt32 safetyOffset;

        Float32 cycleUsage;

        AudioBufferList* streamsInputStr;
        AudioBufferList* streamsOutputStr;

        AudioHardwareIOProcStreamUsage* streamUsageInput;
        jvxBool streamUsageInputSupported;

        AudioHardwareIOProcStreamUsage* streamUsageOutput;
        jvxBool streamUsageOutputSupported;

        AudioValueRange* samplerates;
        UInt32 numSamplerates;

        Float64 nominalSamplerate;
        Float64 actualSamplerate;
    } deviceConfigurationStruct;

    deviceConfigurationStruct configurationStruct;

    typedef struct
    {
        int idPhysicalBuffer;
        void* linkBuf;
    } oneBufRef;
    /*	typedef struct
     {

     typedef struct
     {
     std::string friendlyNameSoundcard;
     std::string descriptionSoundcard;
     std::string systemNameSoundcard;
     int idSoundcard;
     std::vector<oneDevice> lstDevices;
     }oneCard;



     struct
     {
     std::vector<snd_pcm_access_t> availableAccessTypes;
     std::vector<snd_pcm_format_t> availableFormatTypes;

     struct
     {
	    //unsigned int numChannels;
	    unsigned int numChannelsMin;
	    unsigned int numChannelsMax;

	    struct
	    {
     //unsigned int val;
     unsigned int valMin;
     unsigned int valMax;
     //unsigned int allMax;

     int dir;
     int dirMin;
     int dirMax;
	    } period;

     } input;

     struct
     {
	    //unsigned int numChannels;
	    unsigned int numChannelsMin;
	    unsigned int numChannelsMax;

	    struct
	    {
     //unsigned int val;
     unsigned int valMin;
     unsigned int valMax;
     //unsigned int allMax;

     int dir;
     int dirMin;
     int dirMax;
	    } period;

     } output;

     struct
     {
	    //unsigned int val;
	    unsigned int valMin;
	    unsigned int valMax;

	    //int dir;
	    int dirMin;
	    int dirMax;
     } sRate;

     struct
     {
	    //snd_pcm_uframes_t val;
	    snd_pcm_uframes_t valMin;
	    snd_pcm_uframes_t valMax;
	    //snd_pcm_uframes_t allMax;
     } buffersize;

     //std::vector<int> periods;
     std::vector<int> buffersizes;

     } detectedProps;
     */
    //oneDevice thisDevice;

    struct
    {
        struct
        {
            std::vector<jvxInt32> chan_map_input;
            std::vector<jvxInt32> chan_map_output;
            AudioDeviceIOProcID procID;
        } references;
        /*
         struct
         {
         IrtpWriteLogfiles* theHandle;
         HrtpLogFileWriter wrapper;
         oneEntryLogfile RPA;
         oneEntryLogfile RPB;
         oneEntryLogfile RPC;
         oneEntryLogfile RPD;
         } logfile;
         */

        struct
        {
            std::string description;
            jvxSize idx;
            AudioDeviceID hdlDevice;
        } deviceProps;

        std::vector<long> sizesBuffers;

        std::vector<long> samplerates;

        std::vector<jvxInt32> usefulBSizes;
    } runtime;

    struct
    {
        jvxTimeStampData theTimestamp;
        jvxInt64 timestamp_previous;
        jvxData deltaT_average_us;
        jvxData deltaT_theory_us;

    } inProcessing;




public:
    JVX_CALLINGCONVENTION CjvxAudioCoreAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

    virtual JVX_CALLINGCONVENTION ~CjvxAudioCoreAudioDevice();

    virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;

    virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties* callGate,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize propId, jvxPropertyCategoryType category, 
							    jvxSize offsetStart, jvxBool contentOnly)override;

  
    // Interface IjvxConfiguration

    	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, 
			IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
			const char* filename = "", jvxInt32 lineno = -1)override;
			
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

  	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
  virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset , jvxSize* idx_stage,
	    jvxSize tobeAccessedByStage,
	    callback_process_start_in_lock clbk,
	    jvxHandle* priv_ptr  )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage ,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr	)override;
  	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

  jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
  
    void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);

    // ================================================================================================

    jvxErrorType initialize(std::string description, jvxSize idx, AudioDeviceID hdlDevice);

    jvxErrorType activateCoreAudioProperties();
    jvxErrorType deactivateCoreAudioProperties();

    jvxErrorType prepareCoreAudio();
    jvxErrorType postprocessCoreAudio();

    jvxErrorType startCoreAudio();
    jvxErrorType stopCoreAudio();

    void process(const AudioTimeStamp* inNow, const AudioBufferList* inInputData,
                 const AudioTimeStamp* inInputTime, AudioBufferList * outOutputData,
                 const AudioTimeStamp* inOutputTime);

    void getClockDomain();
    void getDeviceAlive();
    void getDeviceRunningElsewhere();
    void getDeviceOverload();
    void getDeviceHogMode();
    void getDeviceLatencies();
    void getDeviceFramesize();
    void getDeviceFramesizesAvailable();
    void getDeviceUseVarFramesize();
    void getDeviceStreams();
    void getDeviceAudioStreams();
    void getDeviceSafetyOffset();
    void getDeviceCycleUsage();
    void getDeviceAudioStreamUsage();
    void getDeviceSamplerates();
    void getNominalSamplerate();
    void getActualSamplerate();

    jvxErrorType setDeviceFramesize(UInt32 fSize);
    jvxErrorType setNominalSamplerate(Float64 sRate);

    void evaluateProperties();
    void clearProperties();
    void clearPropertiesInit();

    /*

     static Boolean GetPropertyData_AudioDevice(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
     Boolean inSection, UInt32 inPropertyID,
     UInt32* ioDataSize, void* outData);
     static Boolean GetPropertyDataInfo_AudioDevice(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
     Boolean inSection, UInt32 inPropertyID,
     UInt32* ioDataSize, Boolean* writeable);

     */
    static Boolean GetPropertyDataSize(AudioObjectID mAudioDeviceID, UInt32 inChannel,
                                       Boolean inSection, UInt32 inPropertyID,
                                       UInt32* ioDataSize);

    static Boolean GetPropertyData(AudioObjectID mAudioDeviceID, UInt32 inChannel,
                                   Boolean inSection, UInt32 inPropertyID,
                                   UInt32* ioDataSize, void* outData);

    static Boolean GetPropertyData_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
                                                Boolean inSection, UInt32 inPropertyID,
                                                UInt32* ioDataSize, void* outData);

    static Boolean SetPropertyData_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
                                                Boolean inSection, UInt32 inPropertyID,
                                                UInt32 ioDataSize, void* outData);

    static Boolean GetPropertyDataSize_AudioDeviceN(AudioDeviceID mAudioDeviceID, UInt32 inChannel,
                                                    Boolean inSection, UInt32 inPropertyID,
                                                    UInt32* ioDataSize);

    static Boolean GetPropertyDataSize_AudioStream(AudioStreamID mAudioStreamID, UInt32 inPropertyID, UInt32* ioDataSize);

    static Boolean GetPropertyData_AudioStream(AudioStreamID mAudioStreamID, UInt32 inPropertyID,
                                               UInt32* ioDataSize, void* outData);

    /**
     * Function to convert a CFStringRef (who needs this by the way?)
     * into a STD::STRING
     *///=======================================================
    static std::string CFString2StdString(CFStringRef& str);

};

#endif
