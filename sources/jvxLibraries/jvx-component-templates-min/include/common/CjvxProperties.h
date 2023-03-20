#ifndef __CJVXPROPERTIES_H__
#define __CJVXPROPERTIES_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxPropertiesTypeConvert.h"

typedef enum
{
	JVX_PROPERTY_CALLBACK_SET,
	JVX_PROPERTY_CALLBACK_GET,
	JVX_PROPERTY_CALLBACK_INSTALL,
	JVX_PROPERTY_CALLBACK_UNINSTALL,
	JVX_PROPERTY_CALLBACK_DIRECT
} jvxPropertyCallbackPurpose;

typedef jvxErrorType (*property_callback)(
	jvxCallManagerProperties& callGate, 
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

class CjvxPropertySubModule : public IjvxProperties_report
{
public:
	IjvxProperties* propRef = nullptr;
	IjvxProperties_report* parentReport = nullptr;

	// IjvxObject* objRef = nullptr;
	jvxSize offsetShift = 0;
	jvxSize propSpan = JVX_SIZE_UNSELECTED;

	// This is the key to store the submodule
	std::string propDescriptorTag;

	// This is the prefix as used in the tree
	std::string propDescriptionPrefix;

	bool operator < (const CjvxPropertySubModule& str) const
	{
		return (offsetShift < str.offsetShift);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_properties_modified(const char* props_set) override;
};

class CjvxProperties
{
public:

	// ==================================================================================
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
		JVX_THREAD_ID threadId = JVX_NULLTHREAD;
		std::list<oneProperty> registeredProperties;
		std::vector<oneCallback> registeredCallbacks;

		// This is a sorted list: the elements have increasing id offsets
		std::list<CjvxPropertySubModule*> registeredSubmodules;
		jvxSize nextPropId = JVX_PROPERTIES_OFFSET_UNSPECIFIC;
		jvxSize propIdSpan = JVX_SIZE_UNSELECTED;
		//jvxSize maxPropId;
		jvxSize propSetRevision = 0;
		jvxBool wasStarted = false;
		jvxBool reportIfNoChange = false;
		jvxSize inPropertyGroupCnt = 0;
		std::string moduleReference;
		jvxBool reportMissedCallbacks = false;		
	};
	_common_set_properties_t _common_set_properties;

	struct
	{
		int startCntStack = 0; // Alwas report on transition 1->0
		std::list<std::string> collectedProps;
		IjvxProperties_report* reportRef = nullptr;
		IjvxProperties_report* reportRef_stack = nullptr;
		std::string reportPrefix;
	} _common_set_property_report;

	CjvxObjectMin& theObjRef;
	std::ostream* jout = &std::cout;

	std::map<jvxSize, std::string> tableGroupIdTranslations;
	CjvxPropertiesTypeConvert convert;

public:

	CjvxProperties(const std::string& moduleName, CjvxObjectMin& objRef);

	 ~CjvxProperties();

 	  jvxErrorType _reset_properties();

	  jvxErrorType _reset_callbacks();

	  jvxErrorType _number_properties(jvxCallManagerProperties& callGate, jvxSize* num);

	   jvxErrorType _get_revision(jvxCallManagerProperties& callGate, jvxSize* revision);

	 jvxErrorType _lock_properties(jvxCallManagerProperties& callGate);

	  jvxErrorType _trylock_properties(jvxCallManagerProperties& callGate);

	  jvxErrorType _unlock_properties(jvxCallManagerProperties& callGate);

	  jvxErrorType _description_property(
		  jvxCallManagerProperties& callGate,
		  jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
		  const jvx::propertyAddress::IjvxPropertyAddress& idProp);
	 
	   jvxErrorType _set_property(
		   jvxCallManagerProperties& callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident,
		   const jvx::propertyDetail::CjvxTranferDetail& detail);

	   jvxErrorType _get_property(
		   jvxCallManagerProperties& callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident,
		   const jvx::propertyDetail::CjvxTranferDetail& detail);

	   jvxErrorType _install_property_reference(
		   jvxCallManagerProperties& callGate, 
		   const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident);

	   jvxErrorType _uninstall_property_reference(
		   jvxCallManagerProperties& callGate,
		   const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		   const jvx::propertyAddress::IjvxPropertyAddress& ident);

	 jvxErrorType _get_property_extended_info(
		 jvxCallManagerProperties& callGate,
		 jvxHandle* fld,
		 jvxSize requestId,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident);

	 jvxErrorType _get_meta_flags(jvxCallManagerProperties& callGate,
		 jvxAccessRightFlags_rwcd* access_flags,
		 jvxConfigModeFlags* mode_flags,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident);

	 jvxErrorType _set_meta_flags(jvxCallManagerProperties& callGate,
		 jvxAccessRightFlags_rwcd* access_flags,
		 jvxConfigModeFlags* mode_flags,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident);

	 jvxErrorType _max_property_span(jvxSize* idSpan);
	 
	// ================================================================

	 jvxErrorType _setThreadId_properties(JVX_THREAD_ID theThreadId);

	 jvxErrorType _register_callback(std::string tag, property_callback theCallback, jvxHandle* priv);

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

	 jvxErrorType _update_property_size(jvxSize numberElements, jvxPropertyCategoryType cat, jvxSize idProp, jvxBool lockOnChange);

	 jvxErrorType _update_property(jvxHandle* fld, jvxSize numberElements, jvxPropertyCategoryType cat, jvxSize idProp, jvxBool lockOnChange);

	 jvxErrorType _update_property_access_type(jvxPropertyAccessType accessType, jvxPropertyCategoryType cat, jvxSize idProp);

	 jvxErrorType _update_property_access_type_all(jvxPropertyAccessType accessType);

	 jvxErrorType _undo_update_property_access_type(jvxPropertyCategoryType cat, jvxSize idProp);

	 jvxErrorType _undo_update_property_access_type_all();

	 jvxErrorType _update_property_decoder_type(jvxPropertyDecoderHintType decTp, jvxPropertyCategoryType cat, jvxSize idProp);

	 jvxErrorType _unregister_property(jvxPropertyCategoryType cat, jvxSize idProp);

	 jvxErrorType _request_takeover_property(
		 jvxCallManagerProperties& callGate,
		 const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		 jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		 jvxPropertyCategoryType category, jvxSize propId);

	 jvxErrorType _translate_group_id(
		 jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr);

	 jvxErrorType _update_properties_on_start();

	 jvxErrorType _update_properties_on_stop();

	 jvxErrorType _register_sub_module(IjvxProperties* theProps, jvxSize offsetShft, std::string propDescriptorTag, std::string propDescriptionPrefix);

	 jvxSize _update_max_prop_id();

	 jvxErrorType _unregister_sub_module(std::string propDescriptorTag);
	 jvxErrorType _unregister_sub_module(IjvxProperties* arg);

	 jvxErrorType _reset_property_group(jvxCallManagerProperties& callGate);

	 jvxErrorType _start_property_group(jvxCallManagerProperties& callGate);

	 jvxErrorType _status_property_group(
		 jvxCallManagerProperties& callGate,
		 jvxSize* startCnt);

	 jvxErrorType _stop_property_group(jvxCallManagerProperties& callGate);

	 // =========================================================================================

	 jvxErrorType _install_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport, const char* prefix);

	 jvxErrorType _uninstall_ref_property_report_collect(jvxCallManagerProperties& callGate, IjvxProperties_report* extReport);

	 jvxErrorType _start_property_report_collect(jvxCallManagerProperties& callGate);

	 jvxErrorType _stop_property_report_collect(jvxCallManagerProperties& callGate);

	 jvxErrorType add_property_report_collect(const std::string& propDescr, jvxBool reportDescriptorChanged = false);

	 static std::string& property_changed_descriptor_tag_add(std::string& descriptor);

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
		 jvxCallManagerProperties& callGate,
		 const jvx::propertyAddress::IjvxPropertyAddress& ident,
		 jvxSize& globIdx, jvxPropertyCategoryType& cat);

	 jvxErrorType _translate_address_descriptor(
			 jvxCallManagerProperties& callGate,
			 const jvx::propertyAddress::IjvxPropertyAddress& ident,
			 jvxApiString& descr);

	 void resetPropertyIdCount();

private:
	jvxErrorType _check_access_property_obj(jvxCallManager& callGate, jvxCheckAccessEnum accEnum = jvxCheckAccessEnum::JVX_CHECK_ACCESS_NONE);
	jvxErrorType _check_access_property(jvxCallManager& callGate, const oneProperty& prop, jvxCheckAccessEnum accEnum = jvxCheckAccessEnum::JVX_CHECK_ACCESS_NONE);

	jvxErrorType _install_match_external_buffer(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	jvxErrorType _uninstall_match_external_buffer(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	jvxErrorType _install_match_callback(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	jvxErrorType _uninstall_match_callback(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this);

	template <class T> jvxErrorType _install_match_external(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
	{
		jvxErrorType res = JVX_NO_ERROR;

		const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T>* ptrRawInstall =
			castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T> >(
				selection->rawpointer, selection->propDescriptor->format);
		const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<T>* ptrRawToInstall =
			castPropRawPointer<const  jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<T> >(
				&install_this, selection->propDescriptor->format);

		if (ptrRawInstall && ptrRawToInstall)
		{
			T** ptrRawStore = ptrRawInstall->typedPointer();
			T* ptrRawIn = ptrRawToInstall->typedPointer();
			if (*ptrRawStore == nullptr)
			{
				*ptrRawStore = ptrRawIn;
			}
			else
			{
				res = JVX_ERROR_ALREADY_IN_USE;
			}
		} // if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
		else
		{
			// Error: pointers were of incorrect type
			res = JVX_ERROR_INVALID_FORMAT;
		}
		return res;
	}

	template <class T> jvxErrorType _uninstall_match_external(
		std::list<oneProperty>::iterator& selection,
		const jvx::propertyRawPointerType::IjvxRawPointerType& install_this)
	{
		jvxErrorType res = JVX_NO_ERROR;

		const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T>* ptrRawInstall =
			castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T> >(
				selection->rawpointer, selection->propDescriptor->format);
		const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<T>* ptrRawToInstall =
			castPropRawPointer<const  jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<T> >(
				&install_this, selection->propDescriptor->format);

		if (ptrRawInstall && ptrRawToInstall)
		{
			T** ptrRawStore = ptrRawInstall->typedPointer();
			T* ptrRawIn = ptrRawToInstall->typedPointer();
			if (*ptrRawStore == ptrRawIn)
			{
				*ptrRawStore = nullptr;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		} // if (extBufPtrFromRawInstall && extBufPtrFromRawToInstall)
		else
		{
			// Error: pointers were of incorrect type
			res = JVX_ERROR_INVALID_FORMAT;
		}
		return res;
	}

	std::list<oneProperty>::iterator findSelectionFromAddress(propAddressing& addressOptions);

	jvxErrorType _number_properties_local(jvxCallManagerProperties& callGate, jvxSize* num);

	jvxErrorType _forward_submodules(jvxCallManagerProperties& callGate,
		propAddressing& addresses,
		std::function<jvxErrorType(const jvx::propertyAddress::IjvxPropertyAddress& addr, CjvxPropertySubModule* theSubModule)> func,
		std::function <void(CjvxPropertySubModule* theSubModule)> func_post_success = nullptr);

	jvxErrorType _run_hook(const std::string& hookname,
		const std::string& propname,
		jvxCallManagerProperties& callGate,
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
	if((res == JVX_NO_ERROR) && (callGate.on_set.modification_impact == true))


#define UPDATE_PROPERTY_ACCESS_TYPE(accTp, prop) CjvxProperties::_update_property_access_type(accTp, prop.category, prop.globalIdx)
#define UNDO_UPDATE_PROPERTY_ACCESS_TYPE(prop) CjvxProperties::_undo_update_property_access_type(prop.category, prop.globalIdx)

#define JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category) \
	jvxSize propId = JVX_SIZE_UNSELECTED; \
	jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN; \
	_translate_address_idx(callGate, ident, propId, category)

#define JVX_TRANSLATE_PROP_ADDRESS_DESCRIPTOR(ident, propDescr) \
	jvxApiString propDescr; \
	_translate_address_descriptor(callGate, ident, propDescr)

#define JVX_PROPERTIES_ALL_START(clsName) \
	clsName::init_all(); \
	clsName::allocate_all(); \
	clsName::register_all(this)

#define JVX_PROPERTIES_ALL_STOP(clsName) \
	clsName::unregister_all(this); \
	clsName::deallocate_all(); 

#endif


