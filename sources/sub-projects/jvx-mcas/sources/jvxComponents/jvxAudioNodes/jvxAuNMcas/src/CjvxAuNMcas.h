#ifndef __CJVXAUNMCAS_H__
#define __CJVXAUNMCAS_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"

#include "playControl.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCalls.h"
#endif

class CjvxAuNMcas: public CjvxAudioNode, public genSimple_node
#ifdef JVX_EXTERNAL_CALL_ENABLED
    , public CjvxMexCalls
#endif

{
private:
	playControl* myPlayControl;

public:

	
	JVX_CALLINGCONVENTION CjvxAuNMcas(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNMcas();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender) override;

	jvxErrorType JVX_CALLINGCONVENTION set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	jvxErrorType ic_set_properties_callback_pre(jvxSize * id, jvxPropertyCategoryType * cat, jvxSize * offsetStart, jvxSize * numElements, jvxDataFormat * form);

#ifdef JVX_EXTERNAL_CALL_ENABLED

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "",
		jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

#include "mcg_export_project_prototypes.h"
#endif

//	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_properties_callback_pre);
//	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_properties_callback_post);

private:
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output,
		jvxSize bSize, jvxSize numChans_input, jvxSize numChans_output, jvxDataFormat format);

};

#endif
