#ifndef __CJVXACCESSPROPERTIES_QHTTP_H__
#define __CJVXACCESSPROPERTIES_QHTTP_H__

#include "jvx.h"
#include <map>
#include "CjvxWebHostControl.h"
#include "common/CjvxProperties.h"

typedef enum
{
	JVX_ACCESS_PROPERTY_MODE_INIT,
	JVX_ACCESS_PROPERTY_MODE_COLLECT,
	JVX_ACCESS_PROPERTY_MODE_TRANSFER,
	JVX_ACCESS_PROPERTY_MODE_WAIT_RESPONSE,
	JVX_ACCESS_PROPERTY_MODE_READOUT
} processMode;

typedef enum
{
	JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_CONTENT,
	JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTION,
	// JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_DESCRIPTOR_ONLY, <- I think this construct is no longer used!
	JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_EXTENDED_INFO,

	JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_SET_CONTENT,

	JVX_ACCESS_PROPERTY_OPERATION_PROPERTY_GET_PROPERTY_LIST
} jvxOneAssociatedRequestType;

class oneRequestEntry
{
public:
	jvxState stat;
	jvxOneAssociatedRequestType req_type;
//	jvxSize uniqueId;
	std::string json_response;
	std::string http_arg;
	jvxErrorType errRes;
	jvxBool contentOnly;
	//jvxSize prop_id;
	std::string tag;
	jvxContext ctxt;

	//jvxPropertyCategoryType prop_cat;
};

static jvxBool operator == (const oneRequestEntry& one, const oneRequestEntry& theother)
{
	if (
		(one.req_type == theother.req_type) &&
		(one.tag == theother.tag))
		//(one.prop_id == theother.prop_id) &&
		//(one.prop_cat == theother.prop_cat))
	{
		return true;
	}
	return false;
}

static jvxBool operator == (const oneRequestEntry& one, const jvxOneAssociatedRequestType& comp)
{
	if (
		(one.req_type == comp))
	{
		return true;
	}
	return false;
}

typedef struct
{
	processMode currentMode;
	IjvxAccessProperties_delayed_report* response;
	jvxHandle* priv;
	jvxSize myUniqueId_inGroup;
	std::map<jvxSize, oneRequestEntry> requests;
} oneAssociatedGroup;

class CjvxAccessProperties_Qhttp : public QObject, public IjvxAccessProperties, public IjvxHttpApi_httpreport
{
private:
	typedef struct
	{
		jvxPropertyDescriptor descr;
		jvxExtendedProps extProps;
	} jvxOnePropertyConnected;

	jvxState theState;

	//std::list<oneAssociatedGroup> theGroups;
	jvxSize myUniqueId_groups;
	std::map<jvxSize, oneAssociatedGroup> thegroups;
	
	IjvxHttpApi* httpRefPtr;
	jvxComponentIdentification tpRef;
	std::string descrorRef;
	std::string moduleRef;
	std::string tagRef;
	std::string descrionRef;

	std::map<std::string, jvxOnePropertyConnected> allPropsCached;
	IjvxFactoryHost* hFHost = nullptr;

#ifdef JVX_OBJECTS_WITH_TEXTLOG
	CjvxLogEmbedding embLog;
#endif

	jvxErrorType addRequestToGroup(oneAssociatedGroup& entry,
		const std::string& descriptor,
		jvxOneAssociatedRequestType tp,
		const std::string& httparg, jvxBool contentOnly,
		jvxContext* ctxt
			//jvxPropertyCategoryType cat,
			//jvxSize id
	);
	jvxErrorType transform_request_to_uri(std::string& uri, oneRequestEntry & entry);

	jvxErrorType complete_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, const char* response, jvxHandle* priv, jvxCBitField reqId) override;
	jvxErrorType failed_transfer_http(jvxSize uniqueIdGroup, jvxSize uniqueId_ingroup, jvxErrorType errRes, jvxHandle* priv, jvxCBitField reqId) override;
	void check_for_all_props_complete(jvxSize uniqueIdGroup);

	jvxErrorType convertResponseToContent__descriptor(
		jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr, 
		const jPAD& ident, oneRequestEntry req,
		jvxAccessProtocol* accProt) ;
	jvxErrorType convertResponseToContent__extendedInfo(jvxHandle* whattoget, jvxSize tp, oneRequestEntry req,
		jvxAccessProtocol* accProt);
	jvxErrorType convertResponseToContent__get_content(jvxHandle* fld,
		jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
		jvxBool contentOnly, jvxPropertyDecoderHintType decTp, const char* descriptor, oneRequestEntry req,
		jvxAccessProtocol* accProt, jvxCallManagerProperties& callgate);
	jvxErrorType convertResponseToContent__set_content(jvxHandle* fld,
		jvxSize offsetStart, jvxSize numElements, jvxDataFormat format,
		jvxBool contentOnly, jvxPropertyDecoderHintType decTp, const char* descriptor, oneRequestEntry req,
		jvxAccessProtocol* accProt );
	jvxErrorType convertResponseToContent__proplist(std::map<std::string, jvxPropertyDescriptorP>& theProps, 
		const jPAD& ident, oneRequestEntry req, jvxAccessProtocol* accProt);

	std::map<std::string, jvxPropertyDescriptorP> thePropCache;

	Q_OBJECT

public:
	CjvxAccessProperties_Qhttp();
	~CjvxAccessProperties_Qhttp();

	virtual jvxErrorType JVX_CALLINGCONVENTION description_object(jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_object(jvxApiString* astr, jvxApiString* tagstr) override;

	virtual jvxErrorType initialize(IjvxHttpApi* httpRef, jvxComponentIdentification tp, 
		const char* description, const char* descriptor, const char* tag,
		IjvxFactoryHost* hFHost);
	virtual jvxErrorType terminate(IjvxHttpApi** httpRef = NULL, jvxComponentIdentification* tp = NULL);
	virtual jvxErrorType state(jvxState* theStat);

	virtual jvxErrorType reference_status(jvxBool* hasValidObjectRef, jvxBool* hasValidPropertyRef) override;
	virtual jvxErrorType get_features(jvxBitField* theFeatures)override;

	jvxErrorType create_refresh_property_cache(jvxCallManagerProperties& callGate,
		const char* descriptor_expr,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override;

	jvxErrorType clear_property_cache()override;

	virtual jvxErrorType get_reference_component_description(jvxApiString* descror, jvxApiString* descrion, jvxApiString* modref, jvxComponentIdentification* cpId) override;

	virtual jvxErrorType get_descriptor_property(
		jvxCallManagerProperties& callGate,
		jvx::propertyDescriptor::IjvxPropertyDescriptor& theDescr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;

	jvxErrorType get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override;

	jvxErrorType set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override;
	
	virtual jvxErrorType install_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;

	virtual jvxErrorType uninstall_referene_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& ptrRaw,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;


	virtual jvxErrorType get_property_extended_info(jvxCallManagerProperties& callGate,
		jvxHandle* fld, jvxSize requestId, const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;

	virtual jvxErrorType translate_group_id(jvxCallManagerProperties& callGate, jvxSize gId, jvxApiString& astr) override;

	virtual jvxErrorType get_revision(jvxSize*rev, jvxCallManagerProperties&callGate) override;

	virtual jvxErrorType block_check_pending(jvxBool* isPending);

	// ====================================================================================================================
	// The following functions are only useful for feature JVX_PROPERTY_ACCESS_NONBLOCKING
	// ====================================================================================================================

	virtual jvxErrorType property_start_delayed_group(jvxCallManagerProperties& callGate,
		IjvxAccessProperties_delayed_report* theRef, jvxHandle* priv,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;

	virtual jvxErrorType property_stop_delayed_group(jvxCallManagerProperties& callGate, jvxHandle** priv,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault()) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_user_data(jvxSize idUserData, jvxHandle* floating_pointer) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION user_data(jvxSize idUserData, jvxHandle** floating_pointer) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION state_object(jvxState* stat) override;

	virtual jvxErrorType get_number_properties(jvxCallManagerProperties& callGate, jvxSize* num,
		const jvx::propertySpecifics::IjvxConnectionType& spec = jvx::propertySpecifics::CjvxConnectionTypeDefault())override;

	/*
	virtual jvxErrorType get_description_property_full__cnt(jvxSize idx,
		jvxPropertyCategoryType* category, jvxUInt64* allowStateMask, jvxUInt64* allowThreadingMask, jvxDataFormat* format,
		jvxSize* num, jvxPropertyAccessType* accessType, jvxPropertyDecoderHintType* decTp, jvxSize* handleIdx,
		jvxPropertyContext* context, jvxApiString* name, jvxApiString* description, jvxApiString* descriptor,
		jvxBool* isValid, jvxFlagTag* accessFlags, jvxSize* processId, jvxCallManagerProperties&callGate) override;
	*/

	/*
	virtual jvxErrorType get_property_descriptor__id(jvxPropertyDescriptor& theStr, jvxSize idGlob, jvxSize* processId = NULL)override;


	virtual jvxErrorType get_property__id(jvxHandle* fld, jvxSize numElements, jvxDataFormat, jvxSize handleIdx,
		jvxPropertyCategoryType category, jvxSize offsetStart, jvxBool onlyContent,
		jvxBool* isValid = NULL, jvxPropertyAccessType* accessType = NULL, jvxPropertyCallPurpose callPurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC, jvxSize* processId = NULL
		)override;
	virtual jvxErrorType set_property__id(jvxHandle* fld,
		jvxSize numElements, jvxDataFormat, jvxSize handleIdx,
		jvxPropertyCategoryType category, jvxSize offsetStart,
		jvxBool onlyContent, jvxPropertyCallPurpose callPurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC, jvxSize* processId = NULL, jvxBool binList = false)override;
	virtual jvxErrorType set_property_ext_ref__id(jvxExternalBuffer* fld, jvxBool* isValid, jvxSize propId, jvxPropertyCategoryType category, jvxSize* processId = NULL)override;
	virtual jvxErrorType get_property_extended_info__id(jvxHandle* fld, jvxSize requestId, jvxSize propId, jvxPropertyCategoryType category, jvxSize* processId = NULL)override;

	virtual jvxErrorType property_start_delayed_group(IjvxAccessProperties_delayed_report* theRef, jvxHandle* priv, jvxSize* processId = NULL)override;
	virtual jvxErrorType property_stop_delayed_group( jvxSize* processId = NULL)override;
	virtual jvxErrorType get_revision(jvxSize* rev = NULL)override;
	virtual jvxErrorType block_check_pending(jvxBool* isPending)override;

	virtual jvxErrorType set_user_data(jvxSize idUserData, jvxHandle* floating_pointer)override;
	virtual jvxErrorType user_data(jvxSize idUserData, jvxHandle** floating_pointer)override;
	virtual jvxErrorType state_object(jvxState* stat) override;
	*/
public slots:
	void reschedule_all_props_in(jvxSize);

signals:
	void emit_reschedule_all_props_in(jvxSize);

};

#endif
