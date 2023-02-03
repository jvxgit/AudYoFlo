#ifndef __CJVXSPASYNCIO_H__
#define __CJVXSPASYNCIO_H__

#include "jvx.h"
#include "jvxNodes/CjvxBareNtask.h"

#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"

#include "pcg_exports_node.h"

extern "C"
{
#include "jvx_audio_stack_sample_dispenser_cont.h"
#include "jvx_buffer_estimation/jvx_estimate_buffer_fillheight.h"
}

#define JVX_NTASK_ENABLE_LOGFILE

#ifdef JVX_NTASK_ENABLE_LOGFILE
#include "jvxNodes/Cjvx_logfile_core.h"
#define JVX_NUMBER_ENTRIES_TIMING 8
#define JVX_ID_PRIM_AUDIO 1
#define JVX_ID_SEC_AUDIO 2
#endif

#ifdef JVX_ASYNCIO_MASTER_PRI
#define JVX_ASYNCIO_CLASSNAME CjvxSPAsyncIo_pm
#else
#define JVX_ASYNCIO_CLASSNAME CjvxSPAsyncIo_sm
#endif

/*
 * This module is supposed to operate as follows:
 * -- any other chain --> master (device) --> <default ocon> ----------------------------------------------->      <default icon> node <default ocon>         ---------> <default icon> output
 *       (AOC)                                                                                         |-> <async-secondary icon> node <async-secondary ocon> -|
 *                                 seconary chain -> master (CjvxSPAsyncIo) --> <async-secondary ocon> -                                                       -> <async-secondary icon> output
 *                                      (SC)           |                                                                                                                                    |
 * -- pri chain --> <default icon> CjvxSPAsyncIo -> BUFFER -------------------------------------------------------------------------------------------------------------------------------BUFFER --> output
 *      (PC)
 * 
 * Update Abhängigkeiten:
 * <samplerate, buffersize, format, subformat, segx, segy> -> AOC::node::test_connect_icon -> request update CjvxSPAsyncIo::SC, SC: set preferred parameters
 * SC:test_connect_icon: accept only preferred parameters (slave mode)
 * <num in channels> -> PC::CjvxSPAsyncIo::test_connect_icon ->  request update CjvxSPAsyncIo::SC, SC: set preferred parameters 
 * SC::CjvxSPAsyncIo::transfer_backward_ocon_id -> request update CjvxSPAsyncIo::PC
 *
 */
class JVX_ASYNCIO_CLASSNAME:
	public CjvxBareNtask, public IjvxManipulate,
	public IjvxConnectionMaster, public CjvxConnectionMaster,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory,
	public genSPAsyncio_node
{
private:	

	enum class autoConfigurationMode
	{
		JVX_ASYNC_MODE_PRIMARY_MASTER,
		JVX_ASYNC_MODE_SECONDARY_MASTER
	};

	IjvxInputConnector* sec_input = nullptr;
	IjvxOutputConnector* sec_output = nullptr;

	IjvxInputConnector* pri_input = nullptr;
	IjvxOutputConnector* pri_output = nullptr;

	autoConfigurationMode auto_mode = autoConfigurationMode::JVX_ASYNC_MODE_PRIMARY_MASTER;

	CjvxNegotiate_input neg_secondary_input ;
	CjvxNegotiate_output neg_secondary_output;

	/*
	jvxCommonSetNodeParams _common_set_node_params_sec;
	*/

	buffer_cfg cfg_sec_buffer;
	jvxBool sec_zeroCopyBuffering_rt = false;

	struct
	{
		struct
		{
			//jvxCBitField stateFlags;
			JVX_MUTEX_HANDLE safeAccessSecLink;
			jvxBool secLinkRead;
			jvxSize numChannelsPrimary;
			jvxSize numChannelsSecondary;
			jvxSize buffersizePrimary;
			jvxSize buffersizeSecondary;
			jvxSize ratePrimary;
			jvxSize rateSecondary;
			jvxDataFormat formatPrimary;
			jvxDataFormat formatSecondary;

			jvxBool bothSidesStarted;
			jvxAudioStackDispenserCont myAudioDispenser;
			
			JVX_MUTEX_HANDLE safeAccess_lock; // <- lock used in audio sample disposer
			jvxHandle** work_buffers_from_hw;
			jvxHandle** work_buffers_to_hw;
		} crossThreadTransfer;

		struct 
		{
			jvxHandle* fHeightEstimator;
			jvxSize numberEventsConsidered_perMinMaxSection;
			jvxSize num_MinMaxSections;
			jvxData recSmoothFactor;
			jvxSize numOperations;
		} fHeight;

		jvxAudioStackDispenserProfile prof_prim;
		jvxAudioStackDispenserProfile prof_sec;
		jvxTimeStampData tstamp;
		jvxBool tstamp_init = false;

		std::list<jvxLinkDataAttached_oneFrameLostElement> attachedData;
		jvxSize cntLost;
	} inProcessing;

#ifdef JVX_NTASK_ENABLE_LOGFILE
	Cjvx_logfile_core logWriter_main_signal;
	Cjvx_logfile_core logWriter_timing;
	jvxSize idMainIn = JVX_SIZE_UNSELECTED;
	jvxSize idMainOut = JVX_SIZE_UNSELECTED;
	jvxSize idTiming = JVX_SIZE_UNSELECTED;
	jvxSize entries_timing[JVX_NUMBER_ENTRIES_TIMING] = { 0 };
#endif
	IjvxDataConnectionCommon* ctxtMain = nullptr;
	IjvxDataConnectionCommon* ctxtSec = nullptr;
public:

	JVX_CALLINGCONVENTION JVX_ASYNCIO_CLASSNAME(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~JVX_ASYNCIO_CLASSNAME();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType prepare() override;
	virtual jvxErrorType postprocess() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_manipulate_value(jvxSize id, jvxVariant* varray)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_manipulate_value(jvxSize id, jvxVariant* varray)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,	
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, 
		jvxSize idTask)override;

	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out,
			jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	jvxErrorType transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_in,
			jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	jvxErrorType prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// =====================================================================
	// Master interface: Default implementation requires to find current settings
	// =====================================================================
#define JVX_CONNECTION_MASTER_SKIP_CONNECT
#define JVX_CONNECTION_MASTER_SKIP_TEST
#define JVX_CONNECTION_MASTER_SKIP_PREPARE
#define JVX_CONNECTION_MASTER_SKIP_START
#define JVX_CONNECTION_MASTER_SKIP_STOP
#define JVX_CONNECTION_MASTER_SKIP_POSTPROCESS
#define JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR sec_output
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_POSTPROCESS
#undef JVX_CONNECTION_MASTER_SKIP_STOP
#undef JVX_CONNECTION_MASTER_SKIP_START
#undef JVX_CONNECTION_MASTER_SKIP_PREPARE
#undef JVX_CONNECTION_MASTER_SKIP_TEST
#undef JVX_CONNECTION_MASTER_SKIP_CONNECT

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain_master(const jvxChainConnectArguments& args,
		const jvxLinkDataDescriptor_con_params* JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================

	jvxErrorType process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage) override;

	// =====================================================================================
	// =====================================================================================

	jvxErrorType startAudioSync_nolock(jvxSize buffersize, jvxSize srate, jvxDataFormat form, jvxSize numChannelsIn, jvxSize numChannelsOut);

	jvxErrorType stopAudioSync_nolock();
	
	jvxErrorType audio_io_software(jvxHandle** bufs_to_sw, jvxHandle** bufs_from_sw, jvxSize bsize);
	
	jvxErrorType audio_io_hardware(jvxHandle** bufs_from_hw, jvxHandle** bufs_to_hw, jvxSize bsize);

	static jvxDspBaseErrorType lock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType lock_sync_threads_ic();
	static jvxDspBaseErrorType unlock_sync_threads_cb(jvxHandle* prv);
	jvxDspBaseErrorType unlock_sync_threads_ic();

	//void constraint_parameters_primary_chain(const jvxLinkDataDescriptor* ld, jvxBool* requires_update_chain);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_control_flow_param);

	JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DECLARE(release_attached);

#ifdef JVX_NTASK_ENABLE_LOGFILE

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans) override;
#endif
};

#endif
