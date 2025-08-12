#include "common/CjvxInputOutputConnectorCore.h"

CjvxInputOutputConnectorCore::CjvxInputOutputConnectorCore()
{
	_common_set_io_common.object = NULL;
	_common_set_io_common.descriptor = "none";
	_common_set_io_common.myParent = NULL;
	_common_set_io_common.theMaster = NULL;
	_common_set_io_common.myRuntimeId = JVX_SIZE_UNSELECTED;
}

jvxErrorType
CjvxInputOutputConnectorCore::_descriptor_connector(jvxApiString* str)
{
	if (str)
	{
		str->assign(_common_set_io_common.descriptor);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputOutputConnectorCore::_set_parent_factory(IjvxConnectorFactory* my_parent)
{
	_common_set_io_common.myParent = my_parent;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputOutputConnectorCore::_parent_factory(IjvxConnectorFactory** my_parent)
{
	if (my_parent)
		*my_parent = _common_set_io_common.myParent;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputOutputConnectorCore::activate(
	IjvxObject* theObjRef,
	IjvxConnectorFactory* myPar, 
	IjvxConnectionMaster* master, 
	const std::string& descror)
{
	_common_set_io_common.object = theObjRef;
	_common_set_io_common.myParent = myPar;
	_common_set_io_common.descriptor = descror;
	_common_set_io_common.theMaster = master;

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxInputOutputConnectorCore::deactivate()
{
	_common_set_io_common.object = NULL;
	_common_set_io_common.myParent = NULL;
	_common_set_io_common.theMaster = NULL;
	_common_set_io_common.descriptor.clear();

	return JVX_NO_ERROR;
}
