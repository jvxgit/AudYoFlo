#ifndef __CJVXAUDIOENCODER_H__
#define __CJVXAUDIOENCODER_H__

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

	class CjvxAudioEncoder : public IjvxAudioEncoder, public CjvxObject, 
		public IjvxProperties, public CjvxProperties,
		public IjvxConnectorFactory, public CjvxConnectorFactory,
		public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector
	{
	protected:

		CjvxNegotiate_input neg_input;
		CjvxNegotiate_output neg_output;
		std::string my_config_token;

	public:

		// ===================================================================================================
		// ===================================================================================================
		JVX_CALLINGCONVENTION CjvxAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
		virtual JVX_CALLINGCONVENTION ~CjvxAudioEncoder();

		// ===================================================================================================
		// ===================================================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION start()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

		// ==================================================
		// ==================================================

#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#include "codeFragments/simplify/jvxObjects_simplify.h"

// Interfaces and default implementations for connections
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
#include "codeFragments/simplify/jvxProperties_simplify.h"

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
//#define JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_OCON
//#define JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_OCON
//#define JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
//#undef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
//#undef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_OCON
//#undef JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_OCON
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#undef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON

	// ==================================================
	//virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(IjvxConnectionMaster* theMaster JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	//virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	
	// Need to define this function since option JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS is set!
	virtual void test_set_output_parameters() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	// ==================================================
	virtual jvxErrorType activate_encoder();
	virtual jvxErrorType deactivate_encoder();
	// ==================================================

	virtual void accept_input_parameters() = 0;
	// virtual void accept_output_parameters() = 0;
	virtual jvxErrorType check_backward_parameters(jvxLinkDataDescriptor* lp_cp, jvxLinkDataDescriptor& forward, jvxBool& forwardRequest) = 0;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
