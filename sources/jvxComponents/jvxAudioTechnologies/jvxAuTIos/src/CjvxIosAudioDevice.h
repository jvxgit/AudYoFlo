#ifndef __CJVXAUDIOCOREAUDIODEVICE_H__
#define __CJVXAUDIOCOREAUDIODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "pcg_exports_device.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#include <AudioUnit/AudioUnit.h>

#include "jvxAudioProcessor_cpp.h"

#define ALPHA_SMOOTH_DELTA_T 0.995

class CjvxIosAudioTechnology;

class CjvxIosAudioDevice: public CjvxAudioDevice,  
	public genIosAudio_device
{
    friend class CjvxIosAudioTechnology;

private:

    struct
    {
        std::string description;
        jvxSize inputPortId;
        CjvxIosAudioTechnology* theParent;
    } device;

    struct
    {
        jvxTimeStampData theTimestamp;
        jvxInt64 timestamp_previous;
        jvxData deltaT_average_us;
        jvxData deltaT_theory_us;

    } inProcessing;

    struct
    {
        std::vector<jvxInt32> srates;
        std::vector<jvxInt32> bsizes;
        std::vector<jvxDataFormat> formats;
    } shortcuts;


public:

    JVX_CALLINGCONVENTION CjvxIosAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

    jvxErrorType init(jvxSize iPortId, CjvxIosAudioTechnology* par);

    virtual JVX_CALLINGCONVENTION ~CjvxIosAudioDevice();

    virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;

    virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

    virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
        jvxCallManagerProperties* callGate,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize propId, 
		jvxPropertyCategoryType category, jvxSize offsetStart, jvxBool contentOnly)override;

    // Interface IjvxConfiguration

  	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename = "", jvxInt32 lineno = -1 )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(
        jvxCallManagerConfiguration* callConf, 
    	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections
    )override;

    // Interface IjvxDataProcessorSelector

     	// ===================================================================================
	// New linkdata connection interface
	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
        jvxSize pipeline_offset , jvxSize* idx_stage,
	    jvxSize tobeAccessedByStage,
	    callback_process_start_in_lock clbk,
	    jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
        jvxSize idx_stage ,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr )override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
  
    jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
    
    void updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);

    void processBuffer();
    // ================================================================================================

};

#endif
