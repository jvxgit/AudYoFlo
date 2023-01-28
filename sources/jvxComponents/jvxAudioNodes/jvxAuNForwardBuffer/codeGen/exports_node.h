#ifndef _GENCAPTUREFILE_NODE_H__
#define _GENCAPTUREFILE_NODE_H__

#include "jvx-helpers.h"
class genCaptureFile_node
{
public:
	genCaptureFile_node()
	{
		init_all();
	};

	//=================================================
	// Member Variables
	//=================================================



	// Section capture::capture
	struct 
	{
		// DESCRIPTION: prefix
		jvxPropertyContainerSingle<std::string> prefix;
		// DESCRIPTION: toggle
		jvxPropertyContainerSingle<jvxCBool> toggle;
		// DESCRIPTION: wav_file_type
		jvxPropertyContainerSingle<jvx_oneSelectionOption_cpp> wav_file_type;
		// DESCRIPTION: wav_file_resolution
		jvxPropertyContainerSingle<jvx_oneSelectionOption_cpp> wav_file_resolution;
		// DESCRIPTION: num_buffers
		jvxPropertyContainerSingle<jvxSize> num_buffers;
	} capture;

	// Section monitor::monitor
	struct 
	{
		// DESCRIPTION: buffer_drops
		jvxPropertyContainerSingle<jvxSize> buffer_drops;
	} monitor;

	//=================================================
	// Init functions
	//=================================================

	// Comment: capture
	inline void init__capture()
	{
		// Initialize the struct to neutral defined values.
		capture.prefix.description = "prefix";
		capture.prefix.descriptor = "prefix";
		capture.prefix.value = "";
		capture.prefix.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.prefix.config_mode_flags = 3;
		capture.toggle.description = "toggle";
		capture.toggle.descriptor = "toggle";
		capture.toggle.value = 0;
		capture.toggle.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.toggle.config_mode_flags = 3;
		capture.wav_file_type.description = "wav_file_type";
		capture.wav_file_type.descriptor = "wav_file_type";
		jvx_bitFClear(capture.wav_file_type.value.selection);
		jvx_bitFClear(capture.wav_file_type.value.selectable);
		jvx_bitFClear(capture.wav_file_type.value.exclusive);
		capture.wav_file_type.value.entries.clear();
		capture.wav_file_type.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.wav_file_type.config_mode_flags = 3;
		capture.wav_file_resolution.description = "wav_file_resolution";
		capture.wav_file_resolution.descriptor = "wav_file_resolution";
		jvx_bitFClear(capture.wav_file_resolution.value.selection);
		jvx_bitFClear(capture.wav_file_resolution.value.selectable);
		jvx_bitFClear(capture.wav_file_resolution.value.exclusive);
		capture.wav_file_resolution.value.entries.clear();
		capture.wav_file_resolution.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.wav_file_resolution.config_mode_flags = 3;
		capture.num_buffers.description = "num_buffers";
		capture.num_buffers.descriptor = "num_buffers";
		capture.num_buffers.value = 0;
		capture.num_buffers.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.num_buffers.config_mode_flags = 3;
	};

	// Comment: monitor
	inline void init__monitor()
	{
		// Initialize the struct to neutral defined values.
		monitor.buffer_drops.description = "buffer_drops";
		monitor.buffer_drops.descriptor = "buffer_drops";
		monitor.buffer_drops.value = 0;
		monitor.buffer_drops.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		monitor.buffer_drops.config_mode_flags = 3;
	};

	//=================================================
	// Allocation functions
	//=================================================

	// Comment: capture
	inline void allocate__capture()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: prefix
		capture.prefix.value = "SPEAKER_CAPTURE";
		capture.prefix.valid = true;
		capture.prefix.frozen = false;
		capture.prefix.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.prefix.config_mode_flags = 3;
		capture.prefix.onlySelectionToFile = false;
	// DESCRIPTION: toggle
		capture.toggle.value = 0;
		capture.toggle.valid = true;
		capture.toggle.frozen = false;
		capture.toggle.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.toggle.config_mode_flags = 3;
		capture.toggle.onlySelectionToFile = false;
	// DESCRIPTION: wav_file_type
		capture.wav_file_type.value.selection = 0;
		capture.wav_file_type.value.exclusive = 0;
		capture.wav_file_type.value.entries.push_back("Wav 64 Sony");
		capture.wav_file_type.value.selectable |= (1 << 0);
		capture.wav_file_type.value.entries.push_back("Wav 64");
		capture.wav_file_type.value.selectable |= (1 << 1);
		capture.wav_file_type.value.entries.push_back("Wav 32");
		capture.wav_file_type.value.selection |= (1 << 2);
		capture.wav_file_type.value.selectable |= (1 << 2);
		capture.wav_file_type.valid = true;
		capture.wav_file_type.frozen = false;
		capture.wav_file_type.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.wav_file_type.config_mode_flags = 3;
		capture.wav_file_type.onlySelectionToFile = false;
	// DESCRIPTION: wav_file_resolution
		capture.wav_file_resolution.value.selection = 0;
		capture.wav_file_resolution.value.exclusive = 0;
		capture.wav_file_resolution.value.entries.push_back("16 Bit");
		capture.wav_file_resolution.value.selection |= (1 << 0);
		capture.wav_file_resolution.value.selectable |= (1 << 0);
		capture.wav_file_resolution.value.entries.push_back("24 Bit");
		capture.wav_file_resolution.value.selectable |= (1 << 1);
		capture.wav_file_resolution.value.entries.push_back("32 Bit");
		capture.wav_file_resolution.value.selectable |= (1 << 2);
		capture.wav_file_resolution.valid = true;
		capture.wav_file_resolution.frozen = false;
		capture.wav_file_resolution.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.wav_file_resolution.config_mode_flags = 3;
		capture.wav_file_resolution.onlySelectionToFile = false;
	// DESCRIPTION: num_buffers
		capture.num_buffers.value = 10;
		capture.num_buffers.valid = true;
		capture.num_buffers.frozen = false;
		capture.num_buffers.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		capture.num_buffers.config_mode_flags = 3;
		capture.num_buffers.onlySelectionToFile = false;
	};

	// Comment: monitor
	inline void allocate__monitor()
	{
		// Allocate memory and update registration with property handler if desired.
	// DESCRIPTION: buffer_drops
		monitor.buffer_drops.value = 0;
		monitor.buffer_drops.valid = true;
		monitor.buffer_drops.frozen = false;
		monitor.buffer_drops.access_flags = JVX_COMBINE_FLAGS_RCWD(16383,16383,16383,16383);
		monitor.buffer_drops.config_mode_flags = 3;
		monitor.buffer_drops.onlySelectionToFile = false;
	};

	//=================================================
	// Deallocation functions
	//=================================================

	// Comment: capture
	inline void deallocate__capture()
	{
		// If necessary, deallocate.
	// DESCRIPTION: prefix
		capture.prefix.value = "SPEAKER_CAPTURE";
		capture.prefix.reset();
	// DESCRIPTION: toggle
		capture.toggle.reset();
	// DESCRIPTION: wav_file_type
		capture.wav_file_type.value.selection = 0;
		capture.wav_file_type.value.exclusive = 0;
		capture.wav_file_type.value.entries.clear();
		capture.wav_file_type.reset();
	// DESCRIPTION: wav_file_resolution
		capture.wav_file_resolution.value.selection = 0;
		capture.wav_file_resolution.value.exclusive = 0;
		capture.wav_file_resolution.value.entries.clear();
		capture.wav_file_resolution.reset();
	// DESCRIPTION: num_buffers
		capture.num_buffers.reset();
	};

	// Comment: monitor
	inline void deallocate__monitor()
	{
		// If necessary, deallocate.
	// DESCRIPTION: buffer_drops
		monitor.buffer_drops.reset();
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

	// Comment: capture
	inline void register__capture(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		// Register properties with property management system.
		jvxErrorType resL = JVX_NO_ERROR;
		theProps->_lock_properties_local();
		capture.prefix.descriptor = jvx_makePathExpr(reg_prefix, capture.prefix.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << capture.prefix.descriptor << ">." << std::endl;
		capture.prefix.id = JVX_SIZE_UNSELECTED;
		capture.prefix.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		capture.prefix.format = JVX_DATAFORMAT_STRING;
		resL = theProps->_register_property(&capture.prefix.value, 1, JVX_DATAFORMAT_STRING, &capture.prefix.id, capture.prefix.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "prefix", capture.prefix.description.c_str(), capture.prefix.descriptor.c_str(), &capture.prefix.valid, &capture.prefix.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &capture.prefix.access_flags, &capture.prefix.config_mode_flags, "/set_processing_prop", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << capture.prefix.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:12";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, capture.prefix.id, capture.prefix.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		capture.toggle.descriptor = jvx_makePathExpr(reg_prefix, capture.toggle.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << capture.toggle.descriptor << ">." << std::endl;
		capture.toggle.id = JVX_SIZE_UNSELECTED;
		capture.toggle.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		capture.toggle.format = JVX_DATAFORMAT_16BIT_LE;
		resL = theProps->_register_property(&capture.toggle.value, 1, JVX_DATAFORMAT_16BIT_LE, &capture.toggle.id, capture.toggle.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "toggle", capture.toggle.description.c_str(), capture.toggle.descriptor.c_str(), &capture.toggle.valid, &capture.toggle.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SIMPLE_ONOFF, JVX_PROPERTY_CONTEXT_UNKNOWN, &capture.toggle.access_flags, &capture.toggle.config_mode_flags, "/set_processing_prop", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << capture.toggle.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:20";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, capture.toggle.id, capture.toggle.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		capture.wav_file_type.descriptor = jvx_makePathExpr(reg_prefix, capture.wav_file_type.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << capture.wav_file_type.descriptor << ">." << std::endl;
		capture.wav_file_type.id = JVX_SIZE_UNSELECTED;
		capture.wav_file_type.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		capture.wav_file_type.format = JVX_DATAFORMAT_SELECTION_LIST;
		resL = theProps->_register_property(&capture.wav_file_type.value, 1, JVX_DATAFORMAT_SELECTION_LIST, &capture.wav_file_type.id, capture.wav_file_type.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "wav_file_type", capture.wav_file_type.description.c_str(), capture.wav_file_type.descriptor.c_str(), &capture.wav_file_type.valid, &capture.wav_file_type.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SINGLE_SELECTION, JVX_PROPERTY_CONTEXT_UNKNOWN, &capture.wav_file_type.access_flags, &capture.wav_file_type.config_mode_flags, "/set_processing_prop", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << capture.wav_file_type.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:28";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, capture.wav_file_type.id, capture.wav_file_type.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		capture.wav_file_resolution.descriptor = jvx_makePathExpr(reg_prefix, capture.wav_file_resolution.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << capture.wav_file_resolution.descriptor << ">." << std::endl;
		capture.wav_file_resolution.id = JVX_SIZE_UNSELECTED;
		capture.wav_file_resolution.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		capture.wav_file_resolution.format = JVX_DATAFORMAT_SELECTION_LIST;
		resL = theProps->_register_property(&capture.wav_file_resolution.value, 1, JVX_DATAFORMAT_SELECTION_LIST, &capture.wav_file_resolution.id, capture.wav_file_resolution.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "wav_file_resolution", capture.wav_file_resolution.description.c_str(), capture.wav_file_resolution.descriptor.c_str(), &capture.wav_file_resolution.valid, &capture.wav_file_resolution.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_SINGLE_SELECTION, JVX_PROPERTY_CONTEXT_UNKNOWN, &capture.wav_file_resolution.access_flags, &capture.wav_file_resolution.config_mode_flags, "/set_processing_prop", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << capture.wav_file_resolution.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:44";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, capture.wav_file_resolution.id, capture.wav_file_resolution.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		capture.num_buffers.descriptor = jvx_makePathExpr(reg_prefix, capture.num_buffers.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << capture.num_buffers.descriptor << ">." << std::endl;
		capture.num_buffers.id = JVX_SIZE_UNSELECTED;
		capture.num_buffers.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		capture.num_buffers.format = JVX_DATAFORMAT_SIZE;
		resL = theProps->_register_property(&capture.num_buffers.value, 1, JVX_DATAFORMAT_SIZE, &capture.num_buffers.id, capture.num_buffers.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "num_buffers", capture.num_buffers.description.c_str(), capture.num_buffers.descriptor.c_str(), &capture.num_buffers.valid, &capture.num_buffers.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &capture.num_buffers.access_flags, &capture.num_buffers.config_mode_flags, "/set_processing_prop", "", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << capture.num_buffers.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:60";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, capture.num_buffers.id, capture.num_buffers.cat);
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
		monitor.buffer_drops.descriptor = jvx_makePathExpr(reg_prefix, monitor.buffer_drops.descriptor);
		if(v_register)
			 std::cout << "Registered property <" << monitor.buffer_drops.descriptor << ">." << std::endl;
		monitor.buffer_drops.id = JVX_SIZE_UNSELECTED;
		monitor.buffer_drops.cat = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
		monitor.buffer_drops.format = JVX_DATAFORMAT_SIZE;
		resL = theProps->_register_property(&monitor.buffer_drops.value, 1, JVX_DATAFORMAT_SIZE, &monitor.buffer_drops.id, monitor.buffer_drops.cat, 0x1E, 0xFFFFFFFFFFFFFFFF, "buffer_drops", monitor.buffer_drops.description.c_str(), monitor.buffer_drops.descriptor.c_str(), &monitor.buffer_drops.valid, &monitor.buffer_drops.frozen, JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE, JVX_PROPERTY_DECODER_NONE, JVX_PROPERTY_CONTEXT_UNKNOWN, &monitor.buffer_drops.access_flags, &monitor.buffer_drops.config_mode_flags, "", "/get_processing_monitor", "", "");
		if(resL != JVX_NO_ERROR) std::cout << "Failed to register property with descriptor <" << monitor.buffer_drops.descriptor << ">, reason: " << jvxErrorType_descr(resL) << ">." << std::endl;
#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN
		{
			std::string str_origin = "O:/jvx/audio/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile/codeGen/exports_node.pcg:77";
			jvxApiString jstr_origin;
			jstr_origin.assign_const(str_origin.c_str(), str_origin.size());
			theProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, monitor.buffer_drops.id, monitor.buffer_drops.cat);
		}
#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN
		theProps->_unlock_properties_local();

	};

	//=================================================
	// Unregister functions 
	//=================================================

	// Comment: capture
	inline void unregister__capture(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(capture.prefix.cat , capture.prefix.id);
		capture.prefix.format = JVX_DATAFORMAT_NONE;
		capture.prefix.id = JVX_SIZE_UNSELECTED;
		capture.prefix.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(capture.toggle.cat , capture.toggle.id);
		capture.toggle.format = JVX_DATAFORMAT_NONE;
		capture.toggle.id = JVX_SIZE_UNSELECTED;
		capture.toggle.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(capture.wav_file_type.cat , capture.wav_file_type.id);
		capture.wav_file_type.format = JVX_DATAFORMAT_NONE;
		capture.wav_file_type.id = JVX_SIZE_UNSELECTED;
		capture.wav_file_type.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(capture.wav_file_resolution.cat , capture.wav_file_resolution.id);
		capture.wav_file_resolution.format = JVX_DATAFORMAT_NONE;
		capture.wav_file_resolution.id = JVX_SIZE_UNSELECTED;
		capture.wav_file_resolution.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unregister_property(capture.num_buffers.cat , capture.num_buffers.id);
		capture.num_buffers.format = JVX_DATAFORMAT_NONE;
		capture.num_buffers.id = JVX_SIZE_UNSELECTED;
		capture.num_buffers.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	// Comment: monitor
	inline void unregister__monitor(CjvxProperties* theProps)
	{
		// Unregister properties with property management system.
		theProps->_lock_properties_local();
		theProps->_unregister_property(monitor.buffer_drops.cat , monitor.buffer_drops.id);
		monitor.buffer_drops.format = JVX_DATAFORMAT_NONE;
		monitor.buffer_drops.id = JVX_SIZE_UNSELECTED;
		monitor.buffer_drops.cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		theProps->_unlock_properties_local();

	};

	//=================================================
	// Config file read functions 
	//=================================================

	// Comment: capture
	inline void put_configuration__capture(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Put configuration to initialize the properties.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, capture.toggle.config_mode_flags,callConf->access_flags, capture.toggle.access_flags);
		token = capture.toggle.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, ir, token.c_str(), (jvxInt16*)&capture.toggle.value, &capture.toggle.access_flags, &capture.toggle.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + capture.toggle.descriptor + "> from config file.");
		}
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, capture.wav_file_type.config_mode_flags,callConf->access_flags, capture.wav_file_type.access_flags);
		token = capture.wav_file_type.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignmentSelectionList(theReader, ir, token.c_str(), &capture.wav_file_type.value, capture.wav_file_type.onlySelectionToFile, true, &capture.wav_file_type.access_flags, &capture.wav_file_type.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + capture.wav_file_type.descriptor + "> from config file.");
		}
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, capture.wav_file_resolution.config_mode_flags,callConf->access_flags, capture.wav_file_resolution.access_flags);
		token = capture.wav_file_resolution.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignmentSelectionList(theReader, ir, token.c_str(), &capture.wav_file_resolution.value, capture.wav_file_resolution.onlySelectionToFile, true, &capture.wav_file_resolution.access_flags, &capture.wav_file_resolution.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + capture.wav_file_resolution.descriptor + "> from config file.");
		}
		whattodo = jvx_check_config_mode_put_configuration(callConf->config_mode_flags, capture.num_buffers.config_mode_flags,callConf->access_flags, capture.num_buffers.access_flags);
		token = capture.num_buffers.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		if(HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, ir, token.c_str(), &capture.num_buffers.value, &capture.num_buffers.access_flags, &capture.num_buffers.config_mode_flags, whattodo) != JVX_NO_ERROR)
		{
				warnings_properties.push_back("Failed to read property <" + capture.num_buffers.descriptor + "> from config file.");
		}
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

	// Comment: capture
	inline void get_configuration__capture(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)
	{
		// Get configuration to store the property values.
		jvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;
		std::string token;
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, capture.toggle.config_mode_flags, callConf->access_flags, capture.toggle.access_flags);
		token = capture.toggle.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, ir, token.c_str(), (jvxInt16*)&capture.toggle.value, &capture.toggle.access_flags, &capture.toggle.config_mode_flags, whattodo);
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, capture.wav_file_type.config_mode_flags, callConf->access_flags, capture.wav_file_type.access_flags);
		token = capture.wav_file_type.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignmentSelectionList(theReader, ir, token.c_str(), &capture.wav_file_type.value, capture.wav_file_type.onlySelectionToFile, &capture.wav_file_type.access_flags, &capture.wav_file_type.config_mode_flags, whattodo);
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, capture.wav_file_resolution.config_mode_flags, callConf->access_flags, capture.wav_file_resolution.access_flags);
		token = capture.wav_file_resolution.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignmentSelectionList(theReader, ir, token.c_str(), &capture.wav_file_resolution.value, capture.wav_file_resolution.onlySelectionToFile, &capture.wav_file_resolution.access_flags, &capture.wav_file_resolution.config_mode_flags, whattodo);
		whattodo = jvx_check_config_mode_get_configuration(callConf->config_mode_flags, capture.num_buffers.config_mode_flags, callConf->access_flags, capture.num_buffers.access_flags);
		token = capture.num_buffers.descriptor;
		if(replace_this && replace_by_this)
		{
			token = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);
		}
		HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, ir, token.c_str(), &capture.num_buffers.value, &capture.num_buffers.access_flags, &capture.num_buffers.config_mode_flags, whattodo);
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

	jvxSize translate__capture__wav_file_type_from()
	{
		if (jvx_bitTest(capture.wav_file_type.value.selection, 0))
			return JVX_WAV_64_SONY;
		else if (jvx_bitTest(capture.wav_file_type.value.selection, 1))
			return JVX_WAV_64;
		return JVX_WAV_32;
	};


	 void  translate__capture__wav_file_type_to(jvxSize val)
	{
		switch(val)
		{
		case JVX_WAV_64_SONY:
			jvx_bitZSet(capture.wav_file_type.value.selection, 0);
			break;
		case JVX_WAV_64:
			jvx_bitZSet(capture.wav_file_type.value.selection, 1);
			break;
		default:
			jvx_bitZSet(capture.wav_file_type.value.selection, 2);
		}
	};
	jvxSize translate__capture__wav_file_resolution_from()
	{
		if (jvx_bitTest(capture.wav_file_resolution.value.selection, 0))
			return 16;
		else if (jvx_bitTest(capture.wav_file_resolution.value.selection, 1))
			return 24;
		return 32;
	};


	 void  translate__capture__wav_file_resolution_to(jvxSize val)
	{
		switch(val)
		{
		case 16:
			jvx_bitZSet(capture.wav_file_resolution.value.selection, 0);
			break;
		case 24:
			jvx_bitZSet(capture.wav_file_resolution.value.selection, 1);
			break;
		default:
			jvx_bitZSet(capture.wav_file_resolution.value.selection, 2);
		}
	};
	//=================================================
	// Register callbacks
	//=================================================

	inline void register_callbacks(CjvxProperties* theProps, property_callback cb_set_processing_prop, property_callback cb_get_processing_monitor, jvxHandle* privData, std::vector<std::string>* errMessages)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = theProps->_register_callback("/set_processing_prop", cb_set_processing_prop, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /set_processing_prop, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_register_callback("/get_processing_monitor", cb_get_processing_monitor, privData);
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to register callback /get_processing_monitor, reason: " + std::string(jvxErrorType_descr(res)) + ".");
	};
	//=================================================
	// Unregister callbacks
	//=================================================

	inline void unregister_callbacks(CjvxProperties* theProps, std::vector<std::string>* errMessages)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = theProps->_unregister_callback("/set_processing_prop");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /set_processing_prop, reason: " + std::string(jvxErrorType_descr(res)) + ".");
		res = theProps->_unregister_callback("/get_processing_monitor");
		if(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back("Failed to unregister callback /get_processing_monitor, reason: " + std::string(jvxErrorType_descr(res)) + ".");
	};
	// All functions called at once.
	inline void init_all()
	{
		init__capture();
		init__monitor();
	};
	// All functions called at once.
	inline void allocate_all()
	{
		allocate__capture();
		allocate__monitor();
	};
	// All functions called at once.
	inline void deallocate_all()
	{
		deallocate__capture();
		deallocate__monitor();
	};
	// All functions called at once.
	inline void register_all(CjvxProperties* theProps, const std::string reg_prefix = "", jvxBool v_register = false)
	{
		register__capture(theProps, reg_prefix,v_register);
		register__monitor(theProps, reg_prefix,v_register);
	};
	// All functions called at once.
	inline void unregister_all(CjvxProperties* theProps)
	{
		unregister__capture(theProps);
		unregister__monitor(theProps);
	};
	// All functions called at once.
	inline void put_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>& warnings_properties)
	{
		put_configuration__capture(callConf, theReader, ir, warnings_properties);
		put_configuration__monitor(callConf, theReader, ir, warnings_properties);
	};
	// All functions called at once.
	inline void get_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir)
	{
		get_configuration__capture(callConf, theReader, ir);
		get_configuration__monitor(callConf, theReader, ir);
	};
};
#endif
