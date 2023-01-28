#ifndef __IJVXCONFIGURATIONLINE_H__
#define __IJVXCONFIGURATIONLINE_H__

/*
JVX_INTERFACE IjvxConfigurationLine_report
{
	virtual JVX_CALLINGCONVENTION ~IjvxConfigurationLine_report(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_configuration_changed(IjvxObject* sender) = 0;
};
*/

JVX_INTERFACE IjvxConfigurationLine
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxConfigurationLine(){};

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_lines(jvxSize* num_lines ) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION master_line(jvxSize idxLine, jvxComponentIdentification* tp_master) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_line(const jvxComponentIdentification& tp_master, const char* description) = 0;

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION add_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_slaves_line(const jvxComponentIdentification& tp_master, jvxSize* num_lines) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION slave_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxComponentIdentification* tp_slave) = 0;

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION add_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_property_ids_line(const jvxComponentIdentification& tp_master, jvxSize* num_lines) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION property_id_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxSize* the_prop_id) = 0;

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_property_was_set(const jvxComponentIdentification& tpFrom, jvxPropertyCategoryType cat, jvxSize propIdx, 
		jvxBool onlyContent, const jvxComponentIdentification& tpTo) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION refresh_all() = 0;

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_line(const jvxComponentIdentification& tp_master) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_lines() = 0;


};

#endif