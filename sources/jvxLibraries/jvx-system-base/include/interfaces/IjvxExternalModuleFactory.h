#ifndef __IJVXEXTERNAL_MODULE_FACTORY_H__
#define __IJVXEXTERNAL_MODULE_FACTORY_H__

JVX_INTERFACE IjvxExternalModuleFactory
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxExternalModuleFactory() {};

	// Trigger an "invite" to include an external component. If isInvite is true it is the invite, if false, it is the "kick-out"
	virtual jvxErrorType JVX_CALLINGCONVENTION invite_external_components(IjvxHiddenInterface* hostRef, jvxBool inviteToJoin) = 0;
};

#endif
