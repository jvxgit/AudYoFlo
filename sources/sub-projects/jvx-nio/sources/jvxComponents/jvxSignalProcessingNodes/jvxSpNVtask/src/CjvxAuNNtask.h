#ifndef __CJVXAUNNTASK_H__
#define __CJVXAUNNTASK_H__

#include "jvx.h"

#include "common/CjvxNegotiate.h"

#define JVX_SHIFT_OFFSET_SUBMODULE_AUNTASK 256


// ==============================================================

#include "jvxNodes/Cjvx_logfile_core.h"

class CjvxAuNNtaskLog : public CjvxAuNTasks
{
	Cjvx_logfile_core logWriter_main_signal;
	jvxSize idMainIn;
	jvxSize idMainOut;

public:
	JVX_CALLINGCONVENTION CjvxAuNNtaskLog(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAuNNtaskLog();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties* callGate,
		jvxHandle* fld,
		jvxSize numElements,
		jvxDataFormat format,
		jvxSize propId,
		jvxPropertyCategoryType category,
		jvxSize offsetStart,
		jvxBool contentOnly) override;

	jvxErrorType prepare_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType process_buffers_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage)override;

};

// ==============================================================

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCalls.h"

class CjvxAuNNtaskm : public CjvxAuNTasks, public CjvxMexCalls
{
protected:

	jvxAudioStackDispenserCont myAudioDispenser;
	jvxHandle* fHeightEstimator;
	jvxHandle**work_buffers_fromExt;
	jvxHandle**work_buffers_toExt;
	jvxData* tstamp_buffer_fromExt;
	jvxData* tstamp_buffer_toExt;
	jvxData fillheight_avrg;

public:

	JVX_CALLINGCONVENTION CjvxAuNNtaskm(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAuNNtaskm();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

#include "mcg_export_project_prototypes.h"

};
#endif

#endif
