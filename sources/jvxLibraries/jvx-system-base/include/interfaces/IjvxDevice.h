#ifndef __IJVXDEVICE_H__
#define __IJVXDEVICE_H__

JVX_INTERFACE IjvxDevice: public IjvxNode
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxDevice(){};
	virtual jvxErrorType JVX_CALLINGCONVENTION capabilities_device(jvxDeviceCapabilities& caps) = 0;

	/*
	// Some additional functions to better 
	virtual jvxErrorType JVX_CALLINGCONVENTION filterOptionsDevice(jvxBool* avail = nullptr, 
		jvxSize* numDataStreams = nullptr, 
		jvxSize* assSlot = nullptr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION filterOptionsDataStream(jvxSize idx = 0, 
		jvxDeviceCapabilities* caps = nullptr, 
		jvxDataFormatGroup* dataGroup = nullptr, 
		jvxApiString* astr = nullptr) = 0;
	*/
};

#endif
