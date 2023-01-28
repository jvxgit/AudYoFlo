#include "jvx.h"

#include "jvxAppCtrNodes/CjvxAppCtrNode.h"

CjvxApplicationControllerNode::CjvxApplicationControllerNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	JVX_DECLARE_OBJECT_REFERENCES(JVX_COMPONENT_APPLICATION_CONTROLLER_NODE, IjvxNode);
}

CjvxApplicationControllerNode::~CjvxApplicationControllerNode()
{
	terminate();
}


