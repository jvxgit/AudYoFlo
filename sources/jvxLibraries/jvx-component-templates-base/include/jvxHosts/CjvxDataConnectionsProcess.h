#ifndef __CJVXDATACONNECTIONPROCESS_H__
#define __CJVXDATACONNECTIONPROCESS_H__

#include "jvx.h"
#include "jvxHosts/CjvxDataConnectionsCommon.h"
#include "common/CjvxProperties.h"
#include "pcg_CjvxDataConnectionProcess_pcg.h"

// ====================================================================================================

class CjvxDataConnectionsProcess: public IjvxDataConnectionProcess, public CjvxDataConnectionCommon,
	public IjvxConfiguration, public IjvxProperties, public CjvxProperties, public IjvxDataChainInterceptorReport,
	public CjvxDataConnectionProcess_genpcg
{
protected:
	
	class _common_set_conn_proc_t
	{
	public:
		IjvxConnectionMaster* associated_master;
		jvxBool lastTestOk;
		jvxBool lastCheckReady;
		jvxBool test_on_connect;
		const jvxLinkDataDescriptor_con_params* init_params_ptr = nullptr;
		jvxLinkDataDescriptor_con_params init_params;
		jvxBool marked_essential_start;

		// The ID indicates that this process depends on another process, e.g., if a chain is part of another one
		jvxSize id_depends_on = JVX_SIZE_UNSELECTED;

		// This flag indicates that processes requested to test the chain may forward this request to another process 
		// this process depends on. This feature was introduced to run the full chain test function even if an element of the 
		// involved chain is from within a fixed connection chain
		jvxBool fwd_test_depends = false;

		jvxSize catId = JVX_SIZE_UNSELECTED;
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(fdb);
	};
	
	_common_set_conn_proc_t _common_set_conn_proc;

	jvxTimeStampData tStampRun;
	jvxTick timestamp_start_us;

	jvxInt64* timestamp_buffer_start_enter;
	jvxInt64* timestamp_buffer_start_leave;
	jvxInt64* timestamp_buffer_process_enter;
	jvxInt64* timestamp_buffer_process_leave;
	jvxInt64* timestamp_buffer_stop_enter;
	jvxInt64* timestamp_buffer_stop_leave;
	jvxBool log_data;
	jvxSize log_num_channels;
	jvxSize log_chans;
	jvxBool tagFromMaster = true;

public:

	CjvxDataConnectionsProcess(
		CjvxDataConnections* parentArg,
		const std::string& nm, 
		jvxSize unique_id_system, 
		jvxBool interceptors, 
		jvxBool essential_for_start,
		jvxBool verbose_out,
		jvxBool report_global,
		jvxSize idProcDepends,
		jvxBool tagNameFromMaster = true);
	~CjvxDataConnectionsProcess();

	void set_name_unique_descriptor(IjvxCallProt* fdb)override;
	
#include "codeFragments/simplify/jvxDataConnectionCommon_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION set_category_id(jvxSize catId) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION category_id(jvxSize* catId) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION depends_on_process(jvxSize* uIdProcess) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION create_bridge(IjvxOutputConnectorSelect* conn_from, 
		IjvxInputConnectorSelect* conn_to, const char* nm, jvxSize* unique_id, jvxBool dedicatedThread, 
		jvxBool boostThread, jvxSize oconIdTrigger, jvxSize iconIdTrigger)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connection_master_factory_is_involved(IjvxConnectionMasterFactory* and_this)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION associated_master(IjvxConnectionMaster** theMasterPtr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION associate_master(IjvxConnectionMaster* theMaster, IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION match_master(jvxBool* doesMatchRet, jvxComponentIdentification tpMatch, const char* wildcardMatch) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deassociate_master() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION link_triggers_connection() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unlink_triggers_connection() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain(jvxBool storeProtocol JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))  override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))  override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_chain(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION apply_update(jvxConnectionEventType evTp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION status_chain(jvxBool* lastTestOk JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_forward_test_depend(jvxBool test_on_connect) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_forward_test_depend(jvxBool* test_on_connect) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_test_on_connect(jvxBool test_on_connect, const jvxLinkDataDescriptor_con_params* init_params = nullptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_test_on_connect(jvxBool* test_on_connect, jvxLinkDataDescriptor_con_params* init_params = nullptr) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION marked_essential_for_start(jvxBool* marked_essential)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION check_process_ready(jvxApiString* reason_if_not_ready)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION detail_connection_not_ready(jvxApiString* reason_if_not_ready) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION iterator_chain(IjvxConnectionIterator** it) override;

	virtual void get_configuration_local(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)override;
	virtual jvxErrorType create_bridge_check()override;
	virtual IjvxConnectionMaster* get_master_ref()override;
	virtual jvxErrorType remove_bridge_local(jvxSize unique_id)override;
	virtual jvxErrorType remove_all_bridges_local()override;

	// ===============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType, jvxHandle**) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType, jvxHandle*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;

	// ===============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	// ===============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_event_interceptor(jvxDataChainReportType tp, jvxSize inercceptorid) override;

	void prepare_local();
	void start_local();
	void stop_local();
	void postprocess_local();

	jvxErrorType getFirstErrorReason(IjvxCallProt* fdbLastTest, jvxErrorType* res, jvxApiString* strOnReturn);


};

#endif
