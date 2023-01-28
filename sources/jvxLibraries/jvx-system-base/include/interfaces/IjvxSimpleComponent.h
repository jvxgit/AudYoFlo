#ifndef __IJVXSIMPLECOMPONENT_H__
#define __IJVXSIMPLECOMPONENT_H__

JVX_INTERFACE IjvxSimpleComponent : public IjvxInterfaceFactory, public IjvxCoreStateMachine, public IjvxSystemStatus
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxSimpleComponent() {};
};

#endif
