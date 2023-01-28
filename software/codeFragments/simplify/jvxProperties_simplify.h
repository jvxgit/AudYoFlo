// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
// Interface CjvxProperties
// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --

virtual jvxErrorType JVX_CALLINGCONVENTION number_properties(
	jvxCallManagerProperties& callGate,
	jvxSize* num)override
{
	return _number_properties(callGate, num);
}

virtual jvxErrorType JVX_CALLINGCONVENTION get_revision(
	jvxCallManagerProperties& callGate,
	jvxSize* revision)override
{
	return _get_revision(callGate, revision);
}

virtual jvxErrorType JVX_CALLINGCONVENTION lock_properties(
	jvxCallManagerProperties& callGate)override
{
	return _lock_properties(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION trylock_properties(jvxCallManagerProperties& callGate)override
{
	return _trylock_properties(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION unlock_properties(jvxCallManagerProperties& callGate)override
{
	return _unlock_properties(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION get_meta_flags(
	jvxCallManagerProperties& callGate,
	jvxAccessRightFlags_rwcd* access_flags,
	jvxConfigModeFlags* mode_flags,
	const jvx::propertyAddress::IjvxPropertyAddress& ident) override
{
		return _get_meta_flags(callGate, access_flags, mode_flags, ident);
}

virtual jvxErrorType JVX_CALLINGCONVENTION set_meta_flags(
	jvxCallManagerProperties& callGate,
	jvxAccessRightFlags_rwcd* access_flags,
	jvxConfigModeFlags* mode_flags,
	const jvx::propertyAddress::IjvxPropertyAddress& ident) override
{
	return _set_meta_flags(callGate, access_flags, mode_flags, ident);
}

/*
		*
virtual jvxErrorType get_flagtag_property(jvxSize propId,
	jvxPropertyCategoryType category, jvxFlagTag* value_return,
	jvxFlagTag flagtag,
	jvxAccessProtocol* accProt )override
{
	return(_get_flagtag_property(propId, category, value_return, flagtag, accProt,
		static_cast<IjvxObject*>(this)));
};

virtual jvxErrorType set_flagtag_property(jvxSize propId,
	jvxPropertyCategoryType category, jvxFlagTag* value_return,
	jvxFlagTag flagtag ,
	jvxAccessProtocol* accProt )override
{
	return(_set_flagtag_property(propId, category, value_return,
		flagtag , accProt, static_cast<IjvxObject*>(this)));
};
*/

#if 0
virtual jvxErrorType JVX_CALLINGCONVENTION description_property(
	jvxCallManagerProperties& callGate,
	jvxSize idx,
	jvxPropertyCategoryType* category,
	jvxUInt64* settype,
	jvxUInt64* stateAccess,
	jvxDataFormat* format,
	jvxSize* num,
	jvxPropertyAccessType* accessType,
	jvxPropertyDecoderHintType* decTp,
	jvxSize* globalIdx,
	jvxPropertyContext* ctxt,
	jvxApiString* name,
	jvxApiString* description,
	jvxApiString* descriptor,
	jvxBool* isValid,
	jvxFlagTag* accessFlags)override
{
	jvxPropertyDescriptor theDescr;
	jvx::propertyAddress::CjvxPropertyAddressLinear ident(idx);
	jvxErrorType res = _description_property(
		callGate,
		&theDescr,
		&ident,
		static_cast<IjvxObject*>(this));
	if (res == JVX_NO_ERROR)
	{
		if (category) *category = theDescr.category;
		if (settype) *settype = theDescr.allowedStateMask;
		if (stateAccess) *stateAccess = theDescr.allowedThreadingMask;
		if (format) *format = theDescr.format;
		if (num) *num = theDescr.num;
		if (accessType) *accessType = theDescr.accessType;
		if (decTp) *decTp = theDescr.decTp;
		if (globalIdx) *globalIdx = theDescr.globalIdx;
		if (ctxt) *ctxt = theDescr.ctxt;
		if (name) *name  = theDescr.name;
		if (description) *description = theDescr.description;
		if (descriptor) *descriptor  = theDescr.descriptor;
		if (isValid) *isValid = theDescr.isValid;
		if (accessFlags) *accessFlags = theDescr.accessFlags;
	}
	return res;

	/*idx,
				     category,
				     settype,
				     stateAccess,
				     format,
				     num,
				     accessType,
				     decTp,
				     globalIdx,
				     ctxt,
					 name,
				     description,
				     descriptor,
				     isValid,
		accessFlags,
		static_cast<IjvxObject*>(this)));
		*/
}
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION description_property(
	jvxCallManagerProperties& callGate,
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident) override
{	
	jvxErrorType res = _description_property(
		callGate,
		theDescr,
		ident);	
	return res;
}

virtual jvxErrorType JVX_CALLINGCONVENTION get_property_extended_info(
	jvxCallManagerProperties& callGate, jvxHandle* fld,
		jvxSize requestId,
		const jvx::propertyAddress::IjvxPropertyAddress& ident)override
{
	return(_get_property_extended_info(callGate, fld,
		requestId,
		ident));
}

virtual jvxErrorType JVX_CALLINGCONVENTION translate_group_id(
	jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr) override
{
	return(_translate_group_id(callGate, gId, astr));
}

virtual jvxErrorType JVX_CALLINGCONVENTION install_property_reference(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)override
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _install_property_reference(
		callGate,
		ptrRaw,
		ident);

	return res;
}

virtual jvxErrorType JVX_CALLINGCONVENTION uninstall_property_reference(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)override
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _uninstall_property_reference(
		callGate,
		ptrRaw,
		ident);
	return res;
}

#ifndef JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET

virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& detail)override
{
	jvxErrorType res = JVX_NO_ERROR;
	return(_set_property(callGate, rawPtr, ident, detail));
}
#endif

#ifndef JVX_PROPERTY_SIMPLIFY_OVERWRITE_GET

virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)override
{
	return(_get_property(callGate, rawPtr, ident, trans));
}

#endif

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
	jvxSize propId)override

{
	return(_request_takeover_property(
		callGate,
		tpFrom,
		objFrom,
		fld,
		numElements,
		format,
		offsetStart,
		onlyContent,
		category,
		propId));
}

/*
virtual jvxErrorType JVX_CALLINGCONVENTION freeze_properties(jvxSize *props,
							     jvxPropertyCategoryType *cats,
							     jvxSize num,
	jvxFlagTag flagtag,
	jvxAccessProtocol* accProt)override
{
	return(_freeze_properties(props,
				  cats,
				  num, flagtag, accProt, static_cast<IjvxObject*>(this)));
}

virtual jvxErrorType JVX_CALLINGCONVENTION unfreeze_properties(jvxSize *props,
							       jvxPropertyCategoryType *cats,
							       jvxSize num,
	jvxFlagTag flagtag,
	jvxAccessProtocol* accProt)override
{
	return(_unfreeze_properties(props,
				    cats,
				    num, flagtag, accProt, static_cast<IjvxObject*>(this)));
}
*/

virtual jvxErrorType JVX_CALLINGCONVENTION reset_property_group(jvxCallManagerProperties& callGate)override
{
	return _reset_property_group(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION start_property_group(jvxCallManagerProperties& callGate)override
{
	return _start_property_group(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION status_property_group(jvxCallManagerProperties& callGate, jvxSize* startCnt)override
{
	return _status_property_group(callGate, startCnt);
}

#ifndef JVX_PROPERTY_SIMPLIFY_OVERWRITE_STOP_GROUP
virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_group(jvxCallManagerProperties& callGate)override
{
	return _stop_property_group(callGate);
}
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION install_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport, const char* prefix) override
{
	return _install_ref_property_report_collect(callGate, extReport, prefix);
}

virtual jvxErrorType JVX_CALLINGCONVENTION uninstall_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport) override
{
	return _uninstall_ref_property_report_collect(callGate, extReport);
}

virtual jvxErrorType JVX_CALLINGCONVENTION start_property_report_collect(jvxCallManagerProperties& callGate) override
{
	return _start_property_report_collect(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_report_collect(jvxCallManagerProperties& callGate) override
{
	return _stop_property_report_collect(callGate);
}

virtual jvxErrorType JVX_CALLINGCONVENTION max_property_span(jvxSize* idSpan) override
{
	return _max_property_span(idSpan);
}