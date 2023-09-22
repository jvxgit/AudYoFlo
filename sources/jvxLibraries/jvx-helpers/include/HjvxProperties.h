#ifndef __HJVXCONFIGPROPERTIES_H__
#define __HJVXCONFIGPROPERTIES_H__

struct jvx_propertyReferenceTriple
{
	IjvxObject* theObj;
	jvxHandle* theHdl;
	IjvxProperties* theProps;
	jvxComponentIdentification cpId;
	jvx_propertyReferenceTriple()
	{
		theObj = nullptr;
		theHdl = nullptr;
		theProps = nullptr;
	};
};

class jvx_componentIdPropsCombo
{
public:
	jvx_propertyReferenceTriple* refTriple;
	IjvxAccessProperties* accProps;
public:
	jvx_componentIdPropsCombo()
	{
		reset();
	}

	void reset()
	{
		refTriple = NULL;
		accProps = NULL;
	};
};
void jvx_initPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple);

bool jvx_isValidPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple);
bool jvx_isValidNoPropertyReferenceTriple(jvx_propertyReferenceTriple* theTuple);

bool jvx_findPropertyDescriptor(
	const std::string& token, jvxSize* id, 
	jvxDataFormat* form = nullptr, 
	jvxInt32* numElements = nullptr, 
	jvxPropertyDecoderHintType* decHt = nullptr, 
	std::string* descriptor = nullptr);

bool jvx_findPropertyDescriptorId(jvxSize id, jvxDataFormat* form, jvxInt32* numElements, jvxPropertyDecoderHintType* decHt);

bool jvx_findPropertyNameId(jvxSize id, std::string& name);

jvxErrorType jvx_componentIdentification_properties_direct_add(
	IjvxHost* hHost,
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp, jvx_componentIdPropsCombo* newRefElm);

jvxErrorType jvx_componentIdentification_properties_find(
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp, jvx_componentIdPropsCombo& newRefElm);

jvxErrorType jvx_componentIdentification_properties_erase(
	IjvxHost* hHost,
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>& theRegisteredComponentReferenceTriples,
	jvxComponentIdentification tp);

bool JVX_CHECK_STATUS_PROPERTY_ONLY_READ(jvxBool isValid, jvxPropertyAccessType accessTp);
bool JVX_CHECK_STATUS_PROPERTY_ONLY_READ(jvxPropertyAccessType accessTp);

 jvxErrorType
jvx_pushPropertyStringList(std::vector<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, 
	jvxSize id, jvxPropertyCategoryType cat,
	jvxCallManagerProperties& callGate);

 jvxErrorType
jvx_pushPullPropertyStringList(std::vector<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, jvxSize id, 
	jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate);

jvxErrorType
jvx_pushPullPropertyStringList(std::list<std::string>& entries, IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat,
		 jvxCallManagerProperties& callGate);

jvxErrorType
jvx_pullPropertyStringList(std::vector<std::string>& entries,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, 
	jvxCallManagerProperties& callGate);

 jvxErrorType
jvx_pushPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat,
	jvxCallManagerProperties& callGate);

 jvxErrorType
jvx_pushPullPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate);

 jvxErrorType
jvx_pullPropertyString(std::string& entry,  IjvxObject* obj, IjvxProperties* prop, jvxSize id, jvxPropertyCategoryType cat, jvxCallManagerProperties& callGate);

 jvxExternalBuffer* jvx_allocate2DFieldExternalBuffer_full(jvxSize bLength,
	 jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	 jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	 jvxSize* szFld, jvxSize numBuffers, jvxSize seg_x, jvxSize seg_y);

 jvxExternalBuffer* jvx_allocate2DFieldExternalBuffer_inc(jvxSize bLength,
	 jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	 jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	 jvxSize* szFld, jvxSize numBuffers, jvxSize seg_x, jvxSize seg_y);

 jvxExternalBuffer* jvx_allocate1DCircExternalBuffer(jvxSize bLength,
	 jvxSize numChannels, jvxDataFormat form, jvxDataFormatGroup subform,
	 jvx_lock_buffer lockf, jvx_try_lock_buffer try_lockf, jvx_unlock_buffer unlockf,
	 jvxSize* szFld);

 void jvx_deallocateExternalBuffer(jvxExternalBuffer* header);

 void jvx_valuesToValueInRange(jvxValueInRange& theObj, jvxData minV = 0.0, jvxData maxV = 1.0, jvxData val = 0.0);

// ==================================================================

 void jvx_initPropertyDescription(jvxPropertyDescriptor& theStr);

jvxErrorType jvx_getPropertyDescription(
	IjvxProperties* theProps, 
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theStr, 
	jvx::propertyAddress::IjvxPropertyAddress& ident,
	jvxCallManagerProperties& callGate);

jvxErrorType jvx_getPropertyDescription(
	IjvxHost* theHost, 
	jvx::propertyDescriptor::IjvxPropertyDescriptor& theStr, 
	jvxComponentIdentification tp, 
	jvx::propertyAddress::IjvxPropertyAddress& ident,
	jvxCallManagerProperties& callGate);

jvxErrorType jvx_get_property(IjvxProperties* theProps, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	const char* descriptor,
	jvxCallManagerProperties& callGate);

jvxErrorType jvx_get_property(IjvxProperties* theProps, jvxHandle* fld,
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	jvxSize idxprop,
	jvxCallManagerProperties& callGate);

jvxErrorType jvx_props_property(IjvxProperties* theProps,
	jvxSize* numElements,
	jvxDataFormat* format,
	const char* descriptor,
	jvxCallManagerProperties& callGate);

 jvxErrorType jvx_get_property(IjvxHost* theHost, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	jvxComponentIdentification tp,
	const char* descriptor,
	 jvxCallManagerProperties& callGate);

 jvxErrorType jvx_set_property(IjvxProperties* theProps, jvxHandle* fld, 
	jvxSize offsetStart,
	jvxSize numElements,
	jvxDataFormat format,
	jvxBool contentOnly,
	const char* descriptor,
	 jvxCallManagerProperties& callGate);


// jvxErrorType jvx_status_get_property(IjvxProperties* theProps, const char* descriptor);
 
//jvxErrorType jvx_status_set_property(IjvxProperties* theProps, const char* descriptor);
 jvxErrorType jvx_install_property_reference(
	 IjvxProperties* theProps,
	 jvxCallManagerProperties& callGate,
	 jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	 jvx::propertyAddress::IjvxPropertyAddress& ident);

 jvxErrorType jvx_uninstall_property_reference(
	 IjvxProperties* theProps,
	 jvxCallManagerProperties& callGate,
	 jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
	 jvx::propertyAddress::IjvxPropertyAddress& ident);

jvxCBitField jvx_check_config_mode_put_configuration(jvxConfigModeFlags config_mode_inCall, jvxConfigModeFlags config_mode_inProperty,
	jvxAccessRightFlags access_right_inCall, jvxAccessRightFlags_rwcd access_right_inProperty);
jvxCBitField jvx_check_config_mode_get_configuration(jvxConfigModeFlags config_mode_inCall, jvxConfigModeFlags config_mode_inProperty,
	jvxAccessRightFlags access_right_inCall, jvxAccessRightFlags_rwcd access_right_inProperty);


namespace jvx {
	namespace helper
	{
		namespace properties
		{
			jvxErrorType valueInRangeFromString(jvxValueInRange* valRange, std::string txtToken);
			jvxErrorType selectionFromString(jvxSelectionList* selLst, std::string txtToken);
			std::string collectedPropsToString(const std::list<std::string>& propLst);
			std::list<std::string> stringToCollectedProps(const std::string& propLstStr);

#define JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_SELID_SHIFT 0
#define JVX_STRING_PROPERTY_FINE_TUNING_SELECTIONLIST_BITFIELD_SHIFT 1


			/*
			 This function is used to tranform a string previously received and pass it forward to a component
			 */
			jvxErrorType fromString(const jvx_propertyReferenceTriple& theTriple, jvxCallManagerProperties& callGate, 
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore& theDescr,
				std::string loadTarget, std::string loadTargetPP,
				jvxSize offsetStart, jvxCBitField fineTuningParams = 0);

			/*
			 * This function is used to get the property value from a string previously received and store it in a property pointer.
			 * This function is the core processing of the previous one.
			 */
			jvxErrorType fromString(jvxHandle* fld, jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
				jvxBool& contentOnly, jvxPropertyDecoderHintType decTp, std::string loadTarget, std::string loadTargetPP,
				jvxCBitField fineTuningParams = 0, jvxSize llsel = 0);

			jvxErrorType fromString(IjvxAccessProperties* propRef,
				jvxCallManagerProperties& callGate,
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore& theDescr,
				std::string loadTarget, std::string loadTargetPP,
				jvxSize offsetStart,
				jvxCBitField fineTuningParams = 0);
			/*
				This function is used to transform a property passed in as a pointer into a string representation.
				This function is the core processor from the previous function.
			*/
			jvxErrorType toString(jvxHandle* fld, jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
				jvxBool contentOnly, jvxPropertyDecoderHintType decTp, std::string& retVal, std::string& retValPostProcess,
				jvxSize numDigits, jvxBool binList, jvxContext* ctxt = NULL, jvxBool* noEntry = nullptr, jvxCBitField fineTuningParams = 0);

			/* This function is used to obtain a property value from a component and trasnform it into a string.*/
			jvxErrorType toString(const jvx_propertyReferenceTriple& theTriple, jvxCallManagerProperties& callGate, 
				const jvx::propertyDescriptor::CjvxPropertyDescriptorCore& theDescr, std::string& retVal, std::string& retValPostProcess, jvxSize numDigits,
				jvxBool binList = false, jvxSize offStart = 0, jvxSize numElms = JVX_SIZE_UNSELECTED, 
				jvxBool contentOnly = false, jvxBool* noEntry = nullptr, jvxCBitField fineTuningParams = 0);


			/* Function used to convert an array into a string */
			jvxErrorType toString(IjvxAccessProperties* propRef, jvxCallManagerProperties& callGate,
				const jvxPropertyDescriptor& theDescr, std::string& retVal,
				std::string& retValPostProcess,
				jvxSize numDigits, jvxBool binList = false, jvxSize offStart = 0, 
				jvxSize numElms = JVX_SIZE_UNSELECTED, jvxBool contentOnly = false, jvxBool* noEntry = nullptr,
				jvxCBitField fineTuningParams = 0);

			 void numSubEntriesSingleObjectField(jvxSize& numSubElements, jvxDataFormat format, jvxHandle* fld);
		}
	}
}

jvxErrorType jvx_selectionSetAdd(jvxPropertyContainerSingle<jvxSelectionList_cpp>&, const std::string& entry, jvxPropertySelectionListAddress addr);

// ============================================================================================
// Callbacks from the device or node to the control entity. The callback reference is set 
// via the set_property call
// ============================================================================================

#define JVX_PROPERTIES_DIRECT_C_CALLBACK_EXECUTE_FULL(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxHandle* privateData, jvxFloatingPointer* content) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(content)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxFloatingPointer* content)

#define JVX_PROPERTIES_DIRECT_C_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxHandle* privateData, jvxFloatingPointer* content); \
jvxErrorType \
ic_ ## callbackname(jvxFloatingPointer* content)

// ============================================================================================
// Callback definitions for all HOOK functions
// ============================================================================================

/*
#define JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE(baseclass, callbackname) jvxErrorType \
baseclass::callbackname(jvxCallManagerProperties& callGate, \
	const jvxHandle* fld, jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	jvxSize* id, jvxPropertyCategoryType* cat, jvxSize* offsetStart, jvxSize* numElements, \
	jvxDataFormat* form, jvxPropertyCallbackPurpose purp, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		if( (!id) || (!cat) || (!offsetStart) || (!numElements) ) return JVX_ERROR_INVALID_ARGUMENT; \
		return(this_pointer->ic_ ## callbackname(callGate, fld, rawPtr, id, cat, offsetStart, numElements, form, purp)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} 
*/

#define JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(baseclass, callbackname) jvxErrorType \
baseclass::callbackname( \
	jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune,  \
	jvxPropertyCallbackPurpose purp, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(callGate, rawPtr, ident, tune, purp)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
baseclass::ic_ ## callbackname(jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune, \
	jvxPropertyCallbackPurpose purp)

#define JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL_H(baseclass, callbackname) static jvxErrorType \
callbackname(jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune, \
	jvxPropertyCallbackPurpose purp, jvxHandle* privateData) \
{ \
	if (privateData) \
	{ \
		baseclass* this_pointer = reinterpret_cast<baseclass*>(privateData); \
		return(this_pointer->ic_ ## callbackname(callGate, rawPtr, ident, tune, purp)); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
ic_ ## callbackname(jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune, \
	jvxPropertyCallbackPurpose purp)


#define JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(callbackname) \
static jvxErrorType \
callbackname(jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune, \
	jvxPropertyCallbackPurpose purp, jvxHandle* privateData); \
jvxErrorType \
ic_ ## callbackname(jvxCallManagerProperties& callGate, \
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr, \
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident, \
	jvx::propertyDetail::CjvxTranferDetail& tune, \
	jvxPropertyCallbackPurpose purp)

#define JVX_PROPERTIES_FORWARD_C_CALLBACK_REGISTER(CLASS, mess, ...) CLASS::register_callbacks(static_cast<CjvxProperties*>(this), __VA_ARGS__, reinterpret_cast<jvxHandle*>(this), mess);

#define JVX_PROPERTY_CHECK_ID_CAT(idi, cati, genTp) ((idi == genTp.globalIdx) && (cati == genTp.category))
#define JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(prop) JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, prop)

#define JVX_PROPERTY_GET_SET(is_set, val_prop, val_app) \
	if(is_set) \
	{ \
		val_app = val_prop; \
	} \
	else \
	{ \
		val_prop = val_app; \
	}

#define JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, expr, sec, valPtr, processor) { \
	jvxConfigData* datTmp = NULL; 	\
	jvxValue val; \
	res = processor->getReferenceEntryCurrentSection_name(sec, &datTmp, expr); \
	if (res == JVX_NO_ERROR) \
	{ \
		processor->getAssignmentValue(datTmp, &val); val.toContent(valPtr); \
	} \
	else \
	{ \
		std::cout << "Failed to read config entry <" << expr << ">: " << jvxErrorType_txt(res) << std::endl; \
	}}

#define JVX_GET_CONFIGURATION_INJECT_VALUE(res, expr, sec, value, processor) { \
	jvxValue val; \
	jvxConfigData* datTmp = NULL; \
	val.assign(value); \
	res = processor->createAssignmentValue(&datTmp, expr, val); \
	if (datTmp) \
	{ \
		processor->addSubsectionToSection(sec, datTmp); \
	}}


#define JVX_PUT_CONFIGURATION_EXTRACT_STRING(res, expr, sec, strRef, processor) { \
	jvxConfigData* datTmp = NULL; 	\
	jvxApiString astr; \
	res = processor->getReferenceEntryCurrentSection_name(sec, &datTmp, expr); \
	if (res == JVX_NO_ERROR) \
	{ \
		processor->getAssignmentString(datTmp, &astr); strRef = astr.std_str(); \
	} \
	else \
	{ \
		std::cout << "Failed to read config entry <" << expr << ">: " << jvxErrorType_txt(res) << std::endl; \
	}}

#define JVX_GET_CONFIGURATION_INJECT_STRING(res, expr, sec, str, processor) { \
	jvxConfigData* datTmp = NULL; \
	res = processor->createAssignmentString(&datTmp, expr, str.c_str()); \
	if (datTmp) \
	{ \
		processor->addSubsectionToSection(sec, datTmp); \
	}}

#define JVX_NODE_PROP_CHECK_AND_SET_1FIELD(propa, propb, field, bval) if(propa.field != propb.field) {propa.field = propb.field; param_overwritten = true; }
#define JVX_NODE_PROP_CHECK_AND_SET_2FIELDS(propa, propb, field1, field2, bval) if(propa.field1 != propb.field2) {propa.field1 = propb.field2; param_overwritten = true; }

#define JVX_SET_PROPERTY_INDEX_POSTHOOK(idxSet, numSets, prop, strs, subfield) \
{ \
	if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, prop)) \
	{ \
			if(idxSet < numSets) \
			{ \
				strs[idxSet].subfield = prop.value; \
			} \
	} \
}

#define JVX_GET_PROPERTY_INDEX_PREHOOK(idxSet, numSets, prop, strs, subfield) \
{ \
	if (JVX_PROPERTY_CHECK_ID_CAT(*id, *cat, prop)) \
	{ \
			if(idxSet < numSets) \
			{ \
				prop.value = strs[idxSet].subfield; \
			} \
	} \
}

#define JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawData) \
		jvxHandle* fld = nullptr; \
		jvxSize numElements = 0; \
		jvxDataFormat format = JVX_DATAFORMAT_NONE; \
		const jvx::propertyRawPointerType::CjvxRawPointerType* ptrArgs = castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerType>(&rawData, JVX_DATAFORMAT_NONE); \
		assert(ptrArgs); \
		if(ptrArgs) \
		{ \
			fld = ptrArgs->raw(); \
			numElements = ptrArgs->size(); \
			format = ptrArgs->ftype(); \
		} 
#endif
