#include "jvx_dsp_base.h"

#define FLD_NAME_COPYRIGHT_STRING "COPYRIGHT_STRING"
#define	FLD_NAME_VERSION_STRING "VERSION_STRING"
#define FLD_NAME_MODULENAME_STRING "NAME_STRING"
#define FLD_NAME_STATE_BITFIELD "STATE_BITFIELD"
#define FLD_NAME_COMPONENTS "COMPONENTS"
#define FLD_NAME_FLOAT_CONFIGURATION_ID "FLOAT_CONFIGURATION_ID"

#define EXPRESSION_COPYRIGHT "copyright 2015 by Javox Solutions GmbH"
#define EXPRESSION_VERSION "0.0.1"
#define EXPRESSION_MODULENAME "Jvx-Matlab-Host"

#define FLD_NAME_COMPONENT_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_COMPONENT_DESCRIPTOR_STRING "DESCRIPTOR_STRING"
#define FLD_NAME_COMPONENT_MODULE_NAME_STRING "MODULE_NAME_STRING"
#define FLD_NAME_COMPONENT_FEATURE_CLASS_BITFIELD "FEATURE_CLASS_BITFIELD"
#define FLD_NAME_COMPONENT_STATE_BITFIELD "STATE_BITFIELD"
#define FLD_NAME_COMPONENT_ISREADY_BOOL "ISREADY_BOOL"
#define FLD_NAME_COMPONENT_REASON_IF_NOT_READY_STRING "REASON_IF_NOT_READY_STRING"
#define FLD_NAME_COMPONENT_SUBFIELD "SUBFIELD"

#define FLD_NAME_COMMAND_ID_INT32 "COMMAND_ID_INT32"
#define FLD_NAME_COMMAND_DESCRIPTION_STRING "COMMAND_DESCRIPTION_STRING"
#define FLD_NAME_COMMAND_STATE_ALLOW_BITFIELD "COMMAND_STATE_ALLOW_BITFIELD"

#define FLD_NAME_CONDITION_ID_INT32 "CONDITION_ID_INT32"
#define FLD_NAME_CONDITION_DESCRIPTION_STRING "CONDITION_DESCRIPTION_STRING"
#define FLD_NAME_CONDITION_STATE_ALLOW_BITFIELD "CONDITION_STATE_ALLOW_BITFIELD"

#define FLD_NAME_RELJUMP_ID_INT32 "RELJUMP_ID_INT32"
#define FLD_NAME_RELJUMP_DESCRIPTION_STRING "RELJUMP_DESCRIPTION_STRING"
#define FLD_NAME_RELJUMP_STATE_ALLOW_BITFIELD "RELJUMP_STATE_ALLOW_BITFIELD"

#define PREFIX_ONE_COMPONENT "COMPONENT"
#define PREFIX_ONE_SEQUENCE "SEQUENCE"
#define PREFIX_ONE_SEQUENCE_STEP "STEP"
#define PREFIX_ONE_COMMAND "COMMAND"
#define PREFIX_ONE_CONDITION "CONDITION"
#define PREFIX_ONE_RELJUMP "RELJUMP"
#define PREFIX_ONE_CONNECTION_RULE "CONNECTION_RULE"
#define PREFIX_ONE_RULE_BRIDGE "CONNECTION_RULE_BRIDGE"
#define PREFIX_ONE_CONNECTION_PROCESS "CONNECTION_PROCESS"
#define PREFIX_ONE_CONNECTION_PROCESS_BRIDGE "CONNECTION_PROCESS_BRIDGE"


#define FLD_NAME_COMPONENT_IDENTIFICATION_TYPE "COMPONENT_IDENTIFICATION_TYPE"
#define FLD_NAME_COMPONENT_IDENTIFICATION_SLOT_ID "COMPONENT_IDENTIFICATION_SLOT_ID"
#define FLD_NAME_COMPONENT_IDENTIFICATION_SLOT_SUB_ID "COMPONENT_IDENTIFICATION_SLOT_SUB_ID"

#define FLD_NAME_SEQUENCE_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_SEQUENCE_LABEL_STRING "LABEL_STRING"
#define FLD_NAME_MARKED_FOR_PROCESS_BOOL "ENGAGE_BOOL"
#define FLD_NAME_RUN_STEPS "RUN_STEPS"
#define FLD_NAME_LEAVE_STEPS "LEAVE_STEPS"

#define FLD_NAME_SEQUENCE_STEP_DESCRIPTION_STRING "DESCRIPTION_STRING"
#define FLD_NAME_SEQUENCE_STEP_ELEMENT_TYPE_INT32 "ELEMENT_TYPE_INT32"
#define FLD_NAME_SEQUENCE_STEP_COMPONENT_TYPE_INT32 "COMPONENT_TYPE_INT32"
#define FLD_NAME_SEQUENCE_STEP_FUNCTION_ID_INT32 "FUNCTION_ID_INT32"
#define FLD_NAME_SEQUENCE_STEP_TIMEOUT_MS_INT64 "TIMEOUT_MS_INT64"
#define FLD_NAME_SEQUENCE_STEP_LABEL_STRING "LABEL_STRING"
#define FLD_NAME_SEQUENCE_STEP_LABEL_TRUE_STRING "LABEL_TRUE_STRING"
#define FLD_NAME_SEQUENCE_STEP_LABEL_FALSE_STRING "LABEL_FALSE_STRING"
#define FLD_NAME_SEQUENCE_STEP_BREAK_ACTIVE "BREAK_ACTIVE"
#define FLD_NAME_SEQUENCE_STEP_ASSOCIATED_MODE "ASSOCIATED_MODE"

#define FLD_NAME_CONNECTION_RULE_NAME "CONNECTION_RULE_NAME"
#define FLD_NAME_CONNECTION_RULE_IS_DEFAULT "CONNECTION_RULE_IS_DEFAULT"
#define FLD_NAME_CONNECTION_RULE_MASTER "CONNECTION_RULE_MASTER"
#define FLD_NAME_CONNECTION_RULE_BRIDGES "CONNECTION_RULE_BRIDGES"
#define FLD_NAME_CONNECTION_RULE_MASTER_COMP_ID "CONNECTION_RULE_MASTER_COMP_ID"
#define FLD_NAME_CONNECTION_RULE_MASTER_FAC_SEL "CONNECTION_RULE_MASTER_FAC_SEL"
#define FLD_NAME_CONNECTION_RULE_MASTER_MAS_SEL "CONNECTION_RULE_MASTER_MAS_SEL"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_NAME "CONNECTION_RULE_BRIDGE_NAME"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_COMP_ID  "CONNECTION_RULE_BRIDGE_FROM_COMP_ID"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_FAC_SEL "CONNECTION_RULE_BRIDGE_FROM_FAC_SEL"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_FROM_SEL "CONNECTION_RULE_BRIDGE_FROM_SEL"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_TO_COMP_ID "CONNECTION_RULE_BRIDGE_TO_COMP_ID"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_TO_FAC_SEL "CONNECTION_RULE_BRIDGE_TO_FAC_SEL"
#define FLD_NAME_CONNECTION_RULE_BRIDGE_TO_SEL "CONNECTION_RULE_BRIDGE_TO_SEL"

#define FLD_NAME_CONNECTION_PROCESS_DESCRIPTION  "CONNECTION_PROCESS_DESCRIPTION"
#define FLD_NAME_CONNECTION_PROCESS_UID "CONNECTION_PROCESS_UID"
#define FLD_NAME_CONNECTION_PROCESS_PROC_IS_AUTO_SETUP "CONNECTION_PROCESS_AUTO_SETUP"
#define FLD_NAME_CONNECTION_PROCESS_MASTER "CONNECTION_PROCESS_MASTER"
#define FLD_NAME_CONNECTION_PROCESS_BRIDGES "CONNECTION_PROCESS_BRIDGES"
#define FLD_NAME_CONNECTION_PROCESS_PATH_DESCRIPTION "CONNECTION_PROCESS_PATH_DESCRIPTION"
#define FLD_NAME_CONNECTION_PROCESS_TEST_SUCCESS "CONNECTION_PROCESS_TEST_SUCCESS"
#define FLD_NAME_CONNECTION_PROCESS_CHAIN_OK "CONNECTION_PROCESS_CHAIN_OK"
#define FLD_NAME_CONNECTION_PROCESS_REASON_IF_NOT_READY "CONNECTION_PROCESS_REASON_IF_NOT_READY"

#define FLD_NAME_CONNECTION_PROCESS_MASTER_NAME "CONNECTION_PROCESS_MASTER_NAME"
#define FLD_NAME_CONNECTION_PROCESS_MASTER_DESCRIPTION "CONNECTION_PROCESS_MASTER_DESCRIPTION"
#define FLD_NAME_CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID "CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID"
#define FLD_NAME_CONNECTION_PROCESS_MASTER_FACTORY_NAME  "CONNECTION_PROCESS_MASTER_FACTORY_NAME"
#define FLD_NAME_CONNECTION_PROCESS_BRIDGE_DESCRIPTOR "CONNECTION_PROCESS_BRIDGE_DESCRIPTOR"
#define FLD_NAME_CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR "CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR"
#define FLD_NAME_CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR "CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR"
#define FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_DESCRIPTION "CONNECTION_PROCESS_INPUT_CONNECTOR_DESCRIPTION"
#define FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID "CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID"
#define FLD_NAME_CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_NAME "CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_NAME"
#define FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_DESCRIPTION "CONNECTION_PROCESS_OUTPUT_CONNECTOR_DESCRIPTION"
#define FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID "CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID"
#define FLD_NAME_CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_NAME "CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_NAME"

#define FLD_NAME_PROCESS_STATUS_STATUS_INT32 "STATUS_INT32"
#define FLD_NAME_PROCESS_STATUS_IDX_SEQ_INT32 "IDX_SEQ_INT32"
#define FLD_NAME_PROCESS_STATUS_QUEUE_TYPE_INT32 "QUEUE_TYPE_INT32"
#define FLD_NAME_PROCESS_STATUS_IDX_STEP_INT32 "IDX_STEP_INT32"
#define FLD_NAME_PROCESS_STATUS_LOOP_ENABLED_BOOL "LOOP_ENABLED_BOOL"

#define EXCEPTION_ERROR_MESSAGE_ID -1
#define WRONG_DATATYPE_ID_ERROR_MESSAGE_ID -2
#define WRONG_ID_ERROR_MESSAGE_ID -3
#define WRONG_DATATYPE_CONTENT_ERROR_MESSAGE_ID -4
#define MISSING_CONTENT_ERROR_MESSAGE_ID -5
#define UNSUPPORTED_OPTION_ERROR_MESSAGE_ID -6
#define MISC_ERROR_MESSAGE_ID -7

#define JVX_FIO_STATE_UNINITIALIZED 0
#define JVX_FIO_STATE_INITIALIZED 1
#define JVX_FIO_STATE_SCANNED 2

//! Enum to specify the fileReaderMatlab commands in integer format
typedef enum
{
	JVX_HOST_COMMAND_INITIALIZE = 0,
	JVX_HOST_COMMAND_ACTIVATE,
	JVX_HOST_COMMAND_READ_CONFIG_FILE,
	JVX_HOST_COMMAND_INFO,

	JVX_HOST_COMMAND_SET_CONFIG_ENTRY,
	JVX_HOST_COMMAND_GET_CONFIG_ENTRY,

	JVX_HOST_COMMAND_WRITE_CONFIG,

	JVX_HOST_COMMAND_SELECT_COMPONENT,
	JVX_HOST_COMMAND_ACTIVATE_COMPONENT,
	JVX_HOST_COMMAND_ISREADY_COMPONENT,
	JVX_HOST_COMMAND_STATUS_COMPONENT,
	JVX_HOST_COMMAND_CONDITIONS_COMPONENT, 
	JVX_HOST_COMMAND_COMMANDS_COMPONENT, 
	JVX_HOST_COMMAND_RELATIVE_JUMS_COMPONENT,
	JVX_HOST_COMMAND_DEACTIVATE_COMPONENT,
	JVX_HOST_COMMAND_UNSELECT_COMPONENT,

	JVX_HOST_COMMAND_INFO_SEQUENCER,
	JVX_HOST_COMMAND_ADD_SEQUENCE,
	JVX_HOST_COMMAND_MARK_SEQUENCE_DEFAULT,
	JVX_HOST_COMMAND_REMOVE_SEQUENCE,
	JVX_HOST_COMMAND_RESET_SEQUENCE,
	JVX_HOST_COMMAND_INSERT_STEP_SEQUENCE,
	JVX_HOST_COMMAND_EDIT_STEP_SEQUENCE,
	JVX_HOST_COMMAND_REMOVE_STEP_SEQUENCE,
	JVX_HOST_COMMAND_SWITCH_STEP_SEQUENCE,
	JVX_HOST_COMMAND_SET_ACTIVE_SEQUENCE,
	JVX_HOST_COMMAND_START_PROCESS_SEQUENCER,
	JVX_HOST_COMMAND_TRIGGER_STOP_PROCESS_SEQUENCER,
	JVX_HOST_COMMAND_TRIGGER_STOP_SEQUENCE_SEQUENCER,
	JVX_HOST_COMMAND_WAIT_COMPLETION_PROCESS_SEQUENCER,
	JVX_HOST_COMMAND_STATUS_PROCESS_SEQUENCER,
	JVX_HOST_COMMAND_TRIGGER_EXTERNAL_PROCESS,

	JVX_HOST_COMMAND_INFO_PROPERTIES,
	JVX_HOST_COMMAND_LIST_PROPERTY_TAGS,
	JVX_HOST_COMMAND_GET_PROPERTY_IDX,
	JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID,
	JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR,
	JVX_HOST_COMMAND_SET_PROPERTY_IDX,
	JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID,
	JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR,
	JVX_HOST_COMMAND_GET_PROPERTY_IDX_OFF,
	JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID_OFF,
	JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR_OFF,
	JVX_HOST_COMMAND_SET_PROPERTY_IDX_OFF,
	JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID_OFF,
	JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR_OFF,
	JVX_HOST_COMMAND_START_PROPERTY_GROUP, 
	JVX_HOST_COMMAND_STATUS_PROPERTY_GROUP, 
	JVX_HOST_COMMAND_STOP_PROPERTY_GROUP,

	JVX_HOST_COMMAND_MSG_QUEUE_SET_CALLBACK_FUNCTION,
	JVX_HOST_COMMAND_MSG_QUEUE_GET_CALLBACK_FUNCTION,
	JVX_HOST_COMMAND_MSG_QUEUE_PROCESS_NEXT_MESSAGE,
	JVX_HOST_COMMAND_MSG_QUEUE_ADD_MESSAGE,

	JVX_HOST_CONNECTION_LIST_RULES,
	JVX_HOST_CONNECTION_LIST,
	JVX_HOST_CONNECTION_ADD_RULE,
	JVX_HOST_CONNECTION_REM_RULE,

	JVX_HOST_CONNECTION_TEST_CHAIN_MASTER,
	JVX_HOST_CONNECTION_RESULT_TEST_CHAIN_MASTER,
	JVX_HOST_CONNECTION_CHECK_READY_FOR_START,

	JVX_HOST_COMMAND_EXTERNAL_CALL,

	JVX_HOST_COMMAND_DEACTIVATE,
	JVX_HOST_COMMAND_TERMINATE,

	JVX_HOST_LOOKUP_TYPE_ID_4_NAME,
	JVX_HOST_LOOKUP_TYPE_NAME_4_VALUE,
	JVX_HOST_LOOKUP_CONST_4_NAME,

	JVX_HOST_EXPORT_CONFIG_STRING,
	JVX_HOST_EXPORT_CONFIG_STRUCT,
	JVX_HOST_STRING_TO_CONFIG_STRUCT,

	JVX_HOST_COMMAND_LIMIT
} JVX_HOST_COMMAND;

 typedef struct
 {
	 JVX_HOST_COMMAND cmdId;
	 std::string cmdToken;
	 std::string cmdDescription;
 } jvxFioCmdMapper;

 static jvxFioCmdMapper mapperCommands[JVX_HOST_COMMAND_LIMIT] =
 {
	 {JVX_HOST_COMMAND_INITIALIZE, "initialize", "--"},
	 {JVX_HOST_COMMAND_ACTIVATE, "activate", "--"},
	 {JVX_HOST_COMMAND_READ_CONFIG_FILE, "read_config_file", "--"},
	 {JVX_HOST_COMMAND_INFO, "info", "--"},

	 {JVX_HOST_COMMAND_SET_CONFIG_ENTRY, "set_config_entry", "\n -> Input argument #1: <string> to specify entry key (name)\n -> Input argument #2..: Specific data to be passed to component: \n ---++---> <numeric, 1x1 scalar>: Input #2: store an assignment value\n ---++---> <numeric, NxM scalar>: Input #2: store a value list\n ---++---> <string>: Input #2: store an assignment value\n ---++---> <cell array of strings>: Input #2: store a string list"},
	 {JVX_HOST_COMMAND_GET_CONFIG_ENTRY, "get_config_entry", "\n -> Input argument #1: <string> to specify entry key (name)"},
	 {JVX_HOST_COMMAND_WRITE_CONFIG, "write_config_file", "\n -> (Optional) Input argument #1: <string> to specify config file name"},

	 {JVX_HOST_COMMAND_SELECT_COMPONENT, "select_component", "\n -> Input argument #1: <numeric> to indicate component type\n -> Input argument #2: <numeric> to indicate index;"},
	 {JVX_HOST_COMMAND_ACTIVATE_COMPONENT, "activate_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_ISREADY_COMPONENT, "isready_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_STATUS_COMPONENT, "status_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_CONDITIONS_COMPONENT, "conditions_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_COMMANDS_COMPONENT, "commands_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_RELATIVE_JUMS_COMPONENT, "rel_jumps_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_DEACTIVATE_COMPONENT, "deactivate_component", "\n -> Input argument #1: <numeric> to indicate component type;"},
	 {JVX_HOST_COMMAND_UNSELECT_COMPONENT, "unselect_component", "\n -> Input argument #1: <numeric> to indicate component type;"},

	 {JVX_HOST_COMMAND_INFO_SEQUENCER, "info_sequencer", ""},
	 {JVX_HOST_COMMAND_ADD_SEQUENCE, "add_sequence", "\n -> Input argument #1: <string> for description\n -> Input argument #2: <string> for description (optional);"},
	 {JVX_HOST_COMMAND_MARK_SEQUENCE_DEFAULT, "mark_default_sequence", "\n -> Input argument #1: <string> for description"},
	 {JVX_HOST_COMMAND_REMOVE_SEQUENCE, "remove_sequence", "\n -> Input argument #1: <numeric> for index of sequence"},
	 {JVX_HOST_COMMAND_RESET_SEQUENCE, "reset_sequence", ""},
	 {JVX_HOST_COMMAND_INSERT_STEP_SEQUENCE, "insert_step_sequence", "\n -> Input argument #1: <numeric> for index of sequence\n -> Input argument #2: <numeric> for sequence element type\n -> Input argument #3: <numeric> for sequencer queue type\n -> Input argument #4: <numeric> for target component type\n -> Input argument #5: <string> for sequence step description\n -> Input argument #6: <numeric> for function/command id\n -> Input argument #7: <numeric> for sequencer step timeout [ms]\n -> Input argument #8,#9,#10: <string> for sequencer label, label cond1 and label cond2\n -> Input argument #11: <numeric> for insert position"},
	 {JVX_HOST_COMMAND_EDIT_STEP_SEQUENCE, "edit_step_sequence", "\n -> Input argument #1: <numeric> for index of sequence\n -> Input argument #2: <numeric> for sequence element type\n -> Input argument #3: <numeric> for sequencer queue type\n -> Input argument #4: <numeric> for target component type\n -> Input argument #5: <string> for sequence step description\n -> Input argument #6: <numeric> for function/command id\n -> Input argument #7: <numeric> for sequencer step timeout [ms]\n -> Input argument #8,#9,#10: <string> for sequencer label, label cond1 and label cond2\n -> Input argument #11: <numeric> for insert position"},
	 {JVX_HOST_COMMAND_REMOVE_STEP_SEQUENCE, "remove_step_sequence", "\n -> Input argument #1: <numeric> for index of sequence\n -> Input argument #2: <numeric> for sequence queue type\n -> Input argument #3: <numeric> for step index"},
	 {JVX_HOST_COMMAND_SWITCH_STEP_SEQUENCE, "switch_steps_sequence", "\n -> Input argument #1: <numeric> for index of sequence\n -> Input argument #2: <numeric> for sequence queue type\n -> Input argument #3: <numeric> for step index [1]\n -> Input argument #4: <numeric> for step index [2]"},
	 {JVX_HOST_COMMAND_SET_ACTIVE_SEQUENCE, "set_active_sequence", "\n -> Input argument #1: <numeric> for index of sequence\n -> Input argument #2: <bool> for bool to indicate activation"},
	 {JVX_HOST_COMMAND_START_PROCESS_SEQUENCER, "start_process_sequence", "\n -> Input argument #1: <numeric> for sequencer timeout [ms]\n -> Input argument #2: <numeric> for report event mask\n -> Input argument #3: <bool> for bool to indicate external trigger"},
	 {JVX_HOST_COMMAND_TRIGGER_STOP_PROCESS_SEQUENCER, "trigger_stop_process", "--"},
	 {JVX_HOST_COMMAND_TRIGGER_STOP_SEQUENCE_SEQUENCER, "trigger_stop_sequence", "--"},
	 {JVX_HOST_COMMAND_WAIT_COMPLETION_PROCESS_SEQUENCER, "wait_completion_process", "--"},
	 {JVX_HOST_COMMAND_STATUS_PROCESS_SEQUENCER, "status_process_sequence", "--"},
	 {JVX_HOST_COMMAND_TRIGGER_EXTERNAL_PROCESS, "trigger_external_process", "\n -> Input argument #1: <numeric> for timestamp [us]"},

	 {JVX_HOST_COMMAND_INFO_PROPERTIES, "info_properties", "\n -> Input argument #1: <numeric|string> to indicate component type"},
	 {JVX_HOST_COMMAND_LIST_PROPERTY_TAGS, "list_properties", "\n -> (Optional) Input argument #1: <numeric|string> to indicate component type"},
	 
	{JVX_HOST_COMMAND_START_PROPERTY_GROUP, "start_property_group", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type"},
	{JVX_HOST_COMMAND_STATUS_PROPERTY_GROUP, "status_property_group", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type"},
	{JVX_HOST_COMMAND_STOP_PROPERTY_GROUP, "stop_property_group", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type"},

	 {JVX_HOST_COMMAND_GET_PROPERTY_IDX, "get_property_idx", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type\n "
		"-> Input argument #2: <numeric> to indicate property index [0..N-1]"},
	 {JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID, "get_property_uniqueid", "\n"
		"-> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate category\n "
		"-> Input argument #3: <numeric> to specify unique ID of property \n "},
	 {JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR, "get_property_descriptor", "\n"
		"-> Input argument #1: <numeric|string> to indicate component type\n "
		"-> Input argument #2: <string> to indicate descriptor"},

	 {JVX_HOST_COMMAND_SET_PROPERTY_IDX, "set_property_idx", "\n -> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate property index [0..N-1] \n "
		"-> Input argument #3..: Specific data to be passed to component:\n "
			"---++---> <numeric>: Input #3: 1xM Array of approriate type\n "
			"---++---> <string>: Input #3: string value\n "
			"---++---> <string_list>: Input #3: cell of strings\n "
			"---++---> <selection_list>:\n"
				"---++---+++---> option 1: Input #3: selection;\n "
				"---++---+++---> option 2: Input #3: selection, Input #4: exclusive; Input #5: cell array of string\n"
			"---++---> <value_in_range>:\n "
				"---++---+++---> option 1: Input #3: value in " JVX_DATA_DESCRIPTOR "\n"
				"---++---+++---> option 2: Input #3: value in " JVX_DATA_DESCRIPTOR ", Input #4: minimum value in " JVX_DATA_DESCRIPTOR "; Input #5: maximum value in " JVX_DATA_DESCRIPTOR},
	 {JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID, "set_property_uniqueid", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate category\n "
		"-> Input argument #3: <numeric> to specify unique ID of property\n "
		"-> Input argument #4..: Specific data to be passed to component:\n "
			"---++---> <numeric>: Input #4: 1xM Array of approriate type\n "
			"---++---> <string>: Input #4: string value\n "
			"---++---> <string_list>: Input #4: cell of strings\n "
			"---++---> <selection_list>:\n "
				"---++---+++---> option 1: Input #4: selection;\n "
				"---++---+++---> option 2: Input #4: selection, Input #5: exclusive; Input #6: cell array of string\n "
			"---++---> <value_in_range>:\n "
				"---++---+++---> option 1: Input #4: value in " JVX_DATA_DESCRIPTOR "\n "
				"---++---+++---> option 2: Input #4: value in " JVX_DATA_DESCRIPTOR ", Input #5: minimum value in " JVX_DATA_DESCRIPTOR "; Input #6: maximum value in " JVX_DATA_DESCRIPTOR},
	 {JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR, "set_property_descriptor", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type\n "
		"-> Input argument #2: <string> to indicate descriptor\n" 
		"-> Input argument #3..: Specific data to be passed to component:\n "
			"---++---> <numeric>: Input #3: 1xM Array of approriate type\n "
			"---++---> <string>: Input #3: string value\n "
			"---++---> <string_list>: Input #3: cell of strings\n "
			"---++---> <selection_list>:\n "
				"---++---+++---> option 1: Input #3: selection;\n "
				"---++---+++---> option 2: Input #3: selection, Input #4: exclusive; Input #5: cell array of string\n "
			"---++---> <value_in_range>:\n "
				"---++---+++---> option 1: Input #3: value in " JVX_DATA_DESCRIPTOR "\n "
				"---++---+++---> option 2: Input #3: value in " JVX_DATA_DESCRIPTOR ", Input #4: minimum value in " JVX_DATA_DESCRIPTOR "; Input #5: maximum value in " JVX_DATA_DESCRIPTOR},

	{ JVX_HOST_COMMAND_GET_PROPERTY_IDX_OFF, "get_property_idx_o", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type\n "
		"-> Input argument #2: <numeric> to indicate property index [0..N-1] \n "
		"-> Input argument #3: <numeric> to indicate property offset" },
	 { JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID_OFF, "get_property_uniqueid_o", "\n"
		"-> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate category\n "
		"-> Input argument #3: <numeric> to specify unique ID of property \n " 
		"-> Input argument #4: <numeric> to indicate property offset" },
	 { JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR_OFF, "get_property_descriptor_o", "\n"
		"-> Input argument #1: <numeric|string> to indicate component type\n "
		"-> Input argument #2: <string> to indicate descriptor \n "
		"-> Input argument #3: <numeric> to indicate property offset" },

	 { JVX_HOST_COMMAND_SET_PROPERTY_IDX_OFF, "set_property_idx_o", "\n -> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate property index [0..N-1] \n "
		"-> Input argument #3: <numeric> to indicate property offset \n "
		"-> Input argument #4..: Specific data to be passed to component:\n "
			"---++---> <numeric>: Input #3: 1xM Array of approriate type\n "
			 "---++---> <string>: Input #3: string value\n "
			"---++---> <string_list>: Input #3: cell of strings\n "
			"---++---> <selection_list>:\n"
				"---++---+++---> option 1: Input #3: selection;\n "
				"---++---+++---> option 2: Input #3: selection, Input #4: exclusive; Input #5: cell array of string\n"
			"---++---> <value_in_range>:\n "
				"---++---+++---> option 1: Input #3: value in " JVX_DATA_DESCRIPTOR "\n"
				"---++---+++---> option 2: Input #3: value in " JVX_DATA_DESCRIPTOR ", Input #4: minimum value in " JVX_DATA_DESCRIPTOR "; Input #5: maximum value in " JVX_DATA_DESCRIPTOR },
	{ JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID_OFF, "set_property_uniqueid_o", "\n "
		"-> Input argument #1: <numeric|string> to indicate component type\n"
		"-> Input argument #2: <numeric> to indicate category\n "
		"-> Input argument #3: <numeric> to specify unique ID of property\n "
		"-> Input argument #4: <numeric> to indicate property offset \n "
		"-> Input argument #5..: Specific data to be passed to component:\n "
			"---++---> <numeric>: Input #4: 1xM Array of approriate type\n "
			"---++---> <string>: Input #4: string value\n "
			"---++---> <string_list>: Input #4: cell of strings\n "
			"---++---> <selection_list>:\n "
				"---++---+++---> option 1: Input #4: selection;\n "
				"---++---+++---> option 2: Input #4: selection, Input #5: exclusive; Input #6: cell array of string\n "
			"---++---> <value_in_range>:\n "
				"---++---+++---> option 1: Input #4: value in " JVX_DATA_DESCRIPTOR "\n "
				"---++---+++---> option 2: Input #4: value in " JVX_DATA_DESCRIPTOR ", Input #5: minimum value in " JVX_DATA_DESCRIPTOR "; Input #6: maximum value in " JVX_DATA_DESCRIPTOR },
	{ JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR_OFF, "set_property_descriptor_o", "\n "
					 "-> Input argument #1: <numeric|string> to indicate component type\n "
					 "-> Input argument #2: <string> to indicate descriptor\n"
					 "-> Input argument #3: <numeric> to indicate property offset \n "
					 "-> Input argument #4..: Specific data to be passed to component:\n "
					 "---++---> <numeric>: Input #3: 1xM Array of approriate type\n "
					 "---++---> <string>: Input #3: string value\n "
					 "---++---> <string_list>: Input #3: cell of strings\n "
					 "---++---> <selection_list>:\n "
					 "---++---+++---> option 1: Input #3: selection;\n "
					 "---++---+++---> option 2: Input #3: selection, Input #4: exclusive; Input #5: cell array of string\n "
					 "---++---> <value_in_range>:\n "
					 "---++---+++---> option 1: Input #3: value in " JVX_DATA_DESCRIPTOR "\n "
					 "---++---+++---> option 2: Input #3: value in " JVX_DATA_DESCRIPTOR ", Input #4: minimum value in " JVX_DATA_DESCRIPTOR "; Input #5: maximum value in " JVX_DATA_DESCRIPTOR },

	{ JVX_HOST_CONNECTION_LIST_RULES, "list_connection_rules", "\n ->  Optional Input argument #1: <string> to specify connection rule to view. If no argument, all connection rules are returned." },
	{ JVX_HOST_CONNECTION_LIST, "list_connections", "\n ->  Optional Input argument #1: <string> to specify connection to view. If no argument, all connections are returned." },
	{ JVX_HOST_CONNECTION_ADD_RULE, "add_connection_rule", "\n "
		 "-> Input argument #1: <string> to specify connection rule\n"
		 "-> Input argument #2: <component ident> for master \n "
		 "-> Input argument #3: <string> master factory wildcard\n "
		 "-> Input argument #4: <string> master wildcard\n "
				"-> Input arguments #5..: sets of 7 arguments for each bridge..\n "
				"---++---> <string> bridge name \n "
				"---++---> <component ident> bridge from component \n "
				"---++---> <string> bridge from factory wildcard \n "
				"---++---> <string> bridge from wildcard \n "
					"---++---> <component ident> bridge to component \n "
					"---++---> <string> bridge to factory wildcard \n "
					"---++---> <string> bridge to wildcard \n " },
	{ JVX_HOST_CONNECTION_REM_RULE, "rem_connection_rule", "\n ->  Input argument #1: <string> to specify connection rule" },

	{ JVX_HOST_CONNECTION_TEST_CHAIN_MASTER , "test_connection_master", "\n ->  Input argument #1: <id> process id, typically this function is called from within message queue callback" },
	{ JVX_HOST_CONNECTION_RESULT_TEST_CHAIN_MASTER , "result_test_connection_master", "\n ->  Input argument #1: <id> process id" },
	{ JVX_HOST_CONNECTION_CHECK_READY_FOR_START, "check_ready_for_start", "\n -> " },

	 {JVX_HOST_COMMAND_MSG_QUEUE_SET_CALLBACK_FUNCTION, "set_callback_msgq", "\n ->  Input argument #1: <string> to specify callback prefix"},
	 {JVX_HOST_COMMAND_MSG_QUEUE_GET_CALLBACK_FUNCTION, "get_callback_msgq", "\n ->  "},
	 {JVX_HOST_COMMAND_MSG_QUEUE_PROCESS_NEXT_MESSAGE, "trigger_process_msgq", "\n -> "},
	 {JVX_HOST_COMMAND_MSG_QUEUE_ADD_MESSAGE, "add_user_message_msgq", "\n -> Input argument #1: <numeric> to indicate message type-> Input argument #2: Type and structure depends on argument #1, refer to manual"},

	 {JVX_HOST_COMMAND_EXTERNAL_CALL, "external_call", "--"},

	 { JVX_HOST_LOOKUP_TYPE_ID_4_NAME, "lookup_type_id__name", "\n -> Input argument #1: <string> to indicate lookup category-> Input argument #2: <string> to indicate lookup expression" },
	 { JVX_HOST_LOOKUP_TYPE_NAME_4_VALUE, "lookup_type_name__value", "\n -> Input argument #1: <string> to indicate lookup category-> Input argument #2: <numeric> to indicate lookup id" },
	 { JVX_HOST_LOOKUP_CONST_4_NAME, "lookup_const__name", "\n -> Input argument #1: <string> to indicate const lookup specifier" },

	 { JVX_HOST_EXPORT_CONFIG_STRING, "export_config_string", "\n -> Input argument #1: <numeric|string> to indicate component type" },
	 { JVX_HOST_EXPORT_CONFIG_STRUCT, "export_config_struct", "\n -> Input argument #1: <numeric|string> to indicate component type-> (Optional) Input argument #2: <string> to parse to create config struct" },
	 { JVX_HOST_STRING_TO_CONFIG_STRUCT, "convert_string_config_struct", "\n -> Input argument #1: <string> to parse to create config struct" },
	 
	 { JVX_HOST_COMMAND_DEACTIVATE, "deactivate", "--" },

	 {JVX_HOST_COMMAND_TERMINATE, "terminate", "--"}
 };

 // ================================================================================================

 /*
//! Typedef specification for the access of fields for datachunks
typedef enum
{
	JVX_FIO_COPY_DATA = 0,
	JVX_FIO_COPY_ID_CHUNK = 1,
	JVX_FIO_COPY_ID_DATA_TYPE = 2,
	JVX_FIO_COPY_ID_USER = 3,
	JVX_FIO_COPY_SUB_ID_USER = 4,
	JVX_FIO_COPY_TIMING = 5,
	JVX_FIO_COPY_NUMBER_ELEMENTS = 6,
	JVX_FIO_COPY_LIMIT = 7
} JVX_FIO_COPY_TYPE;

typedef struct
 {
	 JVX_FIO_COPY_TYPE cmdId;
	 std::string cmdToken;
	 std::string cmdDescription;
 } jvxFioDataCopyTypeMapper;

static jvxFioDataCopyTypeMapper mapperDataCopyTypes[JVX_FIO_COPY_LIMIT] =
{
	 {JVX_FIO_COPY_DATA, "copy_data", "--"},
	 {JVX_FIO_COPY_ID_CHUNK, "copy_id_chunk", "--"},
	 {JVX_FIO_COPY_ID_DATA_TYPE, "copy_id_data_type", "--"},
	 {JVX_FIO_COPY_ID_USER, "copy_id_user", "--"},
	 {JVX_FIO_COPY_SUB_ID_USER, "copy_sub_id_user", "--"},
	 {JVX_FIO_COPY_TIMING, "copy_timing", "--"},
	 {JVX_FIO_COPY_NUMBER_ELEMENTS, "copy_number_elements", "--"}
};
*/
