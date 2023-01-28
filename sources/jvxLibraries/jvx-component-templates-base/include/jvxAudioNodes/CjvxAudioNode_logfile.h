#ifndef __CJVXAUDIONODELOG_H__
#define __CJVXAUDIONODELOG_H__

#include "CjvxAudioNode.h"

#include "compatibility/IjvxDataProcessor.h"
#include "compatibility/CjvxDataProcessor.h"
#include "pcg_CjvxAudioNode_logfile_pcg.h"
#include "CjvxLogSignalsToFile.h"

class CjvxAudioNode_logfile: public CjvxAudioNode, public CjvxAudioNode_logfile_genpcg
{

protected:

	struct
	{
		jvxSize numFramesLog;
		jvxSize timeoutMsec;
	} 	_common_set_audio_log_to_file;

	CjvxLogSignalsToFile theLogger;
	std::string logger_prop_prefix;
	jvxSize idInChannels;
	jvxSize idOutChannels;
	
public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxAudioNode_logfile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioNode_logfile();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
			jvxHandle* sectionToContainAllSubsectionsForMe, 
			const char* filename,
			jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,	jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	jvxErrorType log_write_all_channels(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender);
	jvxErrorType log_prepare_all_channels();
	jvxErrorType log_start_all_channels();
	jvxErrorType log_stop_all_channels();
	jvxErrorType log_postprocess_all_channels();
};

#endif
