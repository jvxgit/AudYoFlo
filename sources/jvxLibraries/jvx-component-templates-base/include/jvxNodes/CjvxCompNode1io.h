#ifndef __CJVXCOMPNODE_H__
#define __CJVXCOMPNODE_H__

#include "CjvxNodeBase1io.h"

#include "compatibility/IjvxDataProcessor.h"
#include "compatibility/CjvxDataProcessor.h"

class CjvxCompNode1io:
	public IjvxDataProcessor, public CjvxDataProcessor,
	public CjvxNodeBase1io
{
protected:

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxCompNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCompNode1io();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

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


#define JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API


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
#undef JVX_INTERFACE_SUPPORT_LINKDATA_SLAVE

	jvxErrorType prepare_lock();
	jvxErrorType postprocess_lock();

};

#endif
