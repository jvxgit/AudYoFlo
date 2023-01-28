#ifndef __CJVXAUNmyNewProject_H__
#define __CJVXAUNmyNewProject_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCalls.h"
#endif

class CjvxAuNmyNewProject: public CjvxAudioNode, public genSimple_node
#ifdef JVX_EXTERNAL_CALL_ENABLED
    , public CjvxMexCalls
#endif

{
	
public:

	JVX_CALLINGCONVENTION CjvxAuNmyNewProject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAuNmyNewProject();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender) override;

#ifdef JVX_EXTERNAL_CALL_ENABLED

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "",jvxInt32 lineno = -1 )override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

#include "mcg_export_project_prototypes.h"
#endif

private:

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffer( jvxHandle** buffers_input,jvxHandle** buffers_output,
		jvxSize bSize, jvxSize numChans_input, jvxSize numChans_output, jvxDataFormat format);

};

#endif
