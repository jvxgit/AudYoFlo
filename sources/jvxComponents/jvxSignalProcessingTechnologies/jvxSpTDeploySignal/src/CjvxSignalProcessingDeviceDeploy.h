#ifndef __CJVXSIGNALPROCESSINGDEVICEDEPLOY_H__
#define __CJVXSIGNALPROCESSINGDEVICEDEPLOY_H__

#include "jvx.h"
#include "jvxTechnologies/CjvxFullMasterDevice.h"
#include "pcg_CjvxSignalProcessingDeviceDeploy.h"
#include "IjvxSignalProcessingDeploy.h"


class CjvxSignalProcessingDeviceDeploy : public CjvxFullMasterDevice, 
	public CjvxSignalProcessingDeviceDeploy_genpcg,
	public IjvxSignalProcessingDeploy
{
private:
	IjvxSignalProcessingDeploy_interact* interactRef;
	JVX_MUTEX_HANDLE safeAccessChain;
	JVX_ATTACHED_LINK_DATA_MEMBER_LOST_FRAMES(lst_lost);
	
	jvxSize lost_cnt_reported; 
	jvxSize lost_cnt_new;

public:
	JVX_CALLINGCONVENTION CjvxSignalProcessingDeviceDeploy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingDeviceDeploy();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() override;

	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_interface_ptr);

public:

	virtual jvxErrorType JVX_CALLINGCONVENTION setDataInteractReference(IjvxSignalProcessingDeploy_interact* interact) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION setOutputParameters(jvxLinkDataDescriptor_con_params* params) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION setInputParameters(jvxLinkDataDescriptor_con_params* params) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerTest() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION exchangeData(IjvxSignalProcessingDeploy_data* data, jvxLinkDataAttachedChain* single_data_copy) override;
	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerPrepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerStart() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerStop() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerPostprocess() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerProcess() override;

	*/
	JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DECLARE(release_lost_frames);
};

#endif