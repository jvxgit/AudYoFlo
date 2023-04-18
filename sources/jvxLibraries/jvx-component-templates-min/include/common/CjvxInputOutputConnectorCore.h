#ifndef _CJVXINPUTOUTPUTCONNECTOR_H__
#define _CJVXINPUTOUTPUTCONNECTOR_H__

#include "jvx.h"

class CjvxInputOutputConnectorCore
{
public:
	class common_set_io_common_t
	{
	public:
		IjvxObject* object = nullptr;
		std::string descriptor;

		IjvxConnectorFactory* myParent = nullptr;
		IjvxConnectionMaster* theMaster = nullptr;
		jvxSize myRuntimeId = JVX_SIZE_UNSELECTED;

	};

	common_set_io_common_t _common_set_io_common;

	CjvxInputOutputConnectorCore();

	jvxErrorType _descriptor_connector(jvxApiString* str);
	jvxErrorType _set_parent_factory(IjvxConnectorFactory* my_parent);

	jvxErrorType _parent_factory(IjvxConnectorFactory** my_parent);

	jvxErrorType activate(IjvxObject* theObjRef,
		IjvxConnectorFactory* myPar,
		IjvxConnectionMaster* master,
		const std::string& descror);
	jvxErrorType deactivate();
};

#endif