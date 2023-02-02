#ifndef __CJVXAUDIODECODER_H__
#define __CJVXAUDIODECODER_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxNegotiate.h"

#include "compatibility/IjvxDataProcessor.h"
#include "compatibility/CjvxDataProcessor.h"

#include "pcg_CjvxAudioCodec_pcg.h"
#include "CjvxAudioCodec.h"

#include "jvx_audiocodec_helpers.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	class CjvxAudioDecoder : public IjvxAudioDecoder, public CjvxObject, 
		public IjvxProperties, public CjvxProperties,
		public IjvxConnectorFactory, public CjvxConnectorFactory,
		public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
		//public IjvxDataProcessor, public CjvxDataProcessor,
		public CjvxAudioCodec_genpcg
	{
	protected:

		CjvxNegotiate_input neg_input;
		CjvxNegotiate_output neg_output;

		std::string encode_hint_transferred;
		CjvxAudioCodec* myParent;
		wav_params params;
		jvxBool lendian = false;
	public:

		// ===================================================================================================
		// ===================================================================================================
		JVX_CALLINGCONVENTION CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
		virtual JVX_CALLINGCONVENTION ~CjvxAudioDecoder();

		// ===================================================================================================
		// ===================================================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION start()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

		// ===================================================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION set_configure_token(const char* token) override;
		
		// ===================================================================================================

		/*
		virtual jvxErrorType JVX_CALLINGCONVENTION reference_object(IjvxObject** refObject)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION supports_multithreading(jvxBool* supports)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect, jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
		*/

		// ===================================================================================================

#include "codeFragments/simplify/jvxStateMachine_simplify.h"

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxProperties_simplify.h"

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

		// Interfaces and default implementations for connections
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

		// ===============================================================================

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

// ===============================================================================

#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#undef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON

	// ==================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// Need to define this function since option JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS is set!
	virtual void test_set_output_parameters();
	// ==================================================

	// ==================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	// ==================================================

	// ==================================================
	virtual jvxErrorType activate_decoder();
	virtual jvxErrorType deactivate_decoder();
	// ==================================================

	void set_parent(CjvxAudioCodec* parent){myParent = parent;};
	};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
