#ifndef __IJVXSTATEMACHINE_H__
#define __IJVXSTATEMACHINE_H__

JVX_INTERFACE IjvxDataProcessor;

// This is the level of CjvxObjectMin
JVX_INTERFACE IjvxCoreStateMachine
{

public:

	virtual JVX_CALLINGCONVENTION ~IjvxCoreStateMachine(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** dependsOn) = 0;
};

// This is the level of CjvxObject
JVX_INTERFACE IjvxStateMachine: public IjvxCoreStateMachine// , public IjvxInterfaceReference
{

public:

	virtual JVX_CALLINGCONVENTION ~IjvxStateMachine(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* suc, jvxApiString* reasonIfNot) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_alive(jvxBool* alive) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() = 0;

};

#endif
