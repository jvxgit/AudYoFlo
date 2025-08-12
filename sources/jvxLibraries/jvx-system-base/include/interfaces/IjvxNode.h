#ifndef __IJVXNODE_H__
#define __IJVXNODE_H__

JVX_INTERFACE IjvxNode: public IjvxSimpleNode, public IjvxSystemStatus
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxNode(){};	
};

// Hint: earlier denoted as IjvxMainComponent. Since it has no function, the extra layer was removed.
// typedef IjvxNode IjvxMainComponent;

#endif
