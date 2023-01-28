#ifndef __IJVXACCESSPROPERTIES_H__
#define __IJVXACCESSPROPERTIES_H__

typedef enum
{
	JVX_PROPERTY_ACCESS_DELAYED_RESPONSE_SHIFT = 0x0,
} jvxAccessPropertiesFeatures;


JVX_INTERFACE IjvxAccessProperties_delayed_report
{
	
public:
	virtual ~IjvxAccessProperties_delayed_report() {};
	virtual jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr) = 0;
	virtual jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle* privatePtr) = 0;
};

JVX_INTERFACE IjvxAccessProperties
{
public:
	virtual ~IjvxAccessProperties() {};

	virtual jvxErrorType reference_status(jvxBool* hasValidObjectRef, jvxBool* hasValidPropertyRef) = 0;

	virtual jvxErrorType get_features(jvxBitField* theFeatures) = 0;

	virtual jvxErrorType get_reference_component_description(
		jvxApiString* descror, 
		jvxApiString* descrion, 
		jvxApiString* modref, 
		jvxComponentIdentification* cpId) = 0;

	// ====================================================================================================================
	// ====================================================================================================================

	virtual jvxErrorType create_refresh_property_cache(
		jvxCallManagerProperties& callGate,
		const char* descriptor_expr,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType clear_property_cache() = 0;

	// ====================================================================================================================
	// Browse properties
	// ====================================================================================================================

	virtual jvxErrorType get_number_properties(jvxCallManagerProperties& callGate, jvxSize* num, 
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	// jvx_getPropertyDescription(theRefTriple, descr, propname.c_str());
	virtual jvxErrorType get_descriptor_property(
		jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;
	
	virtual jvxErrorType install_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType uninstall_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans = jvx::propertyDetail::CjvxTranferDetail(),
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans = jvx::propertyDetail::CjvxTranferDetail(),
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	// property_extended_info
	virtual jvxErrorType get_property_extended_info(jvxCallManagerProperties& callGate,
		jvxHandle* fld, jvxSize requestId, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident, 
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType translate_group_id(
		jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr) = 0;

	virtual jvxErrorType get_revision(jvxSize*rev, jvxCallManagerProperties& callGate) = 0;

	// ====================================================================================================================
	// The following functions are only useful for feature JVX_PROPERTY_ACCESS_NONBLOCKING
	// ====================================================================================================================

	virtual jvxErrorType property_start_delayed_group(
		jvxCallManagerProperties& callGate,
		IjvxAccessProperties_delayed_report* theRef = nullptr, jvxHandle* priv = nullptr,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType property_stop_delayed_group(
		jvxCallManagerProperties& callGate , jvxHandle** priv = nullptr,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_user_data(jvxSize idUserData, jvxHandle* floating_pointer) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION user_data(jvxSize idUserData, jvxHandle** floating_pointer) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION state_object(jvxState* stat) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION description_object(jvxApiString* astr ) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_object(jvxApiString* astr = NULL, jvxApiString* tagstr = NULL) = 0;
};

#endif
