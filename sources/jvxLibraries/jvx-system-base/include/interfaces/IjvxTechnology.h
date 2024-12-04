#ifndef __IJVXTECHNOLOGY_H__
#define __IJVXTECHNOLOGY_H__

JVX_INTERFACE IjvxTechnology: public IjvxNode
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxTechnology(){};

	//! Request a reference to a device. A typical device is handled straight forward. If a proxy device is in use, requesting a device may 
	//! yield a new instance to allow dynamic devices. 
	//! We may reference a proxy device directly as well if argument requestFlags contains bit set at jvxRequestDeviceOptions::JVX_REQUEST_REFERENCE_PROXY_SHIFT
	virtual jvxErrorType JVX_CALLINGCONVENTION request_device(jvxSize idx, IjvxDevice** dev, jvxCBitField requestFlags = 0) = 0;

	//! Return the device
	virtual jvxErrorType JVX_CALLINGCONVENTION return_device(IjvxDevice*) = 0;

	// This function is used to return the id of a given selected device. We use this
	// to allow re-arrangement of the available devices: do not rely on the initial 
	// id, instead, return new value on request if elements have shown up!
	virtual jvxErrorType JVX_CALLINGCONVENTION ident_device(jvxSize* id, IjvxDevice*) = 0;

	// Always request number and the availabl devices within the main thread. 
	// The number and order of devices may switch but not within a call in the
	// main thread
	virtual jvxErrorType JVX_CALLINGCONVENTION number_devices(jvxSize*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION name_device(jvxSize, jvxApiString* name, jvxApiString* fName = nullptr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION description_device(jvxSize, jvxApiString*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_device(jvxSize, jvxApiString*, jvxApiString* substr = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION module_name_device(jvxSize, jvxApiString*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION capabilities_device(jvxSize, jvxDeviceCapabilities& caps) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION location_info_device(jvxSize, jvxComponentIdentification& tpId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION status_device(jvxSize idx, jvxState* stat) = 0;
};

#endif
