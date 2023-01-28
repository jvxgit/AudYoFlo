#ifndef __CJVXMEASUREIR2DEV_H__
#define __CJVXMEASUREIR2DEV_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAuNTasks.h"
// #include "CjvxSignalProcessingDeviceDeploy.h"
#include "IjvxSignalProcessingDeploy.h"

class CjvxAuNMeasureIr2Dev: public CjvxAuNTasks, public IjvxSignalProcessingDeploy_interact
{
private:
	// CjvxSignalProcessingDeviceDeploy* theDeployer;
	IjvxObject* theObjDeployHdl;
	jvxComponentIdentification theObjDeployId;
	IjvxDevice* theObjDeployDev;
	IjvxProperties* theObjDeployProps;
	IjvxSignalProcessingDeploy* theObjDeployIf;
	jvxLinkDataDescriptor_con_params params_receive;
	jvxLinkDataDescriptor_con_params params_to_emit;

	IjvxSignalProcessingDeploy_data inProcessing;
	
	jvxSize numGlitchesReported;
public:
	JVX_CALLINGCONVENTION CjvxAuNMeasureIr2Dev(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAuNMeasureIr2Dev();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override; 

	virtual jvxErrorType test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override;

	jvxErrorType passParametersToDeployModule();

	virtual jvxErrorType JVX_CALLINGCONVENTION process_io(
		jvxLinkDataDescriptor* pri_in, jvxSize idx_stage_pri_in,
		jvxLinkDataDescriptor* sec_in, jvxSize idx_stage_sec_in,
		jvxLinkDataDescriptor* pri_out, jvxLinkDataDescriptor* sec_out)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() override;

	// Interface functions
	jvxErrorType is_ready(jvxBool* isReady, jvxApiString* reason) override;
public:
	virtual jvxErrorType JVX_CALLINGCONVENTION process_request_data_start(jvxLinkDataDescriptor* dataOut) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_request_data_stop(jvxLinkDataDescriptor* dataIn) override;

};

#endif