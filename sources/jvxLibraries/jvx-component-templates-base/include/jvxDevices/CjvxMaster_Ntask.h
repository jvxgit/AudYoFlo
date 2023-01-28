#ifndef __CJVXMASTER_NTASK_H__
#define __CJVXMASTER_NTASK_H__

#include "jvx.h"

#include "common/CjvxConnectionMaster.h"
#include "jvxNodes/CjvxInputOutputConnector_Ntask.h"

class CjvxMasterNtask;

JVX_INTERFACE CjvxMasterNtask_report
{
public:
	virtual JVX_CALLINGCONVENTION ~CjvxMasterNtask_report() {};

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is called BEFORE the link is followed towards the chain. The refto arg must be used in callback to move chain forward
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready_master_id(jvxBool* is_ready, jvxApiString* reasonifnot, jvxSize idCtxt
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
};

class CjvxOutputConnectorNtask;
class CjvxInputConnectorNtask;

class CjvxMasterNtask: public IjvxConnectionMaster, public CjvxConnectionMaster
{
public:

	struct
	{
		CjvxOutputConnectorNtask* connOut;
		CjvxInputConnectorNtask* connIn;
		jvxSize ctxtId;
		CjvxMasterNtask_report* cbRef;
	} _common_set_ld_master_ntask;

public:
	CjvxMasterNtask(jvxSize ctxId, const char* descr, IjvxConnectionMasterFactory* par,
		CjvxMasterNtask_report* report, CjvxObject* objRef);
	
	~CjvxMasterNtask();


#define JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR _common_set_ld_master_ntask.connOut

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION  transfer_chain_forward_master(jvxLinkDataTransferType tp, 
		jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

#define JVX_CONNECTION_MASTER_SKIP_PREPARE
#define JVX_CONNECTION_MASTER_SKIP_POSTPROCESS
#define JVX_CONNECTION_MASTER_SKIP_START
#define JVX_CONNECTION_MASTER_SKIP_STOP
#define JVX_CONNECTION_MASTER_SKIP_TEST
#define JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
#undef JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
#undef JVX_CONNECTION_MASTER_SKIP_PREPARE
#undef JVX_CONNECTION_MASTER_SKIP_POSTPROCESS
#undef JVX_CONNECTION_MASTER_SKIP_START
#undef JVX_CONNECTION_MASTER_SKIP_STOP
#undef JVX_CONNECTION_MASTER_SKIP_TEST

	jvxErrorType test_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType prepare_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType postprocess_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType start_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType stop_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType link_master(CjvxOutputConnectorNtask* conn_out, CjvxInputConnectorNtask* conn_in);
	jvxErrorType unlink_master();
};

#endif