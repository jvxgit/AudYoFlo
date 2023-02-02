#ifndef __CJVXACCESSPROPERTIES__H__
#define __CJVXACCESSPROPERTIES__H__

#include "interfaces/properties_hosts/IjvxAccessProperties.h"

class CjvxAccessProperties_direct: public IjvxAccessProperties
{
private:
	jvx_propertyReferenceTriple* theTripleRef;
	
public:

	CjvxAccessProperties_direct()
	{
		theTripleRef = NULL;
	};
	
	~CjvxAccessProperties_direct()
	{
		terminate();
	};

	virtual jvxErrorType description_object(jvxApiString* astr)override
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		astr->assign("<no object>");
		if (theTripleRef->theObj)
		{
			res = theTripleRef->theObj->description(astr);
			res = JVX_NO_ERROR;
		}
		return res;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_object(jvxApiString* astr , jvxApiString* tagstr ) override
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (theTripleRef->theObj)
		{
			res = theTripleRef->theObj->descriptor(astr, tagstr);
			res = JVX_NO_ERROR;
		}
		return res;
	};

	void initialize(jvx_propertyReferenceTriple* copyRefs)
	{
		theTripleRef = copyRefs;
	}

	void terminate()
	{
		theTripleRef = NULL;
	};
	
	virtual jvxErrorType get_features(jvxBitField* theFeatures)override
	{
		if(theFeatures)
		{
			jvx_bitZero(*theFeatures);
		}
		return JVX_NO_ERROR;
	};
	
	virtual jvxErrorType get_reference_component_description(
		jvxApiString* descror, 
		jvxApiString* descrion, 
		jvxApiString* modref, 
		jvxComponentIdentification* cpId) override
	{
		assert(theTripleRef);
		if (theTripleRef->theObj)
		{
			if (cpId)
			{
				theTripleRef->theObj->request_specialization(NULL, cpId, NULL, NULL);
			}
			if (descrion)
			{
				theTripleRef->theObj->description(descrion);
			}
			if (descror)
			{
				theTripleRef->theObj->descriptor(descror);
			}
			if (modref)
			{
				theTripleRef->theObj->module_reference(modref, nullptr);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_NOT_READY;
	};

	jvxErrorType get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			return(theTripleRef->theProps->get_property(
					callGate, rawPtr,
					ident, trans));
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	/*
		virtual jvxErrorType get_property__descriptor(jvxHandle* fld,
			jvxSize offsetStart,
			jvxSize numElements,
			jvxDataFormat format,
			jvxBool contentOnly,
			const char* descriptor, 
			jvxPropertyDecoderHintType decTp,  
			jvxSize* processId,
			jvxCallManagerProperties& callGate)override
		{
			jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descriptor);
			jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);
	
		virtual jvxErrorType get_property__id(jvxHandle* fld,
			jvxSize numElements,
			jvxDataFormat format,
			jvxSize handleIdx,
			jvxPropertyCategoryType category,
			jvxSize offsetStart,
			jvxBool onlyContent,
			jvxSize* processId,
			jvxCallManagerProperties& callGate)override
		{
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(handleIdx, category);
			jvx::propertyDetail::CjvxTranferDetail trans(onlyContent, offsetStart);
			assert(theTripleRef);
			if (theTripleRef->theProps)
			{
				return theTripleRef->theProps->get_property(
					callGate, fld,
					numElements, format,ident, trans);
			}
			return JVX_ERROR_UNSUPPORTED;
		};
	*/
	virtual jvxErrorType create_refresh_property_cache(jvxCallManagerProperties& callGate,
		const char* descriptor_expr,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType clear_property_cache() override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->set_property(
				callGate, rawPtr, ident, trans);
		}
		return res;
	}

	/*
	virtual jvxErrorType set_property__descriptor(jvxHandle* fld,
		jvxSize offsetStart,
		jvxSize numElements,
		jvxDataFormat format,
		jvxBool contentOnly,
		const char* descriptor, 
		jvxPropertyDecoderHintType decTp,
		jvxSize* processId, jvxBool binList,
		jvxCallManagerProperties& callGate)override
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(descriptor);
			jvx::propertyDetail::CjvxTranferDetail trans(contentOnly, offsetStart);

			res = theTripleRef->theProps->set_property(
					callGate,
					fld,
					numElements,
					format,
					&ident,
					&trans);			
		}
		return res;
	};

	virtual jvxErrorType set_property__id(jvxHandle* fld,
		jvxSize numElements,
		jvxDataFormat format,
		jvxSize handleIdx,
		jvxPropertyCategoryType category,
		jvxSize offsetStart,
		jvxBool onlyContent,
		jvxSize* processId, jvxBool binList,
		jvxCallManagerProperties& callGate)override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(handleIdx, category);
			jvx::propertyDetail::CjvxTranferDetail trans(onlyContent, offsetStart);

			return theTripleRef->theProps->set_property(
				callGate,
				fld, numElements, format,
				&ident, &trans);
		}
		return JVX_ERROR_UNSUPPORTED;
	};
	*/

	virtual jvxErrorType reference_status(jvxBool* hasValidObjectRef, jvxBool* hasValidPropertyRef) override
	{
		if (hasValidObjectRef)
		{
			*hasValidObjectRef = false;
		}
		if(hasValidPropertyRef)
		{ 
			*hasValidPropertyRef = false;
		}

		if (theTripleRef)
		{
			if(theTripleRef->theObj && theTripleRef->theHdl)
			{
				if (hasValidObjectRef)
				{
					*hasValidObjectRef = true;
				}
			}

			if (theTripleRef->theObj && theTripleRef->theHdl && theTripleRef->theProps)
			{
				if (hasValidPropertyRef)
				{
					*hasValidPropertyRef = true;
				}
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	};
	
	/*
	virtual jvxErrorType get_property_description_simple__descriptor( 
		jvxSize* numElements, jvxDataFormat* format, const char* descriptor, 
		jvxSize* processId ,
		jvxCallManagerProperties& callGate)override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
			jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(descriptor); 
			jvxErrorType res;

			res = theTripleRef->theProps->description_property(callGate, &descr, &idProp);
			if (res == JVX_NO_ERROR)
			{
				if (format) *format = descr.format;
				if (numElements) *numElements = descr.num;
			}
			return(res);
		}
		return JVX_ERROR_UNSUPPORTED;
	};
	*/
	/*
	virtual jvxErrorType install_referene_property(
		jvxExternalBuffer* fld,
		jvxBool* isValid, const char* descriptor, jvxSize* processId,
		jvxCallManagerProperties& callGate) override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			theTripleRef->theProps->install_property_reference(
				callGate,
				&ptrRaw,
				&ident
	virtual jvxErrorType install_property_ext_ref__descriptor(jvxExternalBuffer* fld, jvxBool* isValid, const char* descriptor, jvxSize* processId,
		jvxCallManagerProperties& callGate)override
	{
	*/

	/*
	virtual jvxErrorType set_property_ext_ref__descriptor(jvxExternalBuffer* fld, jvxBool* isValid, const char* descriptor, jvxSize* processId,
		jvxCallManagerProperties& callGate)override
	{

	virtual jvxErrorType set_property_ext_ref__id(jvxExternalBuffer* fld,
								jvxBool* isValid,
								jvxSize propId,
								jvxPropertyCategoryType category, jvxSize* processId ,
		jvxCallManagerProperties& callGate)override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			jvxDataFormat form = JVX_DATAFORMAT_POINTER;
			if (fld)
			{
				form = fld->formFld;
			}
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(fld, isValid, form);
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(propId, category);
			if (fld)
			{
				return theTripleRef->theProps->install_property_reference(callGate, &ptrRaw, &ident);
			}
			else
			{
				return theTripleRef->theProps->uninstall_property_reference(callGate, &ptrRaw, &ident);
			}
		}
		return JVX_ERROR_UNSUPPORTED;
	};

	*/
	virtual jvxErrorType install_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec) override
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->install_property_reference(callGate,
				ptrRaw, ident);
		}
		return(res);
	};

	virtual jvxErrorType uninstall_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->uninstall_property_reference(callGate,
				ptrRaw, ident);
		}
		return(res);
	};
	
	virtual jvxErrorType get_descriptor_property(
		jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override
	{
		jvxErrorType res = JVX_ERROR_UNSUPPORTED;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->description_property(callGate, theDescr, ident);
		}
		return res;
	}

#if 0
	virtual jvxErrorType get_property_descriptor__descriptor(
		jvxPropertyDescriptor& theDescr, const char* descriptor, jvxSize* processId,
		jvxCallManagerProperties& callGate)override
	{
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{

			jvxSize i, num;
			jvxErrorType res = JVX_NO_ERROR;
			jvxBool foundit = false;
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

			theTripleRef->theProps->number_properties(callGate, &num);
			for (i = 0; i < num; i++)
			{
				ident.idx = i;
				res = theTripleRef->theProps->description_property(callGate, &theDescr, &ident);
				/*
				res = theTripleRef->theProps->description_property(callGate, i, &theStr.category, &theStr.allowedStateMask, &theStr.allowedThreadingMask,
					&theStr.format, &theStr.num, &theStr.accessType, &theStr.decTp,
					&theStr.globalIdx, &theStr.ctxt, &nm, &descr, &descror,
					&theStr.isValid,
					&theStr.accessFlags);
					*/

				assert(res == JVX_NO_ERROR);
				if (theDescr.descriptor == descriptor)
				{
					foundit = true;
					break;
				}
			}
			if (foundit)
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_ELEMENT_NOT_FOUND;
			}
			return(res);
		}
		return JVX_ERROR_UNSUPPORTED;
	};
#endif

	// ==============================================================================================

	virtual jvxErrorType get_property_extended_info(jvxCallManagerProperties& callGate,
		jvxHandle* fld, jvxSize requestId, const jvx::propertyAddress::IjvxPropertyAddress& ident, 
		const jvx::propertySpecifics::IjvxConnectionType& spec)override
	{
		jvxErrorType res = JVX_ERROR_INVALID_SETTING;

		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->get_property_extended_info(callGate, fld, requestId, ident);						
		}
		return res;
	};

	virtual jvxErrorType translate_group_id(
		jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr)override
	{
		jvxErrorType res = JVX_ERROR_INVALID_SETTING;

		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->translate_group_id(callGate, gId, astr);
		}
		return res;
	};

	// ===========================================================
	
	virtual jvxErrorType get_number_properties(
		jvxCallManagerProperties& callGate, jvxSize* num,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (num)
			*num = 0;
		if (theTripleRef)
		{
			if (theTripleRef->theProps)
			{
				res = theTripleRef->theProps->number_properties(callGate, num);
			}
		}
		return res;
	};
	
	/*
	virtual jvxErrorType get_description_property_full__cnt(jvxSize idx,
		jvxPropertyCategoryType* category,
		jvxUInt64* allowStateMask,
		jvxUInt64* allowThreadingMask,
		jvxDataFormat* format,
		jvxSize* num,
		jvxPropertyAccessType* accessType,
		jvxPropertyDecoderHintType* decTp,
		jvxSize* handleIdx,
		jvxPropertyContext* context,
		jvxApiString* name,
		jvxApiString* description,
		jvxApiString* descriptor,
		jvxBool* isValid, 
		jvxFlagTag* accessTp,
		jvxSize* processId ,
		jvxCallManagerProperties& callGate)override
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(idx);
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			jvxErrorType res = theTripleRef->theProps->description_property(callGate, &theDescr, &ident);
			if (res == JVX_NO_ERROR)
			{
				if (category) *category = theDescr.category;
				if (allowStateMask) *allowStateMask = theDescr.allowedStateMask;
				if (allowThreadingMask) *allowThreadingMask = theDescr.allowedThreadingMask;
				if (format) *format = theDescr.format;
				if (num) *num = theDescr.num;
				if (accessType) *accessType = theDescr.accessType;
				if (decTp) *decTp = theDescr.decTp;
				if (handleIdx) *handleIdx = theDescr.globalIdx;
				if (context) *context = theDescr.ctxt;
				if (name) *name = theDescr.name;
				if (description) *description = theDescr.description;
				if (descriptor) *descriptor = theDescr.descriptor;
				if (isValid) *isValid = theDescr.isValid;
				if (accessTp) *accessTp = theDescr.accessFlags;
			}
			return res;
		}
		return JVX_ERROR_UNSUPPORTED;
	};	
	*/
	
	// The following functions are only useful for feature JVX_PROPERTY_ACCESS_NONBLOCKING
	virtual jvxErrorType property_start_delayed_group(jvxCallManagerProperties& callGate,
		IjvxAccessProperties_delayed_report* theRef, jvxHandle* priv,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		assert(theTripleRef); 
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->start_property_group(callGate);
		}
		return res;
	};
	
	virtual jvxErrorType property_stop_delayed_group(
		jvxCallManagerProperties& callGate, jvxHandle** priv,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->stop_property_group(callGate);
		}
		return res;
	};

	virtual jvxErrorType get_revision(jvxSize*rev, jvxCallManagerProperties& callGate) override
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		assert(theTripleRef);
		if (theTripleRef->theProps)
		{
			res = theTripleRef->theProps->get_revision(callGate, rev);
		}
		return res;
	};

	virtual jvxErrorType block_check_pending(jvxBool* isPending)
	{
		// Pending requests are not an option
		if (isPending)
			*isPending = false;
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType set_user_data(jvxSize idUserData, jvxHandle* floating_pointer)override
	{
		jvxErrorType res = JVX_ERROR_INVALID_SETTING;
		assert(theTripleRef);
		if (theTripleRef->theObj)
		{
			res = theTripleRef->theObj->set_user_data(idUserData, floating_pointer);
		}
		return res;
	};

	virtual jvxErrorType user_data(jvxSize idUserData, jvxHandle** floating_pointer)override
	{
		jvxErrorType res = JVX_ERROR_INVALID_SETTING;
		if (floating_pointer)
		{
			*floating_pointer = NULL;
		}
		assert(theTripleRef);
		if (theTripleRef->theObj)
		{
			res = theTripleRef->theObj->user_data(idUserData, floating_pointer);
		}
		return res;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION state_object(jvxState* stat) override
	{
		if (stat)
		{
			*stat = JVX_STATE_NONE;
		}
		assert(theTripleRef);
		if (theTripleRef->theObj)
		{
			theTripleRef->theObj->state(stat);
		}
		return JVX_NO_ERROR;
	}
};

#endif
