#ifndef __CJVXAUNSYNTHESIZER_H__
#define __CJVXAUNSYNTHESIZER_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"
#include "IjvxIntegrateWaveGenerators.h"

// =========================================================================

class CjvxAuNSynthesizer: public CjvxAudioNode
{
	IjvxIntegrateWaveGenerators* myWaveGenerators;
	struct
	{
		jvxBool endProcessingTriggered;
	} runtime;

public:

	JVX_CALLINGCONVENTION CjvxAuNSynthesizer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNSynthesizer();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;


private:

};

#endif
