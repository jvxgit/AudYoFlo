#ifndef _CJVXCONFIGURATIONLINE_H__
#define _CJVXCONFIGURATIONLINE_H__

#include "jvx-helpers.h"

class CjvxConfigurationLine
{
protected:

	typedef struct
	{
		jvxSize selector;
	} jvxOneConfigurationId;

	typedef struct
	{
		jvxComponentIdentification selector;
	} jvxOneSlaveConfigurationLine;

	typedef struct
	{
		std::string descriptor;
		jvxComponentIdentification selector;
		std::vector<jvxOneSlaveConfigurationLine> slaves_tp;
		std::vector<jvxOneConfigurationId> link_properties;
	} jvxOneConfigurationLine;

	struct
	{
		std::vector<jvxOneConfigurationLine> theLines;
	} _common_configurationline;
public:

	JVX_CALLINGCONVENTION CjvxConfigurationLine();

	virtual JVX_CALLINGCONVENTION ~CjvxConfigurationLine();

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_lines(jvxSize* num_lines );

	virtual jvxErrorType JVX_CALLINGCONVENTION _master_line(jvxSize idxLine, jvxComponentIdentification* tp_master);

	virtual jvxErrorType JVX_CALLINGCONVENTION _add_line(const jvxComponentIdentification& tp_master, const char* description);

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _add_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_slave_line(const jvxComponentIdentification& tp_master, const jvxComponentIdentification& tp_slave);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_slaves_line(const jvxComponentIdentification& tp_master, jvxSize* num_slaves);

	virtual jvxErrorType JVX_CALLINGCONVENTION _slave_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxComponentIdentification* tp_slave);

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _add_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id);


	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_configuration_property_line(const jvxComponentIdentification& tp_master, jvxSize system_property_id);

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_property_ids_line(const jvxComponentIdentification& tp_master, jvxSize* num_Props);

	virtual jvxErrorType JVX_CALLINGCONVENTION _property_id_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, jvxSize* the_prop_id);

	// ======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_line(const jvxComponentIdentification& tp_master);

	virtual jvxErrorType JVX_CALLINGCONVENTION _report_property_was_set(const jvxComponentIdentification& tp_master, jvxPropertyCategoryType cat, jvxSize propIdx,
		IjvxHost* theHost, jvxBool onlyContent, const jvxComponentIdentification& tpTo);

	virtual jvxErrorType JVX_CALLINGCONVENTION _refresh_all(IjvxHost* theHost);

	virtual jvxErrorType JVX_CALLINGCONVENTION _updateAllAfterStateChanged(jvxComponentIdentification tp, jvxState newState, IjvxHost* theHost);

	virtual jvxErrorType JVX_CALLINGCONVENTION _remove_all_lines();
};

#endif