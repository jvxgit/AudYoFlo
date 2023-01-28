#ifndef __IJVXPROPERTIES_H__
#define __IJVXPROPERTIES_H__

JVX_INTERFACE IjvxProperties_report
{
public:
	~IjvxProperties_report() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION report_properties_modified(const char* props_set) = 0;
};

JVX_INTERFACE IjvxProperties: public IjvxInterfaceReference
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxProperties(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_properties(
		jvxCallManagerProperties& callGate,
		jvxSize* num) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_revision(
		jvxCallManagerProperties& callGate,
		jvxSize* revision) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION lock_properties(
		jvxCallManagerProperties& callGate) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION trylock_properties(
		jvxCallManagerProperties& callGate) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unlock_properties(
		jvxCallManagerProperties& callGate) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property_extended_info(
		jvxCallManagerProperties& callGate,
		jvxHandle* fld,
		jvxSize requestId,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION get_meta_flags(
		jvxCallManagerProperties& callGate,
		jvxAccessRightFlags_rwcd* access_flags,
		jvxConfigModeFlags* mode_flags,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_meta_flags(
		jvxCallManagerProperties& callGate,
		jvxAccessRightFlags_rwcd* access_flags,
		jvxConfigModeFlags* mode_flags,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION description_property(
		jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail = jvx::propertyDetail::CjvxTranferDetail()) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail = jvx::propertyDetail::CjvxTranferDetail()) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION install_property_reference(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION uninstall_property_reference(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_takeover_property(
		jvxCallManagerProperties& callGate,
		const jvxComponentIdentification& tpFrom,
		IjvxObject* objFrom,
		jvxHandle* fld,
		jvxSize numElements,
		jvxDataFormat format,
		jvxSize offsetStart,
		jvxBool onlyContent,
		jvxPropertyCategoryType category,
		jvxSize propId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION translate_group_id(
		jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION max_property_span(jvxSize* idSpan) = 0;

	// Property group functions
	virtual jvxErrorType JVX_CALLINGCONVENTION reset_property_group(jvxCallManagerProperties& callGate) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_property_group(jvxCallManagerProperties& callGate) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_group(jvxCallManagerProperties& callGate) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION status_property_group(jvxCallManagerProperties& callGate, jvxSize* startCnt) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION install_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport, const char* prefix) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION uninstall_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_property_report_collect(jvxCallManagerProperties& callGate) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_report_collect(jvxCallManagerProperties& callGate) = 0;

};

#endif
