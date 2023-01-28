#ifndef __CJVXAUNSYNTHESIZER_H__
#define __CJVXAUNSYNTHESIZER_H__

#include "jvx.h"
#include "templates/jvxComponents/jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"
#include "jvxGeneratorWave/IjvxIntegrateWaveGenerators.h"

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

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxHandle* fld,
							jvxSize numElements,jvxDataFormat format,
							jvxSize propId,jvxPropertyCategoryType category,
							jvxSize offsetStart, jvxBool contentOnly, 
							jvxFlagTag accessFlags,
							jvxAccessProtocol* accProt,
							jvxPropertyCallPurpose callPurpose, jvxSize* context)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, jvxFlagTag flagtag,
								     const char* filename,
								     jvxInt32 lineno, jvxConfigurationPurpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections, jvxFlagTag flagtag,
									 jvxConfigurationPurpose = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;


private:

};

#endif
