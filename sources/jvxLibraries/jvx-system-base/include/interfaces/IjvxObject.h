#ifndef __IJVXOBJECT_H__
#define __IJVXOBJECT_H__

// Forward declaration
JVX_INTERFACE IjvxInterfaceFactory;

JVX_INTERFACE IjvxOwner
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxOwner() {};

	virtual jvxErrorType register_object(IjvxObject* reg_me) = 0;
	virtual jvxErrorType unregister_object(IjvxObject* unreg_me) = 0;
};

JVX_INTERFACE IjvxObject: public IjvxInformation, public IjvxOwner // , public IjvxInterfaceReference
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxObject(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION check_access(jvxCallManager& callGate) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_location_info(const jvxComponentIdentification& tp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION location_info(jvxComponentIdentification& tp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION state(jvxState*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION lock_state() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION try_lock_state() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unlock_state() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION name(jvxApiString*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION description(jvxApiString*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor(jvxApiString*, jvxApiString* = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION module_reference(jvxApiString*, jvxComponentAccessType* acTp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION compile_options(jvxApiString* str) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class(jvxBitField* ft) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION version(jvxApiString*) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION lasterror(jvxApiError*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_specialization(jvxHandle** obj, jvxComponentIdentification*, jvxBool* multipleObjects, jvxApiString* = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_user_data(jvxSize idUserData, jvxHandle* floating_pointer) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION user_data(jvxSize idUserData, jvxHandle** floating_pointer) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_unique_object_id(jvxSize* uId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_factory(IjvxInterfaceFactory** ref) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION increment_reference() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION current_reference(jvxSize* cnt) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION decrement_reference() = 0;

};

#endif
