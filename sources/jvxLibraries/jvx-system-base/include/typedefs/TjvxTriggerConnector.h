#ifndef __TJVXTRIGGERCONNECTOR_H__
#define __TJVXTRIGGERCONNECTOR_H__

enum class jvxTriggerConnectorPurpose
{
	JVX_CONNECTOR_TRIGGER_CONNECT,  /* Variable argument is a pointer to an instance of const jvxChainConnectArguments */
	JVX_CONNECTOR_TRIGGER_DISCONNECT, /* Variable argument is a pointer to an instance of const jvxChainConnectArguments */
	JVX_CONNECTOR_TRIGGER_ITERATOR_NEXT, /* Variable argument is a pointer to an pointer reference of IjvxConnectionIterator - to be filled by the referenced connector */
	JVX_CONNECTOR_TRIGGER_TEST,
	JVX_CONNECTOR_TRIGGER_PREPARE,
	JVX_CONNECTOR_TRIGGER_START,
	JVX_CONNECTOR_TRIGGER_STOP,
	JVX_CONNECTOR_TRIGGER_POSTPROCESS,
	JVX_CONNECTOR_TRIGGER_FORWARD, 
	JVX_CONNECTOR_TRIGGER_BACKWARD
};

#endif
