#ifndef __IJVXMANIPULATE_H__
#define __IJVXMANIPULATE_H__

JVX_INTERFACE IjvxManipulate
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxManipulate(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_manipulate_value(jvxSize id, jvxVariant* varray) = 0;	
	virtual jvxErrorType JVX_CALLINGCONVENTION get_manipulate_value(jvxSize id, jvxVariant* varray) = 0;	
};

#endif