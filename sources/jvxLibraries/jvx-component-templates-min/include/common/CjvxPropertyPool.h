#ifndef _CJVXPROPERTYPOOL_H__
#define _CJVXPROPERTYPOOL_H__

#include "jvx-helpers.h"

class onePropertyPoolMember
{
public:
	jvxComponentIdentification tp;
	jvxBool detectLoop;
	jvxBool isMaster;
	onePropertyPoolMember()
	{
		detectLoop = false;
		isMaster = false;
	};
};

class onePropertyPoolEntry
{
public:
	jvxSize propid;
	std::string description;
	std::list<onePropertyPoolMember> pool;
	jvxDataFormat format;
	onePropertyPoolEntry()
	{
		propid = JVX_SIZE_UNSELECTED;
		format = JVX_DATAFORMAT_NONE;
	};
};

// =======================================================================================

class CjvxPropertyPool
{
protected:
	std::map<jvxSize, onePropertyPoolEntry> pool_propids;
public:

	JVX_CALLINGCONVENTION CjvxPropertyPool();
	virtual JVX_CALLINGCONVENTION ~CjvxPropertyPool();

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_system_properties(jvxSize* num_properties );
	virtual jvxErrorType JVX_CALLINGCONVENTION _system_property_id(jvxSize id, jvxSize* id_prop, jvxApiString* astr );
	virtual jvxErrorType JVX_CALLINGCONVENTION _number_entries_pool_property(jvxSize id_prop, jvxSize* num_pool_associations);
	virtual jvxErrorType JVX_CALLINGCONVENTION _entry_pool_property(jvxSize id_prop, jvxSize id_prop_entry, 
		jvxComponentIdentification* tp, jvxBool* isMaster);

	// ===========================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION _add_system_property(jvxSize id_prop, jvxDataFormat form, const char* descr);
	virtual jvxErrorType JVX_CALLINGCONVENTION _add_entry_system_property(jvxSize id_prop, jvxComponentIdentification tp, jvxBool isMaster);
	virtual jvxErrorType JVX_CALLINGCONVENTION _clear_entries_system_property(jvxSize id_prop);
	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_system_property(jvxSize id_prop);

	// ===========================================================================================

	jvxErrorType _report_property_changed(
		IjvxHost* host, jvxSize propId,
		jvxPropertyCategoryType category, jvxBool contentOnly,
		jvxSize offsetStart, jvxSize numElements, 
		jvxComponentIdentification tpOrigin);

};

#endif