#ifndef __TJVXTRIGGERCONNECTOR_H__
#define __TJVXTRIGGERCONNECTOR_H__

JVX_INTERFACE IjvxConnectionIterator;

class CjvxIteratorOconIcon
{
public:
	jvxSize idx = 0;
	IjvxConnectionIterator** onReturn = nullptr;
	jvxApiString* nmOcon = nullptr;
	jvxApiString* nmIcon = nullptr;
};

enum class jvxTriggerConnectorPurpose
{
	JVX_CONNECTOR_TRIGGER_CONNECT,  /* Variable argument is a pointer to an instance of const jvxChainConnectArguments */
	JVX_CONNECTOR_TRIGGER_DISCONNECT, /* Variable argument is a pointer to an instance of const jvxChainConnectArguments */
	JVX_CONNECTOR_TRIGGER_ITERATOR_NEXT_HANDLE_ICON_OCON, /* Return three args via CjvxIteratorOconIcon */
	// JVX_CONNECTOR_TRIGGER_ITERATOR_NEXT_OCON_NAME, /* Variable argument is jvxApiString* */
	JVX_CONNECTOR_TRIGGER_TEST,
	JVX_CONNECTOR_TRIGGER_PREPARE,
	JVX_CONNECTOR_TRIGGER_START,
	JVX_CONNECTOR_TRIGGER_STOP,
	JVX_CONNECTOR_TRIGGER_POSTPROCESS,
	JVX_CONNECTOR_TRIGGER_FORWARD_, 
	JVX_CONNECTOR_TRIGGER_BACKWARD_
};

#endif
