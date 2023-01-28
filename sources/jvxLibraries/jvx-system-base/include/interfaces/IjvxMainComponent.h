#ifndef __IJVXMAINCOMPONENT_H__
#define __IJVXMAINCOMPONENT_H__

JVX_INTERFACE IjvxMainComponent: public IjvxInterfaceFactory, public IjvxStateMachine, public IjvxSystemStatus
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxMainComponent(){};

};

#endif