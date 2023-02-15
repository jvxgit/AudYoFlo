#ifndef __CJVXBARENODE1IONODELOG_H__
#define __CJVXBARENODE1IONODELOG_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

#include "compatibility/IjvxDataProcessor.h"
#include "compatibility/CjvxDataProcessor.h"
#include "jvxNodes/Cjvx_logfile_core.h"

class CjvxBareNode1io_logfile: public CjvxBareNode1ioRearrange
{
protected:
	struct
	{
		jvxSize numFramesLog;
		jvxSize timeoutMsec;
		std::string logger_prop_prefix;
	} 	_common_set_audio_log_to_file;
	jvxSize idInChannels;
	jvxSize idOutChannels;
	Cjvx_logfile_core theMainLogFile;

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxBareNode1io_logfile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxBareNode1io_logfile();

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
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	jvxErrorType log_write_all_channels(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxData* heightPercent);
	jvxErrorType log_prepare_all_channels();
	jvxErrorType log_start_all_channels();
	jvxErrorType log_stop_all_channels();
	jvxErrorType log_postprocess_all_channels();
};

#endif