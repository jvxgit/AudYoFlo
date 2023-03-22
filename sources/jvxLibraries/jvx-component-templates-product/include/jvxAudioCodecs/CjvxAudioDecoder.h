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

#include "CjvxAudioCodec.h"

#include "jvx_audiocodec_helpers.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	class CjvxAudioDecoder : public IjvxAudioDecoder, public CjvxObject, 
		public IjvxProperties, public CjvxProperties,
		public IjvxConnectorFactory, public CjvxConnectorFactory,
		public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector
	{
	protected:
		wav_params params;

	protected:

		CjvxNegotiate_input neg_input;
		CjvxNegotiate_output neg_output;

		// std::string encode_hint_transferred;
		jvxBool lendian = false;
		
		jvxDataFormat codecFormat = JVX_DATAFORMAT_BYTE;
		jvxDataFormatGroup codecFormatGroup = JVX_DATAFORMAT_GROUP_NONE;
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
		// ===================================================================================================
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

	virtual void accept_input_parameters() = 0;
	virtual void test_set_output_parameters() = 0;
	virtual jvxErrorType updated_backward_format_spec(jvxLinkDataDescriptor& forward, jvxLinkDataDescriptor* ld_cp) = 0;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
