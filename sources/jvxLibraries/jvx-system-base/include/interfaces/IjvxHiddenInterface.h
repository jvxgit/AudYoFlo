#ifndef __IJVXHIDDENINTERFACE_H__
#define __IJVXHIDDENINTERFACE_H__

// This class attaches an object reference to an interface to allow to get more information in case only
// the interface pointer is available. It should be, however, also allowed to return a nullptr reference!
JVX_INTERFACE IjvxInterfaceReference
{
public:
	virtual ~IjvxInterfaceReference(){};
	virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) = 0;
};

// This interfaces branches to all sub interfaces. An object may expose different kinds of interfaces.
// The returned pointer MUST be handled with care since it is a blank pointer reference. Make sure
// all calls are wrapped into a call to the casting function <reqInterface> or <reqInterfaceObj>
// defined in HjvxCast.h to avoid pointer problems!
JVX_INTERFACE IjvxHiddenInterface
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHiddenInterface(){};
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType, jvxHandle**) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType, jvxHandle*) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) = 0;
};

#endif
