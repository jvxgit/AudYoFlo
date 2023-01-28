#ifndef __CJVXDATACONNECTIONSGROUP_H__
#define __CJVXDATACONNECTIONSGROUP_H__

#include "jvx.h"
#include "jvxHosts/CjvxDataConnectionsCommon.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxDataChainInterceptorGroup.h"

#include "jvxHosts/CjvxDataConnectionsGroup_inout.h"
#include "pcg_CjvxDataConnectionProcess_pcg.h"
#include "pcg_CjvxDataConnectionGroup_pcg.h"

#include "jvx-helpers.h"

// #define CJVXDATACONNECTIONSGROUP_VERBOSE

#include "development/jvx_define_constants_proc_group_logs.h"

// =====================================================================================
// ====================================================================================================

class CjvxDataConnectionsGroup: public IjvxDataConnectionGroup, public CjvxDataConnectionCommon,
	public IjvxProperties, public CjvxProperties, 
	public IjvxConfiguration,
	public IjvxDataChainInterceptorReport,
#ifdef JVX_BUFFERING_LOGFILE
	public IjvxReportTimingLogfile, public HjvxDataLogger,
#endif
	public CjvxDataConnectionProcess_genpcg, public CjvxDataConnectionGroup_genpcg
{
	friend class CjvxDataConnectionsGroup_local_master;
	friend class CjvxDataConnectionsGroup_external;
private:
	
	// std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > > bridges_in_use;
	struct oneChainEntry
	{
		jvxSize idx_read;
		jvxSize num_buffer;
		jvxSize num_current;
		jvxDataConnectionsThreadIdentification* operation_ids;
		//JVX_MUTEX_HANDLE safe_Access;
		jvxSize operationIdCount;
		jvxDataConnectionsObjectIdentification* obj_id;
		CjvxDataChainInterceptorGroup* ptr;
		jvxThreadProcessingStrategy thread_strat_set;
		jvxProcessingGroupThreadMode thread_model_set;
		oneChainEntry()
		{
			ptr = NULL;
			idx_read = 0;
			num_buffer = 0;
			num_current = 0;
			operation_ids = NULL;
			operationIdCount = 1;
			obj_id = NULL;
			thread_strat_set = JVX_PROCESSING_STRATEGY_WAIT;
			thread_model_set = JVX_THREAD_PROC_GROUP_MODE_SEQ_CHAIN_THREADS;
			//JVX_INITIALIZE_MUTEX(safe_Access);
		};
		~oneChainEntry()
		{
			//JVX_TERMINATE_MUTEX(safe_Access);
		};
	};

protected:
	
	CjvxDataConnectionsGroup_external exposeConnectors;

	std::map<CjvxDataChainInterceptorGroup*, oneChainEntry> interceptors_in;
	std::list<CjvxDataChainInterceptorGroup*> interceptors_out;

	std::map<jvxSize, std::vector<jvxSize> > mapped_ids;

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

	jvxDataConnectionsThreadParameters threadMode;

#ifdef JVX_BUFFERING_LOGFILE
	IjvxObject* logFileObj;
	IjvxDataLogger* logFileHdl;
#endif

protected:

public:

	CjvxDataConnectionsGroup(
		CjvxDataConnections* parentArg,
		const std::string& nm, 
		jvxSize unique_id_system, 
		const std::string& descr,
		jvxBool verbose_out);
	~CjvxDataConnectionsGroup();

#ifdef JVX_BUFFERING_LOGFILE

	virtual jvxErrorType JVX_CALLINGCONVENTION register_instance(const std::string& descr, jvxSize& id_write) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION memory_instance(jvxUInt32** buffer, jvxSize* entries, jvxSize id_write) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION write_single_instance(jvxSize id_write) override;

#endif

	void set_name_unique_descriptor(IjvxCallProt* fdb)override;

#define JVX_DATACONNECTION_SUPPRESS_CONNECTOR_FACTORY_INVOLVED
#include "codeFragments/simplify/jvxDataConnectionCommon_simplify.h"
#undef JVX_DATACONNECTION_SUPPRESS_CONNECTOR_FACTORY_INVOLVED
	virtual jvxErrorType connector_factory_is_involved(IjvxConnectorFactory* rem_this)override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION create_bridge(IjvxOutputConnector* conn_from, IjvxInputConnector* conn_to, const char* nm, jvxSize* unique_id, jvxBool dedicatedThread, jvxBool boostThread)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual void get_configuration_local(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)override;
	virtual jvxErrorType create_bridge_check()override;
	virtual IjvxConnectionMaster* get_master_ref()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_bridge_local(jvxSize unique_id)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_bridges_local() override;
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

	// jvxErrorType connect_chain_local(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	void about_release_intereptor(CjvxDataChainInterceptor* ptr) override;

	/*
	jvxErrorType trigger_thread_interceptors(IjvxConnectionMaster* master);
	jvxErrorType get_thread_mode(jvxDataConnectionsThreadParameters* threadParameters);
	*/

	// =====================================================================

	jvxErrorType prepare_thread_pipeline(CjvxDataChainInterceptorGroup* entry,
		jvxDataConnectionsObjectIdentification** ptrCOnnectionId);
	jvxErrorType postprocess_thread_pipeline(CjvxDataChainInterceptorGroup* entry);

	// =====================================================================

	/*
	jvxErrorType start_thread_pipeline(CjvxDataChainInterceptorGroup* entry);
	jvxErrorType stop_thread_pipeline(CjvxDataChainInterceptorGroup* entry);
	*/
	// =====================================================================

	void updateTaskList();

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_data_prehook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_chain_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_chain_prehook);
};

#endif