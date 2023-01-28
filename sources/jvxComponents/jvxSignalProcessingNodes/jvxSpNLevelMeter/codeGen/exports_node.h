#ifndef _GENSPEAKEREQUALIZER_NODE_H__
#define _GENSPEAKEREQUALIZER_NODE_H__

#include "jvx-helpers.h"
class genSpeakerEqualizer_node
{
public:
	genSpeakerEqualizer_node()
	{
		init_all();
	};

	//=================================================
	// Member Variables
	//=================================================



	// Section config::config
	struct 
	{
		// DESCRIPTION: Select Measurement
		jvxPropertyContainerSingle<jvx_oneSelectionOption_cpp> select_channel;
		// DESCRIPTION: bypass
		jvxPropertyContainerSingle<jvxCBool> bypass;
		// DESCRIPTION: eq_definition_file
		jvxPropertyContainerSingle<std::string> eq_definition_file;
		// DESCRIPTION: clear_definition_file
		jvxPropertyContainerSingle<jvxCBool> clear_definition_file;
	} config;

	// Section mode::mode
	struct 
	{
		// DESCRIPTION: slave_mode
		jvxPropertyContainerSingle<jvxCBool> slave_mode;
		// DESCRIPTION: involved
		jvxPropertyContainerSingle<jvxCBool> involved;
		// DESCRIPTION: function
		jvxPropertyContainerSingle<jvx_oneSelectionOption_cpp> function;
	} mode;

	// Section parameter::parameter
	struct 
	{
		// DESCRIPTION: delay
		jvxPropertyContainerSingle<jvxSize> delay;
		// DESCRIPTION: gain
		jvxPropertyContainerSingle<jvxData> gain;
		// DESCRIPTION: fir_eq
		jvxPropertyContainerSingle<std::string> fir_eq;
	} parameter;

	// Section monitor::monitor
	struct 
	{
		// DESCRIPTION: last_error
		jvxPropertyContainerSingle<std::string> last_error;
	} monitor;

	//=================================================
	// Init functions
	//=================================================

	// Comment: config
	inline void init__config()
	{
		// Initialize the struct to neutral defined values.
		config.select_channel.description = "Select Measurement";
		config.select_channel.descriptor = "select_channel";
		jvx_bitFClear(config.select_channel.value.selection);
		jvx_bitFClear(config.select_channel.value.selectable);
		jvx_bitFClear(config.select_channel.value.exclusive);
		config.select_channel.value.entries.clear();
		config.select_channel.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.select_channel.config_mode_flags = 3;
		config.bypass.description = "bypass";
		config.bypass.descriptor = "bypass";
		config.bypass.value = 0;
		config.bypass.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.bypass.config_mode_flags = 3;
		config.eq_definition_file.description = "eq_definition_file";
		config.eq_definition_file.descriptor = "eq_definition_file";
		config.eq_definition_file.value = "";
		config.eq_definition_file.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.eq_definition_file.config_mode_flags = 3;
		config.clear_definition_file.description = "clear_definition_file";
		config.clear_definition_file.descriptor = "clear_definition_file";
		config.clear_definition_file.value = 0;
		config.clear_definition_file.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.clear_definition_file.config_mode_flags = 3;
	};

	// Comment: mode
	inline void init__mode()
	{
		// Initialize the struct to neutral defined values.
		mode.slave_mode.description = "slave_mode";
		mode.slave_mode.descriptor = "slave_mode";
		mode.slave_mode.value = 0;
		mode.slave_mode.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.slave_mode.config_mode_flags = 3;
		mode.involved.description = "involved";
		mode.involved.descriptor = "involved";
		mode.involved.value = 0;
		mode.involved.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.involved.config_mode_flags = 3;
		mode.function.description = "function";
		mode.function.descriptor = "function";
		jvx_bitFClear(mode.function.value.selection);
		jvx_bitFClear(mode.function.value.selectable);
		jvx_bitFClear(mode.function.value.exclusive);
		mode.function.value.entries.clear();
		mode.function.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.function.config_mode_flags = 3;
	};

	// Comment: parameter
	inline void init__parameter()
	{
		// Initialize the struct to neutral defined values.
		parameter.delay.description = "delay";
		parameter.delay.descriptor = "delay";
		parameter.delay.value = 0;
		parameter.delay.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.delay.config_mode_flags = 3;
		parameter.gain.description = "gain";
		parameter.gain.descriptor = "gain";
		parameter.gain.value = 0;
		parameter.gain.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.gain.config_mode_flags = 3;
		parameter.fir_eq.description = "fir_eq";
		parameter.fir_eq.descriptor = "fir_eq";
		parameter.fir_eq.value = "";
		parameter.fir_eq.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.fir_eq.config_mode_flags = 3;
	};

	// Comment: monitor
	inline void init__monitor()
	{
		// Initialize the struct to neutral defined values.
		monitor.last_error.description = "last_error";
		monitor.last_error.descriptor = "last_error";
		monitor.last_error.value = "";
		monitor.last_error.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		monitor.last_error.config_mode_flags = 3;
	};

	//=================================================
	// Allocation functions
	//=================================================

	// Comment: config
	inline void allocate__config()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: Select Measurement
		config.select_channel.value.selection = 0;
		config.select_channel.value.exclusive = 0;
		config.select_channel.value.entries.push_back("REPLACEME");
		config.select_channel.value.selection |= (1 << 0);
		config.select_channel.value.exclusive |= (1 << 0);
		config.select_channel.value.selectable |= (1 << 0);
		config.select_channel.valid = true;
		config.select_channel.frozen = false;
		config.select_channel.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.select_channel.config_mode_flags = 3;
		config.select_channel.onlySelectionToFile = false;
	// DESCRIPTION: bypass
		config.bypass.value = 0;
		config.bypass.valid = true;
		config.bypass.frozen = false;
		config.bypass.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.bypass.config_mode_flags = 3;
		config.bypass.onlySelectionToFile = false;
	// DESCRIPTION: eq_definition_file
		config.eq_definition_file.valid = true;
		config.eq_definition_file.frozen = false;
		config.eq_definition_file.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.eq_definition_file.config_mode_flags = 3;
		config.eq_definition_file.onlySelectionToFile = false;
	// DESCRIPTION: clear_definition_file
		config.clear_definition_file.value = 0;
		config.clear_definition_file.valid = true;
		config.clear_definition_file.frozen = false;
		config.clear_definition_file.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		config.clear_definition_file.config_mode_flags = 3;
		config.clear_definition_file.onlySelectionToFile = false;
	};

	// Comment: mode
	inline void allocate__mode()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: slave_mode
		mode.slave_mode.value = 0;
		mode.slave_mode.valid = true;
		mode.slave_mode.frozen = false;
		mode.slave_mode.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.slave_mode.config_mode_flags = 3;
		mode.slave_mode.onlySelectionToFile = false;
	// DESCRIPTION: involved
		mode.involved.value = 1;
		mode.involved.valid = true;
		mode.involved.frozen = false;
		mode.involved.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.involved.config_mode_flags = 3;
		mode.involved.onlySelectionToFile = false;
	// DESCRIPTION: function
		mode.function.value.selection = 0;
		mode.function.value.exclusive = 0;
		mode.function.value.entries.push_back("DELAY");
		mode.function.value.selection |= (1 << 0);
		mode.function.value.selectable |= (1 << 0);
		mode.function.value.entries.push_back("GAIN");
		mode.function.value.selection |= (1 << 1);
		mode.function.value.selectable |= (1 << 1);
		mode.function.value.entries.push_back("FIREQ");
		mode.function.value.selection |= (1 << 2);
		mode.function.value.selectable |= (1 << 2);
		mode.function.valid = true;
		mode.function.frozen = false;
		mode.function.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		mode.function.config_mode_flags = 3;
		mode.function.onlySelectionToFile = false;
	};

	// Comment: parameter
	inline void allocate__parameter()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: delay
		parameter.delay.value = 0;
		parameter.delay.valid = true;
		parameter.delay.frozen = false;
		parameter.delay.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.delay.config_mode_flags = 3;
		parameter.delay.onlySelectionToFile = false;
	// DESCRIPTION: gain
		parameter.gain.value = 0;
		parameter.gain.valid = true;
		parameter.gain.frozen = false;
		parameter.gain.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.gain.config_mode_flags = 3;
		parameter.gain.onlySelectionToFile = false;
	// DESCRIPTION: fir_eq
		parameter.fir_eq.value = "[]";
		parameter.fir_eq.valid = true;
		parameter.fir_eq.frozen = false;
		parameter.fir_eq.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		parameter.fir_eq.config_mode_flags = 3;
		parameter.fir_eq.onlySelectionToFile = false;
	};

	// Comment: monitor
	inline void allocate__monitor()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: last_error
		monitor.last_error.valid = true;
		monitor.last_error.frozen = false;
		monitor.last_error.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		monitor.last_error.config_mode_flags = 3;
		monitor.last_error.onlySelectionToFile = false;
	};

	//=================================================
	// Deallocation functions
	//=================================================

	// Comment: config
	inline void deallocate__config()
	{
		// If necessary, deallocate.
	// DESCRIPTION: Select Measurement
		config.select_channel.value.selection = 0;
		config.select_channel.value.exclusive = 0;
		config.select_channel.value.entries.clear();
		config.select_channel.reset();
	// DESCRIPTION: bypass
		config.bypass.reset();
	// DESCRIPTION: eq_definition_file
		config.eq_definition_file.value = "";
		config.eq_definition_file.reset();
	// DESCRIPTION: clear_definition_file
		config.clear_definition_file.reset();
	};

	// Comment: mode
	inline void deallocate__mode()
	{
		// If necessary, deallocate.
	// DESCRIPTION: slave_mode
		mode.slave_mode.reset();
	// DESCRIPTION: involved
		mode.involved.reset();
	// DESCRIPTION: function
		mode.function.value.selection = 0;
		mode.function.value.exclusive = 0;
		mode.function.value.entries.clear();
		mode.function.reset();
	};

	// Comment: parameter
	inline void deallocate__parameter()
	{
		// If necessary, deallocate.
	// DESCRIPTION: delay
		parameter.delay.reset();
	// DESCRIPTION: gain
		parameter.gain.reset();
	// DESCRIPTION: fir_eq
		parameter.fir_eq.value = "[]";
		parameter.fir_eq.reset();
	};

	// Comment: monitor
	inline void deallocate__monitor()
	{
		// If necessary, deallocate.
	// DESCRIPTION: last_error
		monitor.last_error.value = "";
		monitor.last_error.reset();
	};

	//=================================================
	// Associate functions 
	//=================================================

	//=================================================

	// Deassociate functions 
	//=================================================

	//=================================================
	// Register functions 
	//=================================================

	// Comment: config
	inline void register__config(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		// Register properties with property management system.
		jvxErrorType resL = JVX_NO_ERROR;
		theProps->_lock_properties_local();
		config.select_channel.descriptor = jvx_makePathExpr(reg_prefix, config.select_channel.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << config.select_channel.descriptor << ">." << std::endl;
		config.select_channel.id = JVX_SIZE_UNSELECTED;
		config.select_channel.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		config.select_channel.format = JVX_DATAFORMAT_SELECTION_LIST;
		resL = theProps->_register_property(&config.select_channel.value, 1, JVX_DATAFORMAT_SELECTION_LIST, &config.select_channel.id, config.select_channel.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "select_channel", config.select_channel.description.c_str(), config.select_channel.descriptor.c_str(), &config.select_channel.valid, &config.select_channel.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SINGLE_SELECTION, JVX_PROPERTY_CONTEXT_UNKNOWN, &config.select_channel.access_flags, &config.select_channel.config_mode_flags, "/set_channel_posthook", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << config.select_channel.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:12";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, config.select_channel.id, config.select_channel.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		config.bypass.descriptor = jvx_makePathExpr(reg_prefix, config.bypass.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << config.bypass.descriptor << ">." << std::endl;
		config.bypass.id = JVX_SIZE_UNSELECTED;
		config.bypass.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		config.bypass.format = JVX_DATAFORMAT_16BIT_LE;
		resL = theProps->_register_property(&config.bypass.value, 1, JVX_DATAFORMAT_16BIT_LE, &config.bypass.id, config.bypass.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "bypass", config.bypass.description.c_str(), config.bypass.descriptor.c_str(), &config.bypass.valid, &config.bypass.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_PROPERTY_CONTEXT_UNKNOWN, &config.bypass.access_flags, &config.bypass.config_mode_flags, "", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << config.bypass.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:24";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, config.bypass.id, config.bypass.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		config.eq_definition_file.descriptor = jvx_makePathExpr(reg_prefix, config.eq_definition_file.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << config.eq_definition_file.descriptor << ">." << std::endl;
		config.eq_definition_file.id = JVX_SIZE_UNSELECTED;
		config.eq_definition_file.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		config.eq_definition_file.format = JVX_DATAFORMAT_STRING;
		resL = theProps->_register_property(&config.eq_definition_file.value, 1, JVX_DATAFORMAT_STRING, &config.eq_definition_file.id, config.eq_definition_file.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "eq_definition_file", config.eq_definition_file.description.c_str(), config.eq_definition_file.descriptor.c_str(), &config.eq_definition_file.valid, &config.eq_definition_file.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_FILENAME_OPEN, JVX_PROPERTY_CONTEXT_UNKNOWN, &config.eq_definition_file.access_flags, &config.eq_definition_file.config_mode_flags, "/select_eq_file_posthook", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << config.eq_definition_file.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:31";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, config.eq_definition_file.id, config.eq_definition_file.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		config.clear_definition_file.descriptor = jvx_makePathExpr(reg_prefix, config.clear_definition_file.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << config.clear_definition_file.descriptor << ">." << std::endl;
		config.clear_definition_file.id = JVX_SIZE_UNSELECTED;
		config.clear_definition_file.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		config.clear_definition_file.format = JVX_DATAFORMAT_16BIT_LE;
		resL = theProps->_register_property(&config.clear_definition_file.value, 1, JVX_DATAFORMAT_16BIT_LE, &config.clear_definition_file.id, config.clear_definition_file.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "clear_definition_file", config.clear_definition_file.description.c_str(), config.clear_definition_file.descriptor.c_str(), &config.clear_definition_file.valid, &config.clear_definition_file.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_PROPERTY_CONTEXT_UNKNOWN, &config.clear_definition_file.access_flags, &config.clear_definition_file.config_mode_flags, "/select_eq_file_posthook", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << config.clear_definition_file.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:38";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, config.clear_definition_file.id, config.clear_definition_file.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		theProps->_unlock_properties_local();

	};

	// Comment: mode
	inline void register__mode(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		// Register properties with property management system.
		jvxErrorType resL = JVX_NO_ERROR;
		theProps->_lock_properties_local();
		mode.slave_mode.descriptor = jvx_makePathExpr(reg_prefix, mode.slave_mode.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << mode.slave_mode.descriptor << ">." << std::endl;
		mode.slave_mode.id = JVX_SIZE_UNSELECTED;
		mode.slave_mode.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		mode.slave_mode.format = JVX_DATAFORMAT_16BIT_LE;
		resL = theProps->_register_property(&mode.slave_mode.value, 1, JVX_DATAFORMAT_16BIT_LE, &mode.slave_mode.id, mode.slave_mode.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "slave_mode", mode.slave_mode.description.c_str(), mode.slave_mode.descriptor.c_str(), &mode.slave_mode.valid, &mode.slave_mode.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_PROPERTY_CONTEXT_UNKNOWN, &mode.slave_mode.access_flags, &mode.slave_mode.config_mode_flags, "", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << mode.slave_mode.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:53";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, mode.slave_mode.id, mode.slave_mode.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		mode.involved.descriptor = jvx_makePathExpr(reg_prefix, mode.involved.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << mode.involved.descriptor << ">." << std::endl;
		mode.involved.id = JVX_SIZE_UNSELECTED;
		mode.involved.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		mode.involved.format = JVX_DATAFORMAT_16BIT_LE;
		resL = theProps->_register_property(&mode.involved.value, 1, JVX_DATAFORMAT_16BIT_LE, &mode.involved.id, mode.involved.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "involved", mode.involved.description.c_str(), mode.involved.descriptor.c_str(), &mode.involved.valid, &mode.involved.frozen, JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_PROPERTY_CONTEXT_UNKNOWN, &mode.involved.access_flags, &mode.involved.config_mode_flags, "", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << mode.involved.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:61";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, mode.involved.id, mode.involved.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		mode.function.descriptor = jvx_makePathExpr(reg_prefix, mode.function.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << mode.function.descriptor << ">." << std::endl;
		mode.function.id = JVX_SIZE_UNSELECTED;
		mode.function.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		mode.function.format = JVX_DATAFORMAT_SELECTION_LIST;
		resL = theProps->_register_property(&mode.function.value, 1, JVX_DATAFORMAT_SELECTION_LIST, &mode.function.id, mode.function.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "function", mode.function.description.c_str(), mode.function.descriptor.c_str(), &mode.function.valid, &mode.function.frozen, JVX_PROPERTY_ACCESS_READ_ONLY, JVX_PROPERTY_DECODER_MULTI_SELECTION, JVX_PROPERTY_CONTEXT_UNKNOWN, &mode.function.access_flags, &mode.function.config_mode_flags, "", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << mode.function.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:68";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, mode.function.id, mode.function.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		theProps->_unlock_properties_local();

	};

	// Comment: parameter
	inline void register__parameter(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		// Register properties with property management system.
		jvxErrorType resL = JVX_NO_ERROR;
		theProps->_lock_properties_local();
		parameter.delay.descriptor = jvx_makePathExpr(reg_prefix, parameter.delay.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << parameter.delay.descriptor << ">." << std::endl;
		parameter.delay.id = JVX_SIZE_UNSELECTED;
		parameter.delay.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		parameter.delay.format = JVX_DATAFORMAT_SIZE;
		resL = theProps->_register_property(&parameter.delay.value, 1, JVX_DATAFORMAT_SIZE, &parameter.delay.id, parameter.delay.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "delay", parameter.delay.description.c_str(), parameter.delay.descriptor.c_str(), &parameter.delay.valid, &parameter.delay.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &parameter.delay.access_flags, &parameter.delay.config_mode_flags, "/set_parameter_channel_posthook", "/get_parameter_channel_prehook", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << parameter.delay.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:88";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, parameter.delay.id, parameter.delay.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		parameter.gain.descriptor = jvx_makePathExpr(reg_prefix, parameter.gain.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << parameter.gain.descriptor << ">." << std::endl;
		parameter.gain.id = JVX_SIZE_UNSELECTED;
		parameter.gain.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		parameter.gain.format = JVX_DATAFORMAT_DATA;
		resL = theProps->_register_property(&parameter.gain.value, 1, JVX_DATAFORMAT_DATA, &parameter.gain.id, parameter.gain.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "gain", parameter.gain.description.c_str(), parameter.gain.descriptor.c_str(), &parameter.gain.valid, &parameter.gain.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &parameter.gain.access_flags, &parameter.gain.config_mode_flags, "/set_parameter_channel_posthook", "/get_parameter_channel_prehook", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << parameter.gain.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
		{
			std::string str_showhint = "MINVAL=-30,MAXVAL=30,LABELTRANSFORM=JVX_WW_SHOW_DB20";
			jvxApiString jstr_showhint;
			jstr_showhint.assign_const(str_showhint.c_str(), str_showhint.size());
			theProps->_set_property_extended_info(&jstr_showhint, JVX_PROPERTY_INFO_SHOWHINT, parameter.gain.id, parameter.gain.cat);
		}
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:93";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, parameter.gain.id, parameter.gain.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		parameter.fir_eq.descriptor = jvx_makePathExpr(reg_prefix, parameter.fir_eq.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << parameter.fir_eq.descriptor << ">." << std::endl;
		parameter.fir_eq.id = JVX_SIZE_UNSELECTED;
		parameter.fir_eq.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		parameter.fir_eq.format = JVX_DATAFORMAT_STRING;
		resL = theProps->_register_property(&parameter.fir_eq.value, 1, JVX_DATAFORMAT_STRING, &parameter.fir_eq.id, parameter.fir_eq.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "fir_eq", parameter.fir_eq.description.c_str(), parameter.fir_eq.descriptor.c_str(), &parameter.fir_eq.valid, &parameter.fir_eq.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &parameter.fir_eq.access_flags, &parameter.fir_eq.config_mode_flags, "/set_parameter_channel_posthook", "/get_parameter_channel_prehook", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << parameter.fir_eq.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:99";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, parameter.fir_eq.id, parameter.fir_eq.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		theProps->_unlock_properties_local();

	};

	// Comment: monitor
	inline void register__monitor(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		// Register properties with property management system.
		jvxErrorType resL = JVX_NO_ERROR;
		theProps->_lock_properties_local();
		monitor.last_error.descriptor = jvx_makePathExpr(reg_prefix, monitor.last_error.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << monitor.last_error.descriptor << ">." << std::endl;
		monitor.last_error.id = JVX_SIZE_UNSELECTED;
		monitor.last_error.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		monitor.last_error.format = JVX_DATAFORMAT_STRING;
		resL = theProps->_register_property(&monitor.last_error.value, 1, JVX_DATAFORMAT_STRING, &monitor.last_error.id, monitor.last_error.cat, 0x1C, 0xFFFFFFFFFFFFFFFF, "last_error", monitor.last_error.description.c_str(), monitor.last_error.descriptor.c_str(), &monitor.last_error.valid, &monitor.last_error.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &monitor.last_error.access_flags, &monitor.last_error.config_mode_flags, "", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << monitor.last_error.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/codeGen/exports_node.pcg:110";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, monitor.last_error.id, monitor.last_error.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		theProps->_unlock_properties_local();

	};

	//=================================================
	// Unregister functions 
	//=================================================

	// Comment: config
	inline void unregister__config(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(config.select_channel.cat , config.select_channel.id);
		config.select_channel.format = JVX_DATAFORMAT_NONE;
		config.select_channel.id = JVX_SIZE_UNSELECTED;
		config.select_channel.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(config.bypass.cat , config.bypass.id);
		config.bypass.format = JVX_DATAFORMAT_NONE;
		config.bypass.id = JVX_SIZE_UNSELECTED;
		config.bypass.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(config.eq_definition_file.cat , config.eq_definition_file.id);
		config.eq_definition_file.format = JVX_DATAFORMAT_NONE;
		config.eq_definition_file.id = JVX_SIZE_UNSELECTED;
		config.eq_definition_file.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(config.clear_definition_file.cat , config.clear_definition_file.id);
		config.clear_definition_file.format = JVX_DATAFORMAT_NONE;
		config.clear_definition_file.id = JVX_SIZE_UNSELECTED;
		config.clear_definition_file.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	// Comment: mode
	inline void unregister__mode(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(mode.slave_mode.cat , mode.slave_mode.id);
		mode.slave_mode.format = JVX_DATAFORMAT_NONE;
		mode.slave_mode.id = JVX_SIZE_UNSELECTED;
		mode.slave_mode.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(mode.involved.cat , mode.involved.id);
		mode.involved.format = JVX_DATAFORMAT_NONE;
		mode.involved.id = JVX_SIZE_UNSELECTED;
		mode.involved.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(mode.function.cat , mode.function.id);
		mode.function.format = JVX_DATAFORMAT_NONE;
		mode.function.id = JVX_SIZE_UNSELECTED;
		mode.function.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	// Comment: parameter
	inline void unregister__parameter(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(parameter.delay.cat , parameter.delay.id);
		parameter.delay.format = JVX_DATAFORMAT_NONE;
		parameter.delay.id = JVX_SIZE_UNSELECTED;
		parameter.delay.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(parameter.gain.cat , parameter.gain.id);
		parameter.gain.format = JVX_DATAFORMAT_NONE;
		parameter.gain.id = JVX_SIZE_UNSELECTED;
		parameter.gain.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(parameter.fir_eq.cat , parameter.fir_eq.id);
		parameter.fir_eq.format = JVX_DATAFORMAT_NONE;
		parameter.fir_eq.id = JVX_SIZE_UNSELECTED;
		parameter.fir_eq.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	// Comment: monitor
	inline void unregister__monitor(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(monitor.last_error.cat , monitor.last_error.id);
		monitor.last_error.format = JVX_DATAFORMAT_NONE;
		monitor.last_error.id = JVX_SIZE_UNSELECTED;
		monitor.last_error.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	//=================================================
	// Config file read functions 
	//=================================================

	// Comment: config
	inline void put_configuration__config(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Put configuration to initialize the properties.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, config.eq_definition_file.config_mode_flags,callConf->access_flags, config.eq_definition_file.access_flags);
		token = config.eq_definition_file.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignmentString(theReader, ir, token.c_str(), &config.eq_definition_file.value, &config.eq_definition_file.access_flags, &config.eq_definition_file.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + config.eq_definition_file.descriptor + "> from config file.");
		}
	};

	// Comment: mode
	inline void put_configuration__mode(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Put configuration to initialize the properties.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, mode.slave_mode.config_mode_flags,callConf->access_flags, mode.slave_mode.access_flags);
		token = mode.slave_mode.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, ir, token.c_str(), (jvxInt16*)&mode.slave_mode.value, &mode.slave_mode.access_flags, &mode.slave_mode.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + mode.slave_mode.descriptor + "> from config file.");
		}
	};

	// Comment: parameter
	inline void put_configuration__parameter(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Put configuration to initialize the properties.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
	};

	// Comment: monitor
	inline void put_configuration__monitor(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Put configuration to initialize the properties.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
	};

	//=================================================
	// Config file write functions 
	//=================================================

	// Comment: config
	inline void get_configuration__config(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Get configuration to store the property values.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, config.eq_definition_file.config_mode_flags, callConf->access_flags, config.eq_definition_file.access_flags);
		token = config.eq_definition_file.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignmentString(theReader, ir, token.c_str(), &config.eq_definition_file.value, &config.eq_definition_file.access_flags, &config.eq_definition_file.config_mode_flags, whattodo);
	};

	// Comment: mode
	inline void get_configuration__mode(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Get configuration to store the property values.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, mode.slave_mode.config_mode_flags, callConf->access_flags, mode.slave_mode.access_flags);
		token = mode.slave_mode.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, ir, token.c_str(), (jvxInt16*)&mode.slave_mode.value, &mode.slave_mode.access_flags, &mode.slave_mode.config_mode_flags, whattodo);
	};

	// Comment: parameter
	inline void get_configuration__parameter(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Get configuration to store the property values.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
	};

	// Comment: monitor
	inline void get_configuration__monitor(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Get configuration to store the property values.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
	};

	//=================================================
	// Translations Variables
	//=================================================

	//=================================================
	// Register callbacks
	//=================================================

	inline void register_callbacks(CjvxProperties* theProps, property_callback cb_set_channel_posthook, property_callback cb_select_eq_file_posthook, property_callback cb_set_parameter_channel_posthook, property_callback cb_get_parameter_channel_prehook, jvxHandle* privData, std::vector<std::string>* errMessages)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = theProps->_register_callback("/set_channel_posthook", cb_set_channel_posthook, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /set_channel_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_register_callback("/select_eq_file_posthook", cb_select_eq_file_posthook, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /select_eq_file_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_register_callback("/set_parameter_channel_posthook", cb_set_parameter_channel_posthook, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /set_parameter_channel_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_register_callback("/get_parameter_channel_prehook", cb_get_parameter_channel_prehook, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /get_parameter_channel_prehook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
	};
	//=================================================
	// Unregister callbacks
	//=================================================

	inline void unregister_callbacks(CjvxProperties* theProps, std::vector<std::string>* errMessages)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = theProps->_unregister_callback("/set_channel_posthook");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /set_channel_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_unregister_callback("/select_eq_file_posthook");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /select_eq_file_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_unregister_callback("/set_parameter_channel_posthook");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /set_parameter_channel_posthook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_unregister_callback("/get_parameter_channel_prehook");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /get_parameter_channel_prehook, reason: " + std::string(jvxErrorType_descr(res)) + ".");
	};
	// All functions called at once.
	inline void init_all()
	{
		init__config();
		init__mode();
		init__parameter();
		init__monitor();
	};
	// All functions called at once.
	inline void allocate_all()
	{
		allocate__config();
		allocate__mode();
		allocate__parameter();
		allocate__monitor();
	};
	// All functions called at once.
	inline void deallocate_all()
	{
		deallocate__config();
		deallocate__mode();
		deallocate__parameter();
		deallocate__monitor();
	};
	// All functions called at once.
	inline void register_all(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		register__config(theProps, reg_prefix,v_register);
		register__mode(theProps, reg_prefix,v_register);
		register__parameter(theProps, reg_prefix,v_register);
		register__monitor(theProps, reg_prefix,v_register);
	};
	// All functions called at once.
	inline void unregister_all(CjvxProperties* theProps)
	{
		unregister__config(theProps);
		unregister__mode(theProps);
		unregister__parameter(theProps);
		unregister__monitor(theProps);
	};
	// All functions called at once.
	inline void put_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties)
	{
		put_configuration__config(callConf, theReader, ir, warnings_properties);
		put_configuration__mode(callConf, theReader, ir, warnings_properties);
		put_configuration__parameter(callConf, theReader, ir, warnings_properties);
		put_configuration__monitor(callConf, theReader, ir, warnings_properties);
	};
	// All functions called at once.
	inline void get_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir)
	{
		get_configuration__config(callConf, theReader, ir);
		get_configuration__mode(callConf, theReader, ir);
		get_configuration__parameter(callConf, theReader, ir);
		get_configuration__monitor(callConf, theReader, ir);
	};
};
#endif
