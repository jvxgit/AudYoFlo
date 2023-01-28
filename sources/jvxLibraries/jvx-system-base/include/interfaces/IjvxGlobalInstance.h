#ifndef __IJVXGLOBALINSTANCE__H__
#define __IJVXGLOBALINSTANCE__H__

JVX_INTERFACE IjvxGlobalInstance
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxGlobalInstance() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION single_instance_post_bootup(IjvxHiddenInterface* hostRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_instance_pre_shutdown() = 0;
};

#endif
