#ifndef _CJVXGENERICWRAPPERDEVICE_H__
#define _CJVXGENERICWRAPPERDEVICE_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "compatibility/IjvxDataProcessor.h"

#include "CjvxGenericWrapper_common.h"
#include "CjvxCircularBuffer.h"

#include "jvx_threads.h"

#include "pcg_exports_device.h"

#include "CjvxGenericWrapperDevice_hostRelocator.h"

#define CIRCBUFFER CjvxCircularBuffer
//#define CIRCBUFFER CjvxCircularBuffer_extSynth

class CjvxGenericWrapperTechnology;

#define JVX_MY_BASE_CLASS_D  CjvxAudioDevice
#define JVX_ASYNC_BUFFER_DEBUG_CNT 1024

class local_proc_params
{
public:
	jvxInt32 chanshw_in = 0;
	jvxInt32 chanshw_out = 0;
	jvxInt32 chansfile_in = 0;
	jvxInt32 chansfile_out = 0;
	jvxInt32 chansext_in = 0;
	jvxInt32 chansext_out = 0;
	jvxInt32 chansdum_in= 0;
	jvxInt32 chansdum_out = 0;
	jvxInt32 chans_in = 0;
	jvxInt32 chans_out = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 buffersize = 0;
	jvxInt32 samplerate = 0;
	jvxBool setup = false;
};


class CjvxGenericWrapperDevice: public JVX_MY_BASE_CLASS_D, 
	public IjvxDataProcessor, 
	public genGenericWrapper_device
{

	friend CjvxGenericWrapperDevice_hostRelocator;
private:

#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
	typedef struct
	{
		jvxInt16 event; // 0: enter device cb; 1: bufferunderrun device cb; 2: leave device cb; 3: start sw processing; 4: no data sw processing; 5: stop sw processing
		jvxInt64 tStamp;
	} oneDbgMeasureEvent;
#endif

	enum class jvxLastOperationChannels
	{
		JVXLASTOPERATIONCHANNELS_OUTPUT,
		JVXLASTOPERATIONCHANNELS_INPUT
	};

	enum class jvxStatePrepapration
	{
		JVX_GENERIC_WRAPPER_PREPARE_STATUS_INIT_BF = 0x1,
		JVX_GENERIC_WRAPPER_CHANNELS_COUNT_COMPLETE_BF = 0x2,
		JVX_GENERIC_WRAPPER_LEVELS_ASSOCIATED_BF = 0x4,

		JVX_GENERIC_WRAPPER_PROCESSING_MODES_SET_BF = 0x8,
		JVX_GENERIC_WRAPPER_NUMBER_BUFFERS_SET_BF = 0x10,
		JVX_GENERIC_WRAPPER_PREPARE_STATUS_LINK_TYPE_PREPARED_BF = 0x20,

		JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_OUTPUT_COMPLETE_BF = 0x40,

		JVX_GENERIC_WRAPPER_OUTPUTFILES_COMPLETE_BF = 0x80,
		JVX_GENERIC_WRAPPER_OUTPUTREFERENCES_COMPLETE_BF = 0x100,
		JVX_GENERIC_WRAPPER_OUTPUTDUMMIES_COMPLETE_BF = 0x200,
		JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_COMPLETE_BF = 0x400,
		JVX_GENERIC_WRAPPER_OUTPUT_BUFFERS_LINKED_BF = 0x800,
		
		JVX_GENERIC_WRAPPER_LINK_DATA_SENDER_TO_RECEIVER_ALLOCATED_BF = 0x1000,

		JVX_GENERIC_WRAPPER_SHORTCUTS_LINKMAPPER_INPUT_COMPLETE_BF = 0x2000,

		JVX_GENERIC_WRAPPER_INPUTFILES_COMPLETE_BF = 0x4000,
		JVX_GENERIC_WRAPPER_INPUTREFERENCES_COMPLETE_BF = 0x8000,
		JVX_GENERIC_WRAPPER_INPUTDUMMIES_COMPLETE_BF = 0x10000,
		JVX_GENERIC_WRAPPER_INPUT_BUFFERS_COMPLETE_BF = 0x20000,
		JVX_GENERIC_WRAPPER_INPUT_BUFFERS_LINKED_BF = 0x40000,

		JVX_GENERIC_WRAPPER_TIMESTAMPS_PREPARED_BF = 0x80000,

		JVX_GENERIC_WRAPPER_SUBDEVICE_PREPARED_BF = 0x100000,
		JVX_GENERIC_WRAPPER_SETUP_COMPLETE_BF = 0x200000

	} ;

	typedef enum
	{
		RES_DACON_TB_PROC = 0,
		RES_DACON_PROC = 1,
		DACON_TB_PROC = 2,
		DACON_PROC = 3,
		RES_TB_PROC = 4,
		RES_PROC = 5,
		TB_PROC = 6,
		PROC = 7,
		NOTHING = 8
	} operationSequence;

	typedef enum
	{
		SELECTION_FILEIO_MODE,
		SELECTION_SYNC_MODE,
		SELECTION_ASYNC_MODE
	} selectOperationMode;

	typedef enum
	{
		OPERATION_MODE_1, // no resampler, no data conversion
		OPERATION_MODE_2, // no resampler, data conversion
		OPERATION_MODE_3, // resampler, no data conversion
		OPERATION_MODE_4 // resampler, data conversion
	} operationMode;

	typedef enum
	{
		TIMEBASE_CHANGE, // Need circular buffer and extra thread
		NO_TIMEBASE_CHANGE // No need for circular buffer and extra thread
	} timebaseMode;

	typedef enum
	{
		TRIGGER_DEVICE,
		TRIGGER_INTERN,
	} clockMode;
	
	struct
	{
		IjvxDevice* connectedDevice;
		CjvxGenericWrapperTechnology* parentRef;
		CjvxGenericWrapperFiles* theFilesRef;
		CjvxGenericWrapperExtReferences* theExtRef;
		CjvxGenericWrapperDummyChannels* theDummyRef;
	} onInit;

	struct
	{
		CjvxGenericWrapperFiles theFiles;
		CjvxGenericWrapperExtReferences theExt;
		CjvxGenericWrapperDummyChannels theDummy;
	} dummyRefs;

	jvxLastOperationChannels lastOperationSetup = jvxLastOperationChannels::JVXLASTOPERATIONCHANNELS_OUTPUT;

	struct
	{
		struct
		{
			struct
			{
				jvxInt32 idInput;
				jvxInt32 idOutput;
			} files;
		} userSelection;

		struct
		{
			jvxSize modifyWeightCnt;
			std::vector<oneEntryChannelMapper> inputChannelMapper;
			std::vector<oneEntryChannelMapper> outputChannelMapper;

			std::vector<jvxSize> inputChannelMapper_hwrev;
			std::vector<jvxSize> outputChannelMapper_hwrev;
		} channelMappings;

		IjvxProperties* theProps;

		struct
		{
			struct
			{
				std::vector<jvxInt32> entries;
				jvxInt32 selection_id;
				jvxBool sellist_exists;
				jvxInt32 entry;
			} buffersize;

			struct
			{
				std::vector<jvxInt32> entries;
				jvxInt32 selection_id;
				jvxBool sellist_exists;
				jvxInt32 entry;
			} samplerate;

			struct
			{
				std::vector<jvxDataFormat> entries;
				jvxInt32 selection_id;
				jvxBool sellist_exists;
				jvxInt32 entry;
			} format;

			jvxInt32 latIn;
			jvxInt32 latOut;
		} props_hw;

		struct
		{

		} props_sw;

		struct
		{
			struct
			{
				jvxBool forceSyncMode;
				jvxInt32 numLoopsMax;
				jvxData disadvantageLoops;
				jvxData deviationFsMax;
				jvxBool autofs;
				jvxInt32 samplerateMaxAuto;
			} autosetup;

			struct
			{
				//jvxInt32 loadEstimate;
				jvxInt32 orderFilterResamplingInput;
				jvxInt32 orderFilterResamplingOutput;
			} resampler;
		} props_user;

		struct
		{
			struct
			{
				IjvxObject* object;
				IjvxThreadController* hdl;
			} coreHdl;

			jvxBool asyncAudioThreadAssoc;
			jvxInt32 asyncAudioThreadId;

		} threadCtrl;

		struct
		{
			// This variable should prevent loops if we set parameters on the connected device from withthe
			// generic wrapper
			jvxBool allParamsLocked;
		} lockParams;


#if defined( JVX_ASYNC_BUFFER_DEBUG_CNT)
		oneDbgMeasureEvent dbg_events[JVX_ASYNC_BUFFER_DEBUG_CNT];
		JVX_MUTEX_HANDLE dbg_safeAccess;
		jvxTimeStampData dbg_tstamp;
		jvxSize dbg_events_cnt;
#endif

	} runtime;

	struct
	{
		// Operation mode specifier
		operationMode operationDevice;
		timebaseMode timebaseDevice;
		clockMode clockDevice;
		struct
		{
			IjvxObject* theObj;
			IjvxResampler* theHdl;
		} resampler_proto;

		struct
		{
			// Computed parameters
			jvxInt32 bSize_hw;
			jvxInt32 sRate_hw;
			jvxDataFormat form_hw;
			jvxInt32 bSize_sw;
			jvxInt32 sRate_sw;
			jvxDataFormat form_sw;
			jvxData downsamplingFactor;
			jvxInt32 bSize_hw_ds;
			jvxData downSamplingFactor_real;
			jvxData sRate_sw_real;
			jvxBool dacon_required;
			jvxBool resampler_required;
			jvxBool timebasechange_required;
			jvxInt32 num_loops;
			jvxInt32 fillHeightInput;
			jvxInt32 fillHeightOutput;
			jvxData delayFilterResamplingInput;
			jvxData delayFilterResamplingOutput;
			jvxData qual_sc_factor;
		} computedParameters;

		struct
		{
			jvxData deltaT_max;
			jvxData deltaT_avrg;
			jvxData deltaT_theory;
			jvxData deltaT_Measure;
		} measureLoad;

		struct
		{
			jvxTimeStampData tStampRef;

			local_proc_params params_fixed_runtime;

			jvxBool fieldsLinkedOutput;
			jvxBool fieldsLinkedInput;

			jvxData deltaT_hw_avrg;
			jvxData deltaT_sw_avrg;
			
			jvxInt64 timeStamp_prev_hw;
			jvxInt64 timeStamp_prev_sw;

			struct
			{
				jvxLinkDataDescriptor myLinkContainer;
			}noDevice;

			jvxCBitField procFlags;

		} inProc;

		struct
		{
			//int numAudioThreadFailed;
		} performance;

		struct
		{
			jvxData* input_avrg;
			jvxData* input_max;
			jvxData* input_gain;
			jvxCBool* input_clip;
			jvxInt16* direct;
			jvxSize num_entries_input;

			jvxData* output_avrg;
			jvxData* output_max;
			jvxData* output_gain;
			jvxCBool* output_clip;
			jvxSize num_entries_output;

			jvxSize* input_channel_map;
			jvxSize* output_channel_map;
		} levels;

		// jvxStatePrepapration stateInPreparation;
		jvxUInt32 stateInPreparation = 0;
		jvxInt32 deltaWrite;

		jvxSize numBuffers_sender_to_receiver_hw;
		jvxSize numBuffers_sender_to_receiver_sw;

		jvxSize numBuffers_receiver_to_sender_hw;
		jvxSize numBuffers_receiver_to_sender_sw;
	} processingControl;

	struct
	{
		operationSequence seq_operation_in;
		operationSequence seq_operation_out;

		// ***************************************************
		IjvxResampler** resamplersIn_hdl;
		IjvxObject** resamplersIn_obj;
		IjvxDataConverter** convertersIn_hdl;
		IjvxObject** convertersIn_obj;
		CIRCBUFFER** circularBuffersIn;
		jvxByte*** ptrA_in_net;
		jvxByte*** ptrA_in_raw;
		jvxSize** ptrA_in_off;
		jvxSize ptrA_in_sz;
		jvxByte** ptrB_in;
		jvxByte** ptrC_in;
		//jvxSize* mapToOutput_in;
		// ***************************************************

		// ***************************************************
		IjvxResampler** resamplersOut_hdl;
		IjvxObject** resamplersOut_obj;
		IjvxDataConverter** convertersOut_hdl;
		IjvxObject** convertersOut_obj;
		CIRCBUFFER** circularBuffersOut;
		jvxByte*** ptrA_out_net;
		jvxByte*** ptrA_out_raw;
		jvxSize** ptrA_out_off;
		jvxSize ptrA_out_sz;
		jvxByte** ptrB_out;
		jvxByte** ptrC_out;
		//jvxSize* mapToInput_out;
		// ***************************************************


		struct
		{
			jvxInt64* buffer;
			jvxInt32 idxRead;
			jvxInt32 fillHeight;
			jvxInt32 length;
			jvxData durOneSample_us_dbl;
		} forTimeStamps;

		/*
		chansMappers* inChansMapper;
		chansMappers* outChansMapper;
		*/

		/*
		char** buf_ptr_B1_in;
		char** buf_ptr_B1_out;

		char** buf_ptr_B2_in;
		char** buf_ptr_B2_out;

		char** buf_ptr_B3_in;
		char** buf_ptr_B3_out;

		char** buf_ptr_Btb_in;
		char** buf_ptr_Btb_out;

		char** container_sw_in;
		char** container_sw_out;
		*/



		size_t szElementSW;
		size_t szElementHW;

		selectOperationMode doSelection;
		jvxFileOperationType fileOperationMode;

		struct
		{
			jvxHandle* thread_handle;
			jvx_thread_handler thread_handler;

			bool endTriggered;
		} threads;

	} proc_fields;

	JVX_MUTEX_HANDLE safeSettings;
	//JVX_MUTEX_HANDLE saveAccessCircBuffers;

	CjvxGenericWrapperDevice_hostRelocator theRelocator;

	jvxSize channelSelectIndex = JVX_SIZE_UNSELECTED;
	jvxSize uIdMain = JVX_SIZE_UNSELECTED;

public:

	/* ========================================*/
	/* Constructor and desctuctor */
	/* ========================================*/

	CjvxGenericWrapperDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual ~CjvxGenericWrapperDevice();

	/* ========================================*/
	/* Hidden interfaces */
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	/* ========================================*/
	/* State machine member functions */
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_location_info(const jvxComponentIdentification& tp) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;

	/* ========================================*/
	/* Property member functions */
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION number_properties(jvxCallManagerProperties& callGate, jvxSize* num)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION description_property(jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
		const jvx::propertyAddress::IjvxPropertyAddress& idProp)override;

	/*virtual jvxErrorType JVX_CALLINGCONVENTION description_property(
		jvxCallManagerProperties& callGate,
		jvxSize idx, jvxPropertyCategoryType* category, jvxUInt64* settype,
		jvxUInt64* stateAccess, jvxDataFormat* format, 
		jvxSize* num, jvxPropertyAccessType* accessType, jvxPropertyDecoderHintType* decTp, jvxSize* globalIdx,
		jvxPropertyContext* ctxt, jvxApiString* name, jvxApiString* description, jvxApiString* descriptor, jvxBool * isValid,
		jvxFlagTag* accessFlags
		)override;
		*/
	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans
	)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	jvxErrorType get_property_extended_info(
			jvxCallManagerProperties& callGate,
			jvxHandle* fld, jvxSize requestId,
			const jvx::propertyAddress::IjvxPropertyAddress& ident)override;

	jvxErrorType forwardPropertyRequest(
		jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor* descr,
		const jvx::propertyAddress::IjvxPropertyAddress& idProp,
		std::function<jvxErrorType(IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& ident)> func,
		std::function<void(jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dnms,
			jvx::propertyDescriptor::CjvxPropertyDescriptorMin* nmin)> funcSuccess);


	virtual jvxErrorType JVX_CALLINGCONVENTION request_takeover_property(jvxCallManagerProperties& callGate,
		const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent, jvxPropertyCategoryType cat, jvxSize propId)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_group(jvxCallManagerProperties& callGate) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_property_group(jvxCallManagerProperties& callGate) override;

	/* ========================================*/
	/* Processing member functions */
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData) override
	{
		assert(0);
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData )override
	{
		assert(0);
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;

	// ==> Default implementations

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_object(IjvxObject** refObject)override
	{
		if (refObject)
		{
			*refObject = static_cast<IjvxObject*>(this);
		}
		return JVX_NO_ERROR;
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_multithreading(jvxBool* supports) override
	{
		if (supports)
			*supports = false;

		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender,
		jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_in, jvxInt32 offset_out, jvxInt32 numEntries) override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData) override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	/* ========================================*/
	/* Configuration member functions */
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class(jvxBitField* ft)override;

	/* ========================================*/
	/* ========================================*/
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION select_master(IjvxDataConnectionProcess* ref) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_master() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION associated_process(IjvxDataConnectionProcess** ref) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain_master(const jvxChainConnectArguments& args,
		const jvxLinkDataDescriptor_con_params* i_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain_master(const jvxChainConnectArguments& args 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	/* ========================================*/
	/* ========================================*/
	/* ========================================*/

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	/* ========================================*/
	virtual jvxErrorType JVX_CALLINGCONVENTION iterator_chain(IjvxConnectionIterator** it) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;

	jvxErrorType number_next_x(jvxSize* num);
	jvxErrorType reference_next_x(jvxSize idx, IjvxConnectionIterator** next);

	jvxErrorType prepare_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType postprocess_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType test_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType test_connect_icon_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	//! Set the start values when alocating devices
	jvxErrorType setDeviceReferences(IjvxDevice* theDevice, CjvxGenericWrapperTechnology* theParent, jvxDeviceCapabilities caps, const std::string& name);
	jvxErrorType deviceReference(IjvxDevice** theDevice);

	//! Add input file
	//jvxErrorType addInputFile_nolock(std::string name);
	jvxErrorType removeInputFile_nolock(jvxSize idx);
	//jvxErrorType addOutputFile_nolock(std::string name);
	jvxErrorType removeOutputFile_nolock(jvxSize idx);

	jvxErrorType addChannel_output_nolock(jvxSize idAdd, jvxInt32 uniqueId, std::string& descr, std::string& fName, jvxGenericWrapperChannelType tp);
	jvxErrorType remChannel_output_nolock(jvxSize idRemove, jvxInt32 uniqueId, jvxGenericWrapperChannelType tp);
	jvxErrorType remChannel_output_id_nolock(jvxInt32 uniqueId, jvxGenericWrapperChannelType tp);
	jvxErrorType renameChannel_output_name_nolock(jvxInt32 uniqueId, std::string newFriendlyName, jvxGenericWrapperChannelType tp);

	jvxErrorType addChannel_input_nolock(jvxSize idAdd, jvxInt32 uniqueId, std::string& descr, std::string& fName, jvxGenericWrapperChannelType tp);
	jvxErrorType remChannel_input_nolock(jvxSize idRemove, jvxInt32 uniqueId, jvxGenericWrapperChannelType tp);
	jvxErrorType remChannel_input_id_nolock(jvxInt32 uniqueId, jvxGenericWrapperChannelType tp);

	jvxErrorType updateResamplerInputProperties_nolock();
	jvxErrorType updateResamplerOutputProperties_nolock();

	jvxErrorType rearrangeChannelMapper_noLock();

	//jvxErrorType updateChannels_lock();
	jvxErrorType updateChannelMapperWithFiles_nolock();

	void rereadChannelStatusHw_noLock();

	void updateChannelExposure_nolock();
	void updateChannelInternal_nolock();

	void pushProperties_lock();

	void pullProperty_buffersize_nolock();
	void pullProperty_samplerate_nolock();
	void pullProperty_format_nolock();
	void pullProperty_stopsilence_nolock();

	void pushProperty_buffersize_nolock();
	void pushProperty_samplerate_nolock();
	void pushProperty_format_nolock();

	jvxErrorType updateChannelMapper_ActiveSelection_noLock();

	// Lock settings
	void lock_settings(){JVX_LOCK_MUTEX(safeSettings);};

	// Unlock settings
	void unlock_settings(){JVX_UNLOCK_MUTEX(safeSettings);};

	void updateDependentVariables_lock(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);

	void updateSWSamplerateAndBuffersize_nolock(jvxLinkDataDescriptor* fromDevice, jvxLinkDataDescriptor* toDevice JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	void pass_channelsetup_nolock(jvxBool inputChannels = true, jvxBool outputChannels = true);

	void numberActiveHardwareChannelsInput(jvxInt32& num);
	void numberActiveHardwareChannelsOutput(jvxInt32& num);
	void findOptimalSetupBSizeSRate();

	void reportUpdateAllPropertiesInput();
	void reportUpdateAllPropertiesOutput();

	// Functions to evaluate the current setting. One instance of the local proc parameters
	// is set on prepare and remains until postprocess
	void populate_countchannels_datatype(local_proc_params& params);
	void release_countchannels_datatype(local_proc_params& params);

	/* Functions to prepare parts of the overall prepare processing functionality */
	jvxErrorType prepare_outputfiles();
	jvxErrorType prepare_outputreferences();
	jvxErrorType prepare_inputfiles();
	jvxErrorType prepare_inputreferences();
	jvxErrorType prepare_detectmode();
	jvxErrorType prepare_preplinkmapper_input(jvxLinkDataDescriptor* theData);
	jvxErrorType prepare_preplinkmapper_output(jvxLinkDataDescriptor* theData);
	jvxErrorType prepare_timestamps();
	jvxErrorType prepare_linkdata(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType prepare_inputprocessing();
	jvxErrorType prepare_outputprocessing(jvxSize prepend, jvxSize append, jvxSize align);
	jvxErrorType prepare_inputDummies();
	jvxErrorType prepare_outputDummies();

	jvxErrorType postprocess_outputfiles();
	jvxErrorType postprocess_outputreferences();
	jvxErrorType postprocess_inputfiles();
	jvxErrorType postprocess_inputreferences();
	jvxErrorType postprocess_detectmode();
	jvxErrorType postprocess_preplinkmapper_input();
	jvxErrorType postprocess_preplinkmapper_output();
	jvxErrorType postprocess_timestamps();
	jvxErrorType postprocess_linkdata(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType postprocess_inputprocessing();
	jvxErrorType postprocess_outputprocessing();
	jvxErrorType postprocess_inputDummies();
	jvxErrorType postprocess_outputDummies();

	/* Process functions */
	jvxErrorType process_sync(jvxSize mt_mask, jvxSize idx_stage);
	jvxErrorType process_async(jvxSize mt_mask, jvxSize idx_stage);
	jvxErrorType iclass_process_async(jvxInt64 timestamp_us);

	static jvxErrorType static_process_async(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);

    void printout_config(IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, std::string prefix);

	jvxErrorType buildPropertiesSubDevice_noLock();
	jvxErrorType rebuildPropertiesSubDevice_noLock();

	void allocateBufferA();
	void allocateBufferA_out(jvxSize prepend, jvxSize append, jvxSize align);

	void deallocateBufferA();
	void deallocateBufferA_out();

	void addPropertyChanged(CjvxProperties* props);

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_channel_gain);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_channel_gain);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_channel_index);

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_device_cap_avail);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_device_sslot);

#include "codeFragments/simplify/jvxInputOutputConnectorX_simplify.h"
	
	//void reconstructExposedParameters();
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif
