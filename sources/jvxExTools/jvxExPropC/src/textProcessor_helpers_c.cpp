#include "scan_token_defines.h"
#include "textProcessor_core.h"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string prefixListToString(std::vector<std::string> lst, std::string sep)
{
	std::string ret;
	jvxSize i;
	for (i = 0; i < lst.size(); i++)
	{
		ret += lst[i] + sep;
	}
	return ret;
}

static std::string tabify(int numTabs)
{
	jvxSize i;
	std::string txt;
	for(i = 0; i < numTabs; i++)
	{
		txt +="\t";
	}
	return(txt);
}

#define JVX_REPLACE_LINE_TOKEN "/*$LINE$*/"
#define JVX_REPLACE_LINE_TOKEN_COMPLETE "\r"
#define JVX_REPLACE_LINE_TOKEN_COMP JVX_REPLACE_LINE_TOKEN JVX_REPLACE_LINE_TOKEN_COMPLETE 

static void correct_line_statements(const std::string& fName)
{
	size_t nextpos = 0;
	size_t pos = 0;
	jvxSize lineCnt = 0;
	std::string content;
	std::string generated;
	std::string fNameL = jvx_replaceCharacter(fName, '\\', '/');
	jvx_readContentFromFile(fNameL, content);
	while (1)
	{
		nextpos = content.find('\n', pos);
		std::string onetoken = content.substr(pos, nextpos-pos);
		if (onetoken == (JVX_REPLACE_LINE_TOKEN_COMP))
		{
			onetoken = "#line " + jvx_size2String(lineCnt + 2) + " \"" + fNameL + "\"" JVX_REPLACE_LINE_TOKEN_COMPLETE;
 		}
		generated += onetoken + "\n";
		pos = nextpos+1;
		lineCnt++;
		if (nextpos == std::string::npos)
		{
			break;
		}
	}
	jvx_writeContentToFile(fNameL, generated);
}

void
textProcessor_core::produceOutput_c_prepare(onePropertyDefinition& elm)
{
	elm.tokenTypeAss = "";
	switch (elm.format)
	{
	case JVX_DATAFORMAT_DATA:
		elm.tokenType = MACRO_TYPE_DATA;
		elm.tokenTypeName = MACRO_DATATYPE_DATA;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		elm.tokenType = MACRO_TYPE_INT64_LE;
		elm.tokenTypeName = MACRO_DATATYPE_64BIT_LE;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		elm.tokenType = MACRO_TYPE_INT32_LE;
		elm.tokenTypeName = MACRO_DATATYPE_32BIT_LE;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		elm.tokenType = MACRO_TYPE_INT16_LE;
		elm.tokenTypeName = MACRO_DATATYPE_16BIT_LE;
		if (elm.decoderType == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
		{
			elm.tokenType = MACRO_TYPE_CBOOL;
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		elm.tokenType = MACRO_TYPE_INT8;
		elm.tokenTypeName = MACRO_DATATYPE_8BIT;
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		elm.tokenType = MACRO_TYPE_UINT64_LE;
		elm.tokenTypeName = MACRO_DATATYPE_U64BIT_LE;
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		elm.tokenType = MACRO_TYPE_UINT32_LE;
		elm.tokenTypeName = MACRO_DATATYPE_U32BIT_LE;
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		elm.tokenType = MACRO_TYPE_UINT16_LE;
		elm.tokenTypeName = MACRO_DATATYPE_U16BIT_LE;
		switch (elm.decoderType)
		{
		case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
			elm.tokenType = MACRO_TYPE_CBOOL;
			break;
		}
		break;
	case JVX_DATAFORMAT_U8BIT:
		elm.tokenType = MACRO_TYPE_UINT8;
		elm.tokenTypeName = MACRO_DATATYPE_U8BIT;
		break;
	case JVX_DATAFORMAT_BYTE:
		elm.tokenType = MACRO_TYPE_BYTE;
		elm.tokenTypeName = MACRO_DATATYPE_BYTE;
		break;
	case JVX_DATAFORMAT_SIZE:
		elm.tokenType = MACRO_TYPE_SIZE;
		elm.tokenTypeName = MACRO_DATATYPE_SIZE;
		break;
	case JVX_DATAFORMAT_STRING:
		elm.tokenType = MACRO_TYPE_STRING;
		elm.tokenTypeName = MACRO_DATATYPE_STRING;
		elm.numElements = 1;
		elm.singleElementField = true;
		elm.associateNumberEntries = false;
		break;
	case JVX_DATAFORMAT_STRING_LIST:
		elm.tokenType = MACRO_TYPE_STRING_LIST;
		elm.tokenTypeName = MACRO_DATATYPE_STRING_LIST;
		elm.numElements = 1;
		elm.singleElementField = true;
		elm.associateNumberEntries = false;
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		elm.tokenType = MACRO_TYPE_SELECTION_LIST;
		elm.tokenTypeAss = "jvxBitField";
		elm.tokenTypeName = MACRO_DATATYPE_SELECTION_LIST;
		// elm.numElements = 1;
		elm.singleElementField = true;
		elm.associateNumberEntries = true;
		break;
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
		elm.tokenType = MACRO_TYPE_VALUE_IN_RANGE;
		elm.tokenTypeAss = MACRO_TYPE_DATA;
		elm.tokenTypeName = MACRO_DATATYPE_VALUE_IN_RANGE;
		//elm.numElements = 1;
		elm.singleElementField = true;
		elm.associateNumberEntries = true;
		break;
	case JVX_DATAFORMAT_HANDLE:
		elm.tokenType = MACRO_TYPE_HANDLE;
		elm.tokenTypeName = MACRO_DATATYPE_HANDLE;
		elm.numElements = 1;
		elm.associateExternal = false;
		elm.dynamic = false;
		elm.validOnInit = false;
		elm.generateConfigFileEntry = false;
		break;
	case JVX_DATAFORMAT_CALLBACK:
		elm.tokenType = MACRO_TYPE_CALLBACK;
		elm.tokenTypeName = MACRO_DATATYPE_CALLBACK_;
		elm.numElements = 1;
		//elm.associateExternal = false;

		elm.associateExternal = true;
		elm.installable = true;
		elm.nullable = true;
		
		elm.dynamic = false;
		elm.validOnInit = false;
		elm.generateConfigFileEntry = false;
		break;

	case JVX_DATAFORMAT_INTERFACE:
		elm.tokenType = "invalid_interface<";
		elm.tokenType += jvxPropertyDecoderHintType_txt(elm.decoderType);
		elm.tokenType += ">";
		elm.tokenTypeName = MACRO_DATATYPE_INTERFACE;
		elm.numElements = 1;
		elm.dynamic = false;
		elm.associateExternal = true;
		elm.installable = true;
		elm.singleElementField = true;
		elm.validOnInit = false;
		switch (elm.decoderType)
		{
		case JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE:
			elm.tokenType = "IjvxPropertyExtender";
			break;
		}
		elm.generateConfigFileEntry = false;
		break;
	default:
		assert(0);
	}
	if (elm.tokenTypeAss.empty())
	{
		elm.tokenTypeAss = elm.tokenType;
	}

	// The following two external interfaces are variants of specific datatypes
	switch (elm.decoderType)
	{
	case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
	case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
		elm.tokenType = "jvxExternalBuffer";
		elm.associateExternal = true;
		elm.installable = true;
		elm.generateConfigFileEntry = false;
		break;
	case JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG:
		elm.tokenType = "IjvxLocalTextLog";
		elm.associateExternal = true;
		elm.installable = true;
		elm.generateConfigFileEntry = false;
		break;

	default:

		// All other elements can not be installed at the moment!
		// elm.installable = false;
		break;
	}
}

void
textProcessor_core::produceOutput_c_variables(std::ostream& out, onePropertyDefinition& elm, int tabOffset)
{
	std::string tokenPrefix = "";
	std::string containerTypeName;

	if(elm.associateExternal)
	{
		switch (elm.format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
		case JVX_DATAFORMAT_CALLBACK:
			containerTypeName = "jvxPropertyContainerSingle";
			break;
		default:
			containerTypeName = "jvxPropertyContainerMulti";
		}

		out << tabify(2+tabOffset) << containerTypeName << "<" << elm.tokenType << "> " << tokenPrefix << elm.name << ";" << std::endl;
		
		/*
		out << tabify(2+tabOffset) << "struct " << std::endl;
		out << tabify(2+tabOffset) << "{" << std::endl;
		out << tabify(3+tabOffset) << tokenType << " *ptr;" << std::endl;
		out << tabify(3+tabOffset) << "jvxSize" << " num;" << std::endl;
		out << tabify(3+tabOffset) << "jvxSize id;" << std::endl;
		out << tabify(3+tabOffset) << "jvxPropertyCategoryType cat;" << std::endl;
		out << tabify(3+tabOffset) << "jvxBool valid;" << std::endl;
		out << tabify(3+tabOffset) << "jvxDataFormat format;" << std::endl;
		out << tabify(3+tabOffset) << "jvxBool frozen;" << std::endl;
		out << tabify(3+tabOffset) << "jvxBool onlySelectionToFile;" << std::endl;
		out << tabify(3 + tabOffset) << "jvxFlagTag config_mode_flags;" << std::endl;
		out << tabify(3+tabOffset) << "std::string descriptor;" << std::endl;
		out << tabify(2+tabOffset) << "} " << tokenPrefix << elm.name << ";" << std::endl;
		*/
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			out << tabify(2+tabOffset) << "jvxPropertyContainerMulti<" << elm.tokenType << "> " << tokenPrefix << elm.name << ";" << std::endl;
			/*
			if(multipleObjects)
			{

				out << tabify(2+tabOffset) << "struct " << std::endl;
				out << tabify(2+tabOffset) << "{" << std::endl;
				out << tabify(3+tabOffset) << tokenType << " *ptr;" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize num;" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize id;" << std::endl;
				out << tabify(3+tabOffset) << "jvxPropertyCategoryType cat;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool valid;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxDataFormat format;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool frozen;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool onlySelectionToFile;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxFlagTag config_mode_flags;" << std::endl;
				out << tabify(3+tabOffset) << "std::string descriptor;" << std::endl;
				out << tabify(2+tabOffset) << "} " << elm.name << ";" << std::endl;
			}
			else
			{
				out << tabify(2+tabOffset) << "struct " << std::endl;
				out << tabify(2+tabOffset) << "{" << std::endl;
				out << tabify(3+tabOffset) << tokenType << " *ptr;" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize id;" << std::endl;
				out << tabify(3+tabOffset) << "jvxPropertyCategoryType cat;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool valid;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxDataFormat format;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool frozen;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool onlySelectionToFile;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxFlagTag config_mode_flags;" << std::endl;
				out << tabify(3+tabOffset) << "std::string descriptor;" << std::endl;
				out << tabify(2+tabOffset) << "} " << elm.name << ";" << std::endl;
			}
			*/
		}
		else
		{
			if(elm.numElements > 1)
			{
				/*
				out << tabify(2+tabOffset) << "struct " << std::endl;
				out << tabify(2+tabOffset) << "{" << std::endl;
				out << tabify(3+tabOffset) << tokenType << " fld[" << elm.numElements << "];" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize num;" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize id;" << std::endl;
				out << tabify(3+tabOffset) << "jvxPropertyCategoryType cat;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool valid;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxDataFormat format;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool frozen;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool onlySelectionToFile;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxFlagTag config_mode_flags;" << std::endl;
				out << tabify(3+tabOffset) << "std::string descriptor;" << std::endl;
				out << tabify(2+tabOffset) << "} " << elm.name << ";" << std::endl;
				*/
				switch (elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					out << tabify(2 + tabOffset) << "jvxPropertyContainerSingle<" << elm.tokenType << "> " << tokenPrefix << elm.name << ";" << std::endl;
					break;
				default:
					out << tabify(2 + tabOffset) << "jvxPropertyContainerMulti<" << elm.tokenType << "> " << tokenPrefix << elm.name << ";" << std::endl;
					out << tabify(2 + tabOffset) << elm.tokenType << " " << elm.name << "_fld[" << elm.numElements << "];" << std::endl;
				}
			}
			else
			{
				/*
				out << tabify(2+tabOffset) << "struct " << std::endl;
				out << tabify(2+tabOffset) << "{" << std::endl;
				out << tabify(3+tabOffset) << tokenType << " value;" << std::endl;
				out << tabify(3+tabOffset) << "jvxSize id;" << std::endl;
				out << tabify(3+tabOffset) << "jvxPropertyCategoryType cat;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool valid;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool frozen;" << std::endl;		
				out << tabify(3 + tabOffset) << "jvxDataFormat format;" << std::endl;
				out << tabify(3+tabOffset) << "jvxBool onlySelectionToFile;" << std::endl;
				out << tabify(3 + tabOffset) << "jvxFlagTag config_mode_flags;" << std::endl;
				out << tabify(3+tabOffset) << "std::string descriptor;" << std::endl;
				out << tabify(2+tabOffset) << "} " << elm.name << ";" << std::endl;
				*/
				out << tabify(2+tabOffset) << "jvxPropertyContainerSingle<" << elm.tokenType << "> " << tokenPrefix << elm.name << ";" << std::endl;
			}

		}
	}
}

void
textProcessor_core::produceOutput_c_init(std::ostream& out, 
	onePropertyDefinition& elm,
	std::string& propertySectionName, 
	std::string& fncSectionName, 
	std::ostream& out_translators,
	std::map<jvxInt32, oneAudioPluginEntry>& collectAudioPluginsIds,
	std::list<oneAudioPluginEntry>& collectInvalidAudioPluginsIds)
{
	jvxSize i;

	out << "\t\t" << propertySectionName << "." << elm.name << ".name = \"" << elm.name << "\";" << std::endl;
	out << "\t\t" << propertySectionName  << "." << elm.name << ".description = \"" << elm.description << "\";" << std::endl;
	out << "\t\t" << propertySectionName  << "." << elm.name << ".descriptor = \"" << elm.descriptor << "\";" << std::endl;
	
	if (elm.invalidateOnStateSwitch != JVX_PROPERTY_INVALIDATE_INACTIVE)
	{
		out << "\t\t" << propertySectionName << "." << elm.name << ".invalidateOnStateSwitch = " << 
			jvxPropertyInvalidateType_str[elm.invalidateOnStateSwitch].full << "; " << std::endl;
	}
	if (elm.invalidateOnTest != JVX_PROPERTY_INVALIDATE_INACTIVE)
	{
		out << "\t\t" << propertySectionName << "." << elm.name << ".invalidateOnTest = " << 
			jvxPropertyInvalidateType_str[elm.invalidateOnTest].full << "; " << std::endl;
	}

	/*
	if (elm.audioPluginDescr.audio_plugin_id_set)
	{
		out << "\t\t" << propertySectionName << "." << elm.name << ".ext_id = " << elm.audioPluginDescr.audio_plugin_id << ";" << std::endl;
	}
	*/
	if (
		(elm.format == JVX_DATAFORMAT_SELECTION_LIST) &&
		(elm.decoderType == JVX_PROPERTY_DECODER_SINGLE_SELECTION) &&
		(
			(!elm.translations.tpName.empty())||
			(!elm.selection.extract_string.empty())
		)
	)
	{

		// Property to enum
		out_translators << "\t" << elm.translations.tpName << " translate__" << fncSectionName << "__" << elm.name << "_from(jvxSize idx = 0)" << std::endl;
		out_translators << "\t{" << std::endl;

		if (
			(!elm.selection.varname.empty()) &&
			(!elm.selection.nummax.empty()))
		{
			out_translators << "\t\tfor(int i = 0; i < (int)" << elm.selection.numprefix << 
				elm.selection.nummax << "; i++)" << std::endl;
			out_translators << "\t\t{" << std::endl;
			out_translators << "\t\t\t if(jvx_bitTest(" << propertySectionName << "." << elm.name << ".value.selection(idx), i )) return (" << 
				elm.translations.tpName << ")i;" << std::endl;
			out_translators << "\t\t}" << std::endl;
			out_translators << "\t\treturn " << elm.selection.numprefix << elm.selection.nummax << ";" << std::endl;
		}
		else
		{
			if(!elm.selection.extract_string.empty())
			{
				out_translators << "\t\t// LOC#1" << std::endl;
				out_translators << "\t\tjvxSize idSel = jvx_bitfieldSelection2Id(" << propertySectionName << "." << elm.name << ");" << std::endl;
				out_translators << "\t\tif(JVX_CHECK_SIZE_UNSELECTED(idSel))" << std::endl;
				out_translators << "\t\t{" << std::endl;									
				out_translators << "\t\t\t return " << elm.translations.tpName << "::" << elm.selection.extract_string << ";" << std::endl;
				out_translators << "\t\t}" << std::endl;									
				out_translators << "\t\treturn (" << elm.translations.tpName << ")idSel;" << std::endl;
			}
			else
			{
			for (i = 0; i < elm.selection.strings.size(); i++)
			{
				if (i == elm.selection.strings.size() - 1)
				{
					if (i < elm.translations.strings.size())
					{
						if (elm.translations.enumClass)
						{
							out_translators << "\t\treturn " << elm.translations.tpName << "::" << elm.translations.strings[i] << ";" << std::endl;
						}
						else
						{
							out_translators << "\t\treturn " << elm.translations.strings[i] << ";" << std::endl;
						}
					}
					else
					{
						out_translators << "\t\treturn (" << elm.translations.tpName << ")" << jvx_size2String(i) << ";" << std::endl;
					}
				}
				else
				{
					if (i == 0)
					{
						out_translators << "\t\t" << std::flush;
					}
					else
					{
						out_translators << "\t\telse " << std::flush;
					}
					out_translators << "if (jvx_bitTest(" << propertySectionName << "." << elm.name << ".value.selection(idx), " << i << "))" << std::endl;
					if (i < elm.translations.strings.size())
					{
						if (elm.translations.enumClass)
						{
							out_translators << "\t\t\treturn " << elm.translations.tpName << "::" << elm.translations.strings[i] << ";" << std::endl;
						}
						else
						{
							out_translators << "\t\t\treturn " << elm.translations.strings[i] << ";" << std::endl;
						}
					}
					else
					{
						out_translators << "\t\t\treturn (" << elm.translations.tpName << ")" << jvx_size2String(i) << ";" << std::endl;
					}
				}
			}
			}
		}
		out_translators << "\t};" << std::endl;

		out_translators << std::endl;
		out_translators << std::endl;
		
		// Enum to property
		out_translators << "\tvoid " << " translate__" << fncSectionName << "__" << elm.name << "_to(" 
		<< elm.translations.tpName << " val, jvxSize idx = 0)" << std::endl;
		out_translators << "\t{" << std::endl;

		if (
			(!elm.selection.varname.empty()) && 
			(!elm.selection.nummax.empty()))
		{
			out_translators << "\t\tfor(int i = 0; i < (int)" << elm.selection.numprefix <<
				elm.selection.nummax << "; i++)" << std::endl;
			out_translators << "\t\t{" << std::endl;
			out_translators << "\t\t\t if(val == (" << elm.translations.tpName << ")i) " <<
				"jvx_bitZSet(" <<
					propertySectionName << "." << elm.name << ".value.selection(idx), i);" << std::endl;				
			out_translators << "\t\t}" << std::endl;
		}
		else
		{
			if(!elm.selection.extract_string.empty())
			{
				out_translators << "\t\t// LOC#2" << std::endl;
				out_translators << "\t\tjvx_bitZSet(" <<
						propertySectionName << "." << elm.name << ".value.selection(idx), (int)val);" << std::endl;
			}
			else
			{
			if (elm.selection.strings.size() == 1)
			{
				out_translators << "\t\tjvx_bitZSet(" <<
					propertySectionName << "." << elm.name << ".value.selection(idx), " <<
					jvx_size2String(0) << ");" << std::endl;
			}
			else
			{
				out_translators << "\t\tswitch(val)" << std::endl;
				out_translators << "\t\t{" << std::endl;
				for (i = 0; i < elm.selection.strings.size(); i++)
				{
					if (i == elm.selection.strings.size() - 1)
					{
						out_translators << "\t\tdefault:" << std::endl;
						out_translators << "\t\t\tjvx_bitZSet(" <<
							propertySectionName << "." << elm.name << ".value.selection(idx), " <<
							jvx_size2String(i) << ");" << std::endl;
					}
					else
					{
						out_translators << "\t\tcase " << std::flush;

						if (i < elm.translations.strings.size())
						{
							if (elm.translations.enumClass)
							{
								out_translators << elm.translations.tpName << "::" << elm.translations.strings[i] << ":" << std::endl;
							}
							else
							{
								out_translators << elm.translations.strings[i] << ":" << std::endl;
							}
						}
						else
						{
							out_translators << "(" << elm.translations.tpName << ")" << jvx_size2String(i) << ":" << std::endl;
						}
						out_translators << "\t\t\tjvx_bitZSet(" << propertySectionName << "." << elm.name << ".value.selection(idx), " << jvx_size2String(i) << ");" << std::endl;
						out_translators << "\t\t\tbreak;" << std::endl;
					}
				}
				out_translators << "\t\t}" << std::endl;
			}
			}
		}
		out_translators << "\t};" << std::endl;
		out_translators << std::endl;
		out_translators << std::endl;
	}

	if(elm.associateExternal)
	{
		// Allocation of memory and /or setting of init values
		out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" << 
			elm.access_rights_read << "," <<
			elm.access_rights_write << "," <<
			elm.access_rights_create << "," <<
			elm.access_rights_destroy <<
			");" << std::endl;
		out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;
		if(elm.installable)
		{
			out << "\t\t" << propertySectionName << "." << elm.name << ".installable = true;" << std::endl;
		}
		if(elm.nullable)
		{
			out << "\t\t" << propertySectionName << "." << elm.name << ".value = nullptr;" << std::endl;
		}
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			if(multipleObjects)
			{
				// Allocation and initialization function
				out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
					elm.access_rights_read << "," <<
					elm.access_rights_write << "," <<
					elm.access_rights_create << "," <<
					elm.access_rights_destroy << 
					");" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;
			}
			else
			{
				// Allocation function
				out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
					elm.access_rights_read << "," <<
					elm.access_rights_write << "," <<
					elm.access_rights_create << "," <<
					elm.access_rights_destroy << 
					");" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;

			}// if(multipleObjects)
		} // if(elm.dynamic)
		else
		{
			if(elm.numElements > 1)
			{
				switch (elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
					// Build up selection list first
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.resize(" << elm.numElements << "); " << std::endl;
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.clear();" << std::endl;
					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.resize(" << elm.numElements << "); " << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = 0;" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = 0;" << std::endl;
					break;
				default:
					std::string szToken = jvx_int2String((int)elm.numElements);

					// Allocation and initialization function
					//out << "\t\t" << propertySectionName  << "." << elm.name << ".num = " << szToken << ";" << std::endl;
					//out << "\t\tmemset(" << propertySectionName  << "." << elm.name << ".fld, 0, sizeof(" << tokenTypeName << ") * " << szToken << ");" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".num = " << szToken << ";" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".ptr = " << propertySectionName << "." << elm.name << "_fld;" << std::endl;
					out << "\t\tmemset(" << propertySectionName << "." << elm.name << "_fld, 0, sizeof(" << elm.tokenTypeName << ") * " << szToken << ");" << std::endl;

					out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
						elm.access_rights_read << "," <<
						elm.access_rights_write << "," <<
						elm.access_rights_create << "," <<
						elm.access_rights_destroy <<
						");" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;
					break;
				}
			}
			else
			{
				// Allocation function for single element numeric datatypes
				if(
					(elm.format == JVX_DATAFORMAT_8BIT) ||
					(elm.format == JVX_DATAFORMAT_BYTE) ||
					(elm.format == JVX_DATAFORMAT_SIZE) ||
					(elm.format == JVX_DATAFORMAT_16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_64BIT_LE) ||
					
					(elm.format == JVX_DATAFORMAT_U8BIT) ||
					(elm.format == JVX_DATAFORMAT_U16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U64BIT_LE) ||

					(elm.format == JVX_DATAFORMAT_DATA))
				{
					out << "\t\t" << propertySectionName  << "." << elm.name << ".value = 0;" << std::endl;

					out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
						elm.access_rights_read << "," <<
						elm.access_rights_write << "," <<
						elm.access_rights_create << "," <<
						elm.access_rights_destroy << 
						");" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;

				} // if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
				else
				{
					switch(elm.format)
					{
					case JVX_DATAFORMAT_SELECTION_LIST:

						// Build up selection list first
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.resize(" << elm.numElements << "); " << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName  << "." << elm.name << ".value.selectable);" << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName  << "." << elm.name << ".value.exclusive);" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.entries.clear();" << std::endl;
						break;
					case JVX_DATAFORMAT_STRING:
						if(!elm.installable)
						{
							out << "\t\t" << propertySectionName  << "." << elm.name << ".value = \"\";" << std::endl;
						}
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						//out << "\t\t" << propertySectionName  << "." << elm.name << ".value.push_back(\"" << elm.init_set.strings[k] << "\");" << std::endl;
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.resize(" << elm.numElements << "); " << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.minVal = 0;" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.maxVal = 0;" << std::endl;
						break;
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_CALLBACK:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value = nullptr;" << std::endl;
						break;
					case JVX_DATAFORMAT_INTERFACE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".ptr = nullptr;" << std::endl;
						break;
					}
					out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
						elm.access_rights_read << "," <<
						elm.access_rights_write << "," <<
						elm.access_rights_create << "," <<
						elm.access_rights_destroy << 
						");" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;
				} // else: if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
			}// if(multipleObjects)
		} // if(elm.dynamic)
	}

	if (elm.audioPluginDescr.audio_plugin_id_set)
	{
		oneAudioPluginEntry newElm;
		newElm.oneEntry = elm.audioPluginDescr;

		switch (elm.audioPluginDescr.audio_plugin_param_type)
		{
		case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO:
			newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr = "$PROP.value = $NORMVAL * $SCALE - $OFF;";
			newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr = "*$NORMVAL = ($PROP.value + $OFF)/(jvxData)$SCALE;";
			if(
				(elm.associateExternal) || 
				(elm.numElements > 1)
				)
			{
				newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr = "*$PROP.ptr = $NORMVAL * $SCALE - $OFF;";
				newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr = "*$NORMVAL = (*$PROP.ptr + $OFF)/(jvxData)$SCALE;";
			}
			break;
		case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_STRLIST:
			newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr =
				" \n\
					jvxSize normSel = JVX_DATA2SIZE($NORMVAL * ($PROP.value.entries.size() - 1)); \n\
					jvx_bitZSet($PROP.value.selection(), normSel);\n\
				";	
			newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr =
				" \n\
					jvxSize idxSel = jvx_bitfieldSelection2Id($PROP.value.selection(), $PROP.value.entries.size());\n\
					if(JVX_CHECK_SIZE_SELECTED(idxSel)) \n\
					{ \n\
						*$NORMVAL = (jvxData)idxSel / (jvxData)($PROP.value.entries.size()-1); \n\
					} \n\
				";
			break;
		case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_BOOL:
			if (elm.format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr =
					" \n\
					if ($NORMVAL > 0.5) \n\
					{ \n\
						jvx_bitZSet($PROP.value.selection(), 0); \n\
					} \n\
					else \n\
					{ \n\
						jvx_bitZSet($PROP.value.selection(), 1); \n\
					}";
				newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr =
					" \n\
					if (jvx_bitTest($PROP.value.selection(), 0)) \n\
					{ \n\
						*$NORMVAL = 1.0; \n\
					} \n\
					else \n\
					{ \n\
						*$NORMVAL = 0.0; \n\
					}";
			}
			else
			{
				if (elm.associateExternal)
				{
					newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr =
						"if($PROP.ptr) *$PROP.ptr = ($NORMVAL > 0.5);";
					newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr =
						"\n\
						if($PROP.ptr) \n\
						{ \n\
							if(*$PROP.ptr) \n\
							{ \n\
								*$NORMVAL = 1.0; \n\
							} \n\
							else \n\
							{ \n\
								*$NORMVAL = 0.0; \n\
							} \n\
						}";
				}
				else
				{
					newElm.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr =
						"$PROP.value = ($NORMVAL > 0.5);";
					newElm.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr =
						"\n\
					if($PROP.value) \n\
					{ \n\
						*$NORMVAL = 1.0; \n\
					} \n\
					else \n\
					{ \n\
						*$NORMVAL = 0.0; \n\
					}";
				}
			}
			break;

		}
		newElm.nm_property = propertySectionName + "." + elm.name;
		newElm.expr_property = jvx_makePathExpr(propertySectionName, elm.descriptor); 
		// "jvx_makePathExpr(reg_prefix, " << propertySectionName << "." << elm.name << ".descriptor
		newElm.const_property = "k_" + jvx_replaceCharacter(newElm.nm_property, '.', '_');
		auto it_id = collectAudioPluginsIds.find(elm.audioPluginDescr.audio_plugin_id);
		if (it_id == collectAudioPluginsIds.end())
		{
			collectAudioPluginsIds[newElm.oneEntry.audio_plugin_id] = newElm;
		}
		else
		{
			collectInvalidAudioPluginsIds.push_back(newElm);
		}
	}
}

void
textProcessor_core::produceOutput_c_allocate(std::ostream& out, onePropertyDefinition& elm, 
	std::string& propertySectionName, 
	std::string& funcSectionName)
{
	jvxSize k;


	// out << "\t\t// Always call init before allocate." << std::endl;
	// out << "\t\tinit__" << funcSectionName  << "();" << std::endl;
	
	if(elm.associateExternal)
	{
		switch (elm.format)
		{
		case JVX_DATAFORMAT_SELECTION_LIST:

			// Build up selection list first
			out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.init_selection);" << std::endl;
			out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
			out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;
			out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.clear();" << std::endl;
			if (
				(!elm.selection.varname.empty()) &&
				(!elm.selection.nummax.empty()))
			{
				out << "\t\tfor(int ii = 0; ii < (int)" << elm.selection.numprefix << elm.selection.nummax << "; ii++)" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" << elm.selection.varname << "[ii]" <<
					elm.selection.varpostfix << "); " << std::endl;
				out << "\t\t}" << std::endl;
			}
			else
			{
				if(!elm.selection.extract_string.empty())
				{
					out << "\t\t// LOC#3" << std::endl;
					out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
					out << "\t\t{" << std::endl;
					out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" 
						<< elm.translations.tpName << "_txt(" << "(" << elm.translations.tpName << ")ii));" << std::endl;
					out << "\t\t}" << std::endl;
				}
				else
				{
					
					for (k = 0; k < elm.selection.strings.size(); k++)
					{
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(\"" << elm.selection.strings[k] << "\");" << std::endl;
					}
				}
			}

			if(!elm.selection.extract_string.empty())
			{
				out << "\t\t// LOC#4" << std::endl;
				if(!elm.selection.extract_selected.empty())
				{
					out << "\t\tjvx_bitZSet(" << propertySectionName << "." << elm.name << ".value.init_selection, (int)" 
						<< elm.translations.tpName << "::" << elm.selection.extract_selected << ");" << std::endl;							
					out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
					out << "\t\t{" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << ii);" << std::endl;
					out << "\t\t}" << std::endl;		
				}
				else
				{
					out << "\t\t#error Failed to identify initialize selection for expression <" << elm.selection.extract_string << ">." << std::endl;
				}
			}
			else
			{
					
				for (k = 0; k < elm.selection.strings.size(); k++)
				{
					if (k < elm.selection.selected.size())
					{
						if (elm.selection.selected[k])
						{
							// Do not "fill" the selection itself but the init variable
							out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_selection |= (1 << " << k << ");" << std::endl;
						}
					}
					if (k < elm.selection.exclusive.size())
					{
						if (elm.selection.exclusive[k])
						{
							out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << " << k << ");" << std::endl;
						}
					}
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.selectable |= (1 << " << k << ");" << std::endl;
				}
			}
			out << "\t\t" << propertySectionName << "." << elm.name << ".isValid = false;" << std::endl;
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_value = " << elm.initValueInRange.value << ";" << std::endl;
			out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = " << elm.initValueInRange.minVal << ";" << std::endl;
			out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = " << elm.initValueInRange.maxVal << ";" << std::endl;
			out << "\t\t" << propertySectionName << "." << elm.name << ".isValid = false;" << std::endl;
			break;

		default:
			// Allocation of memory and /or setting of init values
			if (elm.onlySelectionToConfig)
			{
				out << "\t\t" << propertySectionName << "." << elm.name << ".onlySelectionToFile = true;" << std::endl;
			}
			else
			{
				out << "\t\t" << propertySectionName << "." << elm.name << ".onlySelectionToFile = false;" << std::endl;
			}
			break;
		}
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			if(multipleObjects)
			{
				// Allocation and initialization function
				out << "\t\t" << propertySectionName  << "." << elm.name << ".num = " <<  szToken << ";" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".ptr = new " << elm.tokenType << "[" <<  szToken << "];" << std::endl;
				out << "\t\tmemset(" << propertySectionName  << "." << elm.name << ".ptr, " << "0, sizeof(" << elm.tokenType << ") * " << propertySectionName  << "." << elm.name << ".num);" << std::endl;
				if(elm.init_set.filename_set)
				{
					out << "\t\t{" << std::endl;
					out << "\t\t\t" << elm.tokenType << " tmp[" << propertySectionName  << "." << elm.name << ".num] = " << std::endl;
					out << "\t\t\t{" << std::endl << "\t\t\t\t" << std::flush;
					out << "#include \"" << elm.init_set.string << "\"" << std::endl;
					out << "\t\t\t};" << std::endl;
					out << "\t\t\tmemcpy(" << propertySectionName  << "." << elm.name << ".ptr, tmp, " << "sizeof(" << elm.tokenType << ") * " << propertySectionName  << "." << elm.name << ".num))" << ";" << std::endl;
					out << "\t\t}" << std::endl;
				}
				else
				{
					if(elm.init_set.values.size() > 0)
					{
						jvxSize numValues = elm.init_set.values.size();
						std::string convertedValue;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttokenType tmp[" << numValues << "] = " << std::endl;
						out << "\t\t\t{" << std::endl << "\t\t\t\t" << std::flush;
						for(k = 0; k < numValues; k++)
						{
							if(k != 0)
							{
								out << ", " << std::flush;
							}
							switch(elm.format)
							{
							case JVX_DATAFORMAT_DATA:
								convertedValue = jvx_data2String(elm.init_set.values[k], 20);
								break;
							case JVX_DATAFORMAT_64BIT_LE:
							case JVX_DATAFORMAT_32BIT_LE:
							case JVX_DATAFORMAT_16BIT_LE:
							case JVX_DATAFORMAT_8BIT:
							case JVX_DATAFORMAT_U64BIT_LE:
							case JVX_DATAFORMAT_U32BIT_LE:
							case JVX_DATAFORMAT_U16BIT_LE:
							case JVX_DATAFORMAT_U8BIT:
							case JVX_DATAFORMAT_BYTE:
							case JVX_DATAFORMAT_SIZE:
								convertedValue = jvx_int2String((int)JVX_DATA2INT32(elm.init_set.values[k]));
								break;
							default:
								convertedValue = "error";
							}
							out << convertedValue << std::flush;
						}

						out << std::endl << "\t\t\t};" << std::endl;
						out << "\t\t\tmemcpy(" << propertySectionName  << "." << elm.name << ".fld, tmp, " <<
							"sizeof(" << elm.tokenType << ") * " << "JVX_MIN(" << numValues << "," << propertySectionName  << "." <<
							elm.name << ".num))" << ";" << std::endl;
						out << "\t\t}" << std::endl;
					}
				}
			}
			else
			{
				// Allocation function
				out << "\t\t" << propertySectionName  << "." << elm.name << ".ptr = new " << elm.tokenType << ";" << std::endl;
				if(
					(elm.format == JVX_DATAFORMAT_SIZE) ||
					(elm.format == JVX_DATAFORMAT_BYTE) ||
					(elm.format == JVX_DATAFORMAT_8BIT) ||
					(elm.format == JVX_DATAFORMAT_16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U8BIT) ||
					(elm.format == JVX_DATAFORMAT_U16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_DATA))
				{
					out << "\t\tmemset(" << propertySectionName  << "." << elm.name << ".ptr, " << "0, sizeof(" << elm.tokenType << "));" << std::endl;
					if(elm.init_set.filename_set)
					{
						out << "\t\t*(" << propertySectionName  << "." << elm.name << ".ptr = " << std::endl;
						out << "#include \"" << elm.init_set.string << "\"" << std::endl;
						out << "\t\t;" << std::endl;
					}
					else
					{
						if(elm.init_set.values.size() > 0)
						{
							out << "\t\t*(" << propertySectionName  << "." << elm.name << ".ptr) = " << elm.init_set.values[0] << ";" << std::endl;
						}
					}
				} // if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
				else
				{
					switch(elm.format)
					{
					case JVX_DATAFORMAT_SELECTION_LIST:

						// Build up selection list first
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.init_selection);" << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.entries.clear();" << std::endl;
						if (
							(!elm.selection.varname.empty()) &&
							(!elm.selection.nummax.empty()))						
						{
							out << "\t\tfor(int ii = 0; ii < (int)" << elm.selection.numprefix << elm.selection.nummax << "; ii++)" << std::endl;
							out << "\t\t{" << std::endl;
							out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" << elm.selection.varname << "[ii]" << 
								elm.selection.varpostfix << "); " << std::endl;
							out << "\t\t}" << std::endl;
						}
						else
						{
							if(!elm.selection.extract_string.empty())
							{
								std::vector<std::string> tokens;
								jvx_parseCommandLineOneToken(elm.selection.extract_string, tokens, ',');
								out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
								out << "\t\t{" << std::endl;
								out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" << elm.translations.tpName 
									<< "_txt(" << "(" << elm.translations.tpName << ")ii));" << std::endl;
								out << "\t\t}" << std::endl;
							}
							else
							{
					
								for (k = 0; k < elm.selection.strings.size(); k++)
								{
									out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(\"" << elm.selection.strings[k] << "\");" << std::endl;
								}
							}
						}

						if(!elm.selection.extract_string.empty())
						{
							out << "\t\t// LOC#6" << std::endl;
							if(!elm.selection.extract_selected.empty())
							{
								out << "\t\tjvx_bitZSet(" << propertySectionName << "." << elm.name << ".value.init_selection, (int)" 
									<< elm.translations.tpName << "::" << elm.selection.extract_selected << ");" << std::endl;							
								out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
								out << "\t\t{" << std::endl;
								out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << ii);" << std::endl;
								out << "\t\t}" << std::endl;										
							}
							else
							{
								out << "\t\t#error Failed to identify initialize selection for expression <" << elm.selection.extract_string << ">." << std::endl;
							}
							
						}
						else
						{
					
							for (k = 0; k < elm.selection.strings.size(); k++)
							{
								if (k < elm.selection.selected.size())
								{
									if (elm.selection.selected[k])
									{
										out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_selection |= (1 << " << k << ");" << std::endl;
									}
								}
								if (k < elm.selection.exclusive.size())
								{
									if (elm.selection.exclusive[k])
									{
										out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << " << k << ");" << std::endl;
									}
								}
								out << "\t\t" << propertySectionName << "." << elm.name << ".value.selectable |= (1 << " << k << ");" << std::endl;
							}
						}
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_selection); " << std::endl;

						break;
					case JVX_DATAFORMAT_STRING:
						if(!elm.init_set.string.empty())
						{
							out << "\t\t*(" << propertySectionName  << "." << elm.name << ".ptr) = \"" << elm.init_set.string << "\";" << std::endl;
						}
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						for(k = 0; k < elm.init_set.strings.size(); k++)
						{
							out << "\t\t*(" << propertySectionName  << "." << elm.name << ".ptr).push_back(\"" << elm.init_set.strings[k] << "\");" << std::endl;
						}
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_value = " << elm.initValueInRange.value << ";" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = " << elm.initValueInRange.minVal << ";" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = " << elm.initValueInRange.maxVal << ";" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_value); " << std::endl;

						break;
					}
				} // else: if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
			}// if(multipleObjects)
		} // if(elm.dynamic)
		else
		{
			if(elm.numElements > 1)
			{
				std::string szToken = jvx_int2String((int)elm.numElements);

				switch (elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
					// Build up selection list first
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.init_selection);" << std::endl;
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.clear();" << std::endl;
					if (
						(!elm.selection.varname.empty()) &&
						(!elm.selection.nummax.empty()))
					{
						out << "\t\tfor(int ii = 0; ii < (int)" << elm.selection.numprefix << elm.selection.nummax << "; ii++)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\t" << propertySectionName << "." << elm.name <<
							".value.entries.push_back(" << elm.selection.varname << "[ii]" <<
							elm.selection.varpostfix << "); " << std::endl;
						out << "\t\t}" << std::endl;
					}
					else
					{
						if(!elm.selection.extract_string.empty())
						{
							out << "\t\t// LOC#7" << std::endl;
							out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
							out << "\t\t{" << std::endl;
							out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" << elm.translations.tpName 
								<< "_txt(" << "(" << elm.translations.tpName << ")ii));" << std::endl;
							out << "\t\t}" << std::endl;
							
						}
						else
						{
					
							for (k = 0; k < elm.selection.strings.size(); k++)
							{
								out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(\"" << elm.selection.strings[k] << "\");" << std::endl;
								if (k < elm.selection.selected.size())
								{
									if (elm.selection.selected[k])
									{
										out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_selection |= (1 << " << k << ");" << std::endl;
									}
								}
								if (k < elm.selection.exclusive.size())
								{
									if (elm.selection.exclusive[k])
									{
										out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << " << k << ");" << std::endl;
									}
								}
								out << "\t\t" << propertySectionName << "." << elm.name << ".value.selectable |= (1 << " << k << ");" << std::endl;
							}
						}
					}
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_selection); " << std::endl;

					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_value = " << elm.initValueInRange.value << ";" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = " << elm.initValueInRange.minVal << ";" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = " << elm.initValueInRange.maxVal << ";" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_value); " << std::endl;
					break;

				default:

					// Allocation and initialization function
					//out << "\t\t" << propertySectionName  << "." << elm.name << ".num = " <<  szToken << ";" << std::endl;
					//out << "\t\t" << propertySectionName  << "." << elm.name << ".ptr = new " << tokenType << "[" <<  szToken << "];" << std::endl;
					//out << "\t\tmemset(" << propertySectionName  << "." << elm.name << ".ptr, " << "0, sizeof(" << tokenType << ") * " << propertySectionName  << "." << elm.name << ".num);" << std::endl;
					if (elm.init_set.filename_set)
					{
						out << "\t\t{" << std::endl;
						out << "\t\t\t" << elm.tokenType << " tmp[" << propertySectionName << "." << elm.name << ".num] = " << std::endl;
						out << "\t\t\t{" << std::endl << "\t\t\t\t" << std::flush;
						out << "#include \"" << elm.init_set.string << "\"" << std::endl;
						out << "\t\t\t};" << std::endl;
						out << "\t\t\tmemcpy(" << propertySectionName << "." << elm.name << ".ptr, tmp, " << "sizeof(" << elm.tokenType << ") * " << propertySectionName << "." << elm.name << ".num))" << ";" << std::endl;
						out << "\t\t}" << std::endl;
					}
					else
					{
						if (elm.init_set.values.size() > 0)
						{
							jvxSize numValues = elm.init_set.values.size();
							std::string convertedValue;
							out << "\t\t{" << std::endl;
							out << "\t\t\t" << elm.tokenType << " tmp[" << numValues << "] = " << std::endl;
							out << "\t\t\t{" << std::endl << "\t\t\t\t" << std::flush;
							for (k = 0; k < numValues; k++)
							{
								if (k != 0)
								{
									out << ", " << std::flush;
								}
								switch (elm.format)
								{
								case JVX_DATAFORMAT_DATA:
									convertedValue = jvx_data2String(elm.init_set.values[k], 20);
									break;
								case JVX_DATAFORMAT_64BIT_LE:
								case JVX_DATAFORMAT_32BIT_LE:
								case JVX_DATAFORMAT_16BIT_LE:
								case JVX_DATAFORMAT_8BIT:
								case JVX_DATAFORMAT_U64BIT_LE:
								case JVX_DATAFORMAT_U32BIT_LE:
								case JVX_DATAFORMAT_U16BIT_LE:
								case JVX_DATAFORMAT_U8BIT:
								case JVX_DATAFORMAT_BYTE:
								case JVX_DATAFORMAT_SIZE:
									convertedValue = jvx_int2String((int)JVX_DATA2INT32(elm.init_set.values[k]));
									break;
								default:
									convertedValue = "error";
								}
								out << convertedValue << std::flush;
							}

							out << std::endl << "\t\t\t};" << std::endl;
							out << "\t\t\tmemcpy(" << propertySectionName << "." << elm.name << ".ptr, tmp, " <<
								"sizeof(" << elm.tokenType << ") * " << "JVX_MIN(" << numValues << "," << propertySectionName << "." <<
								elm.name << ".num))" << ";" << std::endl;
							out << "\t\t}" << std::endl;
						}
					}
					break;
				}
			}
			else
			{
				// Allocation function for single element numeric datatypes
				if(
					(elm.format == JVX_DATAFORMAT_SIZE) ||
					(elm.format == JVX_DATAFORMAT_BYTE) ||
					(elm.format == JVX_DATAFORMAT_8BIT) ||
					(elm.format == JVX_DATAFORMAT_16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U8BIT) ||
					(elm.format == JVX_DATAFORMAT_U16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_DATA))
				{
					if(elm.init_set.filename_set)
					{
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value = " << std::endl;
						out << "#include \"" << elm.init_set.string << "\"" << std::endl;
						out << "\t\t;" << std::endl;
					}
					else
					{
						if(elm.init_set.values.size() > 0)
						{
							out << "\t\t" << propertySectionName  << "." << elm.name << ".value = " << elm.init_set.values[0] << ";" << std::endl;
						}
						else
						{
							out << "\t\t" << propertySectionName  << "." << elm.name << ".value = 0;" << std::endl;
						}
					}
				} // if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
				else
				{
					switch(elm.format)
					{
					case JVX_DATAFORMAT_SELECTION_LIST:

						// Build up selection list first
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.init_selection);" << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
						out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.clear();" << std::endl;
						if (
							(!elm.selection.varname.empty()) &&
							(!elm.selection.nummax.empty()))
						{
							out << "\t\tfor(int ii = 0; ii < (int)" << elm.selection.numprefix << elm.selection.nummax << "; ii++)" << std::endl;
							out << "\t\t{" << std::endl;
							out << "\t\t\t" << propertySectionName << "." << elm.name << 
								".value.entries.push_back(" << elm.selection.varname << "[ii]"<< 
								elm.selection.varpostfix << "); " << std::endl;
							out << "\t\t}" << std::endl;
						}
						else
						{
							if(!elm.selection.extract_string.empty())
							{
								out << "\t\t// LOC#8" << std::endl;
								out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
								out << "\t\t{" << std::endl;
								out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(" << elm.translations.tpName 
									<< "_txt(" << "(" << elm.translations.tpName << ")ii));" << std::endl;
								out << "\t\t}" << std::endl;
								if(!elm.selection.extract_selected.empty())
								{
									out << "\t\tjvx_bitZSet(" << propertySectionName << "." << elm.name << ".value.init_selection, (int)" 
										<< elm.translations.tpName << "::" << elm.selection.extract_selected << ");" << std::endl;							
									out << "\t\tfor(int ii = 0; ii < (int)" << elm.translations.tpName << "::" << elm.selection.extract_string << "; ii++)" << std::endl;
									out << "\t\t{" << std::endl;
									out << "\t\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << ii);" << std::endl;
									out << "\t\t}" << std::endl;	
								}
								else
								{
									out << "\t\t#error Failed to identify initialize selection for expression <" << elm.selection.extract_string << ">." << std::endl;
								}
								
							}
							else
							{
					
								for (k = 0; k < elm.selection.strings.size(); k++)
								{
									out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.push_back(\"" << elm.selection.strings[k] << "\");" << std::endl;
									if (k < elm.selection.selected.size())
									{
										if (elm.selection.selected[k])
										{
											out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_selection |= (1 << " << k << ");" << std::endl;
										}
									}
									if (k < elm.selection.exclusive.size())
									{
										if (elm.selection.exclusive[k])
										{
											out << "\t\t" << propertySectionName << "." << elm.name << ".value.exclusive |= (1 << " << k << ");" << std::endl;
										}
									}
									out << "\t\t" << propertySectionName << "." << elm.name << ".value.selectable |= (1 << " << k << ");" << std::endl;
								}
							}
						}
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_selection); " << std::endl;
						break;
					case JVX_DATAFORMAT_STRING:
						if(!elm.installable)
						{
							if(!elm.init_set.string.empty())
							{
								out << "\t\t" << propertySectionName  << "." << elm.name << ".value = \"" << elm.init_set.string << "\";" << std::endl;
							}
						}
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						for(k = 0; k < elm.init_set.strings.size(); k++)
						{
							out << "\t\t" << propertySectionName  << "." << elm.name << ".value.push_back(\"" << elm.init_set.strings[k] << "\");" << std::endl;
						}
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.init_value = " << elm.initValueInRange.value << ";" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.minVal = " << elm.initValueInRange.minVal << ";" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.maxVal = " << elm.initValueInRange.maxVal << ";" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.set_all(" << propertySectionName << "." << elm.name << ".value.init_value); " << std::endl;

						break;
					case JVX_DATAFORMAT_HANDLE:
					case JVX_DATAFORMAT_CALLBACK:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value = nullptr;" << std::endl;
						break;
					case JVX_DATAFORMAT_INTERFACE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".ptr = nullptr;" << std::endl;
						break;
					}
				} // else: if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
			}// if(multipleObjects)
		} // if(elm.dynamic)
		if(elm.validOnInit)
		{
			out << "\t\t" << propertySectionName  << "." << elm.name << ".isValid = true;" << std::endl;
		}
		else
		{
			out << "\t\t" << propertySectionName  << "." << elm.name << ".isValid = false;" << std::endl;
		}

		out << "\t\t" << propertySectionName << "." << elm.name << ".accessFlags = JVX_COMBINE_FLAGS_RCWD(" <<
			elm.access_rights_read << "," <<
			elm.access_rights_write << "," <<
			elm.access_rights_create << "," <<
			elm.access_rights_destroy <<
			");" << std::endl;
		out << "\t\t" << propertySectionName << "." << elm.name << ".configModeFlags = " << elm.config_mode << ";" << std::endl;

		if(elm.onlySelectionToConfig)
		{
			out << "\t\t" << propertySectionName  << "." << elm.name << ".onlySelectionToFile = true;" << std::endl;
		}
		else
		{
			out << "\t\t" << propertySectionName  << "." << elm.name << ".onlySelectionToFile = false;" << std::endl;
		}

	}
}


void
textProcessor_core::produceOutput_c_deallocate(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName)
{
	std::string tokenType = "unknown";
	std::string tokenTypeName = "JVX_DATAFORMAT_NONE";


	if(elm.associateExternal)
	{
		// Nothing to do
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			if(multipleObjects)
			{
				// Deallocation function
				out << "\t\tdelete[](" << propertySectionName  << "." << elm.name << ".ptr);" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".ptr = NULL;" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".num = 0;" << std::endl;
			}
			else
			{
				// Deallocation function
				out << "\t\tdelete(" << propertySectionName  << "." << elm.name << ".ptr);" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".ptr = NULL;" << std::endl;
			}
		}
		else
		{
			if(elm.numElements > 1)
			{
				// Nothing to do here
				switch (elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.selectable);" << std::endl;
					out << "\t\tjvx_bitFClear(" << propertySectionName << "." << elm.name << ".value.exclusive);" << std::endl;					out << "\t\t" << propertySectionName << "." << elm.name << ".value.entries.clear();" << std::endl;
					break;
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = 0;" << std::endl;
					out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = 0;" << std::endl;
					break;
				}
			}
			else
			{
				// Allocation function for single element numeric datatypes
				if(
					(elm.format == JVX_DATAFORMAT_SIZE) ||
					(elm.format == JVX_DATAFORMAT_BYTE) ||
					(elm.format == JVX_DATAFORMAT_8BIT) ||
					(elm.format == JVX_DATAFORMAT_16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U8BIT) ||
					(elm.format == JVX_DATAFORMAT_U16BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U32BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_U64BIT_LE) ||
					(elm.format == JVX_DATAFORMAT_DATA))
				{
					// Nothing to do here
				} // if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
				else
				{
					switch(elm.format)
					{
					case JVX_DATAFORMAT_SELECTION_LIST:

						// Build up selection list first
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.exclusive = 0;" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.selectable = 0 ;" << std::endl;
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.entries.clear();" << std::endl;
						break;
					case JVX_DATAFORMAT_STRING:
						if(!elm.installable)
						{
							out << "\t\t" << propertySectionName  << "." << elm.name << ".value = \"" << elm.init_set.string << "\";" << std::endl;
						}
						break;
					case JVX_DATAFORMAT_STRING_LIST:
						out << "\t\t" << propertySectionName  << "." << elm.name << ".value.clear();" << std::endl;
						break;
					case JVX_DATAFORMAT_VALUE_IN_RANGE:
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.minVal = 0;" << std::endl;
						out << "\t\t" << propertySectionName << "." << elm.name << ".value.maxVal = 0;" << std::endl;
						break;
					case JVX_DATAFORMAT_HANDLE:
						break;
					}
				} // else: if((elm.format == JVX_DATAFORMAT_8BIT) ||(elm.format == JVX_DATAFORMAT_16BIT_LE) ||(elm.format == JVX_DATAFORMAT_32BIT_LE) ||(elm.format == JVX_DATAFORMAT_64BIT_LE) ||(elm.format == JVX_DATAFORMAT_DATA) ||(elm.format == JVX_DATAFORMAT_DATA))
			}// if(multipleObjects)
		}				
	}
	out << "\t\t" << propertySectionName  << "." << elm.name << ".reset();" << std::endl;
}

void
textProcessor_core::produceOutput_c_associate(std::ostream& out, std::ostream& out_inner, bool& isFirstAssociate, onePropertyDefinition& elm, std::string& propertySectionName, std::string& funcSectionName)
{	
	if(
		!(
			(elm.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER)||
			(elm.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER)||
			(elm.decoderType == JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG)||
			(elm.decoderType == JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE)))
	{
		if(
			(elm.associateExternal) && (!elm.installable))
		{
			
			// Association function to provide data from extern
			if (isFirstAssociate)
			{
				out << "\tinline void associate__" << funcSectionName << "(CjvxProperties* theProps, " << std::flush;
				if (elm.associateNumberEntries)
				{
					out << elm.tokenTypeAss << "* ptrExt_" << elm.name << ", jvxSize" << " num_" << elm.name << std::flush;
				}
				else
				{
					out << elm.tokenTypeAss << "* ptrExt_" << elm.name << std::flush;
				}
			}
			else
			{
				if (elm.associateNumberEntries)
				{
					out << ", " << elm.tokenTypeAss << "* ptrExt_" << elm.name << ", jvxSize" << " num_" << elm.name << std::flush;
				}
				else
				{
					out << ", " << elm.tokenTypeAss << "* ptrExt_" << elm.name << std::flush;
				}
			}
			out_inner << "\t\t// " << elm.description << std::endl;
			if (elm.associateNumberEntries)
			{
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".num = " " num_" << elm.name << ";" << std::endl;
			}
			else
			{
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".num = 1;" << std::endl;
			}
			isFirstAssociate = false;

			switch (elm.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".value.assign(ptrExt_" << elm.name << ", num_" << elm.name << "); " << std::endl;
				if (elm.updateOnAssociate)
				{
					out_inner << "\t\ttheProps->_update_property_size(" << propertySectionName << "." <<
						elm.name << ".num, " 
						<< propertySectionName << "." << elm.name << ".category, " 
						<< propertySectionName << "." << elm.name << ".globalIdx, false);" << std::endl;
				}
				break;
			default:
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".ptr = ptrExt_" << elm.name << ";" << std::endl;
				if (elm.updateOnAssociate)
				{
					out_inner << "\t\ttheProps->_update_property(" << propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
						elm.name << ".num , " 
						<< propertySectionName << "." << elm.name << ".category, "
						<< propertySectionName << "." << elm.name << ".globalIdx, false);" << std::endl;
				}
			}

			if (elm.validOnInit)
			{
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".isValid = true;" << std::endl;
			}
			else
			{
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".isValid = false;" << std::endl;
			}
		}
	}
}

void
textProcessor_core::produceOutput_c_deassociate(std::ostream& out, std::ostream& out_inner, bool& isFirstDeassociate, onePropertyDefinition& elm, std::string& propertySectionName, std::string& funcSectionName)
{
	if(
		!(
			(elm.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER)||
			(elm.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER)||
			(elm.decoderType == JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG)||
			(elm.decoderType == JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE)))
	{
		if((elm.associateExternal)&&(!elm.installable))
		{
			if(isFirstDeassociate)
			{
				out << "\tinline void deassociate__" << funcSectionName << "(CjvxProperties* theProps" << std::flush;
				isFirstDeassociate = false;
			}

			out_inner << "\t\t// " << elm.description << std::endl;
			out_inner << "\t\t" << propertySectionName  << "." << elm.name << ".isValid = false;" << std::endl;
			
			switch (elm.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".value.assign(nullptr, 0); " << std::endl;
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".num = 0;" << std::endl;
				out_inner << "\t\ttheProps->_update_property_size(" << propertySectionName << "." <<
					elm.name << ".num, " << propertySectionName << "." << elm.name << ".category, " << propertySectionName << "." << elm.name << ".globalIdx, false);" << std::endl;
				break;

			default:
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".num = 0;" << std::endl;
				out_inner << "\t\t" << propertySectionName << "." << elm.name << ".ptr = NULL;" << std::endl;

				out_inner << "\t\ttheProps->_update_property(" << propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
					elm.name << ".num, " << propertySectionName << "." << elm.name << ".category, " << propertySectionName << "." << elm.name << ".globalIdx, false);" << std::endl;
			}
		}
	}
}
void
textProcessor_core::produceOutput_c_register(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName, std::vector<std::string>& prfxList, std::vector<oneCallback>& allCallbacks)
{
	//std::string addArg1 = ", JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE";
	//std::string addArg2 = ", JVX_PROPERTY_DECODER_NONE";
	//std::string argCtxt = ", JVX_PROPERTY_CONTEXT_UNKNOWN";
	std::string referenceToken = "";
	std::string prefix = prefixListToString(prfxList, "/");
	std::vector<std::string> tmpLst;
	std::vector<oneCallback>::reverse_iterator elmCb;
	jvxBool stopProcess = false;
	std::string rawPointerType;	
			
	// addArg1 = std::string(", ") + jvxPropertyAccessType_str[elm.accessType].full;
	// addArg2 = std::string(", ") + jvxPropertyDecoderHintType_str[elm.decoderType].full;
	switch (elm.format)
	{
	case JVX_DATAFORMAT_CALLBACK:
		referenceToken = "&";
		break;
	default:
		switch (elm.decoderType)
		{
		case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
		case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
		case JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG:
		case JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE:
			referenceToken = "&";
			break;

		case JVX_PROPERTY_DECODER_SIMPLE_ONOFF:
		case JVX_PROPERTY_DECODER_VALUE_OR_DONTCARE:

			if (!elm.associateExternal)
			{
				// Could be INT16 (BOOL) or selection list
				// if(elm.format = JVX_DATAFORMAT_SELECTION_LIST)		
				referenceToken = "&";
			}

			break;

		default:
			break;
		}
	}
	
	// argCtxt = std::string(", ") + jvxPropertyContext_str[elm.contxt].full;

	// ===================================================================

	std::string callback_set = elm.callback_set;

	// Propagate up in property file tree
	tmpLst = prfxList;
	stopProcess = false;

	if (callback_set.size() >= 2)
	{
		if (callback_set.substr(0, 2) == "//")
		{
			callback_set = callback_set.substr(2, std::string::npos);
			for (elmCb = allCallbacks.rbegin(); elmCb != allCallbacks.rend(); elmCb++)
			{
				if (elmCb->theName == callback_set)
				{
					callback_set = "/" + prefixListToString(elmCb->thePrefix, "/") + callback_set;
					stopProcess = true;
					break;
				}
			}
		}
	}

	if (!stopProcess)
	{
		while (1)
		{
			if (callback_set.size() >= 3)
			{
				if (callback_set.substr(0, 3) == "../")
				{
					if (tmpLst.size())
					{
						tmpLst.pop_back();
					}
					callback_set = callback_set.substr(3, std::string::npos);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (callback_set.size())
		{
			if (callback_set[0] != '/')
			{
				callback_set = "/" + prefixListToString(tmpLst, "/") + callback_set;
			}
		}
	}

	// ===================================================================
	
	std::string callback_set_pre = elm.callback_set_pre;

	// Propagate up in property file tree
	tmpLst = prfxList;
	stopProcess = false;

	if (callback_set_pre.size() >= 2)
	{
		if (callback_set_pre.substr(0, 2) == "//")
		{
			callback_set_pre = callback_set_pre.substr(2, std::string::npos);
			for (elmCb = allCallbacks.rbegin(); elmCb != allCallbacks.rend(); elmCb++)
			{
				if (elmCb->theName == callback_set_pre)
				{
					callback_set_pre = "/" + prefixListToString(elmCb->thePrefix, "/") + callback_set_pre;
					stopProcess = true;
					break;
				}
			}
		}
	}

	if (!stopProcess)
	{
		while (1)
		{
			if (callback_set_pre.size() >= 3)
			{
				if (callback_set_pre.substr(0, 3) == "../")
				{
					if (tmpLst.size())
					{
						tmpLst.pop_back();
					}
					callback_set_pre = callback_set_pre.substr(3, std::string::npos);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (callback_set_pre.size())
		{
			if (callback_set_pre[0] != '/')
			{
				callback_set_pre = "/" + prefixListToString(tmpLst, "/") + callback_set_pre;
			}
		}
	}
		
	// ===================================================================
	
	std::string callback_get = elm.callback_get;
	
	// Propagate up in property file tree
	tmpLst = prfxList;
	stopProcess = false;

	if (callback_get.size() >= 2)
	{
		if (callback_get.substr(0, 2) == "//")
		{
			callback_get = callback_get.substr(2, std::string::npos);
			for (elmCb = allCallbacks.rbegin(); elmCb != allCallbacks.rend(); elmCb++)
			{
				if (elmCb->theName == callback_get)
				{
					callback_get = "/" + prefixListToString(elmCb->thePrefix, "/") + callback_get;
					stopProcess = true;
					break;
				}
			}
		}
	}

	if (!stopProcess)
	{
		while (1)
		{
			if (callback_get.size() >= 3)
			{
				if (callback_get.substr(0, 3) == "../")
				{
					if (tmpLst.size())
					{
						tmpLst.pop_back();
					}
					callback_get = callback_get.substr(3, std::string::npos);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (callback_get.size())
		{
			if (callback_get[0] != '/')
			{
				callback_get = "/" + prefixListToString(tmpLst, "/") + callback_get;
			}
		}
	}

	// ===================================================================
	
	std::string callback_get_post = elm.callback_get_post;
	
	// Propagate up in property file tree
	tmpLst = prfxList;
	stopProcess = false;

	if (callback_get_post.size() >= 2)
	{
		if (callback_get_post.substr(0, 2) == "//")
		{
			callback_get_post = callback_get_post.substr(2, std::string::npos);
			for (elmCb = allCallbacks.rbegin(); elmCb != allCallbacks.rend(); elmCb++)
			{
				if (elmCb->theName == callback_get_post)
				{
					callback_get_post = "/" + prefixListToString(elmCb->thePrefix, "/") + callback_get_post;
					stopProcess = true;
					break;
				}
			}
		}
	}

	if (!stopProcess)
	{
		while (1)
		{
			if (callback_get_post.size() >= 3)
			{
				if (callback_get_post.substr(0, 3) == "../")
				{
					if (tmpLst.size())
					{
						tmpLst.pop_back();
					}
					callback_get_post = callback_get_post.substr(3, std::string::npos);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (callback_get_post.size())
		{
			if (callback_get_post[0] != '/')
			{
				callback_get_post = "/" + prefixListToString(tmpLst, "/") + callback_get_post;
			}
		}
	}
	
	out << "\n\t\t//===========================================================================" << std::endl;
	out << "\t\t// Generating code for property " << propertySectionName << "." << elm.name << std::endl;
	out << "\t\t//===========================================================================" << std::endl << std::endl;

	// Reset the description
	out << "\t\t_r.reset();" << std::endl;
	out << "\t\t_c.reset();" << std::endl;
	
	if(prefix.empty())
	{
		out << "\t\t" << propertySectionName << "." << elm.name << ".descriptor = " << "jvx_makePathExpr(reg_prefix, " <<
			propertySectionName << "." << elm.name << ".descriptor.std_str());" << std::endl;
		out << "\t\t" << "if(v_register)\n\t\t\t std::cout << \"Registered property <\" << " << 
			propertySectionName << "." << elm.name << ".descriptor.std_str() << \">.\" << std::endl;" << std::endl;	
	}
	else
	{
		out << "\t\t" << propertySectionName << "." << elm.name << ".descriptor = " << "jvx_makePathExpr(reg_prefix, (\"" << 
			"/" << prefix << "\" + " << propertySectionName << "." << elm.name << ".descriptor.std_str()));" << std::endl;
		out << "\t\t" << "if(v_register)\n\t\t\t std::cout << \"Registered property <\" << " << 
			propertySectionName << "." << elm.name << ".descriptor.std_str() << \">.\" << std::endl;" << std::endl;	
	}

	out << "\n\t\t//==========================================================" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = " << jvx_size2String(elm.id, true) << ";" << std::endl;
	out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_str[elm.category].full << ";" << std::endl;
	out << "\t\tif(force_unspecific)" << std::endl;
	out << "\t\t{" << std::endl;
	out << "\t\t\t" << propertySectionName << "." << elm.name << ".globalIdx = JVX_SIZE_UNSELECTED;" << std::endl;
	out << "\t\t\t" << propertySectionName  << "." << elm.name << ".category = JVX_PROPERTY_CATEGORY_UNSPECIFIC;" << std::endl;
	out << "\t\t}" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".format = " << elm.tokenTypeName << ";" << std::endl << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".allowedStateMask = " << "0x" << std::uppercase << std::hex 
		<< elm.allowStateMask << ";" << std::dec << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".allowedThreadingMask = " << "0x" << std::uppercase << std::hex 
		<< elm.allowThreadingMask  << std::dec << ";" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".accessType = " << jvxPropertyAccessType_str[elm.accessType].full << ";" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".decTp = " << jvxPropertyDecoderHintType_str[elm.decoderType].full << ";" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".ctxt = " << jvxPropertyContext_str[elm.contxt].full << ";" << std::endl << std::endl;
	out << "\t\t_c.callback_set_posthook = \"" << callback_set << "\";" << std::endl;
	out << "\t\t_c.callback_get_prehook = \"" << callback_get << "\";" << std::endl;
	out << "\t\t_c.callback_set_prehook = \"" << callback_set_pre << "\";" << std::endl;
	out << "\t\t_c.callback_get_posthook = \"" << callback_get_post << "\";" << std::endl << std::endl;

	if(elm.associateExternal)
	{
/*
		if(elm.numElements > 1)
		{
			// Register property function
			out << "\t\t" << propertySectionName << "." << elm.name << ".id = " << elm.id << ";" << std::endl;
			out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_txt(elm.category) << ";" << std::endl;


			out << "\t\ttheProps->_register_property(" << propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
				elm.name << ".num , " << tokenTypeName << ", " << propertySectionName  << "." << elm.name << ".cat, &" <<
				propertySectionName << "." << elm.name << ".id, " <<
				"0x" << std::uppercase << std::hex << elm.allowStateMask << ", " << "0x" << std::uppercase << std::hex << elm.allowThreadingMask  <<
				std::dec << ", \"" << elm.description << "\""  << ", \"" << elm.descriptor << "\"" << ", &" << propertySectionName  << "." << elm.name << ".valid" <<
				", &" << propertySectionName  << "." << elm.name << ".frozen" << addArg1 << addArg2 << ");" << std::endl;
		}
		else
		{
			// Register property function
			out << "\t\t" << propertySectionName << "." << elm.name << ".id = " << elm.id << ";" << std::endl;
			out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_txt(elm.category) << ";" << std::endl;

			out << "\t\ttheProps->_register_property(" << propertySectionName << "." << elm.name << ".ptr, 1, " << tokenTypeName << ", " << propertySectionName  << "." << elm.name << ".cat, &" <<
				propertySectionName << "."  << elm.name << ".id, " <<
				"0x" << std::uppercase << std::hex <<elm.allowStateMask << ", " << "0x" << elm.allowThreadingMask << std::dec << ", \"" << elm.description << "\"" << ", \"" << elm.descriptor << "\"" <<
				", &" << propertySectionName  << "." << elm.name << ".valid" <<
				", &" << propertySectionName  << "." << elm.name << ".frozen" <<
				addArg1 << addArg2 << ");" << std::endl;
		}
		*/
		
		if(elm.installable)
		{
			switch (elm.format)
			{
			case JVX_DATAFORMAT_CALLBACK:
				rawPointerType = "CjvxRawPointerTypeCallbackPointer";
				out << "\t\t" << propertySectionName << "." << elm.name << ".num = 1;" << std::endl;
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister," << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName << "." << elm.name << ".value));" << std::endl;
				out << "\t\t_r.fld_prop_cb = " << referenceToken << propertySectionName
					<< "." << elm.name << ".value;" << std::endl;
				out << "\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&" << propertySectionName << "."
					<< elm.name << "), _r, _c, " << std::endl;
				out << "\t\t\t" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister);" << std::endl;
				out << "\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " <<
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
				break;
			default:
				rawPointerType = "CjvxRawPointerTypeExternalPointer";
				rawPointerType += "<" + elm.tokenType + ">";


				out << "\t\t" << propertySectionName << "." << elm.name << ".num = 1;" << std::endl;
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister," << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName << "." << elm.name << ".ptr, " << std::endl;
				out << "\t\t\t" << propertySectionName << "." << elm.name << ".format));" << std::endl;
				out << "\t\t_r.fld_ext = reinterpret_cast<jvxHandle**>(" << referenceToken << propertySectionName
					<< "." << elm.name << ".ptr);" << std::endl;
				out << "\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&" << propertySectionName << "."
					<< elm.name << "), _r, _c, " << std::endl;
				out << "\t\t\t" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister);" << std::endl;
				out << "\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " <<
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
			}
			//out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;			
		}
		else
		{
			if(elm.singleElementField)
			{
				rawPointerType = "CjvxRawPointerTypeObject";
			}
			else
			{
				switch (elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
				case JVX_DATAFORMAT_VALUE_IN_RANGE:
					rawPointerType = "CjvxRawPointerTypeObjectField";
					referenceToken.clear();
					break;
				default:
					rawPointerType = "CjvxRawPointerTypeField";
				}
			}
			rawPointerType += "<" + elm.tokenType + ">";
			
			switch (elm.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister, " << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName
					<< "." << elm.name << ".value" << std::flush;
				out << "));" << std::endl;
				out << "\t\t_r.fld = &" << referenceToken << propertySectionName
					<< "." << elm.name << ".value;" << std::endl;
				break;
			default:
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name
					<< ".rawPtrRegister, " << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName
					<< "." << elm.name << ".ptr" << std::flush;
				if (elm.singleElementField)
				{
					out << "));" << std::endl;
				}
				else
				{
					out << ", " << propertySectionName << "." << elm.name << ".num));" << std::endl;
				}
				out << "\t\t_r.fld = " << referenceToken << propertySectionName
					<< "." << elm.name << ".ptr;" << std::endl;
				break;
			}
			out << "\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&"
				<< propertySectionName << "." << elm.name << "), _r, _c, " << std::endl;
			out << "\t\t\t" << propertySectionName << "." << elm.name
				<< ".rawPtrRegister);" << std::endl;
			out << "\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " <<
				propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
			//out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;
		}
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			/*
			if(multipleObjects)
			{
				rawPointerType = "CjvxRawPointerTypeFieldMulti";
				rawPointerType += "<" + tokenType + ">";
				*/	
				/*
				// Register property function
				out << "\t\t" << propertySectionName << "." << elm.name << ".id = " <<  jvx_size2String(elm.id, true) << ";" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_str[elm.category].full << ";" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".format = " << tokenTypeName << ";" << std::endl;
				*/
				// out << "\t\t" << propertySectionName << "." << elm.name << ".num = " << propertySectionName  << "." << elm.name << ".num;" << std::endl;
				out << "\t\tif(" << propertySectionName << "." << elm.name << ".num)" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name 
				<< ".rawPtrRegister, " << std::endl;
				out << "\t\t\tx::propertyRawPointerType::" << rawPointerType << "("
				<< referenceToken << propertySectionName 
				<< "." << elm.name << ".ptr, " << propertySectionName << "." << elm.name << ".num));" << std::endl;
				out << "\t\t_r.fld = " << referenceToken << propertySectionName 
							<< "." << elm.name << ".ptr;" << std::endl;
				out << "\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&" 
					<< propertySectionName << "." << elm.name << "), _r, _c, " << std::endl;
				out << "\t\t\t" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister);" << std::endl;
				out << "\t\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\telse" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\t\tstd::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: Trying to register field with size <0>.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;

				//out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;
			/*
			}
			else
			{
				rawPointerType = "CjvxRawPointerTypeFieldMulti";
				rawPointerType += "<" + tokenType + ">";
				/ *
				// Register property function
				out << "\t\t" << propertySectionName << "." << elm.name << ".id = " <<  jvx_size2String(elm.id, true) << ";" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_str[elm.category].full << ";" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".format = " << tokenTypeName << ";" << std::endl;
				* /
				out << "\t\t" << propertySectionName << "." << elm.name << ".num = 1;" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\tJVX_DSP_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister, jvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName 
					<< "." << elm.name << ".ptr, " << propertySectionName << "." << elm.name << ".num));" << std::endl;
				out << "\t\t\t_r.fld = " << referenceToken << propertySectionName 
							<< "." << elm.name << ".ptr;" << std::endl;
				out << "\t\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFull*>(&" 
					<< propertySectionName << "." << elm.name << "), _r, _c, " 
					<< "static_cast<jvx::propertyRawPointerType::IjvxRawPointerType*>(&ptrRaw));" << std::endl;
				out << "\t\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;
				//out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;

			}
			*/
		}
		else
		{
			if(elm.numElements > 1)
			{
				rawPointerType = "CjvxRawPointerTypeField";
				rawPointerType += "<" + elm.tokenType + ">";
				
				/*
				out << "\t\t" << propertySectionName << "." << elm.name << ".id = " <<  jvx_size2String(elm.id, true) << ";" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_str[elm.category].full << ";" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".format = " << tokenTypeName << ";" << std::endl;
				*/
				//out << "\t\t" << propertySectionName << "." << elm.name << ".num = " << propertySectionName << "." << elm.name << ".num;" << std::endl;
				out << "\t\tif(" << propertySectionName << "." << elm.name << ".num)" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister, " << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName 
					<< "." << elm.name << ".ptr, " << propertySectionName << "." << elm.name << ".num));" << std::endl;
				out << "\t\t\t_r.fld = " << referenceToken << propertySectionName 
							<< "." << elm.name << ".ptr;" << std::endl;
				out << "\t\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&" 
					<< propertySectionName << "." << elm.name << "), _r, _c, " << std::endl;
				out << "\t\t\t\t" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister);" << std::endl;
				out << "\t\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\telse" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\t\tstd::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: Trying to register field with size <0>.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;
				//out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;
			}
			else
			{
				rawPointerType = "CjvxRawPointerTypeObject";
				rawPointerType += "<" + elm.tokenType + ">";
				referenceToken = "&";
				/*
				// Register property function
				out << "\t\t" << propertySectionName << "." << elm.name << ".id = " <<  jvx_size2String(elm.id, true) << ";" << std::endl;
				out << "\t\t" << propertySectionName  << "." << elm.name << ".category = " << jvxPropertyCategoryType_str[elm.category].full << ";" << std::endl;
				out << "\t\t" << propertySectionName << "." << elm.name << ".format = " << tokenTypeName << ";" << std::endl;
				*/
				out << "\t\t" << propertySectionName << "." << elm.name << ".num = 1;" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\tJVX_SAFE_ALLOCATE_OBJECT(" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister, " << std::endl;
				out << "\t\t\tjvx::propertyRawPointerType::" << rawPointerType << "("
					<< referenceToken << propertySectionName 
					<< "." << elm.name << ".value));" << std::endl;					
				out << "\t\t\t_r.fld = " << referenceToken << propertySectionName << "." << elm.name << ".value;" << std::endl;
				out << "\t\t\tresL = theProps->_register_property(static_cast<jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus*>(&" 
					<< propertySectionName << "." << elm.name << "), _r, _c, " << std::endl;
				out << "\t\t\t\t" << propertySectionName << "." << elm.name 
					<< ".rawPtrRegister);" << std::endl;
				out << "\t\t\tif(resL != JVX_NO_ERROR) std::cout << \"Failed to register property with descriptor <\" << " << 
					propertySectionName << "." << elm.name << ".descriptor.std_str()" << " << \">, reason: \" << jvxErrorType_descr(resL) << \">.\" << std::endl;" << std::endl;
				out << "\t\t}" << std::endl;
				// out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = _d.globalIdx;" << std::endl;
			}
		}
	}
	if (elm.showhint.set)
	{
		out << "\t\t{" << std::endl;
		out << "\t\t\tstd::string str_showhint = \"" << elm.showhint.content << "\";" << std::endl;
		out << "\t\t\tjvxApiString jstr_showhint;" << std::endl;
		out << "\t\t\tjstr_showhint.assign_const(str_showhint.c_str(), str_showhint.size());" << std::endl;
		out << "\t\t\ttheProps->_set_property_extended_info(&jstr_showhint, JVX_PROPERTY_INFO_SHOWHINT, " << propertySectionName << "." << elm.name << ".globalIdx, " <<
			propertySectionName << "." << elm.name << ".category);" << std::endl;
		out << "\t\t}" << std::endl;
	}
	if (elm.moreinfo.set)
	{
		out << "\t\t{" << std::endl;
		out << "\t\t\tstd::string str_moreinfo = \"" << elm.moreinfo.content << "\";" << std::endl;
		out << "\t\t\tjvxApiString jstr_moreinfo;" << std::endl;
		out << "\t\t\tjstr_moreinfo.assign_const(str_moreinfo.c_str(), str_moreinfo.size());" << std::endl;
		out << "\t\t\ttheProps->_set_property_extended_info(&jstr_moreinfo, JVX_PROPERTY_INFO_MOREINFO, " << propertySectionName << "." << elm.name << ".globalIdx, " <<
			propertySectionName << "." << elm.name << ".category);" << std::endl;
		out << "\t\t}" << std::endl;
	}
	if (!elm.updateContextToken.empty())
	{
		out << "\t\t{" << std::endl;
		out << "\t\t\tstd::string str_update_content_token = \"" << elm.updateContextToken << "\";" << std::endl;
		out << "\t\t\tjvxApiString jstr_update_content_token;" << std::endl;
		out << "\t\t\tjstr_update_content_token.assign_const(str_update_content_token.c_str(), str_update_content_token.size());" << std::endl;
		out << "\t\t\ttheProps->_set_property_extended_info(&jstr_update_content_token, JVX_PROPERTY_INFO_UPDATE_CONTENT_TOKEN, " 
			<< propertySectionName << "." << elm.name << ".globalIdx, " <<
			propertySectionName << "." << elm.name << ".category);" << std::endl;
		out << "\t\t}" << std::endl;
	}
	if (elm.origin.set)
	{
		std::string origin = jvx_replaceSpecialCharactersBeforeWriteSystem(elm.origin.content);
		out << "#ifndef JVX_SUPPRESS_PROPERTIES_ORIGIN" << std::endl;
		out << "\t\t{" << std::endl;
		out << "\t\t\tstd::string str_origin = \"" << origin << "\";" << std::endl;
		out << "\t\t\tjvxApiString jstr_origin;" << std::endl;
		out << "\t\t\tjstr_origin.assign_const(str_origin.c_str(), str_origin.size());" << std::endl;
		out << "\t\t\ttheProps->_set_property_extended_info(&jstr_origin, JVX_PROPERTY_INFO_ORIGIN, " << propertySectionName << "." << elm.name << ".globalIdx, " <<
			propertySectionName << "." << elm.name << ".category);" << std::endl;
		out << "\t\t}" << std::endl;
		out << "#endif // JVX_SUPPRESS_PROPERTIES_ORIGIN" << std::endl;
	}
	if (JVX_CHECK_SIZE_SELECTED(elm.groupid))
	{
		out << "#ifndef JVX_SUPPRESS_PROPERTIES_GROUP_ID" << std::endl;
		out << "\t\t{" << std::endl;
		out << "\t\t\tjvxSize groupid = " << elm.groupid << ";" << std::endl;
		out << "\t\t\ttheProps->_set_property_extended_info(&groupid, JVX_PROPERTY_INFO_GROUPID, " << propertySectionName << "." << elm.name << ".globalIdx, " <<
			propertySectionName << "." << elm.name << ".category);" << std::endl;
		out << "\t\t}" << std::endl;
		out << "#endif // JVX_SUPPRESS_PROPERTIES_GROUP_ID" << std::endl;
	}
}

void
textProcessor_core::produceOutput_c_unregister(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName)
{

	std::string catTypeName;
	switch(elm.category)
	{
	case JVX_PROPERTY_CATEGORY_PREDEFINED:
		catTypeName =  "JVX_PROPERTY_CATEGORY_PREDEFINED";
		break;
	case JVX_PROPERTY_CATEGORY_UNSPECIFIC:
		catTypeName =  "JVX_PROPERTY_CATEGORY_UNSPECIFIC";
		break;
	default:
		assert(0);
	}
	
	if(elm.associateExternal)
	{
		/*
		if(elm.numElements > 1)
		{
			out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".cat , " <<
						propertySectionName << "." << elm.name << ".id);" << std::endl;

		}
		else
		{
			out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".cat , " << propertySectionName << "."  << elm.name << ".id" << ");" << std::endl;
		}
		*/
			out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".category , " <<  propertySectionName << "." << elm.name << ".globalIdx);" << std::endl;
	}
	else
	{
		if(elm.dynamic)
		{
			// Variable initialization
			std::string szToken = jvx_int2String((int)elm.numElements);
			bool multipleObjects = false;

			// Check wether reference was used
			if(elm.reference.set)
			{
				if(elm.reference.key == VALUE)
				{
					szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
					multipleObjects = true;
				}
			}

			if(!multipleObjects)
			{
				if(elm.numElements > 1)
				{
					multipleObjects = true;
				}
			}

			if(multipleObjects)
			{
				out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".category , " << elm.name << ".globalIdx" << ");" << std::endl;

			}
			else
			{
				out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".category , " << propertySectionName << "."  << elm.name << ".globalIdx" << ");" << std::endl;
			}
		}
		else
		{
			if(elm.numElements > 1)
			{
				out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".category , " << propertySectionName << "." << elm.name << ".globalIdx" << ");" << std::endl;
			}
			else
			{
				out << "\t\ttheProps->_unregister_property(" << propertySectionName << "." << elm.name << ".category , " << propertySectionName << "." << elm.name << ".globalIdx" << ");" << std::endl;
			}
		}
	}
	out << "\t\tJVX_SAFE_DELETE_OBJECT(" << propertySectionName << "." << elm.name << ".rawPtrRegister);" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".format = JVX_DATAFORMAT_NONE;" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".globalIdx = JVX_SIZE_UNSELECTED;" << std::endl;
	out << "\t\t" << propertySectionName << "." << elm.name << ".category = JVX_PROPERTY_CATEGORY_UNKNOWN;" << std::endl;
}

void
textProcessor_core::produceOutput_c_readconfig(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName)
{
	std::string commandReadConfigSingle = "unknown";
	std::string commandReadConfigMultiple = "unknown";
	std::string addArgument = "";
	std::string argReconfigure = "";
	std::string commandReadTypecast = "";
	std::string endArgument = "";
	
	if(elm.generateConfigFileEntry)
	{
		switch(elm.format)
		{
		case JVX_DATAFORMAT_DATA:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxData>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxData>";
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxInt64>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxInt64>";
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxInt32>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxInt32>";
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxInt16>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxInt16>";
			if(elm.decoderType == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxInt16>";
				commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxInt16>";
				commandReadTypecast = "(jvxInt16*)";
			}
			break;
		case JVX_DATAFORMAT_8BIT:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxInt8>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxInt8>";
			break;
			
		case JVX_DATAFORMAT_U64BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxUInt64>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxUInt64>";
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxUInt32>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxUInt32>";
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxUInt16>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxUInt16>";
			if(elm.decoderType == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxUInt16>";
				commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxUInt16>";
				commandReadTypecast = "(jvxUInt16*)";
			}
			break;
		case JVX_DATAFORMAT_U8BIT:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxUInt8>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxUInt8>";
			break;
			
		case JVX_DATAFORMAT_BYTE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxByte>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxByte>";
			break;
		break;

		case JVX_DATAFORMAT_SIZE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignment<jvxSize>";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_1dList<jvxSize>";
			break;
		case JVX_DATAFORMAT_STRING:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignmentString";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_assignmentString";
			elm.numElements = 1;
			break;
		case JVX_DATAFORMAT_STRING_LIST:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignmentStringList";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_assignmentStringList";
			elm.numElements = 1;
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignmentSelectionList";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_assignmentSelectionList";
			elm.numElements = 1;
			addArgument = ", " + propertySectionName + "." + elm.name + ".onlySelectionToFile";
			if(elm.ignoreproblemsconfig)
			{
				addArgument += ", true"; 
				if(elm.allowOptionsConfigIfNotFound)
				{
					endArgument += ", ";
					endArgument += propertySectionName;
					endArgument += "_";
					endArgument += elm.name;
				}
			}
			else
			{
				addArgument += ", false"; 
			}	
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			commandReadConfigSingle = "HjvxConfigProcessor_readEntry_assignmentValueInRange";
			commandReadConfigMultiple = "HjvxConfigProcessor_readEntry_assignmentValueInRange";
			elm.numElements = 1;
			addArgument = ", " + propertySectionName + "." + elm.name + ".onlySelectionToFile";
			break;
		case JVX_DATAFORMAT_HANDLE:
			elm.numElements = 1;
			elm.associateExternal = false;
			elm.dynamic = false;
			elm.validOnInit = false;
			break;
		case JVX_DATAFORMAT_CALLBACK:
			// Do nothing
			return;
		
		default:
			assert(0);
		}

		out << "\t\twhattodo = jvx_check_config_mode_put_configuration(callConf->configModeFlags, " << propertySectionName << "." << 
			elm.name << ".configModeFlags," << 
			"callConf->accessFlags, " << propertySectionName << "." << elm.name << ".accessFlags);" << std::endl;
		if(elm.associateExternal)
		{
			switch (elm.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
#ifdef JVX_STORE_CONFIG_NAME
				out << "\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
				out << "\t\tif(replace_this && replace_by_this)" << std::endl;
				out << "\t\t{" << std::endl;
				//out << "\t\t\t// Case 1" << std::endl;
				out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), &" << propertySectionName << "." << elm.name << ".value" << addArgument
					<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
				out << "\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
				out << "\t\tif(replace_this && replace_by_this)" << std::endl;
				out << "\t\t{" << std::endl;
				//out << "\t\t\t// Case 2" << std::endl;
				out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << "&" << propertySectionName << "." << elm.name << ".value" << addArgument
					<< ", " <<
					"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
					"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
					"whattodo" << endArgument << ");" << std::endl;
#endif
				break;
			default:
				if (elm.numElements > 1)
				{
					// Config file read function
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
					if (elm.fldConfigIsOptional)
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, " <<
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ");" << std::endl;
#endif
					}
					else
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, " <<
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
						out << "\t\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\t\twarnings_properties.push_back(\"Failed to read property <\" \"" << propertySectionName
							<< "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
						out << "\t\t\t\twarnings_properties.push_back(\"Failed to read property <\" + " << propertySectionName
							<< "." << elm.name << ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
						out << "\t\t\t}" << std::endl;
					}
					out << "\t\t}" << std::endl;
				}
				else
				{
					switch (elm.format)
					{
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_U64BIT_LE:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:
					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_SIZE:
						// Config file read function
						out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
						out << "\t\t{" << std::endl;
						if (elm.fldConfigIsOptional)
						{
#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
								propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
								elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
							out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
								propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
								elm.name << ".num, " <<
								"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
								"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
								"whattodo" << endArgument << ");" << std::endl;
#endif
						}
						else
						{
#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
								propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
								elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags,  whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
							out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
								propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
								elm.name << ".num, " <<
								"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
								"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
								"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
							out << "\t\t\t{" << std::endl;

#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\t\tif(warnings_properties) warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__"
								<< elm.name << "\" \"> from config file.\");" << std::endl;
#else
							out << "\t\t\t\tif(warnings_properties) warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName
								<< "." << elm.name << ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
							out << "\t\t\t}" << std::endl;
						}
						out << "\t\t}" << std::endl;
						break;
					default:
						// Config file read function
						out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
						out << "\t\t{" << std::endl;
						if (elm.fldConfigIsOptional)
						{
#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr" << addArgument
								<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
							out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << propertySectionName << "." << elm.name << ".ptr" << addArgument << ", " <<
								"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
								"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
								"whattodo" << endArgument << ");" << std::endl;
#endif
						}
						else
						{
#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr"
								<< addArgument << ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
							out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
							out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
							out << "\t\t\t{" << std::endl;
							out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
							out << "\t\t\t}" << std::endl;
							out << "\t\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << propertySectionName << "." << elm.name << ".ptr" << addArgument << ", " <<
								"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
								"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
								"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
							out << "\t\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
							out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
							out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName << "." << elm.name <<
								".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
							out << "\t\t\t}" << std::endl;
						}
						out << "\t\t}" << std::endl;
					}
				}
				break;
			}
		}
		else
		{
			if(elm.dynamic)
			{
				// Variable initialization
				std::string szToken = jvx_int2String((int)elm.numElements);
				bool multipleObjects = false;

				// Check wether reference was used
				if(elm.reference.set)
				{
					if(elm.reference.key == VALUE)
					{
						szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
						multipleObjects = true;
					}
				}

				if(!multipleObjects)
				{
					if(elm.numElements > 1)
					{
						multipleObjects = true;
					}
				}

				if(multipleObjects)
				{
					// Read configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
					if(elm.fldConfigIsOptional)
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ");" << std::endl;
#endif
					}
					else
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
						out << "\t\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName << "." 
						<< elm.name << ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
						out << "\t\t\t}" << std::endl;
					}
					out << "\t\t}" << std::endl;
				}
				else
				{
					// Read configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
					if(elm.fldConfigIsOptional)
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name 
							<< ".ptr" << addArgument << ", " << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << propertySectionName << "." << elm.name << 
							".ptr" << addArgument << ", " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ");" << std::endl;
#endif
					}
					else
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr" 
							<< addArgument << ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << propertySectionName << "." << 
							elm.name << ".ptr" << addArgument << ", " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
						out << "\t\t\t{" << std::endl;

#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName << "." << elm.name 
							<< ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
						out << "\t\t\t}" << std::endl;
					}
					out << "\t\t}" << std::endl;
				}
			}
			else
			{
				if(elm.numElements > 1)
				{
					// Read configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
					if(elm.fldConfigIsOptional)
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast << 
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ");" << std::endl;
#endif
					}
					else
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\tif(" << commandReadConfigMultiple << "(theReader, ir, token.c_str(), " << commandReadTypecast << 
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
							elm.name << ".num, " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
						out << "\t\t\t{" << std::endl;

#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName << "." 
							<< elm.name << ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
						out << "\t\t\t}" << std::endl;
					}
					out << "\t\t}" << std::endl;
				}
				else
				{
					// Read configuration functions
					if(elm.fldConfigIsOptional)
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						//out << "\t\t\t// Case 1" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), &" << propertySectionName << "." << elm.name << ".value" << addArgument 
							<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo" << endArgument << ");" << std::endl;
#else
						out << "\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						//out << "\t\t\t// Case 2" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\t" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << "&" << propertySectionName << "." << elm.name << ".value" << addArgument 
							<< ", " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ");" << std::endl;
#endif
					}
					else
					{
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), &" << propertySectionName << "." << elm.name << ".value" << addArgument 
							<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags,whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#else
						out << "\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
						out << "\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t{" << std::endl;
						out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t}" << std::endl;
						out << "\t\tif(" << commandReadConfigSingle << "(theReader, ir, token.c_str(), " << commandReadTypecast << "&" << propertySectionName << "." << elm.name << ".value" << addArgument 
							<< ", " << 
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo" << endArgument << ") != JVX_NO_ERROR)" << std::endl;
#endif
						out << "\t\t{" << std::endl;

#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" \"" << propertySectionName << "__" << elm.name << "\" \"> from config file.\");" << std::endl;
#else
						out << "\t\t\t\tif(warnings_properties)warnings_properties->push_back(\"Failed to read property <\" + " << propertySectionName << "." 
							<< elm.name << ".descriptor.std_str() + \"> from config file.\");" << std::endl;
#endif
						out << "\t\t}" << std::endl;
					}
				}
			}
		}
	}
}

void
textProcessor_core::produceOutput_c_writeconfig(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName)
{
	std::string commandWriteConfigSingle = "unknown";
	std::string commandWriteConfigMultiple = "unknown";
	std::string addArgument = "";
	std::string commandWriteTypecast = "";
	if(elm.generateConfigFileEntry)
	{
		switch(elm.format)
		{
		case JVX_DATAFORMAT_DATA:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxData>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxData>";
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxInt64>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxInt64>";
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxInt32>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxInt32>";
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxInt16>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxInt16>";
			if(elm.decoderType == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxInt16>";
				commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxInt16>";
				commandWriteTypecast = "(jvxInt16*)";
			}
			break;
		case JVX_DATAFORMAT_8BIT:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxInt8>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxInt8>";
			break;
			
		case JVX_DATAFORMAT_U64BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxUInt64>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxUInt64>";
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxUInt32>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxUInt32>";
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxUInt16>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxUInt16>";
			if(elm.decoderType == JVX_PROPERTY_DECODER_SIMPLE_ONOFF)
			{
				commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxUInt16>";
				commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxUInt16>";
				commandWriteTypecast = "(jvxUInt16*)";
			}
			break;
		case JVX_DATAFORMAT_U8BIT:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxUInt8>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxUInt8>";
			break;
			
		case JVX_DATAFORMAT_BYTE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxByte>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxByte>";
			break;
		case JVX_DATAFORMAT_SIZE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignment<jvxSize>";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_1dList<jvxSize>";
			break;
		case JVX_DATAFORMAT_STRING:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignmentString";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_assignmentString";
			elm.numElements = 1;
			break;
		case JVX_DATAFORMAT_STRING_LIST:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignmentStringList";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_assignmentStringList";
			elm.numElements = 1;
			break;
		case JVX_DATAFORMAT_SELECTION_LIST:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignmentSelectionList";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_assignmentSelectionList";
			elm.numElements = 1;
			addArgument = ", " + propertySectionName + "." + elm.name + ".onlySelectionToFile";
			break;
		case JVX_DATAFORMAT_VALUE_IN_RANGE:
			commandWriteConfigSingle = "HjvxConfigProcessor_writeEntry_assignmentValueInRange";
			commandWriteConfigMultiple = "HjvxConfigProcessor_writeEntry_assignmentValueInRange";
			elm.numElements = 1;
			addArgument = ", " + propertySectionName + "." + elm.name + ".onlySelectionToFile";
			break;
		case JVX_DATAFORMAT_HANDLE:
			elm.numElements = 1;
			elm.associateExternal = false;
			elm.dynamic = false;
			elm.validOnInit = false;
			break;
		case JVX_DATAFORMAT_CALLBACK:
			// Do nothing
			return;
		
		default:
			assert(0);
		}

		out << "\t\twhattodo = jvx_check_config_mode_get_configuration(callConf->configModeFlags, " << propertySectionName << "." << elm.name << ".configModeFlags, " << 
			"callConf->accessFlags, " << propertySectionName << "." << elm.name << ".accessFlags);" << std::endl;
		if(elm.associateExternal)
		{
			switch (elm.format)
			{
			case JVX_DATAFORMAT_SELECTION_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				// Write configuration functions
#ifdef JVX_STORE_CONFIG_NAME
				out << "\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
				out << "\t\tif(replace_this && replace_by_this)" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), &" << propertySectionName << "."
					<< elm.name << ".value" << addArgument << ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
				out << "\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
				out << "\t\tif(replace_this && replace_by_this)" << std::endl;
				out << "\t\t{" << std::endl;
				out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
				out << "\t\t}" << std::endl;
				out << "\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << commandWriteTypecast << "&" << propertySectionName << "." << elm.name << ".value" << addArgument
					<< ", " <<
					"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
					"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
					"whattodo);" << std::endl;
#endif
				break;
			default:
				if (elm.numElements > 1)
				{
					// Config file write function
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
					out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
						elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
					out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " << commandWriteTypecast <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
						elm.name << ".num, " <<
						"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
						"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
						"whattodo);" << std::endl;
#endif

					out << "\t\t}" << std::endl;
				}
				else
				{
					switch (elm.format)
					{
					case JVX_DATAFORMAT_DATA:
					case JVX_DATAFORMAT_64BIT_LE:
					case JVX_DATAFORMAT_32BIT_LE:
					case JVX_DATAFORMAT_16BIT_LE:
					case JVX_DATAFORMAT_8BIT:
					case JVX_DATAFORMAT_U64BIT_LE:
					case JVX_DATAFORMAT_U32BIT_LE:
					case JVX_DATAFORMAT_U16BIT_LE:
					case JVX_DATAFORMAT_U8BIT:

					case JVX_DATAFORMAT_BYTE:
					case JVX_DATAFORMAT_SIZE:
						out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
						out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags,  whattodo);" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " << commandWriteTypecast <<
							propertySectionName << "." << elm.name << ".ptr, " << propertySectionName << "." <<
							elm.name << ".num, " <<
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo);" << std::endl;
#endif
						out << "\t\t}" << std::endl;
						break;
					default:
						// Config file write function
						out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
						out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
						out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr" << addArgument
							<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
						out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();" << std::endl;
						out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
						out << "\t\t\t{" << std::endl;
						out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
						out << "\t\t\t}" << std::endl;
						out << "\t\t\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr" << addArgument
							<< ", " <<
							"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
							"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
							"whattodo);" << std::endl;
#endif
						out << "\t\t}" << std::endl;
					}
				}
				break;
			}
		}
		else
		{
			if(elm.dynamic)
			{
				// Variable initialization
				std::string szToken = jvx_int2String((int)elm.numElements);
				bool multipleObjects = false;

				// Check wether reference was used
				if(elm.reference.set)
				{
					if(elm.reference.key == VALUE)
					{
						szToken = elm.reference.solved->parent->name + "." + elm.reference.solved->name;
						multipleObjects = true;
					}
				}

				if(!multipleObjects)
				{
					if(elm.numElements > 1)
					{
						multipleObjects = true;
					}
				}

				if(multipleObjects)
				{
					// Write configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ").ptr" << std::endl;
					out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
					out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
						elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
					out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " << commandWriteTypecast <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
						elm.name << ".num, " <<
						"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
						"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
						"whattodo);" << std::endl;
#endif
					out << "\t\t}" << std::endl;
				}
				else
				{
					// Write configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
					out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << propertySectionName << "." << elm.name << ".ptr" << addArgument 
						<< ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
					out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << commandWriteTypecast << propertySectionName << "." << elm.name << ".ptr" << addArgument 
						<< ", " << 
						"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
						"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
						"whattodo);" << std::endl;
#endif
					out << "\t\t}" << std::endl;
				}
			}
			else
			{
				if(elm.numElements > 1)
				{
					// Write configuration functions
					out << "\t\tif(" << propertySectionName << "." << elm.name << ".ptr)" << std::endl;
					out << "\t\t{" << std::endl;
#ifdef JVX_STORE_CONFIG_NAME
					out << "\t\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
						elm.name << ".num, &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
					out << "\t\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
					out << "\t\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t\t{" << std::endl;
					out << "\t\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t\t}" << std::endl;
					out << "\t\t\t" << commandWriteConfigMultiple << "(theReader, ir, token.c_str(), " << commandWriteTypecast <<
						propertySectionName << "." << elm.name << ".ptr, " << propertySectionName  << "." <<
						elm.name << ".num, " <<
						"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
						"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
						"whattodo);" << std::endl;
#endif
					out << "\t\t}" << std::endl;
				}
				else
				{
					// Write configuration functions
#ifdef JVX_STORE_CONFIG_NAME
					out << "\t\ttoken = \"" << propertySectionName << "__" << elm.name << "\";" << std::endl;	
					out << "\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t{" << std::endl;
					out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t}" << std::endl;
					out << "\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), &" << propertySectionName << "." 
						<< elm.name << ".value" << addArgument << ", &" << propertySectionName << "." << elm.name << ".configModeFlags, whattodo);" << std::endl;
#else
					out << "\t\ttoken = " << propertySectionName << "." << elm.name << ".descriptor.std_str();"  << std::endl;
					out << "\t\tif(replace_this && replace_by_this)" << std::endl;
					out << "\t\t{" << std::endl;
					out << "\t\t\ttoken = jvx_replaceStrInStr(token, *replace_this, *replace_by_this);" << std::endl;
					out << "\t\t}" << std::endl;
					out << "\t\t" << commandWriteConfigSingle << "(theReader, ir, token.c_str(), " << commandWriteTypecast << "&" << propertySectionName << "." << elm.name << ".value" << addArgument 
						<< ", " << 
						"&" << propertySectionName << "." << elm.name << ".accessFlags, " <<
						"&" << propertySectionName << "." << elm.name << ".configModeFlags, " <<
						"whattodo);" << std::endl;
#endif
				}
			}
		}
	}
}

void
textProcessor_core::produceOutput_c_updateTag(std::ostream& out, onePropertyDefinition& elm, std::string& propertySectionName)
{
	out << "\t\tif(jvx_compareStringsWildcard(wildcardstr, " << propertySectionName << "." << elm.name << ".descriptor.std_str()))" << std::endl;
	out << "\t\t{" << std::endl;
	out << "\t\t\t" << propertySectionName << "." << elm.name << ".pTag = newTag;" << std::endl;
	out << "\t\t}" << std::endl;
}

void
textProcessor_core::generateCode_oneElement_c(onePropertyElement theElm,
			std::ostream& streamVariables,
			std::ostream& streamInit,
			std::ostream& streamAllocate,
			std::ostream& streamDeallocate,
			std::ostream& streamRegister,
			std::ostream& streamUnregister,
			std::ostream& streamReadConfig,
			std::ostream& streamWriteConfig,
			std::ostream& streamAssociateFunctions,
			std::ostream& streamDeassociateFunctions,
			std::ostream& streamTranslations,
			std::ostream& streamTagUpdate,
			int tabOffset,
			std::string structToken,
			std::string funcToken,
			std::string prefixToken,
			std::vector<std::string>& lstInitFunctions,
			std::vector<std::string>& lstAllocateFunctions,
			std::vector<std::string>& lstDeallocateFunctions,
			std::vector<std::string>& lstRegisterFunctions,
			std::vector<std::string>& lstUnregisterFunctions,
			std::vector<std::string>& lstPutConfigFunctions,
			std::vector<std::vector<std::string> >& lstPutConfigFunctionArgs,
			std::vector<std::string>& lstGetConfigFunctions,
			std::vector<std::string>& lstupdTagFunctions,
	std::map<jvxInt32, oneAudioPluginEntry>& collectAudioPluginsIds,
	std::list<oneAudioPluginEntry>& collectInvalidAudioPluginsIds)
{
	jvxSize i,j;
	if(theElm.iamagroup)
	{
		streamVariables << tabify(1+tabOffset) << "// Group " << theElm.thePropertyGroup.thePrefix << std::endl;
		streamVariables << tabify(1+tabOffset) << "struct " << std::endl;
		streamVariables << tabify(1+tabOffset) << "{" << std::endl;
		structToken += theElm.thePropertyGroup.thePrefix + ".";
		funcToken += theElm.thePropertyGroup.thePrefix + "__";
		prefixToken = jvx_makePathExpr(prefixToken, theElm.thePropertyGroup.thePrefix, true);

		for(i = 0; i < theElm.thePropertyGroup.thePropertyElements.size(); i++)
		{
			generateCode_oneElement_c(theElm.thePropertyGroup.thePropertyElements[i],
				streamVariables,
				streamInit,
				streamAllocate,
				streamDeallocate,
				streamRegister,
				streamUnregister,
				streamReadConfig,
				streamWriteConfig,
				streamAssociateFunctions,
				streamDeassociateFunctions,
				streamTranslations,
				streamTagUpdate,
				tabOffset + 1,
				structToken,
				funcToken,
				prefixToken,
				lstInitFunctions,
				lstAllocateFunctions,
				lstDeallocateFunctions,
				lstRegisterFunctions,
				lstUnregisterFunctions,
				lstPutConfigFunctions,
				lstPutConfigFunctionArgs,
				lstGetConfigFunctions,
				lstupdTagFunctions,
				collectAudioPluginsIds,
				collectInvalidAudioPluginsIds);
		}

		streamVariables << tabify(1+tabOffset) << "} " << theElm.thePropertyGroup.thePrefix << ";" << std::endl << std::endl;
	}
	else
	{
		std::string propertySectionName = theElm.thePropertySection.name;
		std::string funcSectionName = theElm.thePropertySection.name;
		std::vector<std::string> locPrefixList = theElm.thePropertySection.prefixPathList;
		std::vector<std::string> locPropReturnSelLst;
		
		for(j = 0; j < theElm.thePropertySection.properties.size(); j++)
		{
			onePropertyDefinition& elm = theElm.thePropertySection.properties[j];
			if(elm.generateConfigFileEntry)
			{
				switch(elm.format)
				{
				case JVX_DATAFORMAT_SELECTION_LIST:
				
					// This must conclude the same as in function <produceOutput_c_readconfig>
					if(elm.ignoreproblemsconfig)
					{
						if(elm.allowOptionsConfigIfNotFound)
						{
							
							std::string arg = propertySectionName;
							arg += "_";
							arg += elm.name;
							locPropReturnSelLst.push_back(arg);
						}
					}
					break;
				}
			}
		}
		
		/*
		if (!theElm.thePropertySection.myLocalPrefix.empty())
		{
			locPrefixList.push_back(theElm.thePropertySection.myLocalPrefix);
		}
		*/
		streamVariables << tabify(1+tabOffset) << "// Section " << theElm.thePropertySection.name << "::" << theElm.thePropertySection.description << std::endl;
		streamVariables << tabify(1+tabOffset) << "struct " << std::endl;
		streamVariables << tabify(1+tabOffset) << "{" << std::endl;

		propertySectionName = structToken + propertySectionName;
		funcSectionName = funcToken + theElm.thePropertySection.name;

		// Iniutn
		streamInit << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamInit << "\tinline void init__" << funcSectionName << "()" << std::endl;
		streamInit << "\t{" << std::endl;
		streamInit << "\t\t// Initialize the struct to neutral defined values." << std::endl;
		lstInitFunctions.push_back("init__" + funcSectionName);

		// Allocation
		streamAllocate << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamAllocate << "\tinline void allocate__" << funcSectionName << "()" << std::endl;
		streamAllocate << "\t{" << std::endl;
		streamAllocate << "\t\t// Allocate memory and update registration with property handler if desired." << std::endl;
		lstAllocateFunctions.push_back("allocate__" + funcSectionName);

		// Deallocation
		streamDeallocate << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamDeallocate << "\tinline void deallocate__" << funcSectionName << "()" << std::endl;
		streamDeallocate << "\t{" << std::endl;
		streamDeallocate << "\t\t// If necessary, deallocate." << std::endl;
		lstDeallocateFunctions.push_back("deallocate__" + funcSectionName);

		// Register properties
		streamRegister << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamRegister << "\tinline void register__" << funcSectionName << "(CjvxProperties* theProps, const std::string reg_prefix = \"\", jvxBool force_unspecific = false, jvxBool v_register = false)" << std::endl;
		streamRegister << "\t{" << std::endl;
		streamRegister << "\t\t// Register properties with property management system." << std::endl;
		streamRegister << "\t\tjvxErrorType resL = JVX_NO_ERROR;" << std::endl;
		streamRegister << "\t\tCjvxProperties::pointerReferences _r;" << std::endl;
		streamRegister << "\t\tCjvxProperties::callbackReferences _c;" << std::endl ;		
		streamRegister << "\t\ttheProps->_lock_properties_local();" << std::endl << std::endl;
		lstRegisterFunctions.push_back("register__" + funcSectionName);

		streamUnregister << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamUnregister << "\tinline void unregister__" << funcSectionName << "(CjvxProperties* theProps)" << std::endl;
		streamUnregister << "\t{" << std::endl;
		streamUnregister << "\t\t// Unregister properties with property management system." << std::endl;
		streamUnregister << "\t\ttheProps->_lock_properties_local();" << std::endl;
		lstUnregisterFunctions.push_back("unregister__" + funcSectionName);
		
		streamReadConfig << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamReadConfig << "\tinline void put_configuration__" << funcSectionName 
			<< "(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>* warnings_properties = nullptr, const std::string* replace_this = nullptr, const std::string* replace_by_this = nullptr" << std::flush;
		for(auto elm: locPropReturnSelLst)
		{
			streamReadConfig << ", std::vector<std::string>* " << elm << " = nullptr" << std::flush;
		}
		streamReadConfig << ")" << std::endl;
		
		streamReadConfig << "\t{" << std::endl;
		streamReadConfig << "\t\t// Put configuration to initialize the properties." << std::endl;
		streamReadConfig << "\t\tjvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;" << std::endl;
		streamReadConfig << "\t\tstd::string token;" << std::endl;

		lstPutConfigFunctions.push_back("put_configuration__" + funcSectionName);
		lstPutConfigFunctionArgs.push_back(locPropReturnSelLst);

		streamWriteConfig << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamWriteConfig << "\tinline void get_configuration__" << funcSectionName << "(jvxCallManagerConfiguration* callConf, "
			<< "IjvxConfigProcessor* theReader, jvxHandle* ir, const std::string* replace_this = NULL, const std::string* replace_by_this = NULL)" << std::endl;
		streamWriteConfig << "\t{" << std::endl;
		streamWriteConfig << "\t\t// Get configuration to store the property values." << std::endl;
		streamWriteConfig << "\t\tjvxCBitField whattodo = JVX_PROPERTY_FLAGTAG_OPERATE_DO_NOTHING;" << std::endl;
		streamWriteConfig << "\t\tstd::string token;" << std::endl;
		lstGetConfigFunctions.push_back("get_configuration__" + funcSectionName);

		// Udpate Tag
		streamTagUpdate << "\t// Comment: " << theElm.thePropertySection.comment << std::endl;
		streamTagUpdate << "\tinline void updtag__" << funcSectionName << "(const std::string& newTag = \"\", const std::string& wildcardstr = \"*\")" << std::endl;
		streamTagUpdate << "\t{" << std::endl;
		streamTagUpdate << "\t\t// Update the variable tag." << std::endl;
		lstupdTagFunctions.push_back("updtag__" + funcSectionName);
			
		std::ostringstream streamAssociateFunctions_inner; // seems that a clear is not sufficient here.. Bug?
		std::ostringstream streamDeassociateFunctions_inner; // seems that a clear is not sufficient here.. Bug?
		bool isFirstAssociate = true;
		bool isFirstDeassociate = true;

		for(j = 0; j < theElm.thePropertySection.properties.size(); j++)
		{
			

			onePropertyDefinition& elm = theElm.thePropertySection.properties[j];

			streamVariables << tabify(2+tabOffset) << "// DESCRIPTION: " << elm.description << std::endl;
			streamAllocate << tabify(1) << "// DESCRIPTION: " << elm.description << std::endl;
			streamDeallocate <<tabify(1) << "// DESCRIPTION: " << elm.description << std::endl;

			// produce some variable definition before generating the code
			produceOutput_c_prepare(elm);

			// produce the variable definition in generated code
			produceOutput_c_variables(streamVariables, elm, tabOffset);

			// produce the variable allocation initialization in generated code
			produceOutput_c_init(streamInit, elm, propertySectionName, funcSectionName, streamTranslations, collectAudioPluginsIds,
				collectInvalidAudioPluginsIds);

			// produce the variable allocation initialization in generated code
			produceOutput_c_allocate(streamAllocate, elm, propertySectionName, funcSectionName);

			// produce the variable deallocation if required
			produceOutput_c_deallocate(streamDeallocate, elm, propertySectionName);

			// produce the variable association function
			produceOutput_c_associate(streamAssociateFunctions, streamAssociateFunctions_inner, isFirstAssociate, elm, propertySectionName, funcSectionName);
			produceOutput_c_deassociate(streamDeassociateFunctions, streamDeassociateFunctions_inner, isFirstDeassociate, elm, propertySectionName, funcSectionName);

			// produce the variable registration function			
			produceOutput_c_register(streamRegister, elm, propertySectionName, locPrefixList, intermediateStruct.callbacks);

			// produce the variable un-registration function
			produceOutput_c_unregister(streamUnregister, elm, propertySectionName);

			// produce the variable-read-from-config-file function
			produceOutput_c_readconfig(streamReadConfig, elm, propertySectionName);

			// produce the variable write-to-config-function
			produceOutput_c_writeconfig(streamWriteConfig, elm, propertySectionName);

			produceOutput_c_updateTag(streamTagUpdate, elm, propertySectionName);
			
		} // for(j = 0; j < intermediateStruct.thePropertySections[i].properties.size(); j++)

		streamInit << "\t};" << std::endl << std::endl;
		streamAllocate << "\t};" << std::endl << std::endl;
		streamDeallocate << "\t};" << std::endl << std::endl;

		streamRegister << "\t\ttheProps->_unlock_properties_local();" << std::endl << std::endl;
		streamRegister << "\t};" << std::endl << std::endl;		
		
		streamUnregister << "\t\ttheProps->_unlock_properties_local();" << std::endl << std::endl;
		streamUnregister << "\t};" << std::endl << std::endl;
		streamReadConfig << "\t};" << std::endl << std::endl;
		streamWriteConfig << "\t};" << std::endl << std::endl;
		streamTagUpdate << "\t};" << std::endl << std::endl;

		if(!isFirstAssociate)
		{
			streamAssociateFunctions << ")" << std::endl;
			streamAssociateFunctions << "\t{" << std::endl;
			streamAssociateFunctions << "\t\ttheProps->_lock_properties_local();" << std::endl;
			streamAssociateFunctions << streamAssociateFunctions_inner.str() << std::flush;
			streamAssociateFunctions << "\t\ttheProps->_unlock_properties_local();" << std::endl;
			streamAssociateFunctions << "\t};" << std::endl;
			streamAssociateFunctions << std::endl;

			streamDeassociateFunctions << ")" << std::endl;
			streamDeassociateFunctions << "\t{" << std::endl;
			streamDeassociateFunctions << "\t\ttheProps->_lock_properties_local();" << std::endl;
			streamDeassociateFunctions << streamDeassociateFunctions_inner.str() << std::flush;
			streamDeassociateFunctions << "\t\ttheProps->_unlock_properties_local();" << std::endl;
			streamDeassociateFunctions << "\t};" << std::endl;
			streamDeassociateFunctions << std::endl;
		}

		streamVariables << tabify(1+tabOffset) << "} " << theElm.thePropertySection.name << ";" << std::endl;
		streamVariables << std::endl;
	}
}

#define JVX_MAKE_KUNIT(txt, id) (txt + jvx_size2String(id)) 

void
textProcessor_core::produceOutput_c_AudioPluginParameters(
	std::ostream& out_declare, std::ostream& out_run, 
	std::map<jvxInt32, oneAudioPluginEntry>& audioPluginsIds,
	const std::string& audioplugin_register_tag)
{
	jvxSize i;
	jvxBitField units = 0;
	jvxBool safeme = false;
	std::string flags;
	jvxSize numConstsSync = 0;

	auto elm = audioPluginsIds.begin();

	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (jvx_bitTest(units, elm->second.oneEntry.audio_plugin_param_unit_id) == 0)
			{
				jvx_bitSet(units, elm->second.oneEntry.audio_plugin_param_unit_id);
			}
		}
	}

	out_declare << "\t// ====================================== " << std::endl;
	out_declare << "\t// Declare all units" << std::endl;
	out_declare << "\tenum" << std::endl;
	out_declare << "\t{" << std::endl;
	for (i = 0; i < jvx_bitfieldSelectionMax(units); i++)
	{
		if(jvx_bitTest(units, i))
		{
			if(safeme)
			{
				out_declare << "," << std::endl;
			}
			out_declare << "\t\t" << JVX_MAKE_KUNIT("k_Unit_", i) <<
				" = " << i << std::flush;
			safeme = true;
		}
	}
	out_declare << std::endl;
	out_declare << "\t};" << std::endl;
	out_declare << "\t// ====================================== " << std::endl;
	out_declare << std::endl;

	out_declare << "\t// ====================================== " << std::endl;
	out_declare << "\t// Declare all parameter ids " << std::endl;
	out_declare << "\tenum" << std::endl;
	out_declare << "\t{" << std::endl;
	jvxSize idOffset = 0;
	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		// Here is the place where we find our id
		// <- if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)

		if (elm != audioPluginsIds.begin())
		{
			out_declare << "," << std::endl << std::endl;
		}
		out_declare << "\t\t/* " << elm->second.expr_property << "*/" << std::endl;
		out_declare << "\t\t" << elm->second.const_property << " = " << elm->second.oneEntry.audio_plugin_id << std::flush;
		idOffset = JVX_MAX(idOffset, elm->second.oneEntry.audio_plugin_id);
		if (elm->second.oneEntry.audio_plugin_sync_active)
		{
			numConstsSync++;
		}
	}
	out_declare << std::endl;
	out_declare << "\t};" << std::endl;
	out_declare << "\t// ====================================== " << std::endl;
	out_declare << std::endl;
	out_declare << std::endl;

	out_declare << "\t// ====================================== " << std::endl;
	out_declare << "\t// Declare all parameter ids in an array" << std::endl;
	out_declare << "\t#define JVX_AUDIO_PLUGINS_ALL_IDS_NUM " << numConstsSync << std::endl;
	out_declare << "\t#define JVX_AUDIO_PLUGINS_ALL_IDS_ARR jvxInt32 jvxAllAudioPluginIds[JVX_AUDIO_PLUGINS_ALL_IDS_NUM] = \\" << std::endl;
	out_declare << "\t{ \\" << std::endl;
	jvxSize cnt = 0;
	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.audio_plugin_sync_active)
			{
				if (cnt)
				{
					out_declare << ", \\" << std::endl;
				}
				out_declare << "\t\t" << elm->second.const_property << std::flush;
				cnt++;
			}
		}
	}
	out_declare << "\\" << std::endl;
	out_declare << "\t};" << std::endl;
	out_declare << "\t// ====================================== " << std::endl;
	out_declare << std::endl;
	out_declare << std::endl;

	out_declare << "\t// ====================================== " << std::endl;
	out_declare << "\tconstexpr jvxSize JVX_PLUGIN_ID_OFFSET = " << idOffset + 1 << ";" << std::endl;
	out_declare << "\t// ====================================== " << std::endl;

	out_declare << std::endl;

	out_declare << "\t// ====================================== " << std::endl;
	out_declare << "\t#define JVX_PLUGIN_IDENT_TAG \"" << audioplugin_register_tag << "\"" << std::endl;
	out_declare << "\t// ====================================== " << std::endl;
	
	out_declare << std::endl;

	// =====================================================================================
	// =====================================================================================
	// =====================================================================================

	out_run << "\t// Function to allocate all plugin parameters" << std::endl;
	out_run << "\tinline jvxErrorType plugins_parameter_allocate(IjvxPluginParameterAllocator* plugAlloc,"
		<< " jvxSize* unit_ids = nullptr, const char** unit_names = nullptr, jvxSize numIds = 0)" << std::endl;
	out_run << "\t{" << std::endl;
	out_run << "\t\tif(!plugAlloc)" << std::endl;
	out_run << "\t\t{" << std::endl;
	out_run << "\t\t\treturn JVX_ERROR_INVALID_SETTING; " << std::endl;
	out_run << "\t\t}" << std::endl;
	out_run << std::endl;

	out_run << "\t\t// ====================================== " << std::endl;
	out_run << "\t\t// Allocate all units." << std::endl;
	for (i = 0; i < jvx_bitfieldSelectionMax(units); i++)
	{
		if (jvx_bitTest(units, i))
		{
			out_run << "\t\tif(numIds > " << i << ")" << std::endl;
			out_run << "\t\t{" << std::endl;
			out_run << "\t\t\tplugAlloc->allocate_unit(" << i << ", unit_ids[" << i << "], unit_names[" << i << "]);" << std::endl;
			out_run << "\t\t}" << std::endl;
			out_run << "\t\telse" << std::endl;
			out_run << "\t\t{" << std::endl;
			out_run << "\t\t\tplugAlloc->allocate_unit(" << i << ", " << i << ", \"" << JVX_MAKE_KUNIT("k_Unit_", i) << "\");" << std::endl;
			out_run << "\t\t}" << std::endl;
		}
	}
	out_run << "\t\t// ====================================== " << std::endl;
	out_run << std::endl;

	out_run << "\t\t// ====================================== " << std::endl;
	out_run << "\t\t// Allocate all parameters " << std::endl;
	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.allocate_param)
			{
				flags.clear();
				if (elm->second.oneEntry.audio_plugin_param_flags.empty())
				{
					flags = "PARAM_FLAGS_NOFLAGS";
				}
				else
				{
					auto elmi = elm->second.oneEntry.audio_plugin_param_flags.begin();
					for (; elmi != elm->second.oneEntry.audio_plugin_param_flags.end(); elmi++)
					{
						if (elmi != elm->second.oneEntry.audio_plugin_param_flags.begin())
							flags += " | ";
						flags += *elmi;
					}
				}

				switch (elm->second.oneEntry.audio_plugin_param_type)
				{
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_STRLIST:
					out_run << "\t\t{" << std::endl;
					out_run << "\t\t\tjvxApiStringList lstEntries;" << std::endl;
					for (i = 0; i < elm->second.oneEntry.norm.strlist.audio_plugin_param_norm_strlist.size(); i++)
					{
						out_run << "\t\t\tlstEntries.add(\"" << elm->second.oneEntry.norm.strlist.audio_plugin_param_norm_strlist[i] << "\");" << std::endl;
					}
					out_run << "\t\t\tplugAlloc->allocate_parameter_norm_strlist(" << std::endl;
					out_run << "\t\t\t\t\"" << elm->second.expr_property << "\", " << std::endl;
					out_run << "\t\t\t\t" << elm->second.const_property << ", " << std::endl;
					out_run << "\t\t\t\t" << "&lstEntries" << ", " << std::endl;
					out_run << "\t\t\t\t" << flags << " /* flags */, " << std::endl;
					out_run << "\t\t\t\t" << elm->second.oneEntry.norm.audio_plugin_param_norm_init << " /* init value */, " << std::endl;
					out_run << "\t\t\t\t" << JVX_MAKE_KUNIT("k_Unit_", elm->second.oneEntry.audio_plugin_param_unit_id) << ");" << std::endl;
					out_run << "\t\t}" << std::endl;
					break;

				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORMALIZED:
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_BOOL:
					out_run << "\t\tplugAlloc->allocate_parameter_normalized(" << std::endl;
					out_run << "\t\t\t\"" << elm->second.expr_property << "\", " << std::endl;
					out_run << "\t\t\t" << elm->second.const_property << ", " << std::endl;
					out_run << "\t\t\t" << flags << " /* flags */, " << std::endl;
					out_run << "\t\t\t" << elm->second.oneEntry.norm.audio_plugin_param_norm_init << " /* init value */, " << std::endl;
					out_run << "\t\t\t" << JVX_MAKE_KUNIT("k_Unit_", elm->second.oneEntry.audio_plugin_param_unit_id) << ");" << std::endl;
					break;
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO:

					out_run << "\t\tplugAlloc->allocate_parameter_norm_so(" << std::endl;
					out_run << "\t\t\t\"" << elm->second.expr_property << "\", " << std::endl;
					out_run << "\t\t\t" << elm->second.const_property << ", " << std::endl;
					out_run << "\t\t\t" << flags << " /* flags */, " << std::endl;
					out_run << "\t\t\t\"" << elm->second.oneEntry.norm.audio_plugin_param_norm_unit << "\", " << std::endl;
					out_run << "\t\t\t" << elm->second.oneEntry.norm.audio_plugin_param_norm_offset << " /* offset */, " << std::endl;
					out_run << "\t\t\t" << elm->second.oneEntry.norm.audio_plugin_param_norm_scalefac << " /* scalefac */, " << std::endl;
					out_run << "\t\t\t" << elm->second.oneEntry.norm.audio_plugin_param_norm_init << " /* init value */, " << std::endl;
					out_run << "\t\t\t" << JVX_MAKE_KUNIT("k_Unit_", elm->second.oneEntry.audio_plugin_param_unit_id) << ", " << std::endl;
					out_run << "\t\t\t" << elm->second.oneEntry.audio_plugin_param_num_digits << ");" << std::endl;
					break;
				}
			}
			else
			{
				out_run << "\t\t// No parameter allocated for id " << elm->second.const_property << "." << std::endl;
			}
		}
	}
	out_run << "\t\t// ====================================== " << std::endl;
	out_run << std::endl;

	out_run << "\t\treturn JVX_NO_ERROR;" << std::endl;
	out_run << "\t}" << std::endl;

	// ================================================================
	
	std::vector<std::string> norm_tokens;
	std::vector<std::string> norm_strlst_tokens;
	jvxSize cnt_gen_send = 0;
	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.audio_plugin_param_message_in)
			{
				cnt_gen_send++;
				switch (elm->second.oneEntry.audio_plugin_param_type)
				{
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_STRLIST:
					norm_strlst_tokens.push_back(elm->second.const_property);
					break;

				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORMALIZED:
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_BOOL:
				case jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO:
					norm_tokens.push_back(elm->second.const_property);
					break;
				}
			}
		}
	}

	if (cnt_gen_send)
	{
		out_run << "\t// Function to send a plugin parameter" << std::endl;
		out_run << "\tinline jvxErrorType plugins_parameter_send(IjvxPluginParamMessage* plugMessage,"
			<< " jvxInt32 id, jvxData normVal, const char* tag = nullptr, jvxApiStringList* strLst = nullptr, jvxErrorType resPass = JVX_NO_ERROR, jvxInt16 proc_id = -1)" << std::endl;
		out_run << "\t{" << std::endl;
		out_run << "\t\t// ====================================== " << std::endl;
		out_run << "\t\t;" << std::endl;
		out_run << "\t\tjvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;" << std::endl;
		out_run << "\t\tif(!plugMessage)" << std::endl;
		out_run << "\t\t{" << std::endl;
		out_run << "\t\t\treturn JVX_ERROR_INVALID_SETTING; " << std::endl;
		out_run << "\t\t}" << std::endl;
		out_run << "\t\tswitch(id)" << std::endl;
		out_run << "\t\t{" << std::endl;

		if (norm_strlst_tokens.size())
		{
			auto elmT = norm_strlst_tokens.begin();
			for (; elmT != norm_strlst_tokens.end(); elmT++)
			{
				out_run << "\t\tcase " << *elmT << ":" << std::endl;
			}
			out_run << std::endl;
			out_run << "\t\t\tplugMessage->trigger_transfer_param_norm_strlist(id, normVal, resPass, strLst, tag, proc_id);" << std::endl;
			out_run << "\t\t\tres = JVX_NO_ERROR; " << std::endl;
			out_run << "\t\t\tbreak;" << std::endl;
		}
		if (norm_tokens.size())
		{
			auto elmT = norm_tokens.begin();
			for (; elmT != norm_tokens.end(); elmT++)
			{
				out_run << "\t\tcase " << *elmT << ":" << std::endl;
			}
			out_run << std::endl;
			out_run << "\t\t\tplugMessage->trigger_transfer_param_norm(id, normVal, resPass, tag, proc_id);" << std::endl;
			out_run << "\t\t\tres = JVX_NO_ERROR; " << std::endl;
			out_run << "\t\t\tbreak;" << std::endl;
		}
		out_run << "\t\t}" << std::endl;
		out_run << "\t\treturn res;" << std::endl;
		out_run << "\t}" << std::endl;
	}
}

void
textProcessor_core::produceOutput_c_AudioPluginIds(
	std::ostream& out, std::map<jvxInt32, 
	oneAudioPluginEntry>& audioPluginsIds,
	std::list<oneAudioPluginEntry> collectInvalidAudioPluginsIds, 
	const std::string& audioplugin_register_tag, 
	jvxBool generateLineReferences)
{
	jvxSize numInputs = 0, numOutputs = 0;
	auto elm = audioPluginsIds.begin();
	for(; elm != audioPluginsIds.end(); elm++)
	{ 
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.audio_plugin_fragment_param_2_prop.expr.size())
			{
				numInputs++;
			}
			if (elm->second.oneEntry.audio_plugin_fragment_prop_2_param.expr.size())
			{
				numOutputs++;
			}
		}
	}

	out << "#ifdef JVX_WITHPLUGIN_EXTENDER" << std::endl;

	out << "\t// Function to register input and output properties"  << std::endl;
	out << "\tinline jvxErrorType plugins_extender_register(IjvxPropertyExtenderStreamAudioPlugin* plugExt, IjvxAudioPluginReport* rep)" << std::endl;
	out << "\t{" << std::endl;
	out << "\t\tjvxErrorType res = JVX_NO_ERROR;" << std::endl;
	out << "\t\tassert(rep);" << std::endl;
	out << "\t\tassert(plugExt);" << std::endl;

	out << "\t\t// Register report." << std::endl;
	out << "\t\tres = plugExt->register_report(rep, \"" << audioplugin_register_tag << "\");" << std::endl;
	out << std::endl;
	out << "\t\tif(res == JVX_NO_ERROR)" << std::endl;
	out << "\t\t{" << std::endl;
	out << "\t\t\t// Register input and output parameters." << std::endl;
	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			jvxBool register_input_stream = false;
			if (
				(elm->second.oneEntry.audio_plugin_param_stream_in) &&
				(elm->second.oneEntry.audio_plugin_fragment_param_2_prop.expr.size()))
			{
				register_input_stream = true;
			}

			if (register_input_stream)
			{
				out << "\t\t\tres = plugExt->register_id_parameter(rep, " << elm->second.const_property <<  /* elm->second.oneEntry.audio_plugin_id*/
					", " << elm->second.nm_property << ".descriptor.c_str());" << std::endl;
				out << "\t\t\tif (res != JVX_NO_ERROR)" << std::endl;
				out << "\t\t\t{" << std::endl;
				out << "\t\t\t\treturn res;" << std::endl;
				out << "\t\t\t}" << std::endl;
			}
		}
	}
	out << "\t\t}" << std::endl;
	out << "\t\treturn res;" << std::endl;
	out << "\t}" << std::endl;
	out << std::endl;

	out << "\t// Function to unregister input and output properties" << std::endl;
	out << "\tinline jvxErrorType plugins_extender_unregister(IjvxPropertyExtenderStreamAudioPlugin* plugExt, IjvxAudioPluginReport* rep)" << std::endl;
	out << "\t{" << std::endl;
	out << "\t\tjvxErrorType res = JVX_NO_ERROR;" << std::endl;
	out << "\t\tassert(rep);" << std::endl;
	out << "\t\tassert(plugExt);" << std::endl;
	out << "\t\t// Unegister all." << std::endl;
	out << "\t\tres = plugExt->unregister_report(rep);" << std::endl;
	out << "\t\treturn res;" << std::endl;
	out << "\t}" << std::endl;
	out << std::endl;

	out << std::endl;
	out << "\t// Function to convert input properties" << std::endl;
	out << "\tinline jvxErrorType plugins_extender_input(jvxInt32 idPlugin, jvxData _normalizedValue, std::function<jvxErrorType(jvxInt32, jvxData&, jvxHandle*)>& func)" << std::endl;
	out << "\t{" << std::endl;
	out << "\t\t// Process input parameters." << std::endl;
	out << "\t\tjvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;" << std::endl;
	out << "\t\tswitch(idPlugin)" << std::endl;
	out << "\t\t{" << std::endl;

	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.audio_plugin_fragment_param_2_prop.expr.size())
			{
				out << "\t\t\tcase " << elm->second.const_property /* elm->second.oneEntry.audio_plugin_id*/ << ":" << std::endl;
				out << "\t\t\t\t{" << std::endl;
				out << "\t\t\t\t\t// Generated code for " << elm->second.expr_property << std::endl;
				if (generateLineReferences)
				{
					out << "#line " << elm->second.oneEntry.audio_plugin_fragment_param_2_prop.lineno << " \""
						<< jvx_replaceCharacter(elm->second.oneEntry.audio_plugin_fragment_param_2_prop.fName, '\\', '/') << "\"" << std::endl;
				}
				std::string txt = elm->second.oneEntry.audio_plugin_fragment_param_2_prop.expr;
				if (!elm->second.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr.empty())
				{
					txt = jvx_replaceStrInStr(txt, "$$", elm->second.oneEntry.audio_plugin_fragment_param_2_prop.repl_expr);
					// txt = jvx_replaceStrInStr(txt, "$$", "*$PROP.ptr = $NORMVAL * $SCALE - $OFF;");
				}
				txt = jvx_replaceStrInStr(txt, "$PROP", elm->second.nm_property);
				txt = jvx_replaceStrInStr(txt, "$NORMVAL", "_normalizedValue");
				if (elm->second.oneEntry.audio_plugin_param_type == jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO)
				{
					txt = jvx_replaceStrInStr(txt, "$OFF", jvx_data2String(elm->second.oneEntry.norm.audio_plugin_param_norm_offset, JVX_DATA_2STRING_CONST_FORMAT_G));
					txt = jvx_replaceStrInStr(txt, "$SCALE", jvx_data2String(elm->second.oneEntry.norm.audio_plugin_param_norm_scalefac, JVX_DATA_2STRING_CONST_FORMAT_G));
				}
				txt = jvx_replaceStrInStr(txt, "\\n", "\n");
				std::string txtOut;
				while (1)
				{
					size_t posn = txt.find('\n');
					if (posn == std::string::npos)
					{
						out << "\t\t\t\t\t" << txt << std::endl;
						break;
					}
					else
					{
						out << "\t\t\t\t\t" << txt.substr(0, posn) << std::endl;
						txt = txt.substr(posn + 1, std::string::npos);
						if (txt.size() == 0)
						{
							break;
						}
					}
				}
				if (generateLineReferences)
				{
					// Make sure it is only a \n
					out << JVX_REPLACE_LINE_TOKEN << "\n" << std::flush;
				}

				out << "\t\t\t\t}" << std::endl;
				out << "\t\t\t\tres = JVX_NO_ERROR;" << std::endl;
				out << "\t\t\t\tbreak; " << std::endl;
			}
		}
	}

	out << "\t\t}" << std::endl;
	out << "\t\treturn res;" << std::endl;
	out << "\t}" << std::endl;

	out << std::endl;
	out << "\t// Function to convert output properties" << std::endl;
	out << "\tinline jvxErrorType plugins_extender_output(jvxInt32 idPlugin, jvxData* _normalizedValue, std::function<jvxErrorType(jvxInt32, jvxData&, jvxHandle*)>& func)" << std::endl;
	out << "\t{" << std::endl;
	out << "\t\t// Process output parameters." << std::endl;
	out << "\t\tjvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;" << std::endl;
	out << "\t\t switch(idPlugin)" << std::endl;
	out << "\t\t{" << std::endl;

	elm = audioPluginsIds.begin();
	for (; elm != audioPluginsIds.end(); elm++)
	{
		if (elm->second.oneEntry.audio_plugin_param_type != jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY)
		{
			if (elm->second.oneEntry.audio_plugin_fragment_prop_2_param.expr.size())
			{
				out << "\t\t\tcase " << elm->second.oneEntry.audio_plugin_id << ":" << std::endl;
				out << "\t\t\t\t{" << std::endl;
				out << "\t\t\t\t\t// Generated code for " << elm->second.expr_property << std::endl;
				if (generateLineReferences)
				{
					out << "#line " << elm->second.oneEntry.audio_plugin_fragment_prop_2_param.lineno << " \""
						<< jvx_replaceCharacter(elm->second.oneEntry.audio_plugin_fragment_prop_2_param.fName, '\\', '/') << "\"" << std::endl;
				}
				std::string txt = elm->second.oneEntry.audio_plugin_fragment_prop_2_param.expr;
				if (!elm->second.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr.empty())
				{
					txt = jvx_replaceStrInStr(txt, "$$", elm->second.oneEntry.audio_plugin_fragment_prop_2_param.repl_expr);
				}
				txt = jvx_replaceStrInStr(txt, "$PROP", elm->second.nm_property);
				txt = jvx_replaceStrInStr(txt, "$NORMVAL", "_normalizedValue");
				if (elm->second.oneEntry.audio_plugin_param_type == jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO)
				{
					txt = jvx_replaceStrInStr(txt, "$OFF", jvx_data2String(elm->second.oneEntry.norm.audio_plugin_param_norm_offset, JVX_DATA_2STRING_CONST_FORMAT_G));
					txt = jvx_replaceStrInStr(txt, "$SCALE", jvx_data2String(elm->second.oneEntry.norm.audio_plugin_param_norm_scalefac, JVX_DATA_2STRING_CONST_FORMAT_G));
				}
				txt = jvx_replaceStrInStr(txt, "\\n", "\n");
				std::string txtOut;
				while (1)
				{
					size_t posn = txt.find('\n');
					if (posn == std::string::npos)
					{
						out << "\t\t\t\t\t" << txt << std::endl;
						break;
					}
					else
					{
						out << "\t\t\t\t\t" << txt.substr(0, posn) << std::endl;
						txt = txt.substr(posn + 1, std::string::npos);
						if (txt.size() == 0)
						{
							break;
						}
					}
				}
				if (generateLineReferences)
				{
					// Make sure it is only a \n
					out << JVX_REPLACE_LINE_TOKEN << "\n" << std::flush;
				}

				out << "\t\t\t\t}" << std::endl;
				out << "\t\t\t\tres = JVX_NO_ERROR;" << std::endl;
				out << "\t\t\t\tbreak; " << std::endl;
			}
		}
	}

	out << "\t\t}" << std::endl;
	out << "\t\treturn res;" << std::endl;
	out << "\t}" << std::endl;

	if (collectInvalidAudioPluginsIds.size())
	{
		out << std::endl;
		auto elmi = collectInvalidAudioPluginsIds.begin();
		for (; elmi != collectInvalidAudioPluginsIds.end(); elmi++)
		{
			out << "\t// Invalid plugin connection for <" <<
				elmi->nm_property << ">, id <" << 
				elmi->oneEntry.audio_plugin_id << "> is duplicate!" << std::endl;
		}
	}
	out << std::endl;
	out << "#endif" << std::endl;
}

void
textProcessor_core::generateCode_c(const std::string& outFilenameH, jvxBool generateLineInfo)
{
	jvxSize i,j,l;
	std::ostringstream streamStartArea;
	std::ostringstream streamStopArea;
	std::ostringstream streamVariables;
	std::ostringstream streamInit;
	std::ostringstream streamAllocate;
	std::ostringstream streamDeallocate;
	std::ostringstream streamRegister;
	std::ostringstream streamUnregister;
	std::ostringstream streamAssociateFunctions;
	std::ostringstream streamDeassociateFunctions;
	std::ostringstream streamReadConfig;
	std::ostringstream streamWriteConfig;
	std::ostringstream streamTranslations;
	std::ostringstream streamTagUpdate;
	
	std::ostringstream streamPluginsCalls;
	std::ostringstream streamPluginsParameters_declare;
	std::ostringstream streamPluginsParameters_code;
	

	std::vector<std::string> lstInitFunctions;
	std::vector<std::string> lstAllocateFunctions;
	std::vector<std::string> lstDeallocateFunctions;
	std::vector<std::string> lstRegisterFunctions;
	std::vector<std::string> lstUnregisterFunctions;
	std::vector<std::string> lstPutConfigFunctions;
	std::vector<std::vector<std::string> > lstPutConfigFunctionArgs;
	std::vector<std::string> lstGetConfigFunctions;
	std::vector<std::string> lstTagUpdateFunctions;

	// Start generating code
	streamStartArea << "#ifndef _" << jvx_toUpper(intermediateStruct.fileOutputMiddle) << "_H__" << std::endl;
	streamStartArea << "#define _" << jvx_toUpper(intermediateStruct.fileOutputMiddle) << "_H__" << std::endl << std::endl;
	streamStartArea << "#include \"jvx-helpers.h\"" << std::endl;

	streamStartArea << "class " << intermediateStruct.className << std::endl;
	streamStartArea << "{" << std::endl;
	streamStartArea << "public:" << std::endl;
	streamStartArea << "\t" << intermediateStruct.className << "()" << std::endl;
	streamStartArea << "\t" << "{" << std::endl;
	streamStartArea << "\t\t" << "init_all();" << std::endl;
	streamStartArea << "\t" << "};" << std::endl;
	streamStopArea << "};" << std::endl;
	streamStopArea << "#endif" << std::endl;

	// FILL CONTENT

	//std::cout << streamH.str() << std::endl;
	//	streamVariables << "protected:" << std::endl << std::endl;
	streamVariables << std::endl << std::endl;

	std::map<jvxInt32, oneAudioPluginEntry> audioPluginsIds;
	std::list<oneAudioPluginEntry> collectInvalidAudioPluginsIds;

	for(l = 0; l < intermediateStruct.thePropertyElements.size(); l++)
	{
		generateCode_oneElement_c(intermediateStruct.thePropertyElements[l],
			streamVariables, streamInit, streamAllocate, streamDeallocate, streamRegister, streamUnregister, 
			streamReadConfig, streamWriteConfig,
			streamAssociateFunctions, streamDeassociateFunctions, streamTranslations, streamTagUpdate, 0, "", "", "",
			lstInitFunctions, lstAllocateFunctions, lstDeallocateFunctions, lstRegisterFunctions, 
			lstUnregisterFunctions, lstPutConfigFunctions, lstPutConfigFunctionArgs,
			lstGetConfigFunctions, lstTagUpdateFunctions,
			audioPluginsIds,
			collectInvalidAudioPluginsIds);
	}

	if (audioPluginsIds.size() + collectInvalidAudioPluginsIds.size())
	{
		produceOutput_c_AudioPluginIds(streamPluginsCalls,
			audioPluginsIds,
			collectInvalidAudioPluginsIds,
			intermediateStruct.audio_plugin_register_tag,
			generateLineInfo);

		produceOutput_c_AudioPluginParameters(
			streamPluginsParameters_declare,
			streamPluginsParameters_code,
			audioPluginsIds,
			intermediateStruct.audio_plugin_register_tag);
	}

	streamStartArea << std::endl;

	// OUTPUT TO FILE

	// File stream for h file
	std::ofstream osOutFileH;
	//std::string outFilenameH = outFilename + POSTFIX_H;
	osOutFileH.open(outFilenameH.c_str(), std::ios_base::out);
	if(!osOutFileH.is_open())
	{
		std::cout << "Unable to write to file " << outFilenameH << std::endl;
	}
	//	std::cout << (std::string)streamCpp.str() << std::endl;
	//	std::cout << streamH.str() << std::endl;
	osOutFileH << (std::string)streamStartArea.str() << std::flush;

	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Member Variables" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamVariables.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Init functions" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamInit.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Allocation functions" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamAllocate.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Deallocation functions" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamDeallocate.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Associate functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamAssociateFunctions.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << "\t// Deassociate functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamDeassociateFunctions.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Register functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamRegister.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Unregister functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamUnregister.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Config file read functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamReadConfig.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Config file write functions " << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamWriteConfig.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Translations Variables" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamTranslations.str() << std::flush;
	osOutFileH << "\t//=================================================" << std::endl;
	osOutFileH << "\t// Tag Update Functions" << std::endl;
	osOutFileH << "\t//=================================================" << std::endl << std::endl;
	osOutFileH << (std::string)streamTagUpdate.str() << std::flush;

	if (audioPluginsIds.size() + collectInvalidAudioPluginsIds.size())
	{
		osOutFileH << "\t//=================================================" << std::endl;
		osOutFileH << "\t// Audio Plugin Property Stream Converter" << std::endl;
		osOutFileH << "\t//=================================================" << std::endl << std::endl;
		osOutFileH << (std::string)streamPluginsCalls.str() << std::flush;		
	}
	if(intermediateStruct.callbacks.size())
	{
		osOutFileH << "\t//=================================================" << std::endl;
		osOutFileH << "\t// Register callbacks" << std::endl;
		osOutFileH << "\t//=================================================" << std::endl << std::endl;
		osOutFileH << "\tinline void register_callbacks(CjvxProperties* theProps" << std::flush;		
		for (i = 0; i < intermediateStruct.callbacks.size(); i++)
		{
			osOutFileH << ", \n\t\tproperty_callback cb_" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "_" << std::flush;
			}
			osOutFileH << intermediateStruct.callbacks[i].theName << std::flush;
		}
		osOutFileH << ", jvxHandle* privData, std::vector<std::string>* errMessages = nullptr)" << std::endl;
		osOutFileH << "\t{" << std::endl;
		osOutFileH << "\t\tjvxErrorType res = JVX_NO_ERROR;" << std::endl;
		for(i = 0; i < intermediateStruct.callbacks.size(); i++)
		{
			osOutFileH << "\t\tres = theProps->_register_callback(" << "\"/" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "/" << std::flush;
			}
				
			osOutFileH << intermediateStruct.callbacks[i].theName << "\", cb_" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "_" << std::flush;
			}
			osOutFileH << intermediateStruct.callbacks[i].theName << ", privData);" << std::endl;
			osOutFileH << "\t\tif(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back(\"Failed to register callback /" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "/" << std::flush;
			}
			osOutFileH << intermediateStruct.callbacks[i].theName << std::flush;
			osOutFileH << ", reason: \" + std::string(jvxErrorType_descr(res)) + \".\");" << std::endl;
		}
		//osOutFileH << "\t\treturn res;" << std::endl;
		osOutFileH << "\t};" << std::endl;
			
		osOutFileH << "\t//=================================================" << std::endl;
		osOutFileH << "\t// Unregister callbacks" << std::endl;
		osOutFileH << "\t//=================================================" << std::endl << std::endl;
		osOutFileH << "\tinline void unregister_callbacks(CjvxProperties* theProps, std::vector<std::string>* errMessages = nullptr)" << std::endl;
		osOutFileH << "\t{" << std::endl;
		osOutFileH << "\t\tjvxErrorType res = JVX_NO_ERROR;" << std::endl;
		for(i = 0; i < intermediateStruct.callbacks.size(); i++)
		{
			osOutFileH << "\t\tres = theProps->_unregister_callback(" << "\"/" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "/" << std::flush;
			}
			osOutFileH << intermediateStruct.callbacks[i].theName << "\");" << std::endl;
			osOutFileH << "\t\tif(res != JVX_NO_ERROR) if(errMessages)errMessages->push_back(\"Failed to unregister callback /" << std::flush;
			for (j = 0; j < intermediateStruct.callbacks[i].thePrefix.size(); j++)
			{
				osOutFileH << intermediateStruct.callbacks[i].thePrefix[j] << "/" << std::flush;
			}
			osOutFileH << intermediateStruct.callbacks[i].theName << ", reason: \" + std::string(jvxErrorType_descr(res)) + \".\");" << std::endl;
		}
		//osOutFileH << "\t\treturn res;" << std::endl;
		osOutFileH << "\t};" << std::endl;
	}

	// Init
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void init_all()" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstInitFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstInitFunctions[i] << "();" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Allocate
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void allocate_all()" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstAllocateFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstAllocateFunctions[i] << "();" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Deallocate
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void deallocate_all()" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstDeallocateFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstDeallocateFunctions[i] << "();" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;


	// Register properties
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void register_all(CjvxProperties* theProps, const std::string reg_prefix = \"\", jvxBool force_unspecific = false, jvxBool v_register = false)" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstRegisterFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstRegisterFunctions[i] << "(theProps, reg_prefix, force_unspecific, v_register);" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Unregister
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void unregister_all(CjvxProperties* theProps)" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstRegisterFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstUnregisterFunctions[i] << "(theProps);" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Put config
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void put_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, std::vector<std::string>* warnings_properties = nullptr" << std::flush;
	/*
	for(auto elmOuter: lstPutConfigFunctionArgs)
	{
		for(auto elmIn: elmOuter)
		{
			osOutFileH << ", std::vector<std::string>* " << elmIn << " = nullptr" << std::flush;
		}
	}
	*/
	osOutFileH << ")" << std::endl;
	osOutFileH << "\t{" << std::endl;
	
	for(i = 0; i < lstRegisterFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstPutConfigFunctions[i] << "(callConf, theReader, ir, warnings_properties" << std::flush; 
		/*
		 * Do not add these arguments here - the developer needs to adress the put_config function separately due to the
		 * two replace arguments!!
		if(i < lstPutConfigFunctionArgs.size())
		{
			if( lstPutConfigFunctionArgs[i].size())
			{
				// The replace parameters!!
				osOutFileH << ", nullptr, nullptr" << std::flush;
				for(auto elmIn: lstPutConfigFunctionArgs[i])
				{
					osOutFileH << ", " << elmIn <<std::flush;
				}
			}
		}
		*/
		osOutFileH << ");" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Get config
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void get_configuration_all(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir)" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstRegisterFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstGetConfigFunctions[i] << "(callConf, theReader, ir);" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;

	// Update Tags
	osOutFileH << "\t// All functions called at once." << std::endl;
	osOutFileH << "\tinline void updtag_all(const std::string& newTag = \"\", const std::string& wildcardstr = \"*\")" << std::endl;
	osOutFileH << "\t{" << std::endl;
	for(i = 0; i < lstTagUpdateFunctions.size(); i++)
	{
		osOutFileH << "\t\t" << lstTagUpdateFunctions[i] << "(newTag, wildcardstr);" << std::endl;
	}
	osOutFileH << "\t};" << std::endl;
	
	
	osOutFileH << (std::string)streamStopArea.str() << std::flush;
	osOutFileH.close();

	if (generateLineInfo)
	{
		correct_line_statements(outFilenameH.c_str());
	}

	if (audioPluginsIds.size() + collectInvalidAudioPluginsIds.size())
	{
		size_t pos = std::string::npos;
		std::string fname_prefix = outFilenameH;
		pos = fname_prefix.rfind('.');
		fname_prefix = fname_prefix.substr(0, pos);
		std::ofstream osOutFileHPluginsDecl;
		std::string outFilenameHPluginsDecl = fname_prefix + "_aplug_decl" + POSTFIX_H;
		osOutFileHPluginsDecl.open(outFilenameHPluginsDecl.c_str(), std::ios_base::out);
		if (!osOutFileHPluginsDecl.is_open())
		{
			std::cout << "Unable to write to file " << outFilenameHPluginsDecl << std::endl;
		}
		else
		{
			osOutFileHPluginsDecl << "#ifndef _" << jvx_toUpper(intermediateStruct.fileOutputMiddle + "_aplug_decl") << "_H__" << std::endl;
			osOutFileHPluginsDecl << "#define _" << jvx_toUpper(intermediateStruct.fileOutputMiddle + "_aplug_decl") << "_H__" << std::endl << std::endl;
			osOutFileHPluginsDecl << streamPluginsParameters_declare.str() << std::endl;
			osOutFileHPluginsDecl << "#endif" << std::endl;
		}
		
		std::ofstream osOutFileHPluginsRun;
		std::string outFilenameHPluginsRun = fname_prefix + "_aplug_run" + POSTFIX_H;
		osOutFileHPluginsRun.open(outFilenameHPluginsRun.c_str(), std::ios_base::out);
		if (!osOutFileHPluginsRun.is_open())
		{
			std::cout << "Unable to write to file " << outFilenameHPluginsRun << std::endl;
		}
		else
		{
			osOutFileHPluginsRun << "#ifndef _" << jvx_toUpper(intermediateStruct.fileOutputMiddle + "_aplug_run") << "_H__" << std::endl;
			osOutFileHPluginsRun << "#define _" << jvx_toUpper(intermediateStruct.fileOutputMiddle + "_aplug_run") << "_H__" << std::endl << std::endl;
			osOutFileHPluginsRun << streamPluginsParameters_code.str() << std::endl;
			osOutFileHPluginsRun << "#endif" << std::endl;
		}
	}
}
