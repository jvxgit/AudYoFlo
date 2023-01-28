#ifndef __CJVXCONSOLEHOST_BE_DRIBVEHOST_TYPEDEFS_H__
#define __CJVXCONSOLEHOST_BE_DRIBVEHOST_TYPEDEFS_H__

#define JVX_CONSOLE_BE_SILENT_MODE "JVX_CONSOLE_BE_SILENT_MODE"
#define JVX_CONSOLE_BE_NUM_DIGITS "JVX_CONSOLE_BE_NUM_DIGITS"
#define JVX_CONSOLE_BE_TIMEOUT_SEQ "JVX_CONSOLE_BE_TIMEOUT_SEQ"


#define JVX_DEFINE_PROCESSING_OFF "##>"
#define JVX_DEFINE_PROCESSING_ON "#R#>"

// =============================================================================

#define JVX_INDICATE_TRUE_SHORT "*"
#define JVX_INDICATE_FALSE_SHORT "-"

#define JVX_CREATE_VERSION_GIT(jelm, txt) \
	jelm.makeAssignmentString("git_tag", txt)

#define JVX_CREATE_VERSION_ME(jelm, txt) \
	jelm.makeAssignmentString("me_tag", txt)

#define JVX_CREATE_VERSION_DATA(jelm, txt) \
	jelm.makeAssignmentString("date_tag", txt)

#define JVX_CREATE_SYSTEM(jelm, jarr) \
	jelm.makeArray("system", jarr);

#define JVX_CREATE_SELECTED(jelm, jarr) \
	jelm.makeArray("selected", jarr);

#define JVX_CREATE_COMPONENTS(jelm, jarr) \
	jelm.makeArray("components", jarr);

#define JVX_CREATE_ACTIVE_COMPONENTS(jelm, jarr) \
	jelm.makeArray("active_components", jarr);

#define JVX_CREATE_COMPONENT_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("component_identification", txt)

#define JVX_CREATE_COMPONENT_PARENT_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("parent_component_identification", txt)

#define JVX_CREATE_COMPONENT_TYPE(jelm, txt) \
	jelm.makeAssignmentString("component_type", txt)

#define JVX_CREATE_COMPONENT_TYPE_SEC(jelm, txt) \
	jelm.makeAssignmentString("component_type_sec", txt)

#define JVX_CREATE_COMPONENT_DESCRIPTION(jelm, txt) \
	jelm.makeAssignmentString("description", txt);

#define JVX_CREATE_COMPONENT_NAME(jelm, txt) \
	jelm.makeAssignmentString("name", txt);

#define JVX_CREATE_COMPONENT_DESCRIPTOR(jelm, txt) \
	jelm.makeAssignmentString("descriptor", txt);

#define JVX_CREATE_COMPONENT_DESCRIPTOR_CRC(jelm, txt) \
	jelm.makeAssignmentString("d", txt);

#define JVX_CREATE_COMPONENT_STATE(jelm, txt) \
	jelm.makeAssignmentString("state", txt);

#define JVX_CREATE_COMPONENT_UID(jelm, txt) \
	jelm.makeAssignmentString("uid", txt);

#define JVX_CREATE_SLOT_STATE(jelm, txt) \
	jelm.makeAssignmentString("slot_state", txt)

#define JVX_CREATE_NUMBER_SUB_SLOTS(jelm, num) \
	jelm.makeAssignmentInt("num_sub_slots", num)

#define JVX_CREATE_NUMBER_SLOTS(jelm, num) \
	jelm.makeAssignmentInt("num_slots", num)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_TYPE(jelm, txt) \
	jelm.makeAssignmentString("seq_element_type", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_TARGET_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("seq_element_target", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_FID(jelm, txt) \
	jelm.makeAssignmentString("seq_element_fid", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_TIMEOUT_MS(jelm, txt) \
	jelm.makeAssignmentString("seq_element_timeout_ms", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL(jelm, txt) \
	jelm.makeAssignmentString("seq_element_label", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL_TRUE(jelm, txt) \
	jelm.makeAssignmentString("seq_element_label_true", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL_FALSE(jelm, txt) \
	jelm.makeAssignmentString("seq_element_label_false", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_BREAK_ACTION(jelm, txt) \
	jelm.makeAssignmentString("seq_element_break_action", txt)

#define JVX_CREATE_COMPONENT_SEQ_ELEMENT_ASSOC_MODE(jelm, txt) \
jelm.makeAssignmentString("seq_element_assoc_mode", txt)

#define JVX_CREATE_COMPONENT_SEQ_QUEUS(jelm, jelmlqueues) \
	jelm.makeSection("queues", jelmlqueues)

#define JVX_CREATE_SEQUENCES(jelm, jarr) \
	jelm.makeArray("sequences", jarr);


#define JVX_CREATE_SEQUENCER(jelm, jelml) \
	jelm.makeSection("sequencer", jelml);


#define JVX_CREATE_SEQUENCER_STATE(jelm, txt) \
	jelm.makeAssignmentString("seq_state", txt)

#define JVX_CREATE_SEQUENCER_STATUS(jelm, txt) \
	jelm.makeAssignmentString("seq_status", txt)

#define JVX_CREATE_SEQUENCER_QUEUE_TYPE(jelm, txt) \
	jelm.makeAssignmentString("seq_qtype", txt)

#define JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_DESCRIPTION(jelm, txt) \
	jelm.makeAssignmentString("seq_descr", txt)

#define JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_LABEL(jelm, txt) \
	jelm.makeAssignmentString("seq_label", txt);

#define JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_STEP(jelm, txt) \
	jelm.makeAssignmentString("seq_step_descr", txt)


#define JVX_CREATE_CONNECTIONS(jelm, jarr) \
	jelm.makeArray("connections", jarr);

#define JVX_CREATE_MASTER_NAME(jelm, txt) \
	jelm.makeAssignmentString("master_name", txt)

#define JVX_CREATE_MASTER_DESCRIPTION(jelm, txt) \
	jelm.makeAssignmentString("master_descr", txt)

#define JVX_CREATE_MASTER_PARENT_NAME(jelm, txt) \
	jelm.makeAssignmentString("master_parent", txt)

#define JVX_CREATE_MASTER_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("master_identification", txt)

#define JVX_CREATE_CONNECTION_DESCRIPTOR(jelm, txt) \
	jelm.makeAssignmentString("descriptor", txt);

#define JVX_CREATE_CONNECTION_UID(jelm, uid) \
	elmr.makeAssignmentSize("uid", uid);

#define JVX_CREATE_CONNECTION_ISFROMRULE(jelm, txt) \
	jelm.makeAssignmentString("is_from_rule", txt);

#define JVX_CREATE_CONNECTION_LAST_TEST(jelm, txt) \
	jelm.makeAssignmentString("last_test", txt);

#define JVX_CREATE_CONNECTION_IS_READY(jelm, txt) \
	jelm.makeAssignmentString("ready", txt);

#define JVX_CREATE_CONNECTION_REASON_IF_NOT(jelm, txt) \
	jelm.makeAssignmentString("reason_if_not_ready", txt);

#define JVX_CREATE_CONNECTION_STATUS(jelm, txt) \
	jelm.makeAssignmentString("status", txt);

#define JVX_CREATE_CONNECTION_RULE_NAME(jelm, txt) \
	jelm.makeAssignmentString("rule_name", txt);

#define JVX_CREATE_CONNECTION_RULE_DEFAULT(jelm, txt) \
	jelm.makeAssignmentString("rule_default", txt);

#define JVX_CREATE_CONNECTION_RULE_MAS_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("master_identification", txt)

#define JVX_CREATE_CONNECTION_RULE_MAS_FAC_EXPRESSION(jelm, txt) \
	jelm.makeAssignmentString("master_fac_expr", txt);

#define JVX_CREATE_CONNECTION_RULE_MAS_EXPRESSION(jelm, txt) \
	jelm.makeAssignmentString("master_expr", txt);

#define JVX_CREATE_CONNECTION_RULE_BRIDGE_NAME(jelm, txt) \
	jelm.makeAssignmentString("rule_bridge_name", txt);

#define JVX_CREATE_CONNECTION_RULE_IDENTIFICATION_FROM(jelm, txt) \
	jelm.makeAssignmentString("ocon_identification", txt);

#define JVX_CREATE_CONNECTION_RULE_FAC_NAME_FROM(jelm, txt) \
	jelm.makeAssignmentString("ocon_fac_expr", txt);

#define JVX_CREATE_CONNECTION_RULE_OCON_NAME_FROM(jelm, txt) \
	jelm.makeAssignmentString("ocon_expr", txt);

#define JVX_CREATE_CONNECTION_RULE_IDENTIFICATION_TO(jelm, txt) \
	jelm.makeAssignmentString("icon_identification", txt);

#define JVX_CREATE_CONNECTION_RULE_FAC_NAME_TO(jelm, txt) \
	jelm.makeAssignmentString("icon_fac_expr", txt);

#define JVX_CREATE_CONNECTION_RULE_ICON_NAME_TO(jelm, txt) \
	jelm.makeAssignmentString("icon_expr", txt);

#define JVX_CREATE_CONNECTION_RULES(jelm, jarr) \
	jelm.makeArray("connection_rules", jarr);

#define JVX_CREATE_BRIDGE_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_name", txt)

#define JVX_CREATE_ICON_PARENT_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("icon_parent_identification", txt)

#define JVX_CREATE_OCON_PARENT_IDENTIFICATION(jelm, txt) \
	jelm.makeAssignmentString("ocon_parent_identification", txt)

#define JVX_CREATE_ICON_PARENT_NAME(jelm, txt) \
	jelm.makeAssignmentString("icon_parent_name", txt)

#define JVX_CREATE_OCON_PARENT_NAME(jelm, txt) \
	jelm.makeAssignmentString("ocon_parent_name", txt)

#define JVX_CREATE_ICON_NAME(jelm, txt) \
	jelm.makeAssignmentString("icon_name", txt)

#define JVX_CREATE_OCON_NAME(jelm, txt) \
	jelm.makeAssignmentString("ocon_name", txt)

#define JVX_CREATE_CONNECTION_DROPZONE(jelm, jelml) \
	jelm.makeSection("conn_dz", jelml)


#define JVX_CREATE_CONNECTION_MAS_FAC_DROPZONE(jelm, jelml) \
	jelm.makeSection("conn_mas_dz", jelml)




#define JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_IDENTITY(jelm, txt) \
	jelm.makeAssignmentString("mas_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_NAME(jelm, txt) \
	jelm.makeAssignmentString("mas_fac_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_MASTER_NAME(jelm, txt) \
		jelm.makeAssignmentString("mas_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_MASTER_SELECTED(jelm, txt) \
		jelm.makeAssignmentString("mas_selected_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_MASTER_AVAIL(jelm, txt) \
	jelm.makeAssignmentString("mas_avail_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_CONNECTOR_FACTORY_IDENTITY(jelm, txt) \
	jelm.makeAssignmentString("con_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_CONNECTOR_FACTORY_NAME(jelm, txt) \
	jelm.makeAssignmentString("con_fac_name_dz", txt)


#define JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_IDENTITY(jelm, txt) \
	jelm.makeAssignmentString("icon_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_NAME(jelm, txt) \
	jelm.makeAssignmentString("icon_fac_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_NAME(jelm, txt) \
		jelm.makeAssignmentString("icon_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_SELECTED(jelm, txt) \
		jelm.makeAssignmentString("icon_selected_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_AVAIL(jelm, txt) \
	jelm.makeAssignmentString("icon_avail_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_IDENTITY(jelm, txt) \
	jelm.makeAssignmentString("ocon_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_NAME(jelm, txt) \
	jelm.makeAssignmentString("ocon_fac_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_NAME(jelm, txt) \
		jelm.makeAssignmentString("ocon_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_SELECTED(jelm, txt) \
		jelm.makeAssignmentString("ocon_selected_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_AVAIL(jelm, txt) \
	jelm.makeAssignmentString("ocon_avail_dz", txt)



#define JVX_CREATE_CONNECTION_DROPZONE_BRIDGE_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_FROM_IDENTITY(jelm, txt) \
jelm.makeAssignmentString("bridge_ocon_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_FROM_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_ocon_fac_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_OCON_FROM_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_ocon_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_FROM_IDENTITY(jelm, txt) \
jelm.makeAssignmentString("bridge_icon_fac_id_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_FROM_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_icon_fac_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_ICON_FROM_NAME(jelm, txt) \
	jelm.makeAssignmentString("bridge_icon_name_dz", txt)

#define JVX_CREATE_CONNECTION_DROPZONE_BRIDGE_SELECTED(jelm, txt) \
	jelm.makeAssignmentString("bridge_selected_dz", txt)


#define JVX_CREATE_CONNECTION_DROPZONE_PROCESS_NAME(jelm, txt) \
	jelm.makeAssignmentString("dz_process_name", txt)

#define JVX_CREATE_TECHNOLOGY_HOSTTYPE(jelm, jarr) \
	jelm.makeArray("technology_host_type", jarr);

#define JVX_CREATE_TECHNOLOGY_NODETYPE(jelm, jarr) \
	jelm.makeArray("node_host_type", jarr);

#define JVX_CREATE_CONFIGLINE_SLAVE(jelm, jarr) \
	jelm.makeArray("config_line_slaves", jarr);

#define JVX_CREATE_CONFIGLINE_PROPERTY(jelm, jarr) \
	jelm.makeArray("config_line_properties", jarr);

#define JVX_CREATE_CONFIGURATION_LINES(jelm, jarr) \
	jelm.makeArray("cunfiguration_lines", jarr);

#define JVX_CREATE_COMPONENT_SELECTED(jelm, stat) \
	jelm.makeAssignmentString("selected", stat);

#define JVX_CREATE_PROPERTY_FORMAT(jelm, txt) \
	jelm.makeAssignmentString("format", txt);

#define JVX_CREATE_PROPERTY_ACCESS_TYPE(jelm, txt) \
	jelm.makeAssignmentString("access_type", txt);

#define JVX_CREATE_ACCESS_PROTOCOL(jelm, txt) \
	jelm.makeAssignmentString("access_prot", txt);

#define JVX_CREATE_PROPERTY_VALID(jelm, txt) \
	jelm.makeAssignmentString("valid", txt);

#define JVX_CREATE_PROPERTY_CATEGORY(jelm, txt) \
	jelm.makeAssignmentString("category", txt);

#define JVX_CREATE_PROPERTY_DECODER_TYPE(jelm, txt) \
	jelm.makeAssignmentString("decoder_type", txt);

#define JVX_CREATE_PROPERTY_CONTEXT(jelm, txt) \
	jelm.makeAssignmentString("context", txt);

#define JVX_CREATE_PROPERTY_CALL_CONTEXT(jelm, txt) \
	jelm.makeAssignmentString("call_context", txt);

#define JVX_CREATE_PROPERTY_SHOWHINT(jelm, txt) \
	jelm.makeAssignmentString("showhint", txt);

#define JVX_CREATE_PROPERTY_MOREINFO(jelm, txt) \
	jelm.makeAssignmentString("moreinfo", txt);

#define JVX_CREATE_PROPERTY_ORIGIN(jelm, txt) \
	jelm.makeAssignmentString("origin", txt);

#define JVX_CREATE_PROPERTY_NUM(jelm, val) \
	jelm.makeAssignmentInt("number_elements", val);

#define JVX_CREATE_PROPERTY_ALLOWED_STATE_MASK(jelm, txt) \
	jelm.makeAssignmentString("allowed_smask", txt)

#define JVX_CREATE_PROPERTY_ALLOWED_THREADING_MASK(jelm, txt) \
	jelm.makeAssignmentString("thread_mask", txt)

#define JVX_CREATE_PROPERTY_INVALIDATE_STATE_SWITCH(jelm, b) \
	jelm.makeAssignmentString("inval_sswitch", (b != JVX_PROPERTY_INVALIDATE_INACTIVE ? JVX_INDICATE_TRUE_SHORT: JVX_INDICATE_FALSE_SHORT))

#define JVX_CREATE_PROPERTY_INVALIDATE_TEST(jelm, b) \
	jelm.makeAssignmentString("inval_test", ((b == true) ? JVX_INDICATE_TRUE_SHORT: JVX_INDICATE_FALSE_SHORT))

#define JVX_CREATE_PROPERTY_INSTALLABLE(jelm, b) \
	jelm.makeAssignmentString("instable", (b == true ? JVX_INDICATE_TRUE_SHORT : JVX_INDICATE_FALSE_SHORT))

#define JVX_CREATE_ACCESS_FLAGS_RWCD(jelm, val) \
	jelm.makeAssignmentString("acc_flags", val)

#define JVX_CREATE_CFG_MODE(jelm, val) \
	jelm.makeAssignmentString("cfg_flags", val)

#define JVX_CREATE_PROPERTY_OFFSET(jelm, val) \
	jelm.makeAssignmentInt("offset", val);

#define JVX_CREATE_PROPERTY_NUM_SHORT(jelm, val) \
	jelm.makeAssignmentInt("n", val);

#define JVX_CREATE_PROPERTY_OFFSET_SHORT(jelm, val) \
	jelm.makeAssignmentInt("o", val);

#define JVX_CREATE_PROPERTY_DESCRIPTOR(jel, descror) \
	JVX_CREATE_COMPONENT_DESCRIPTOR(jel, descror)

#define JVX_CREATE_PROPERTY_DESCRIPTOR_CRC(jel, descror_crc) \
	JVX_CREATE_COMPONENT_DESCRIPTOR_CRC(jel, descror_crc)

#define JVX_CREATE_PROPERTY_PROPS_SHORT(jelm, txt)\
	jelm.makeAssignmentString("d", txt);

#define JVX_CREATE_ALL_PROPERTIES(jelm, jarr) \
	jelm.makeArray("selected_properties", jarr);

#define JVX_CREATE_SELECTED_PROPERTY(jelm, jarr) \
	jelm.makeArray("selected_properties", jarr);

#define JVX_CREATE_PROPERTY_GLOBALID(jelm, val) \
	jelm.makeAssignmentInt("global_idx", val);

#define JVX_CREATE_ERROR_RETURN(lstelmr, err, txt) \
	{ \
		CjvxJsonElement jelm; \
		jelm.makeAssignmentString("error_description", txt); \
		lstelmr.addConsumeElement(jelm); \
	} \
	return err; 

#define JVX_CREATE_ERROR_NO_RETURN(lstelmr, err, txt) \
	{ \
		CjvxJsonElement jelm; \
		jelm.makeAssignmentString("error_description", txt); \
		lstelmr.addConsumeElement(jelm); \
	} \
	
#define jvx_make_yes "yes"
#define jvx_make_no "no"

JVX_STATIC_INLINE bool jvx_check_yes(const std::string & checkthis)
{
	if (checkthis == "yes")
		return true;
	if (checkthis == "true")
		return true;
	if (checkthis == "1")
		return true;
	return false;
};

JVX_STATIC_INLINE bool jvx_check_no(const std::string & checkthis)
{
	if (checkthis == "no")
		return true;
	if (checkthis == "false")
		return true;
	if (checkthis == "0")
		return true;
	return false;
};
#endif