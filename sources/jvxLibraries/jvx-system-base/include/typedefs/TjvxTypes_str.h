#ifndef __TJVXTYPES_STR_H__
#define __TJVXTYPES_STR_H__

static jvxTextHelpers jvxSizeSpecialValues[JVX_SIZE_NUM_EXTRA_CASES_WITH_0] =
{
	{"", ""},
	{"unselected", "JVX_SIZE_UNSELECTED"},
	{"dont-care", "JVX_SIZE_DONTCARE"},
	{"slot-retain", "JVX_SIZE_SLOT_RETAIN"},
	{"off-sys", "JVX_SIZE_SLOT_OFF_SYSTEM"},
	{"inc", "JVX_SIZE_STATE_INCREMENT"},
	{"dec", "JVX_SIZE_STATE_DECREMENT"}
};

static jvxSize valuesSizeSpecialValues[JVX_SIZE_NUM_EXTRA_CASES_WITH_0] =
{
	0,
	JVX_SIZE_UNSELECTED,
	JVX_SIZE_DONTCARE,
	JVX_SIZE_SLOT_RETAIN,
	JVX_SIZE_SLOT_OFF_SYSTEM,
	JVX_SIZE_STATE_INCREMENT,
	JVX_SIZE_STATE_DECREMENT
};

inline jvxSize jvxSizeSpecialValues_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_SIZE_NUM_EXTRA_CASES_WITH_0; i++)
	{
		if (strcmp(jvxSizeSpecialValues[i].friendly, txt) == 0)
		{
			return valuesSizeSpecialValues[i];
		}
		if (strcmp(jvxSizeSpecialValues[i].full, txt) == 0)
		{
			return valuesSizeSpecialValues[i];
		}
	}
	return 0;
}

// =====================================================================

static jvxTextHelpers jvxAccessProtocol_str[JVX_ACCESS_PROTOCOL_LIMIT] =
{
	{ "ok", "JVX_ACCESS_PROTOCOL_OK" },
	{"invalid", "JVX_ACCESS_PROTOCOL_INVALID"},
	{"no write", "JVX_ACCESS_PROTOCOL_NO_ACCESS__WRITE"},
	{"no read", "JVX_ACCESS_PROTOCOL_NO_ACCESS__READ"},
	{"no right", "JVX_ACCESS_PROTOCOL_NO_RIGHT"},
	{"no data", "JVX_ACCESS_PROTOCOL_NO_DATA" },
	{"error", "JVX_ACCESS_PROTOCOL_ERROR"},
	{"no call", "JVX_ACCESS_PROTOCOL_NO_CALL"}
};

inline const char* jvxAccessProtocol_txt(jvxSize id)
{
	assert(id < JVX_ACCESS_PROTOCOL_LIMIT);
	return(jvxAccessProtocol_str[id].friendly);
}

inline jvxAccessProtocol jvxAccessProtocol_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_ACCESS_PROTOCOL_LIMIT; i++)
	{
		if (strcmp(jvxAccessProtocol_str[i].friendly, txt) == 0)
		{
			return (jvxAccessProtocol)i;
		}
		if (strcmp(jvxAccessProtocol_str[i].full, txt) == 0)
		{
			return (jvxAccessProtocol)i;
		}
	}
	return JVX_ACCESS_PROTOCOL_LIMIT;
}

// ==========================================================================

static jvxTextHelpers jvxPropertyAccessType_str[JVX_PROPERTY_ACCESS_LIMIT] =
{
	{ "none", "JVX_PROPERTY_ACCESS_NONE" },
	{"full read + write", "JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE"},
	{"read + write", "JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT"},
	{"read", "JVX_PROPERTY_ACCESS_READ_ONLY"},
	{"reados", "JVX_PROPERTY_ACCESS_READ_ONLY_ON_START"},
	{"write", "JVX_PROPERTY_ACCESS_WRITE_ONLY" }
};

inline const char* jvxPropertyAccessType_txt(jvxSize id)
{
	assert(id < JVX_PROPERTY_ACCESS_LIMIT);
	return(jvxPropertyAccessType_str[id].friendly);
}

inline jvxPropertyAccessType jvxPropertyAccessType_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_PROPERTY_ACCESS_LIMIT; i++)
	{
		if (strcmp(jvxPropertyAccessType_str[i].friendly, txt) == 0)
		{
			return (jvxPropertyAccessType)i;
		}
		if (strcmp(jvxPropertyAccessType_str[i].full, txt) == 0)
		{
			return (jvxPropertyAccessType)i;
		}
	}
	return JVX_PROPERTY_ACCESS_LIMIT;
}

// ==================================================================

static jvxTextHelpers jvxPropertyInvalidateType_str[JVX_PROPERTY_INVALIDATE_LIMIT] =
{
	{ "inactive", "JVX_PROPERTY_INVALIDATE_INACTIVE" },
	{ "content", "JVX_PROPERTY_INVALIDATE_CONTENT" },
	{ "descr-content", "JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT" }
};

inline const char* jvxPropertyInvalidateType_txt(jvxSize id)
{
	assert(id < JVX_PROPERTY_INVALIDATE_LIMIT);
	return(jvxPropertyInvalidateType_str[id].friendly);
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxPropertyExtendedInfoType_str[JVX_PROPERTY_INFO_LIMIT] =
{
	{ "none", "JVX_PROPERTY_INFO_UNKNOWN" },
	{ "show hint", "JVX_PROPERTY_INFO_SHOWHINT"},
	{ "more info", "JVX_PROPERTY_INFO_MOREINFO"},
	{ "origin", "JVX_PROPERTY_INFO_ORIGIN"},
	{ "update token", "JVX_PROPERTY_INFO_UPDATE_CONTENT_TOKEN"},
	{ "rec size y", "JVX_PROPERTY_INFO_RECOMMENDED_SIZE_Y"},
	{ "rec size x", "JVX_PROPERTY_INFO_RECOMMENDED_SIZE_X"},
	{ "group id", "JVX_PROPERTY_INFO_GROUPID"}
};
	
inline const char* jvxPropertyExtendedInfoType_txt(jvxSize id)
{
	if(id < JVX_PROPERTY_INFO_LIMIT)
	{
		return(jvxPropertyExtendedInfoType_str[id].friendly);
	}
	return("NONE SYSTEM ID");
}

// ==================================================================
// ==================================================================

#define JVXCONFIGSECTION_NUM 8

static jvxTextHelpers jvxConfigSection_str[JVXCONFIGSECTION_NUM] =
{
	{ "unknown", "JVX_CONFIG_SECTION_TYPE_UNKNOWN" },
	{ "section", "JVX_CONFIG_SECTION_TYPE_SECTION" },
	{ "astring", "JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING" },
	{ "avalue", "JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE" },
	{ "vlist", "JVX_CONFIG_SECTION_TYPE_VALUELIST" },
	{ "slist", "JVX_CONFIG_SECTION_TYPE_STRINGLIST" },
	{ "reference", "JVX_CONFIG_SECTION_TYPE_REFERENCE" },
	{ "hstring", "JVX_CONFIG_SECTION_TYPE_ASSIGNMENTHEXSTRING" }
};

inline const char* jvxSectionType_txt(jvxSize id)
{
	assert(id < JVXCONFIGSECTION_NUM);
	return(jvxConfigSection_str[id].friendly);
}

// ==================================================================
// ==================================================================

#define JVXSTATE_NUM 8

static jvxTextHelpers jvxState_str[JVXSTATE_NUM] =
{
	{"none", "JVX_STATE_NONE"},
	{"init", "JVX_STATE_INIT"},
	{"selected", "JVX_STATE_SELECTED"},
	{"active", "JVX_STATE_ACTIVE"},
	{"prepared", "JVX_STATE_PREPARED"},
	{"processing", "JVX_STATE_PROCESSING"},
	{"complete", "JVX_STATE_COMPLETE"},
	{"dontcare", "JVX_STATE_DONTCARE"}
};

inline const char* jvxState_txt(jvxState stat)
{
	jvxSize i;
	jvxSize id = JVX_SIZE_UNSELECTED;

	if (stat == JVX_STATE_DONTCARE)
	{
		return(jvxState_str[JVXSTATE_NUM - 1].friendly);
	}

	jvxCBitField bf = (jvxCBitField)stat;
	for (i = 0; i < sizeof(jvxCBitField) * 8; i++)
	{
		if(bf & ((jvxCBitField)1 << i))
		{
			id = i;
			break;
		}
	}
	assert(JVX_CHECK_SIZE_SELECTED(id));
	assert(id < JVXSTATE_NUM-1);
	return(jvxState_str[id].friendly);
}

inline std::string jvxState_dec(jvxSize stat)
{
	jvxSize i;
	std::string ret;

	for (i = 0; i < JVXSTATE_NUM; i++)
	{
		if (stat & ((jvxSize)1 << i))
		{
			if (ret.size())
			{
				ret += ":";
				ret += +jvxState_str[i].friendly;
			}
			else
				ret = jvxState_str[i].friendly;
		}
	}
	return(ret);
}

inline jvxSize jvxState_decode(std::string in, jvxBool& err)
{

	jvxSize i;
	std::string token = in;
	jvxSize retVal = 0;
	err = false;
	while (in.size())
	{
		size_t pos = std::string::npos;
		pos = in.find(':');
		if (pos != std::string::npos)
		{
			token = in.substr(0, pos);
			in = in.substr(pos + 1, std::string::npos);
		}
		else
		{
			token = in;
			in.clear();
		}

		// token = removeSpacesLR(token);
		for (i = 0; i < JVXSTATE_NUM; i++)
		{
			if ((jvxState_str[i].friendly == token) || (jvxState_str[i].full == token))
			{
				retVal |= (jvxSize)1 << i; // <- NONE state is equal to 0x1!
				break;
			}
		}
	}
	return(retVal);
}

static jvxCBitField jvxState_map[JVXSTATE_NUM] =
{
	(jvxCBitField)JVX_STATE_NONE, 
	(jvxCBitField)JVX_STATE_INIT,
	(jvxCBitField)JVX_STATE_SELECTED,
	(jvxCBitField)JVX_STATE_ACTIVE,
	(jvxCBitField)JVX_STATE_PREPARED,
	(jvxCBitField)JVX_STATE_PROCESSING,
	(jvxCBitField)JVX_STATE_COMPLETE,
	(jvxCBitField)JVX_STATE_DONTCARE 
};

static jvxTextHelpers jvxStateSwitch_str[JVX_STATE_SWITCH_LIMIT] =
{
	{"none", "JVX_STATE_SWITCH_NONE" },
	{"initialize", "JVX_STATE_SWITCH_INIT" },
	{"select", "JVX_STATE_SWITCH_SELECT" },
	{"activate", "JVX_STATE_SWITCH_ACTIVATE" },
	{"prepare", "JVX_STATE_SWITCH_PREPARE" },
	{"start", "JVX_STATE_SWITCH_START" },
	{"stop", "JVX_STATE_SWITCH_STOP" },
	{"postprocess", "JVX_STATE_SWITCH_POSTPROCESS" },
	{"deactivate", "JVX_STATE_SWITCH_DEACTIVATE" },
	{"unselect", "JVX_STATE_SWITCH_UNSELECT" },
	{"terminate", "JVX_STATE_SWITCH_TERMINATE"}
};

inline const char* jvxStateSwitch_txt(jvxSize id)
{
	assert(id < JVX_STATE_SWITCH_LIMIT);
	return(jvxStateSwitch_str[id].friendly);
}

// ==================================================================
// Component Types
// ==================================================================

static jvxTextHelpers jvxInterfaceType_str[JVX_INTERFACE_LIMIT] =
{
	{"unknown", "JVX_INTERFACE_UNKNOWN"},
	{"connector factory", "JVX_INTERFACE_CONNECTOR_FACTORY"},
	{"connector master factory", "JVX_INTERFACE_CONNECTOR_MASTER_FACTORY"},
	{"configuration", "JVX_INTERFACE_CONFIGURATION" },
	{"tools host", "JVX_INTERFACE_TOOLS_HOST" },
	{"report message", "JVX_INTERFACE_REPORT" },
	{"report system message", "JVX_INTERFACE_REPORT_SYSTEM" },
	{"properties", "JVX_INTERFACE_PROPERTIES" },
	{"sequencer control", "JVX_INTERFACE_SEQUENCERCONTROL" },
	{"sequencer", "JVX_INTERFACE_SEQUENCER" },
	{"configuration line", "JVX_INTERFACE_CONFIGURATIONLINE" },
	{"property pool", "JVX_INTERFACE_PROPERTY_POOL" },
	{"schedule", "JVX_INTERFACE_SCHEDULE" },
	{"uniqueid", "JVX_INTERFACE_UNIQUE_ID"},
	{"host", "JVX_INTERFACE_HOST" },	
	{"host type handler", "JVX_INTERFACE_HOSTTYPEHANDLER" },
	{"event loop", "JVX_INTERFACE_EVENTLOOP" },
	{"qt widget host", "JVX_INTERFACE_QT_WIDGET_HOST" },
	{"qt widget ui", "JVX_INTERFACE_QT_WIDGET_UI" },
	{"binary fast data control", "JVX_INTERFACE_BINARY_FAST_DATA_CONTROL" },
	{"data processor connect", "JVX_INTERFACE_DATAPROCESSOR_CONNECTOR" },
	{"remote call async", "JVX_INTERFACE_REMOTE_CALL_ASYNC" },
	{"printf", "JVX_INTERFACE_PRINTF" },
	{"config done", "JVX_INTERFACE_CONFIGURATION_DONE"},
	{"config extender", "JVX_INTERFACE_CONFIGURATION_EXTENDER"},
	{"data connections", "JVX_INTERFACE_DATA_CONNECTIONS"},
	{"auto data connect", "JVX_INTERFACE_AUTO_DATA_CONNECT"},
	{"http", "JVX_INTERFACE_HTTP_API"},
	{"manipulate", "JVX_INTERFACE_MANIPULATE"},
	{"commandline", "JVX_INTERFACE_COMMAND_LINE"},
	{"bootsteps", "JVX_INTERFACE_BOOT_STEPS"},
	{"command interp", "JVX_INTERFACE_COMMAND_INTERPRETER"},
	{"prop_attach", "JVX_INTERFACE_PROPERTY_ATTACH"},
	{"config_attach", "JVX_INTERFACE_CONFIGURATION_ATTACH"},
	{"package", "JVX_INTERFACE_PACKAGE"},
	{"component host ext", "JVX_INTERFACE_COMPONENT_HOST_EXT"}

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "extensions/EpjvxTypes_interfaces.h"
#endif
};

inline const char* jvxInterfaceType_txt(jvxSize id)
{
	if(id < JVX_INTERFACE_LIMIT)
	{
		return(jvxInterfaceType_str[id].friendly);
	}
	return("Unknown Interface Type");
}

// ==================================================================
// Category
// ==================================================================
static jvxTextHelpers jvxPropertyCategoryType_str[JVX_PROPERTY_CATEGORY_LIMIT] =
{
	{"unknown", "JVX_PROPERTY_CATEGORY_UNKNOWN"},
	{"predefined","JVX_PROPERTY_CATEGORY_PREDEFINED" },
	{"unspecific", "JVX_PROPERTY_CATEGORY_UNSPECIFIC" }
};

inline const char* jvxPropertyCategoryType_txt(jvxSize id)
{
	assert(id < JVX_PROPERTY_CATEGORY_LIMIT);
	return(jvxPropertyCategoryType_str[id].friendly);
}

inline jvxPropertyCategoryType jvxPropertyCategoryType_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_PROPERTY_CATEGORY_LIMIT; i++)
	{
		if (strcmp(jvxPropertyCategoryType_str[i].friendly, txt) == 0)
		{
			return (jvxPropertyCategoryType)i;
		}
		if (strcmp(jvxPropertyCategoryType_str[i].full, txt) == 0)
		{
			return (jvxPropertyCategoryType)i;
		}
	}
	return JVX_PROPERTY_CATEGORY_LIMIT;
}

// ==================================================================
// Property context
// ==================================================================
static jvxTextHelpers jvxPropertyContext_str[JVX_PROPERTY_CONTEXT_LIMIT] =
{
	{"unknown", "JVX_PROPERTY_CONTEXT_UNKNOWN"},
	{"parameter", "JVX_PROPERTY_CONTEXT_PARAMETER"},
	{"info" , "JVX_PROPERTY_CONTEXT_INFO"},
	{"result", "JVX_PROPERTY_CONTEXT_RESULT"},
	{"command", "JVX_PROPERTY_CONTEXT_COMMAND"},
	{"view", "JVX_PROPERTY_CONTEXT_VIEWBUFFER"}
};

inline const char* jvxPropertyContext_txt(jvxSize id)
{
	assert(id < JVX_PROPERTY_CONTEXT_LIMIT);
	return(jvxPropertyContext_str[id].friendly);
}

inline jvxPropertyContext jvxPropertyContext_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_PROPERTY_CONTEXT_LIMIT; i++)
	{
		if (strcmp(jvxPropertyContext_str[i].friendly, txt) == 0)
		{
			return (jvxPropertyContext)i;
		}
		if (strcmp(jvxPropertyContext_str[i].full, txt) == 0)
		{
			return (jvxPropertyContext)i;
		}
	}
	return JVX_PROPERTY_CONTEXT_LIMIT;
}
// ==================================================================
// Property decoder hint type
// ==================================================================

static jvxTextHelpers jvxPropertyDecoderHintType_str[JVX_PROPERTY_DECODER_LIMIT] =
{
	{"none", "JVX_PROPERTY_DECODER_NONE" },
	{"file name (open)", "JVX_PROPERTY_DECODER_FILENAME_OPEN" },
	{"directory (select)", "JVX_PROPERTY_DECODER_DIRECTORY_SELECT" },
	{"file name (save)", "JVX_PROPERTY_DECODER_FILENAME_SAVE" },
	{"ip address", "JVX_PROPERTY_DECODER_IP_ADDRESS" },
	{"plot array", "JVX_PROPERTY_DECODER_PLOT_ARRAY" },
	{"bit field", "JVX_PROPERTY_DECODER_BITFIELD" },
	{"single selection", "JVX_PROPERTY_DECODER_SINGLE_SELECTION" },
	{"multi selection", "JVX_PROPERTY_DECODER_MULTI_SELECTION" },
	{"format (index)", "JVX_PROPERTY_DECODER_FORMAT_IDX" },
	
	{"progress bar", "JVX_PROPERTY_DECODER_PROGRESSBAR" },
	{"input file list", "JVX_PROPERTY_DECODER_INPUT_FILE_LIST" },
	{"output file list", "JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST" },
	{"multi selection change order", "JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER" },
	{"multi channel circ buffer", "JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER" },
	{ "multi channel switch buffer", "JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER" },
	{"command", "JVX_PROPERTY_DECODER_COMMAND" },
	{"simple onoff", "JVX_PROPERTY_DECODER_SIMPLE_ONOFF"},
	{"value or dont care", "JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE"},
	{"subformat (index)", "JVX_PROPERTY_DECODER_SUBFORMAT_IDX"},
	{"dataflow (index)", "JVX_PROPERTY_DECODER_DATAFLOW_IDX"},
	
	{"enum type", "JVX_PROPERTY_DECODER_ENUM_TYPE"},
	{"property extender", "JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE"},
	{"local text log", "JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG"}
};

inline const char* jvxPropertyDecoderHintType_txt(jvxSize id)
{
	assert(id < JVX_PROPERTY_DECODER_LIMIT);
	return(jvxPropertyDecoderHintType_str[id].friendly);
}

inline jvxPropertyDecoderHintType jvxPropertyDecoderHintType_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_PROPERTY_DECODER_LIMIT; i++)
	{
		if (strcmp(jvxPropertyDecoderHintType_str[i].friendly, txt) == 0)
		{
			return (jvxPropertyDecoderHintType)i;
		}
		if (strcmp(jvxPropertyDecoderHintType_str[i].full, txt) == 0)
		{
			return (jvxPropertyDecoderHintType)i;
		}
	}
	return JVX_PROPERTY_DECODER_LIMIT;
}
// ==================================================================
// ==================================================================

static jvxTextHelpers  jvxSequencerStatus_str[JVX_SEQUENCER_STATUS_LIMIT] =
{
	{"none","JVX_SEQUENCER_STATUS_NONE" },
	{"startup", "JVX_SEQUENCER_STATUS_STARTUP" },
	{"in operation", "JVX_SEQUENCER_STATUS_IN_OPERATION" },
	{"waiting", "JVX_SEQUENCER_STATUS_WAITING" },
	{"wants stop process", "JVX_SEQUENCER_STATUS_WANTS_TO_STOP_PROCESS" },
	{"wants stop sequence", "JVX_SEQUENCER_STATUS_WANTS_TO_STOP_SEQUENCE"},
	{"shutdown", "JVX_SEQUENCER_STATUS_SHUTDOWN_IN_PROGRESS" },
	{"error", "JVX_SEQUENCER_STATUS_ERROR" },
	{"shutdown complete", "JVX_SEQUENCER_STATUS_SHUTDOWN_COMPLETE"}
};

inline const char* jvxSequencerStatus_txt(jvxSize id)
{
	assert(id < JVX_SEQUENCER_STATUS_LIMIT);
	return(jvxSequencerStatus_str[id].friendly);
}

inline jvxSequencerStatus jvxSequencerStatus_decode(const char* txt)
{
	jvxSize i;
	for (i = 0; i < (int)JVX_SEQUENCER_STATUS_LIMIT; i++)
	{
		if (strcmp(jvxSequencerStatus_str[i].friendly, txt) == 0)
		{
			return (jvxSequencerStatus)i;
		}
		if (strcmp(jvxSequencerStatus_str[i].full, txt) == 0)
		{
			return (jvxSequencerStatus)i;
		}
	}
	return JVX_SEQUENCER_STATUS_LIMIT;
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_LIMIT] =
{
	{"queue_type_none", "JVX_SEQUENCER_QUEUE_TYPE_NONE" },
	{"queue_type_run", "JVX_SEQUENCER_QUEUE_TYPE_RUN" },
	{"queue_type_leave", "JVX_SEQUENCER_QUEUE_TYPE_LEAVE"}
};

inline const char* jvxSequencerQueueType_txt(jvxSize id)
{
	assert(id < JVX_SEQUENCER_QUEUE_TYPE_LIMIT);
	return(jvxSequencerQueueType_str[id].friendly);
}
// ==================================================================
// ==================================================================

static jvxTextHelpers jvxSequencerEventType_str[JVXSEQUENCEREVENT_NUM] =
{
	/* none with no bit set */
	{"process_startup_complete", "JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE" }, // JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE = 0x1,
	{"sequence_startup_complete", "JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE" }, // JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE = 0x2,
	{"step_complete", "JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP" }, // JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP = 0x4,
	{"step_error", "JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR" }, // JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR = 0x8,
	{"sequence_complete", "JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE" }, // JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE = 0x10,
	{"sequence_error", "JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR" }, // JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR = 0x20,
	{"sequence_abort", "JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT" }, // JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT = 0x40,
	{"process_complete", "JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS" }, // JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS = 0x80,
	{"process_error", "JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR" }, // JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR = 0x100,
	{"process_abort", "JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT" }, // JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT = 0x200,
	{"process_terminated", "JVX_SEQUENCER_EVENT_PROCESS_TERMINATED" }, // JVX_SEQUENCER_EVENT_PROCESS_TERMINATED = 0x400,
	{"info_text", "JVX_SEQUENCER_EVENT_INFO_TEXT" }, // JVX_SEQUENCER_EVENT_INFO_TEXT = 0x800,
	{"debug", "JVX_SEQUENCER_EVENT_DEBUG_MESSAGE"}, // JVX_SEQUENCER_EVENT_DEBUG_MESSAGE = 0x1000
	{"state", "JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE"}, // JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE = 0x2000
	{"break", "JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK"} // JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK = 0x4000
};

static jvxCBitField jvxSequencerEventType_map[JVXSEQUENCEREVENT_NUM] =
{
	(jvxCBitField)JVX_SEQUENCER_EVENT_PROCESS_STARTUP_COMPLETE,
	(jvxCBitField)JVX_SEQUENCER_EVENT_SEQUENCE_STARTUP_COMPLETE,
	(jvxCBitField)JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_STEP,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR,
	(jvxCBitField)JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_SEQUENCE,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT,
	(jvxCBitField)JVX_SEQUENCER_EVENT_SUCCESSFUL_COMPLETION_PROCESS,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT,
	(jvxCBitField)JVX_SEQUENCER_EVENT_PROCESS_TERMINATED,
	(jvxCBitField)JVX_SEQUENCER_EVENT_INFO_TEXT,
	(jvxCBitField)JVX_SEQUENCER_EVENT_DEBUG_MESSAGE,
	(jvxCBitField)JVX_SEQUENCER_EVENT_REPORT_OPERATION_STATE,
	(jvxCBitField)JVX_SEQUENCER_EVENT_REPORT_ENTERED_STEP_BREAK
};

inline const char* jvxSequencerEventType_txt(jvxCBitField bits)
{
	jvxSize i;
	for (i = 0; i < JVXSEQUENCEREVENT_NUM; i++)
	{
		if (bits & (jvxCBitField)1 << i)
		{
			return jvxSequencerEventType_str[i].friendly;
		}
	}
	return("error");
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxSequencerElementType_str[JVX_SEQUENCER_TYPE_COMMAND_LIMIT] =
{
	{"none",  "JVX_SEQUENCER_TYPE_COMMAND_NONE" }, // JVX_SEQUENCER_TYPE_COMMAND_NONE
	{"activate",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_ACTIVATE
	{"prepare",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE
	{"start",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START
	{"stop",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP
	{"post process",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS
	{"deactivate",  "JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE" }, // JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_DEACTIVATE
	{"condition wait",  "JVX_SEQUENCER_TYPE_CONDITION_WAIT" }, // JVX_SEQUENCER_TYPE_CONDITION_WAIT
	{"condition jump",  "JVX_SEQUENCER_TYPE_CONDITION_JUMP" }, // JVX_SEQUENCER_TYPE_CONDITION_JUMP
	{"jump",  "JVX_SEQUENCER_TYPE_JUMP" }, // JVX_SEQUENCER_TYPE_JUMP
	
	{"command",  "JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC" }, // JVX_SEQUENCER_TYPE_COMMAND_SPECIFIC
	{"wait until stop",  "JVX_SEQUENCER_TYPE_WAIT_FOREVER" }, // JVX_SEQUENCER_TYPE_WAIT_FOREVER
	{"relative jump",  "JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP" }, // JVX_SEQUENCER_TYPE_WAIT_CONDITION_RELATIVE_JUMP
	{"break",  "JVX_SEQUENCER_TYPE_BREAK" }, // JVX_SEQUENCER_TYPE_BREAK
	{"request viewer update",  "JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER" }, // JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER
	{"start processing loop",  "JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP" }, // JVX_SEQUENCER_TYPE_REQUEST_START_IN_PROCESSING_LOOP
	{"stop processing loop",  "JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP" }, // JVX_SEQUENCER_TYPE_REQUEST_STOP_IN_PROCESSING_LOOP
	{"callback", "JVX_SEQUENCER_TYPE_CALLBACK"}, // JVX_SEQUENCER_TYPE_CALLBACK
	{"proc prepare", "JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE"}, // JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE
	{"proc start", "JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START"}, // JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START
	
	{"proc stop", "JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP"}, // JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP
	{"proc post process", "JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS"}, // JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS
	{"output text", "JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT"}, // JVX_SEQUENCER_TYPE_COMMAND_OUTPUT_TEXT
	{"call seq", "JVX_SEQUENCER_TYPE_COMMAND_CALL_SEQUENCE"}, // 
	{"switch state", "JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE"}, // JVX_SEQUENCER_TYPE_COMMAND_SWITCH_STATE
	{"reset state", "JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE"}, // JVX_SEQUENCER_TYPE_COMMAND_RESET_STATE
	{"proc test", "JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST"}, // JVX_SEQUENCER_TYPE_COMMAND_PROCESS_TEST
	{"intercept", "JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT"}, // JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT
	{"nop", "JVX_SEQUENCER_TYPE_COMMAND_NOP"}, // JVX_SEQUENCER_TYPE_COMMAND_INTERCEPT
	{"set_property", "JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY"}, // JVX_SEQUENCER_TYPE_COMMAND_SET_PROPERTY

	{"error", "JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR"}, // JVX_SEQUENCER_TYPE_COMMAND_INVOKE_ERROR
	{"fata_error", "JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR"}, // JVX_SEQUENCER_TYPE_COMMAND_INVOKE_FATAL_ERROR
	{"uncond wait",  "JVX_SEQUENCER_TYPE_UNCONDITION_WAIT"}, // JVX_SEQUENCER_TYPE_UNCONDITION_WAIT
	{"wait_tasks", "JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS"}
	// 	JVX_SEQUENCER_TYPE_COMMAND_LIMIT
};

inline const char* jvxSequencerElementType_txt(jvxSize id)
{
	assert(id < JVX_SEQUENCER_TYPE_COMMAND_LIMIT);
	return(jvxSequencerElementType_str[id].friendly);
}

// ==================================================================
// ==================================================================
static jvxTextHelpers jvxCommandRequestFlag_str[JVX_REPORT_REQUEST_LIMIT] =
{
	/*
	JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT = 0,
	JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT = 1,
	JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT = 2,
	JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT = 3,
	JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT = 4,
	JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT = 5,
	JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT = 6,
	JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT = 7,
	JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT = 8,
	JVX_REPORT_REQUEST_RECONFIGURE_SHIFT = 9,
	JVX_REPORT_REQUEST_REACTIVATE_SHIFT = 10,
	JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SEQUENCER_SHIFT = 11,
	JVX_REPORT_COMMAND_RETRIGGER_SEQUENCER_SHIFT = 12,
	*/
	{"update window (flag)",  "JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT" },
	{"update prop viewer (flag)",  "JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT" },
	{"update prop viewer full (flag)", "JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT" },
	{"stop sequencer process(flag)", "JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SHIFT" },
	{"start sequencer (flag)", "JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT" },
	{"update data connections (flag)", "JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT" },
	{"user message (flag)", "JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT"},
	{"update properties (flag)", "JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT"},
	{"update test chain master", "JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT"},
	{"reconfigure", "JVX_REPORT_REQUEST_RECONFIGURE_SHIFT"},
	{"reactivate", "JVX_REPORT_REQUEST_REACTIVATE_SHIFT"},
	{"stop sequencer sequence(flag)", "JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SEQUENCE_SHIFT"},
	{"retrigger sequencer", "JVX_REPORT_REQUEST_COMMAND_RETRIGGER_SEQUENCER_SHIFT"}
};

inline const char* jvxCommandRequestFlag_txt(jvxSize id_shift)
{
	assert(id_shift < JVX_REPORT_REQUEST_LIMIT);
	return(jvxCommandRequestFlag_str[id_shift].friendly);
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxUpdateWindowFlags_str[JVX_UPDATE_WINDOW_LIMIT] =
{
	{"window update", "JVX_UPDATE_WINDOW_SHIFT"},
	{"property viewer", "JVX_UPDATE_PROPERTY_VIEWER_SHIFT"},
	{"property viewer full", "JVX_UPDATE_PROPERTY_VIEWER_FULL_SHIFT"},	
	{"system state transition", "JVX_UPDATE_WINDOW_SYSTEM_STATE_TRANSITION_SHIFT"},
	{"system startup update", "JVX_UPDATE_WINDOW_SYSTEM_STARTUP_SHIFT"},
	{"system shutdown update", "JVX_UPDATE_WINDOW_SYSTEM_SHUTDOWN_SHIFT"},
	{"config read update", "JVX_UPDATE_WINDOW_CONFIG_READ_SHIFT"},
	{"prop connected update", "JVX_UPDATE_WINDOW_PROPERTY_CONNECTED_SHIFT"},
	{"prop disconnected update", "JVX_UPDATE_WINDOW_PROPERTY_DISCONNECTED_SHIFT"},
	{"error update", "JVX_UPDATE_WINDOW_ERROR_SHIFT"},
	{"property viewer startup", "JVX_UPDATE_WINDOW_PROPERTY_VIEWER_STARTUP_SHIFT"},
	{"property viewer shutdown", "JVX_UPDATE_WINDOW_PROPERTY_VIEWER_SHUTDOWN_SHIFT"},
	{"sequencer state transition", "JVX_UPDATE_WINDOW_SEQ_STATE_TRANSITION_SHIFT"},
	{"sequencer state report", "JVX_UPDATE_WINDOW_SEQ_STATE_REPORT_SHIFT"},
	{"remote system state transition", "JVX_UPDATE_WINDOW_SYSTEM_STATE_REMOTE_TRANSISTION_SHIFT"}
};

inline const char* jvxUpdateWindowFlags_txt(jvxSize id_shift)
{
	assert(id_shift < JVX_UPDATE_WINDOW_LIMIT);
	return(jvxUpdateWindowFlags_str[id_shift].friendly);
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxFeatureClass_str[JVXFEATURE_CLASS_NUM] =
{
	{"external call", "JVX_FEATURE_CLASS_EXTERNAL_CALL" },
	{"hookup", "JVX_FEATURE_CLASS_HOOKUP_CALL" },
	{"socket server", "JVX_FEATURE_CLASS_COMPONENT_SOCKET_SERVER" },
	{"socket client", "JVX_FEATURE_CLASS_COMPONENT_SOCKET_CLIENT"},
	{"non essential", "JVX_FEATURE_CLASS_NON_ESSENTIAL"}
};

inline const char* jvxFeatureClass_txt_shift(jvxSize id_shift)
{
	assert(id_shift < JVXFEATURE_CLASS_NUM);
	return(jvxFeatureClass_str[id_shift].friendly);
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxPropertySetType_str[JVXPROPERRTY_SETTYPE_NUM] =
{
	{"init", "JVX_THREADING_INIT" },
	{"async", "JVX_THREADING_ASYNC" },
	{"sync", "JVX_THREADING_SYNC" },
	{"dontcare", "JVX_THREADING_DONTCARE"}
};

inline const char* jvxPropertySetType_txt(jvxSize id)
{
	assert(id < JVXPROPERRTY_SETTYPE_NUM);
	return(jvxPropertySetType_str[id].friendly);
}

static jvxCBitField jvxPropertySetType_map[JVXPROPERRTY_SETTYPE_NUM] =
{
	(jvxCBitField)JVX_THREADING_INIT,
	(jvxCBitField)JVX_THREADING_ASYNC,
	(jvxCBitField)JVX_THREADING_SYNC,
	(jvxCBitField)JVX_THREADING_DONTCARE
};

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxReportPriority_str[JVX_REPORT_PRIORITY_LIMIT] =
{
	{"", "JVX_REPORT_PRIORITY_NONE" },
	{"<error>", "JVX_REPORT_PRIORITY_ERROR" },
	{"<warning>", "JVX_REPORT_PRIORITY_WARNING" },
	{"<success>", "JVX_REPORT_PRIORITY_SUCCESS" },
	{"<info>", "JVX_REPORT_PRIORITY_INFO"},
	{"<verbose>", "JVX_REPORT_PRIORITY_VERBOSE"},
	{"<debug>", "JVX_REPORT_PRIORITY_DEBUG"}
};

inline const char* jvxReportPriority_txt(jvxSize id)
{
	assert(id < JVX_REPORT_PRIORITY_LIMIT);
	return(jvxReportPriority_str[id].friendly);
}

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxComponentAccessType_str[JVX_COMPONENT_ACCESS_LIMIT] = 
{
	{ "unknown", "JVX_COMPONENT_ACCESS_UNKNOWN" },
	{"sub-component", "JVX_COMPONENT_ACCESS_SUB_COMPONENT"},
	{"static", "JVX_COMPONENT_ACCESS_STATIC_LIB"},
	{"dynamic", "JVX_COMPONENT_ACCESS_DYNAMIC_LIB"},
	{"import", "JVX_COMPONENT_ACCESS_IMPORT_LIB"},
	{"system", "JVX_COMPONENT_ACCESS_SYSTEM"}
};

inline const char* jvxComponentAccessType_txt(jvxSize id)
{
	assert(id < JVX_COMPONENT_ACCESS_LIMIT);
	return(jvxComponentAccessType_str[id].friendly);
}

// ==================================================================
// ==================================================================

static const char* jvxComponentSubType_str[] = 
{
	"signal_processing_node/audio_node",
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxTypes_componentsubtypes.h"	
#endif
	NULL
};

// ==================================================================
// ==================================================================

static jvxTextHelpers jvxPropertyStreamCondUpdate_str[JVX_PROP_STREAM_UPDATE_LIMIT] = 
{
	{"timeout", "JVX_PROP_STREAM_UPDATE_TIMEOUT"},
	{"on change", "JVX_PROP_STREAM_UPDATE_ON_CHANGE"},
	{"on request", "JVX_PROP_STREAM_UPDATE_ON_REQUEST"}
};

inline const char* jvxPropertyStreamCondUpdate_txt(jvxSize id)
{
	assert(id < JVX_PROP_STREAM_UPDATE_LIMIT);
	return(jvxPropertyStreamCondUpdate_str[id].friendly);
}

// ======================================================================================

static jvxTextHelpers jvxDeviceCapabilities_str[(int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_LIMIT_SHIFT] =
{
	{"Unknown", "JVX_DEVICE_CAPABILITY_UNKNOWN_SHIFT"},
	{"Input", "JVX_DEVICE_CAPABILITY_INPUT_SHIFT"},
	{"Output", "JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT"},
	{"Duplex", "JVX_DEVICE_CAPABILITY_DUPLEX_SHIFT"},
	{"Other", "JVX_DEVICE_CAPABILITY_OTHER_SHIFT"}
};

inline std::string jvxDeviceCapabilitiesType_txt(jvxCBitField val)
{
	jvxSize i;
	jvxCBitField cmp = 0x1;
	std::string retVal = "";
	for (i = 0; i < (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_LIMIT_SHIFT; i++)
	{
		if (cmp & val)
		{
			if (!retVal.empty())
			{
				retVal += "|";
			}
			retVal += jvxDeviceCapabilities_str[i].friendly;
		}
		cmp <<= 1;
	}
	return retVal;
};

// ===============================================================================

static jvxTextHelpers jvxDeviceDataFlowType_str[(int)jvxDeviceDataFlowType::JVX_DEVICE_DATAFLOW_LIMIT] =
{
	{"unknown", "JVX_DEVICE_DATAFLOW_UNKNOWN"},
	{"active", "JVX_DEVICE_DATAFLOW_ACTIVE"},
	{"passive", "JVX_DEVICE_DATAFLOW_PASSIVE"},
	{"offline", "JVX_DEVICE_DATAFLOW_OFFLINE"},
	{"control", "JVX_DEVICE_DATAFLOW_CONTROL"}
};

inline const char* jvxDeviceDataFlowType_txt(jvxDeviceDataFlowType flow)
{
	assert((int)flow < (int)jvxDeviceDataFlowType::JVX_DEVICE_DATAFLOW_LIMIT);
	return(jvxDeviceDataFlowType_str[(int)flow].friendly);
};

// ===============================================================================

static jvxTextHelpers jvxReportCommandRequest_str[(int)jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_LIMIT] =
{
	{"cmdreq-unspecific", "JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC"},
	{"cmdreq-upd-cp-lst", "JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST"},
	{"cmdreq-upd-stat-cp-lst", "JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST"},
	{"cmdreq-upd-stat-cp", "JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT"},
	{"cmdreq-sys-stat", "JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED"},
	{"cmdreq-trig-seq-imm", "JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE"},
	{"cmdreq-upd-props", "JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES"},
	{"cmdreq-resched-main", "JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN"},
	{"cmdreq-born-sub-dev", "JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE"},
	{"cmdreq-died-sub-dev", "JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE"},
	{"cmdreq-born-comp", "JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT"},
	{"cmdreq-died-comp", "JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT"},
	{"cmdreq-proc-connect", "JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED"},
	{"cmdreq-proc-discon", "JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED"},
	{"cmdreq-req-test-chain", "JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN"},
	{"cmdreq-rep-cp-sswitch", "JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH"},
	{"cmdreq-config-complete", "JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE"},
	{"cmdreq-rep-seq-evt", "JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT"},
	{"cmdreq-rep-seq_cb", "JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK"},
	{"cmdreq-proc-dis-cmplt", "JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE"},
	{"cmdreq-req-upd-prop", "JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY"},
	{"cmdreq-proc-test", "JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS"},
	{"cmdreq-req-test-chain-run", "JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN"}
};
	
inline const char* jvxReportCommandRequest_txt(jvxReportCommandRequest coReq)
{
	assert((int)coReq < (int)jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_LIMIT);
	return(jvxReportCommandRequest_str[(int)coReq].friendly);
};

// ===============================================================================

static jvxTextHelpers jvxReportCommandDataType_str[(int)jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_LIMIT] =
{
	{"cmdreqtp-base", "JVX_REPORT_COMMAND_TYPE_BASE"},
	{"cmdreqtp-sched", "JVX_REPORT_COMMAND_TYPE_SCHEDULE"},
	{"cmdreqtp-ident", "JVX_REPORT_COMMAND_TYPE_IDENT"},
	{"cmdreqtp-uid", "JVX_REPORT_COMMAND_TYPE_UID"},
	{"cmdreqtp-ss", "JVX_REPORT_COMMAND_TYPE_SS"},
	{"cmdreqtp-seq", "JVX_REPORT_COMMAND_TYPE_SEQ"}
};

inline const char* jvxReportCommandDataType_txt(jvxReportCommandDataType coTp)
{
	assert((int)coTp < (int)jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_LIMIT);
	return(jvxReportCommandDataType_str[(int)coTp].friendly);
};

// ==============================================================================
static jvxTextHelpers jvxSocketsConnectionType_str[(int)jvxSocketsConnectionType::JVX_SOCKET_TYPE_LIMIT] =
{
	{"none", "JVX_SOCKET_TYPE_NONE"},
	{"tcp", "JVX_SOCKET_TYPE_TCP"},
	{"udp", "JVX_SOCKET_TYPE_UDP"},
	{"unix", "JVX_SOCKET_TYPE_UNIX"},
	{"pcap", "JVX_SOCKET_TYPE_PCAP"},
	{"bth", "JVX_SOCKET_TYPE_BTH"}
};

inline const char* jvxSocketsConnectionType_txt(jvxSocketsConnectionType soTp)
{
	assert((int)soTp < (int)jvxSocketsConnectionType::JVX_SOCKET_TYPE_LIMIT);
	return(jvxSocketsConnectionType_str[(int)soTp].friendly);
};

inline jvxSocketsConnectionType jvxSocketsConnectionType_decode(const char* txt)
{
	jvxSize i;
	jvxSocketsConnectionType retVal = jvxSocketsConnectionType::JVX_SOCKET_TYPE_NONE;
	for (i = 0; i < (int)jvxSocketsConnectionType::JVX_SOCKET_TYPE_LIMIT; i++)
	{
		if (
			(strcmp(jvxSocketsConnectionType_str[i].friendly, txt) == 0) ||
			(strcmp(jvxSocketsConnectionType_str[i].full, txt) == 0))
		{
			return (jvxSocketsConnectionType)i;
		}
	}
	return retVal;
}

// ==============================================================================
static jvxTextHelpers jvxRemoteConnectionQuality_str[(int)jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_LIMIT] =
{
	{"none", "JVX_REMOTE_CONNECTION_QUALITY_NONE"},
	{"good", "JVX_REMOTE_CONNECTION_QUALITY_GOOD"},
	{"weak", "JVX_REMOTE_CONNECTION_QUALITY_WEAK"},
	{"bad", "JVX_REMOTE_CONNECTION_QUALITY_BAD"},
	{"nodata", "JVX_REMOTE_CONNECTION_QUALITY_NO_DATA"},
	{"noconnect", "JVX_REMOTE_CONNECTION_QUALITY_NOT_CONNECTED"}
};

inline const char* jvxRemoteConnectionQuality_txt(jvxRemoteConnectionQuality soTp)
{
	assert((int)soTp < (int)jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_LIMIT);
	return(jvxRemoteConnectionQuality_str[(int)soTp].friendly);
};

inline jvxRemoteConnectionQuality jvxRemoteConnectionQuality_decode(const char* txt)
{
	jvxSize i;
	jvxRemoteConnectionQuality retVal = jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_NONE;
	for (i = 0; i < (int)jvxRemoteConnectionQuality::JVX_REMOTE_CONNECTION_QUALITY_LIMIT; i++)
	{
		if (
			(strcmp(jvxRemoteConnectionQuality_str[i].friendly, txt) == 0) ||
			(strcmp(jvxRemoteConnectionQuality_str[i].full, txt) == 0))
		{
			return (jvxRemoteConnectionQuality)i;
		}
	}
	return retVal;
}

// ====================================================================		

inline jvxTextHelpers jvxDataTypeSpec_str[JVX_DATA_TYPE_SPEC_LIMIT] =
{
	{"dbl", "JVX_DATA_TYPE_SPEC_DOUBLE"},
	{"flt", "JVX_DATA_TYPE_SPEC_FLOAT"}
};

inline const char* jvxDataTypeSpec_txt(jvxSize id)
{
	assert(id < JVX_DATA_TYPE_SPEC_LIMIT);
	return jvxDataTypeSpec_str[id].friendly;
}

#endif
