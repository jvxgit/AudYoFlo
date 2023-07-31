#include "CjvxHostJvx.h"

jvxErrorType
JVX_HOST_JVX_CLASSNAME::number_system_properties(jvxSize* num_properties)
{
	return _number_system_properties(num_properties);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::system_property_id(jvxSize id, jvxSize* id_prop, jvxApiString* astr)
{
	return _system_property_id(id, id_prop, astr);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::number_entries_pool_property(jvxSize id_prop, jvxSize* num_pool_associations)
{
	return _number_entries_pool_property(id_prop,  num_pool_associations);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::entry_pool_property(jvxSize id_prop, jvxSize id_prop_entry,
		jvxComponentIdentification* tp, jvxBool* isMaster) 
{
	return _entry_pool_property(id_prop, id_prop_entry, tp, isMaster); 
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::add_system_property(jvxSize id_prop, jvxDataFormat format, const char* astr)
{
	return _add_system_property(id_prop, format, astr);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::add_entry_system_property(jvxSize id_prop, jvxComponentIdentification tp, jvxBool isMaster)
{
	return _add_entry_system_property(id_prop, tp, isMaster);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::clear_entries_system_property(jvxSize id_prop)
{
	return _clear_entries_system_property(id_prop);
}
	
jvxErrorType
JVX_HOST_JVX_CLASSNAME::remove_system_property(jvxSize id_prop) 
{
	return _remove_system_property(id_prop);
}

jvxErrorType
JVX_HOST_JVX_CLASSNAME::report_property_changed(
		jvxSize propId, jvxPropertyCategoryType category, jvxBool contentOnly,
		jvxSize offsetStart, jvxSize numElements, jvxComponentIdentification tpOrigin)
{
	return _report_property_changed(static_cast<IjvxHost*>(this), propId, 
		category, contentOnly, offsetStart, numElements, tpOrigin);
}
		