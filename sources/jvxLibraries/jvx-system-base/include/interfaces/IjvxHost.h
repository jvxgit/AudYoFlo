#ifndef __IJVXHOST_H__
#define __IJVXHOST_H__

JVX_INTERFACE IjvxHost: public IjvxFactoryHost, 
	public IjvxHostTypeHandler, // <- this organizes the host type classes etc
	public IjvxComponentHost // <- this organizes the sub components in a grid
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHost(){};

	//! Report when boot process is complete
	virtual jvxErrorType JVX_CALLINGCONVENTION boot_complete(jvxBool* bootComplete) = 0;
};

#endif
