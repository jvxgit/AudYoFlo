#ifndef __IJVXPROPERTYPOOL_H__
#define __IJVXPROPERTYPOOL_H__

JVX_INTERFACE IjvxHost;

JVX_INTERFACE IjvxPropertyPool
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxPropertyPool(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_system_properties(jvxSize* num_properties ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_property_id(jvxSize id, jvxSize* id_prop , jvxApiString* astr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_entries_pool_property(jvxSize id_prop, jvxSize* num_pool_associations) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION entry_pool_property(jvxSize id_prop, jvxSize id_prop_entry, 
		jvxComponentIdentification* tp, jvxBool* isMaster) = 0;

	// ===========================================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION add_system_property(jvxSize id_prop, jvxDataFormat form, const char* descr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_entry_system_property(jvxSize id_prop, jvxComponentIdentification tp, jvxBool isMaster = true) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_entries_system_property(jvxSize id_prop) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_system_property(jvxSize id_prop) = 0;

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_property_changed(
		jvxSize propId,jvxPropertyCategoryType category, jvxBool contentOnly,
		jvxSize offsetStart, jvxSize numElements, 
		jvxComponentIdentification tpOrigin) = 0;
};

#endif