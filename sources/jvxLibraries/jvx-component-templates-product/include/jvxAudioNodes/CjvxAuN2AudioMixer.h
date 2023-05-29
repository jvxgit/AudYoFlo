#ifndef __CFCAUN2AUDIOMIXER_H__
#define __CFCAUN2AUDIOMIXER_H__

#include "jvx.h"
#include "jvxNodes/CjvxNVTasks.h"
#include "pcg_CjvxAuN2AudioMixer.h"

// #define JVX_AUDIOMIXER_DEBUG_SPECIFIC

class CjvxChannelSpecificAttach
{
public:
	jvxHandle* specReference = nullptr;
};


JVX_INTERFACE CjvxAuN2AudioMixer_channelExtender
{
public:
	// Docs for shared pointer:
	// https://medium.com/pranayaggarwal25/custom-deleters-with-shared-ptr-and-unique-ptr-524bb7bd7262
	// 
	virtual ~CjvxAuN2AudioMixer_channelExtender() {};
	virtual std::shared_ptr<CjvxChannelSpecificAttach> allocateAttachChannelSpecific(jvxBool isInput) = 0;
	//virtual void deallocateAttachChannelSpecific(jvxHandle* attached, jvxBool isInput) = 0;
	virtual void updateChannelStorage_attached(jvxBool isInput, std::shared_ptr<CjvxChannelSpecificAttach>& to, const std::shared_ptr<CjvxChannelSpecificAttach>& from) = 0;
	virtual void deleteLinearFields(jvxBool isInput) = 0;
	virtual void reallocateLinearFields(jvxBool isInput, jvxSize lenField) = 0;
	virtual void fillLinearFields(jvxBool isInput, jvxSize cnt, std::shared_ptr<CjvxChannelSpecificAttach>&) = 0;
	virtual void propUpdateLinearFields(CjvxProperties* props, jvxBool isInput) = 0;
	virtual void putConfiguration_attached(jvxBool isInput, std::shared_ptr<CjvxChannelSpecificAttach>&, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const std::string& prefix) = 0;
	virtual void getConfiguration_attached(jvxBool isInput, std::shared_ptr<CjvxChannelSpecificAttach>&, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const std::string& prefix) = 0;
	virtual void reportPropertyUpdateChannels(jvxBool isInput) = 0;
};

class CjvxAuN2AudioMixer : public CjvxNVTasks, public IjvxDirectMixerControl, public gen2AudioMixer_node
{
protected:
	// jvxSize mixBufferChannels_fixed = 2; // <- This implementation always uses stereo buffering

	/* Direct external control of the mixer */
	IjvxDirectMixerControlClient* theDirectMixer_ctrl_clnt = nullptr;
	CjvxAuN2AudioMixer_channelExtender* extender = nullptr;

	class oneEntryChannel
	{
	public:
		oneEntryChannel()
		{
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
			std::cout << "Alloc" << std::endl;
#endif
		}
		~oneEntryChannel()
		{
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
			assert (attSpecificPtr == nullptr);
			std::cout << "Dealloc" << std::endl;
#endif
		}

		std::string name;
		jvxData avrg = 0;
		jvxData max = 0;
		jvxData gain = 1.0;
		jvxSize cntSamplesSinceMax = 0;
		jvxCBool mute = c_false;
		jvxCBool clip = c_false;
		std::shared_ptr<CjvxChannelSpecificAttach> attSpecificPtr;
	};

	class oneEntryProcessChannelList
	{
	public:
		std::list<oneEntryChannel> channels;
		jvxSize uIdConn = JVX_SIZE_UNSELECTED;
		std::string masName = "not-defined";
	};

	class oneProfileParameters
	{
	public:
		std::map<std::string, std::list<oneEntryChannel> > inputChannelsInStorage;
		std::map<std::string, std::list<oneEntryChannel> > outputChannelsInStorage;
	};

	std::map<jvxSize, oneEntryProcessChannelList> registeredChannelListInput;
	std::map<jvxSize, oneEntryProcessChannelList> registeredChannelListOutput;

	std::map<std::string, std::list<oneEntryChannel> > inputChannelsInStorage;
	std::map<std::string, std::list<oneEntryChannel> > outputChannelsInStorage;

	std::map <std::string, oneProfileParameters> profileList;

	class mixer_flds
	{
	public:
		jvxData* fldAvrg = nullptr;
		jvxData* fldMax = nullptr;
		jvxData* fldGain = nullptr;
		jvxBitField* fldMute = nullptr;
		jvxCBool* fldClip = nullptr;
		jvxSize lenField = 0;
	};

	mixer_flds mixer_input;
	mixer_flds mixer_output;

	// ==============================================================================
	// =========================================================================================

	class oneBufferDefinition
	{
	public:
		jvxSize numChannels_fixed = JVX_SIZE_UNSELECTED;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		std::string description;
		jvxSize id = JVX_SIZE_UNSELECTED;
		jvxBool allocatOnPrepare = true;

		jvxBool ready = false;
		jvxSize bufSize = 0;
		jvxSize szElm = 0;
		jvxSize szElmsChannel = 0;
		jvxSize numChannels = JVX_SIZE_UNSELECTED;

		jvxHandle** mixBuffer = nullptr;
	};

	// This list may also be changed during operation to add additional mix buffers.
	// However, make sure that you do not encounter race conditions:
	// Please, always use the functions 
	// <safe_callback_start_connection> and <safe_callback_stop_connection> if you need to
	// add a mixbuffer with a new connection OR use the lock <_common_set_nv_proc::safeAcces_proc_tasks>
	// from base class <CjvxNVTasks> when extending the mixbuffer list!
	std::map<jvxSize, oneBufferDefinition> mixBuffers;

private:

	std::map<jvxSize, oneEntryProcessingVTask>::iterator theGlobalIterator;

public:

	JVX_CALLINGCONVENTION CjvxAuN2AudioMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAuN2AudioMixer();
	
	// =============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// =============================================================================

	virtual jvxErrorType terminate() override;

	// =============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask)override;

	// =========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// =============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType test_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual void test_set_output_parameters(jvxSize ctxtId, jvxSize ctxtSubId, jvxLinkDataDescriptor* theDataOut) override;

	// =============================================================================

	//jvxErrorType attach_input_channel_stage(const jvxLinkDataDescriptor& dat);
	//jvxErrorType detach_input_channel_stage(const jvxLinkDataDescriptor& dat);

	jvxErrorType update_channels_on_test(const jvxLinkDataDescriptor* datIn, const jvxLinkDataDescriptor* datOut);
	void new_setup_to_properties(jvxBool inputSide, jvxBool outputSide);

	void updateChannelFromStorage(std::map<std::string, std::list<oneEntryChannel>>& channelsInStorage, oneEntryChannel& newEntry, std::string masName, jvxBool isInput);
	void updateChannelToStorage(std::map<std::string, std::list<oneEntryChannel>>& channelsInStorage, std::string masName, const std::list<oneEntryChannel>& channels_old, jvxBool isInput);

	void update_profile_list_properties(const std::string& activateThis = "");

#ifdef JVX_FERNLIVE_DEBUG_TEST
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_ntask(
		jvxLinkDataDescriptor* theData_in, 
		jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, 
		CjvxOutputConnectorNtask* refto
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType test_connect_icon_vtask(
		jvxSize idTask,
		jvxSize subIdTask
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
#endif

	// =============================================================================


	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	void addChannelsStorageConfig(IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections, const std::string& prefix,
		std::map<std::string, std::list<oneEntryChannel> >& channelsInStorage,
		jvxBool isInput);

	void getChannelsStorageConfig(IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const std::string& prefix,
		std::map<std::string, std::list<oneEntryChannel> >& channelsInStorage,
		jvxBool isInput);
	// =============================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
			CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage) override;

	/*
	virtual jvxErrorType process_buffers_icon_nvtask_master(
			jvxLinkDataDescriptor* theData_in,
			jvxLinkDataDescriptor* theData_out,
			jvxSize mt_mask, jvxSize idx_stage);
			*/
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_vtask(
		jvxSize mt_mask, jvxSize idx_stage, jvxSize procId) override;

	// =============================================================================

	// Two simple interface functions for modified mixing
	virtual void mix_data_to_mix_buffers(jvxSize uidconn, const oneBufferDefinition& mixBuffer,
		jvxLinkDataDescriptor* theData_in, jvxSize idx_stage,
		jvxSize toOffset = 0, jvxSize fromOffset = 0, jvxSize toNumberLimit = JVX_SIZE_UNSELECTED);

	virtual void copy_data_from_mix_buffers(jvxSize uidconn, const oneBufferDefinition& mixBuffer, jvxLinkDataDescriptor* data_out);
	
	// =============================================================================

	void recursive_vtask_processing();

	// =============================================================================
	// =============================================================================
	// Property callbacks
	// =============================================================================

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_extend_ifx_reference);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_level_get);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_level_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(clear_storage);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(address_profiles);

};

#endif
