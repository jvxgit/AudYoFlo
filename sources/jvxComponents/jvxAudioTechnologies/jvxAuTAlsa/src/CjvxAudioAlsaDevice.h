#ifndef __CJVXAUDIOALSADEVICE_H__
#define __CJVXAUDIOALSADEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "pcg_exports_device.h"

#include "config_alsa.h"

#include <alsa/asoundlib.h>
#include "testtypes.h"
#include "priorityList.h"

//#define JVX_ALSA_LOGTOFILE
// This macro may be used to store the raw audio data to harddrive
// At the moment, this function is a hack which does only work in 32 bit
// mode to track i2s audio input

#ifdef JVX_ALSA_LOGTOFILE
#include "CjvxLogSignalsToFile.h"
#endif

#define ALPHA_SMOOTH_DELTA_T 0.995

class CjvxAudioAlsaDevice: 
	public CjvxAudioDevice,
    public genAlsa_device
{

#ifdef JVX_ALSA_LOGTOFILE
    struct oneEntry
    {
        std::string description;
        jvxSize uId;
        jvxSize numChannels;
        jvxSize bSize;
        jvxDataFormat format;
    };
    CjvxLogSignalsToFile theLogger;
    std::map<jvxSize, oneEntry> registered_logs;

    jvxSize uId_input;
    jvxSize uId_output;
#endif

	friend class CjvxAudioAlsaTechnology;
	/*
	typedef struct
	{
		std::string name;
		bool isSelected;
	} oneChannel;
	*/
private:

	typedef struct
	{
		unsigned int inovwrtnumChannelMin = 0;
		unsigned int inovwrtnumChannelMax = 0;
		unsigned int outovwrtnumChannelMin = 0;
		unsigned int outovwrtnumChannelMax = 0;

		std::vector<snd_pcm_access_t> availableAccessTypes;
	  std::vector<snd_pcm_format_t> availableFormatTypes;

	  struct
	  {
	    //unsigned int numChannels;
	    unsigned int numChannelMin;
	    unsigned int numChannelMax;

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
		  struct
		  {
		    //snd_pcm_uframes_t val;
		    snd_pcm_uframes_t valMin;
		    snd_pcm_uframes_t valMax;
		    //snd_pcm_uframes_t allMax;
		  } buffersize;
	  } input;

	  struct
	  {
	    //unsigned int numChannels;
	    unsigned int numChannelMin;
	    unsigned int numChannelMax;
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
		  struct
		  {
		    //snd_pcm_uframes_t val;
		    snd_pcm_uframes_t valMin;
		    snd_pcm_uframes_t valMax;
		    //snd_pcm_uframes_t allMax;
		  } buffersize;
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



	  //std::vector<int> periods;
	  std::vector<int> buffersizes;

	} deviceProps;

	typedef struct
	{
	  std::string name;
	} oneSubDevice;

/*
 *
 	 typedef struct
	{
		rtpIdx idx;
		char* fld;
	} oneEntryLogfile;
*/
	typedef struct
	{
	  struct
	  {
	    bool exists;
	    int numSubdevices;
	    snd_pcm_sync_id_t syncId;
	    std::string friendlyNameDevice;
	    std::vector<oneSubDevice> subdevices;
	  } output;
	  struct
	  {
	    bool exists;
	    int numSubdevices;
	    snd_pcm_sync_id_t syncId;
	    std::string friendlyNameDevice;
	    std::vector<oneSubDevice> subdevices;
	  } input;
	  jvxBool isPlugConnection;
	  std::string systemNameDevice;
	}oneDevice;

	typedef struct
	{
	  std::string friendlyNameSoundcard;
	  std::string descriptionSoundcard;
	  std::string systemNameSoundcard;

	  int idSoundcard;
	  std::vector<oneDevice> lstDevices;
	}oneCard;

	typedef struct
	{
	  int idPhysicalBuffer;
	  void* linkBuf;
	} oneBufRef;

	deviceProps detectedProps;

	deviceProps detectedPropsOnInit;

	oneDevice thisDevice;

	struct
	{
	  struct
	  {
	    snd_pcm_access_t accessMode;
	    snd_pcm_format_t format;
	    unsigned int sRate;
	    unsigned int periodsInput;
	    unsigned int periodsOutput;
	    snd_pcm_uframes_t buffersize;


	    bool continueLoop;
	    bool readWriteInterleaved;

	    JVX_THREAD_HANDLE threadHdl;
	    struct
	    {

	      snd_pcm_t* pcmHandle;
	      snd_pcm_hw_params_t* params_hw;
	      snd_pcm_sw_params_t* params_sw;
	    } input;

	    struct
	    {
	      snd_pcm_t* pcmHandle;
	      snd_pcm_hw_params_t* params_hw;
	      snd_pcm_sw_params_t* params_sw;
	    } output;

	    jvxInt64 timer_old;
	    jvxInt64 timer_maxDt;
	    jvxInt64 timer_minDt;

	    jvxTimeStampData refTimer;
	    bool asyncCallback;

	    jvxInt32 numInputChansHw;
	    jvxInt32 numOutputChansHw;
	  } inAction;

	  struct
	  {
	    char* fld_inputi;
	    char* fld_outputi;
	    void** fld_inputn;
	    void** fld_outputn;

	    //oneBufRef* refsInput;
	    //oneBufRef* refsOutput;
	    snd_async_handler_t* hdlCallback;
	    jvxSize szOneElement_hw;
	    //jvxSize szOneElement_sw;
	  } ram;
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
		snd_pcm_channel_area_t* theArea_rw_input;
		snd_pcm_channel_area_t* theArea_rw_output;

		std::vector<long> sizesBuffers;
		std::vector<long> sizesBuffersOnInit;

		std::vector<long> samplerates;
		std::vector<long> sampleratesOnInit;

		std::vector<jvxInt32> usefulBSizes;
	} runtime;

	struct
	{
	  bool asyncCallbacks;
	  bool psp;
	  //bool writelog;
	  //std::string logfilename;
	  //bool logfileAvailable;
	} shareWithUi;


	struct
	{

		jvxTimeStampData theTimestamp;
		jvxInt64 timestamp_previous;
		jvxData deltaT_average_us;
		jvxData deltaT_theory_us;

		std::vector<jvxInt32> map_input_chans;
		std::vector<jvxInt32> map_output_chans;
	} inProcessing;


public:
	JVX_CALLINGCONVENTION CjvxAudioAlsaDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioAlsaDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "", jvxInt32 lineno = -1 )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

    virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

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
	    jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
	    jvxSize idx_stage ,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)override;

	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	
	void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	jvxErrorType updateCurrentProperties(jvxBool initRun, jvxBool updateRates, jvxBool updateSizes); //, jvxBool readBuffersizes

	// ================================================================================================

	void props_device(oneDevice& deviceInfo)
	{
		thisDevice = deviceInfo;
	};

	jvxErrorType activateAlsaProperties();
	jvxErrorType deactivateAlsaProperties();
	jvxErrorType prepareAlsa();
	jvxErrorType startAlsa();
	jvxErrorType stopAlsa();
	jvxErrorType postProcessAlsa();
	void resetAlsa();

	jvxBool setSamplerate_alsa(jvxInt32& newrate);
	jvxBool setBuffersize_alsa(jvxInt32& buffersize, jvxInt32& periodsInput, jvxInt32& periodsOutput);


	void openHandles(snd_pcm_t*& input_pcmHandle,snd_pcm_t*& output_pcmHandle, snd_pcm_hw_params_t*& input_params_hw,
		snd_pcm_hw_params_t*& output_params_hw, bool block, std::string deviceName);

	void closeHandles(snd_pcm_t*& input_pcmHandle,
		snd_pcm_t*& output_pcmHandle, snd_pcm_hw_params_t*& input_params_hw,
		snd_pcm_hw_params_t*& output_params_hw);

	jvxErrorType read_write_loop_inclass_pspon();
	jvxErrorType read_write_loop_inclass_pspoff();
	jvxErrorType mmap_read_write_loop_inclass();

	void read_write_callback();
	void mmap_callback();

	void fillBuffersBeforeStart_readwrite();
	void fillBuffersBeforeStart_mmap();
	void copyBuffers_hwToSw_interleaved(const snd_pcm_channel_area_t* hw,
			void** output, jvxInt32 offset, jvxInt32 numSamples);
	void copyBuffers_swToHw_interleaved(const snd_pcm_channel_area_t* hw,
			void** output, jvxInt32 offset, jvxInt32 numSamples);

};

#endif
