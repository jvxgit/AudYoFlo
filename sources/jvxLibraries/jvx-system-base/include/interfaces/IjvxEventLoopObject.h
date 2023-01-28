#ifndef __IJVX_EVENTLOOPOBJECT_H__
#define __IJVX_EVENTLOOPOBJECT_H__

// ==========================================================

JVX_INTERFACE IjvxEventLoopObject: public IjvxEventLoop, public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxEventLoopObject(){};
		
};

#endif
