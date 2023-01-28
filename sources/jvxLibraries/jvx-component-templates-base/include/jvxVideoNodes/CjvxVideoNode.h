#ifndef __CjvxVideoNode_H__
#define __CjvxVideoNode_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"

#include "compatibility/IjvxDataProcessor.h"
#include "compatibility/CjvxDataProcessor.h"

#include "pcg_CjvxVideoNode_pcg.h"

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
#include "CjvxLogSignalsToFile.h"
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
#define JVX_VIDEONODE_CLASSNAME CjvxVideoNode_logfile
#define JVX_VIDEONODE_CLASSNAME_GEN CjvxVideoNode_logfile_genpcg
#else
#define JVX_VIDEONODE_CLASSNAME CjvxVideoNode
#define JVX_VIDEONODE_CLASSNAME_GEN CjvxVideoNode_genpcg
#endif

class JVX_VIDEONODE_CLASSNAME : 
	public IjvxNode, public CjvxObject, 
	public IjvxProperties, public CjvxProperties,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxDataProcessor, public CjvxDataProcessor,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxConfiguration,
	public JVX_VIDEONODE_CLASSNAME_GEN
{
protected:
	typedef struct
	{
		struct
		{
			jvxSize min;
			jvxSize max;
		} seg_x;

		struct
		{
			jvxSize min;
			jvxSize max;
		} seg_y;

		struct
		{
			jvxDataFormat min;
			jvxDataFormat max;
		} format;

		struct
		{
			jvxDataFormatGroup min;
			jvxDataFormatGroup max;
		} subformat;
	} jvxVideoNodePreferredParams;

	typedef struct
	{
		jvxSize seg_x;
		jvxSize seg_y;
		jvxDataFormat format;
		jvxDataFormatGroup subformat;
	} jvxVideoNodeProcessingParams;

	jvxLinkDataDescriptor myData4Next;

protected:
	struct
	{
		jvxVideoNodePreferredParams preferred;
		jvxVideoNodeProcessingParams processing;

		jvxBool acceptOnlyExactMatchLinkDataInput;
		jvxBool acceptOnlyExactMatchLinkDataOutput;

		jvxBool settingReadyToStart;
		std::string settingReadyFailReason;
		jvxBool report_take_over_started;

	} _common_set_video_params;

#ifdef JVX_ALL_VIDEO_TO_LOGFILE
	struct
	{
		jvxSize numFramesLog;
		jvxSize timeoutMsec;
	} 	_common_set_audio_log_to_file;

	CjvxLogSignalsToFile theLogger;
	std::string logger_prop_prefix;
	jvxSize idFrames;
#endif

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxVideoNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxVideoNode();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	// ===================================================================================================
	// Interface IjvxDataProcessor
	// ===================================================================================================
#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_OUTPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_object(IjvxObject** refObject)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_multithreading(jvxBool* supports)override;

	/* TODO: Add member functions for multithreading negotiations */

	virtual jvxErrorType JVX_CALLINGCONVENTION process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	// ===================================================================================================
	// Interface IjvxConfiguration
	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
#if 0
	jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(jvxLinkDataDescriptor* theData) override;

	jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(jvxLinkDataDescriptor* theData) override;

	jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(jvxLinkDataDescriptor* theData) override;

	jvxErrorType JVX_CALLINGCONVENTION process_start_icon(jvxLinkDataDescriptor* theData) override;

	jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxLinkDataDescriptor* theData) override;

	jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxLinkDataDescriptor* theData)override;

	jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(jvxLinkDataDescriptor* theData)override;

	jvxErrorType JVX_CALLINGCONVENTION link_request_reference_object(IjvxObject** obj)override;

	jvxErrorType JVX_CALLINGCONVENTION link_return_reference_object(IjvxObject* obj)override;
#endif
	// ===================================================================================================

#include "codeFragments/simplify/jvxStateMachine_simplify.h"

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"


#ifdef JVX_ALL_AUDIO_TO_LOGFILE
#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
virtual jvxErrorType JVX_CALLINGCONVENTION set_property(jvxHandle* fld,
							jvxSize numElements,
							jvxDataFormat format,
							jvxSize propId,
							jvxPropertyCategoryType category,
							jvxSize offsetStart,
							jvxBool contentOnly,
		jvxFlagTag accessFlags,
		jvxAccessProtocol* accProt,
							jvxPropertyCallPurpose callPurpose, jvxSize* context)override;
#endif

#include "codeFragments/simplify/jvxProperties_simplify.h"

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
#endif

#include "codeFragments/simplify/jvxSequencerControl_simplify.h"


#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY

#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"

#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY

	jvxErrorType reportPreferredParameters(jvxPropertyCategoryType cat, jvxSize propId);
	jvxErrorType prepare_lock();
	jvxErrorType postprocess_lock();

#ifdef JVX_ALL_AUDIO_TO_LOGFILE
	jvxErrorType log_write_all_channels(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender);
	jvxErrorType log_prepare_all_channels();
	jvxErrorType log_start_all_channels();
	jvxErrorType log_stop_all_channels();
	jvxErrorType log_postprocess_all_channels();
#endif

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
