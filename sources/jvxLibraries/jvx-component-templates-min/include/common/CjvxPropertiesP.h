#ifndef __CJVXPROPERTIESP_H__
#define __CJVXPROPERTIESP_H__

#include "jvx.h"

typedef enum
{
	JVX_PROPERTY_CALLBACK_SET,
	JVX_PROPERTY_CALLBACK_GET,
	JVX_PROPERTY_CALLBACK_INSTALL,
	JVX_PROPERTY_CALLBACK_UNINSTALL,
	JVX_PROPERTY_CALLBACK_DIRECT
} jvxPropertyCallbackPurpose;

typedef jvxErrorType (*property_callback)(
	jvxCallManagerProperties* callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType*& rawPtr,
	const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident,
	jvx::propertyDetail::CjvxTranferDetail& tune,
	/*
	jvxSize* offset,
	jvxSize* number, jvxDataFormat* form, 
	*/
	jvxPropertyCallbackPurpose purp,
	jvxHandle* privateData);

struct oneEntryTranslate
{
	jvxPropertyDecoderHintType tpUi;
	jvx::externalBufferType tpHd;
} ;

static oneEntryTranslate entriesTranslateExtBuffers[] = 
{
	{JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER, jvx::JVX_EXTERNAL_BUFFER_1D_CIRCULAR_BUFFER},
	{ JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER, jvx::JVX_EXTERNAL_BUFFER_2D_FIELD_BUFFER },
	{ JVX_PROPERTY_DECODER_NONE, jvx::JVX_EXTERNAL_BUFFER_NONE}
};



class CjvxProperties: public IjvxProperties
{
public:

	// ==================================================================================
	class oneSubModule
	{
	public:
		IjvxProperties* propRef = nullptr;
		IjvxObject* objRef = nullptr;
		jvxSize offsetShift = 0;
		std::string propDescriptorTag;
		std::string propDescriptionPrefix;

		bool operator < (const oneSubModule& str) const
		{
			return (offsetShift < str.offsetShift);
		};
	};

	// ================================================================================
	class propAddressing
	{
	public:
		const jvx::propertyAddress::CjvxPropertyAddressLinear* addrLin = nullptr;
		const jvx::propertyAddress::CjvxPropertyAddressDescriptor* addrDescr = nullptr;
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId* addrIdx = nullptr;

		std::string loc_descriptor;

		void reset()
		{
			addrLin = nullptr;
			addrDescr = nullptr;
			addrIdx = nullptr;
		}

		void cast(const jvx::propertyAddress::IjvxPropertyAddress* addrProp)
		{
			addrLin = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressLinear>(addrProp);
			addrDescr = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressDescriptor>(addrProp);
			addrIdx = castPropAddress<const jvx::propertyAddress::CjvxPropertyAddressGlobalId>(addrProp);
		}

		// =====================================================================

		jvxErrorType prepLin(jvx::propertyAddress::CjvxPropertyAddressLinear& addrLinCp, jvxSize cnt)
		{
			assert(JVX_CHECK_SIZE_SELECTED(addrLin->idx));
			if (addrLin->idx >= cnt)
			{
				addrLinCp.reset(addrLin->idx - cnt);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		};

		// ==========================================================

		jvxErrorType prepDescr(jvx::propertyAddress::CjvxPropertyAddressDescriptor& addrDescrCp, const std::string& propDescrTag)
		{
			std::vector<std::string> lst;
			loc_descriptor = addrDescr->descriptor;
			jvx_decomposePathExpr(loc_descriptor, lst);
			if (lst.size() > 0)
			{
				if (lst[0] == propDescrTag)
				{
					jvx_composePathExpr(lst, loc_descriptor, 1);
					addrDescrCp.reset(loc_descriptor.c_str());
					return JVX_NO_ERROR;
				}
			}
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		};		

		// ===================================================================================

		jvxErrorType prepIdx(jvx::propertyAddress::CjvxPropertyAddressGlobalId& addrIdxCp, jvxSize offset)
		{
			if (addrIdx->id > offset)
			{
				addrIdxCp.reset(addrIdx->id - offset, addrIdx->cat);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}		
	};

	class propDescriptors
	{
	public:
		// Check the type of output
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dFull = nullptr;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* dFullPlus = nullptr;
		jvx::propertyDescriptor::CjvxPropertyDescriptorControl* dContr = nullptr;
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore* dCore = nullptr;
		jvx::propertyDescriptor::CjvxPropertyDescriptorMin* dMin = nullptr;

		void reset()
		{
			dFull = nullptr;
			dContr = nullptr;
			dCore = nullptr;
			dMin = nullptr;
		}
		
		void cast(jvx::propertyDescriptor::IjvxPropertyDescriptor* descr)
		{
			dFullPlus = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus>(descr);
			dFull = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorFull>(descr);
			dContr = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(descr);
			dCore = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(descr);
			dMin = castPropDescriptor<jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(descr);
		}
	};

	class pointerReferences
	{
	public:
		jvxHandle* fld = nullptr;
		jvxHandle** fld_ext = nullptr;
		jvxCallbackPrivate** fld_prop_cb = nullptr;
		
		jvxBool* isValidPtr = nullptr;
		jvxAccessRightFlags_rwcd* accessFlagsPtr = nullptr;
		jvxConfigModeFlags* configModeFlagsPtr = nullptr;

		void reset()
		{
			fld = nullptr;
			fld_ext = nullptr;
			fld_prop_cb = nullptr;
			
			isValidPtr = nullptr;
			accessFlagsPtr = nullptr;
			configModeFlagsPtr = nullptr;
			
		};
	};

	class callbackReferences
	{
	public:
		std::string callback_set_posthook;
		std::string callback_set_prehook;
		std::string callback_get_prehook;
		std::string callback_get_posthook;
		void reset()
		{
			callback_set_posthook.clear();
			callback_set_prehook.clear();
			callback_get_prehook.clear();
			callback_get_posthook.clear();
		};
	};

protected:
	typedef struct
	{
		std::string tag;
		property_callback theCallback;
		jvxHandle* priv;
	} oneCallback;

	class oneProperty
	{
	public:

		// Struct to hold external references
		pointerReferences references;
		callbackReferences callbacks;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* propDescriptor = nullptr;
		jvx::propertyRawPointerType::IjvxRawPointerType* rawpointer = nullptr;
		
		/*
		jvxAccessRightFlags_rwcd access_flags_initval = JVX_ACCESS_ROLE_DEFAULT;
		jvxConfigModeFlags config_mode_flags_initval = JVX_CONFIG_MODE_DEFAULT;
		*/
		jvxExtendedProps extended_props;

		struct
		{
			jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_NONE;
			jvxBool* isValidPtr = nullptr;
			jvxHandle* fld = nullptr;
			jvxSize num = 0;
			jvxBool autoset = false;
		} shadow;
		
	};

	struct _common_set_properties_t
	{
		JVX_MUTEX_HANDLE csec;
		jvxBool useLock;
		JVX_THREAD_ID threadId;
		std::list<oneProperty> registeredProperties;
		std::vector<oneCallback> registeredCallbacks;
		std::vector<oneSubModule> registeredSubmodules;
		jvxSize nextPropId;
		//jvxSize maxPropId;
		jvxSize propSetRevision;
		jvxBool wasStarted;
		jvxBool reportIfNoChange;
		jvxSize inPropertyGroupCnt;
		std::string moduleReference;
		jvxBool reportMissedCallbacks;
	};
	_common_set_properties_t _common_set_properties;

	IjvxObject* theObjReference = nullptr;

public:

	CjvxProperties(const std::string& moduleName, IjvxObject* objRef );

	 ~CjvxProperties();

 	  jvxErrorType _reset_properties();

	  jvxErrorType _reset_callbacks();

	  virtual jvxErrorType JVX_CALLINGCONVENTION number_properties(jvxCallManagerProperties* callGate, jvxSize* num) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  get_revision(jvxCallManagerProperties* callGate, jvxSize* revision) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  lock_properties(jvxCallManagerProperties* callGate)override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  trylock_properties(jvxCallManagerProperties* callGate) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION unlock_properties(jvxCallManagerProperties* callGate) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION description_property(
		  jvxCallManagerProperties* callGate,
		  jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
		  const jvx::propertyAddress::IjvxPropertyAddress& idProp) override;
	 
	  virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		   jvxCallManagerProperties* callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident,
		   const jvx::propertyDetail::CjvxTranferDetail& detail) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  get_property(
		   jvxCallManagerProperties* callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident,
		   const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  install_property_reference(
		   jvxCallManagerProperties* callGate, 
		   const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  uninstall_property_reference(
		   jvxCallManagerProperties* callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  get_property_extended_info(
		 jvxCallManagerProperties* callGate,
		 jvxHandle* fld,
		 jvxSize requestId,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  get_meta_flags(jvxCallManagerProperties* callGate,
		 jvxAccessRightFlags_rwcd* access_flags,
		 jvxConfigModeFlags* mode_flags,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION  set_meta_flags(jvxCallManagerProperties* callGate,
		 jvxAccessRightFlags_rwcd* access_flags,
		 jvxConfigModeFlags* mode_flags,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION request_takeover_property(
		  jvxCallManagerProperties* callGate,
		  const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		  jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		  jvxPropertyCategoryType category, jvxSize propId) override;

	  virtual jvxErrorType JVX_CALLINGCONVENTION status_property_group(
		  jvxCallManagerProperties* callGate,
		  jvxSize* startCnt) override;

	// ================================================================

	 jvxErrorType _initialize_properties(IjvxObject* hObj, bool useLock);

	 jvxErrorType _setThreadId_properties(JVX_THREAD_ID theThreadId);

	 jvxErrorType _register_callback(std::string tag, property_callback theCallback,jvxHandle* priv);

	 jvxErrorType _unregister_callback(std::string tag);

	 jvxErrorType _register_property(
		 jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* propDescr,
		 const pointerReferences& references,
		 const callbackReferences& callbacks,
		 jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw = nullptr);

	   jvxErrorType _set_property_extended_info(jvxHandle* fld,
		  jvxSize requestId,
		  jvxSize propId,
		  jvxPropertyCategoryType category);
	
	   jvxErrorType _update_property(jvxHandle* fld, jvxSize numberElements, jvxPropertyCategoryType cat, jvxSize idProp);

	   jvxErrorType _update_property_access_type(jvxPropertyAccessType accessType, jvxPropertyCategoryType cat, jvxSize idProp);

	   jvxErrorType _update_property_access_type_all(jvxPropertyAccessType accessType);

	   jvxErrorType _undo_update_property_access_type(jvxPropertyCategoryType cat, jvxSize idProp);

	   jvxErrorType _undo_update_property_access_type_all();

	   jvxErrorType _update_property_decoder_type(jvxPropertyDecoderHintType decTp, jvxPropertyCategoryType cat, jvxSize idProp);

	   jvxErrorType _unregister_property(jvxPropertyCategoryType cat, jvxSize idProp);


	 jvxErrorType _update_properties_on_start();
	 
	 jvxErrorType _update_properties_on_stop();

	 jvxErrorType _register_sub_module(IjvxProperties* theProps, IjvxObject* theObj, jvxSize offsetShft, std::string propDescriptorTag, std::string propDescriptionPrefix);

	 jvxSize _update_max_prop_id();

	 jvxErrorType _unregister_sub_module(std::string propDescriptorTag);

	 jvxErrorType reset_property_group(jvxCallManagerProperties* callGate) override;

	 jvxErrorType start_property_group(jvxCallManagerProperties* callGate) override;

	 jvxErrorType stop_property_group(jvxCallManagerProperties* callGate)override;

	// =========================================================================================
	 std::list<CjvxProperties::oneProperty>::iterator findInPropertyList(std::list<CjvxProperties::oneProperty>& lst, jvxSize propId, jvxPropertyCategoryType cat);

		// =========================================================================================
	 std::list<CjvxProperties::oneProperty>::iterator findInPropertyList(std::list<CjvxProperties::oneProperty>& lst, const char* descriptor);

		// =========================================================================================
	inline std::vector<CjvxProperties::oneCallback>::iterator findInCallbackList(std::vector<CjvxProperties::oneCallback>& lst, std::string tag);

	jvxErrorType _lock_properties_local();

	jvxErrorType _trylock_properties_local();

	jvxErrorType _unlock_properties_local();

	jvxErrorType _translate_address_idx(
		jvxCallManagerProperties* callGate,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		jvxSize& globIdx, jvxPropertyCategoryType& cat);

private:
	jvxErrorType _check_access_property_obj(jvxCallManager* callGate, jvxCheckAccessEnum accEnum = jvxCheckAccessEnum::JVX_CHECK_ACCESS_NONE);
	jvxErrorType _check_access_property(jvxCallManager* callGate, const oneProperty& prop, jvxCheckAccessEnum accEnum = jvxCheckAccessEnum::JVX_CHECK_ACCESS_NONE);

	jvxErrorType _install_match_external_buffer(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	jvxErrorType _uninstall_match_external_buffer(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	std::list<oneProperty>::iterator findSelectionFromAddress(propAddressing& addressOptions);

	jvxErrorType _number_properties_local(jvxCallManagerProperties* callGate, jvxSize* num);

	jvxErrorType _forward_submodules(jvxCallManagerProperties* callGate, propAddressing& addresses, 
		std::function<jvxErrorType(const jvx::propertyAddress::IjvxPropertyAddress& addr, jvxSize i)> func,
		std::function <void(jvxSize i)> func_post_success = nullptr);

	jvxErrorType _run_hook(const std::string& hookname, 
		const std::string& propname,
		jvxCallManagerProperties* callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType*& ptrRaw,
		const jvx::propertyAddress::CjvxPropertyAddressGlobalId& ident,
		jvx::propertyDetail::CjvxTranferDetail& tune,
		/*
		jvxDataFormat* format,
		jvxSize* offsetStart,
		jvxSize* numElements,
		*/
		jvxPropertyCallbackPurpose purp);
protected:


};

// Makro to ignore a property set with no impact but return positive result anyway
#define JVX_PROPERTY_CHECK_RESULT(res, callGate) \
	if((res == JVX_NO_ERROR) && (callGate->on_set.modification_impact == true))


#define UPDATE_PROPERTY_ACCESS_TYPE(accTp, prop) CjvxProperties::_update_property_access_type(accTp, prop.category, prop.globalIdx)
#define UNDO_UPDATE_PROPERTY_ACCESS_TYPE(prop) CjvxProperties::_undo_update_property_access_type(prop.category, prop.globalIdx)

#define JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category, obj) \
	jvxSize propId = JVX_SIZE_UNSELECTED; \
	jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN; \
	_translate_address_idx(callGate, ident, propId, category, obj)

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


