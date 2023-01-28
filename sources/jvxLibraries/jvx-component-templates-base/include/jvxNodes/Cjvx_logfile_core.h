#ifndef __CJVX_LOGFILE_CORE_H__
#define __CJVX_LOGFILE_CORE_H__

#include "pcg_CjvxNode_logfile_pcg.h"
#include "CjvxLogSignalsToFile.h"

class Cjvx_logfile_core: public CjvxNode_logfile_genpcg
{
protected:
	struct oneEntry
	{
		std::string description;
		jvxSize uId;
		jvxSize numChannels;
		jvxSize bSize;
		jvxDataFormat format;
	};

	IjvxObject* objRef = nullptr;
	CjvxProperties* propRef = nullptr;
	IjvxHiddenInterface* theHostRef = nullptr;
	
	CjvxLogSignalsToFile theLogger;

	std::map<jvxSize, oneEntry> uIds;

	// ==========================================================================
public:

	JVX_CALLINGCONVENTION Cjvx_logfile_core();
	virtual JVX_CALLINGCONVENTION ~Cjvx_logfile_core();

	virtual jvxErrorType JVX_CALLINGCONVENTION lfc_activate(IjvxObject* objRef,
		CjvxProperties* prpRef, IjvxHiddenInterface* hRef,
		const std::string& logger_prop_prefix, jvxSize numFramesLog, jvxSize timeoutMsec, 
		const char* gTitle_spec = NULL, const char* fPrefix_spec = NULL,
		const char* textActivate_spec = NULL, const char* textLogfilePrefix_spec = NULL);

	virtual jvxErrorType JVX_CALLINGCONVENTION lfc_deactivate();

	virtual jvxErrorType JVX_CALLINGCONVENTION lfc_put_configuration_active(
		jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno,
		std::vector<std::string>& warns);

	virtual jvxErrorType JVX_CALLINGCONVENTION lfc_get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections);

	virtual jvxErrorType JVX_CALLINGCONVENTION lfc_set_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		jvxPropertyCallPurpose callPurpose);

	jvxErrorType lfc_write_all_channels(jvxHandle** theData, jvxSize nChans, jvxSize nElements,
		jvxDataFormat format, jvxSize uId, jvxData* fHeightPerc = NULL);
	jvxErrorType lfc_prepare_one_channel(jvxSize number_channels, jvxSize buffersize, jvxSize samplerate,
			jvxDataFormat format, const std::string& token_chans, jvxSize& uId);

	jvxErrorType lfc_start_all_channels();
	jvxErrorType lfc_stop_all_channels();
	jvxErrorType lfc_postprocess_all_channels();

	//jvxSize lookup_uid(const std::string& description);
};

#endif