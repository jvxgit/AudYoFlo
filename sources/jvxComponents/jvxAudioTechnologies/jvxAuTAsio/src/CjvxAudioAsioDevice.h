#ifndef __CJVXAUDIOASIODEVICE_H__
#define __CJVXAUDIOASIODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "ginclude.h"
#include "asio.h"
#include "asiolist.h"
#include "iasiodrv.h"

#include "pcg_exports_device.h"

#define ALPHA_SMOOTH_DELTA_T 0.995

//#define DOUBLE_BUFFERING_PIPELINE_OUTPUT 5
//#define DOUBLE_BUFFERING_PIPELINE_INPUT 3

class CjvxAudioAsioDevice: 
	public CjvxAudioDevice,  
	public genAsio_device
{
	/*
	typedef struct
	{
		std::string name;
		bool isSelected;
	} oneChannel;
	*/
private:
	struct
	{
		std::string dllPath;
		jvxSize id;
		CLSID clsid;
		std::string description;
		bool selectionOpenControlPanel;
	}my_props;

	struct
	{
		IASIO* ptrToDriver;

		std::string nameDriver;
		int versionDriver;

		ASIOSampleType typeSamples;
		int number_bits_sample;
		int bytes_space_sample;

		long minSizeBuffers;
		long maxSizeBuffers;
		long preferredSizeBuffers;
		long granBuffers;

		std::vector<long> sizesBuffers;

		std::vector<long> samplerates;

		std::vector<ASIOClockSource> theClockSources;

		bool requiresOutputReady;
		bool available;

		struct
		{
			IjvxToolsHost* theToolsHost;
			struct
			{
				IjvxObject* object;
				IjvxThreadController* hdl;
			} coreHdl;

			jvxBool syncAudioThreadAssoc;
			jvxInt32 syncAudioThreadId;
		} threads;
		std::vector<jvxInt32> usefulBSizes;

		struct
		{
			jvxSize idxInput;
		} buffer_idx;
	} runtime;

	struct
	{
		struct
		{
			jvxInt32 numberChannelsUsed;
			jvxHandle** ptrsHW;
		} input;

		struct
		{
			jvxInt32 numberChannelsUsed;
			jvxHandle** ptrsHW;
		} output;

		jvxInt16 bSwitchId;

		ASIOBufferInfo* asioBufferField;

		//jvxLinkDataDescriptor theData_to_queue;
		//jvxLinkDataDescriptor* theData_from_queue;

		jvxTimeStampData theTimestamp;
		jvxInt64 timestamp_previous;
		jvxData deltaT_average_us;
		jvxData deltaT_theory_us;

#ifdef DOUBLE_BUFFERING_PIPELINE
		jvxSize bufferIdx;
#endif
		struct
		{
			jvxState stateEnd;
			jvxSize countEnd;
			jvxCBool active;
			jvxData period;
		} ssilence;
	}inProcessing;

public:

	JVX_CALLINGCONVENTION CjvxAudioAsioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioAsioDevice();

	// ===================================================================================
	// ===================================================================================
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;

	// ===================================================================================
	// ===================================================================================
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans) override;

	// ===================================================================================
	// ===================================================================================
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================
	// ===================================================================================
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================
	// ===================================================================================
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage , 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	// Those two not implemented
	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);
	jvxErrorType activateAsioProperties();
	jvxErrorType deactivateAsioProperties();
	jvxErrorType prepareAsio();
	jvxErrorType startAsio();
	jvxErrorType stopAsio();
	jvxErrorType postProcessAsio();
	void resetAsio();
	void bufferSwitch(long doubleBufferIndex, ASIOBool directProcess);
	ASIOTime* bufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
	void samplerateDidChange(ASIOSampleRate sRate);
	long asioMessage(long selector, long value, void* message, double* opt);

	void props_init(const char* dllPath, jvxSize id)
	{
		my_props.dllPath = dllPath;
		my_props.id = id;
	};
	void props_clsid(CLSID clsid)
	{
		my_props.clsid = clsid;
	};
	void props_description(std::string description)
	{
		my_props.description = description;
	};

	/*
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_SELECTOR
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxAudioDevice
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_SELECTOR
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
	*/
};

#endif
