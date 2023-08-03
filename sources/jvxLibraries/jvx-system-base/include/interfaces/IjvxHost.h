#ifndef __IJVXHOST_H__
#define __IJVXHOST_H__

JVX_INTERFACE IjvxHost: public IjvxFactoryHost, 
	public IjvxComponentHost // <- this organizes the sub components in a grid
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHost(){};	
};

#endif
