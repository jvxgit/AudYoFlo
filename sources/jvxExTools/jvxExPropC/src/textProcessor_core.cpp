#include "scan_token_defines.h"
#include "textProcessor_core.h"
#include "jvx-helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define SAFE_CALL_WITH_WARNING(a,b,c) \
res = a; \
if(res == JVX_ERROR_WRONG_SECTION_TYPE) \
{ \
	std::string fName; \
	jvxInt32 lineno; \
	HjvxConfigProcessor_readEntry_originEntry(theReader, c, b, fName, lineno); \
	std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << b << " is associated to wrong type." << std::endl;\
}

// =====================================================================================
// =====================================================================================
// =====================================================================================
// =====================================================================================

static bool foundInPreviouslyDefinedIds(std::vector<jvxSize> ids, jvxSize oneInt)
{
	jvxSize i;
	for(i = 0; i < ids.size(); i++)
	{
		if(oneInt == ids[i])
		{
			return(true);
		}
	}
	return(false);
}

static void
initSection(onePropertySection& oneSection)
{
	oneSection.name = "not_set";
	oneSection.description = "not_set";
	//oneSection.prefix = "not_set";
	oneSection.comment = "not_set";
	oneSection.mat_tag = "";
	oneSection.generateConfigFileEntries = false;
	oneSection.generateLinkObjectEntries = false;
	oneSection.allowedStateMask = (JVX_STATE_ACTIVE | JVX_STATE_PREPARED | JVX_STATE_PROCESSING);
	oneSection.allowedThreadingMask = JVX_THREADING_DONTCARE;
	oneSection.accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	oneSection.showhint.set = false;
	oneSection.showhint.content.clear();
	oneSection.default_audioplugin_sync_active = "yes";
	oneSection.default_audioplugin_stream_in = "yes";
	oneSection.groupid = JVX_SIZE_UNSELECTED;
}

static void 
initPropertyAudioPlugin(onePropertyAudioPluginDesription& audioPluginDescr)
{
	audioPluginDescr.audio_plugin_id_set = false;
	audioPluginDescr.audio_plugin_id = -1;
	audioPluginDescr.audio_plugin_fragment_param_2_prop.expr = "";
	audioPluginDescr.audio_plugin_fragment_param_2_prop.lineno = -1;
	audioPluginDescr.audio_plugin_fragment_param_2_prop.fName = "<not set>";
	audioPluginDescr.audio_plugin_fragment_param_2_prop.repl_expr = "";

	audioPluginDescr.audio_plugin_fragment_prop_2_param.expr = "";
	audioPluginDescr.audio_plugin_fragment_prop_2_param.lineno = -1;
	audioPluginDescr.audio_plugin_fragment_prop_2_param.fName = "<not set>";
	audioPluginDescr.audio_plugin_fragment_prop_2_param.repl_expr = "";

	audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORMALIZED;
	audioPluginDescr.audio_plugin_param_unit_id = 0;
	audioPluginDescr.audio_plugin_param_flags.clear();
	audioPluginDescr.allocate_param = true;

	audioPluginDescr.audio_plugin_sync_active = false;
	audioPluginDescr.norm.audio_plugin_param_norm_offset = 0;
	audioPluginDescr.norm.audio_plugin_param_norm_scalefac = 1.0;
	audioPluginDescr.norm.audio_plugin_param_norm_unit = "";
	audioPluginDescr.norm.audio_plugin_param_norm_init = 0.0;
	audioPluginDescr.norm.strlist.audio_plugin_param_norm_strlist.clear();

	audioPluginDescr.audio_plugin_param_num_digits = 1;
	
}

static void
initProperty(onePropertyDefinition& oneProperty)
{

	oneProperty.name = "not_set";
	oneProperty.description = "not_set";
	oneProperty.descriptor = "not_set";
	oneProperty.dynamic = false;
	oneProperty.associateExternal = false;
	oneProperty.fldConfigIsOptional = false;
	oneProperty.ignoreproblemsconfig = true;
	oneProperty.installable = false;
	oneProperty.nullable = false;
	oneProperty.singleElementField = false;
	oneProperty.reference.absolute = false;
	oneProperty.reference.key = "NONE";
	//oneProperty.reference.path;
	oneProperty.reference.set = false;
	oneProperty.reference.solved = NULL;
	oneProperty.reference.fname = "unknown";
	oneProperty.reference.lineno = -1;

	oneProperty.showhint.set = false;
	oneProperty.showhint.content.clear();
	
	oneProperty.moreinfo.set = false;
	oneProperty.groupid = JVX_SIZE_UNSELECTED;

	oneProperty.parent = NULL;
	oneProperty.allowThreadingMask = JVX_THREADING_DONTCARE;
	oneProperty.access_rights_read = JVX_ACCESS_ROLE_DEFAULT;
	oneProperty.access_rights_write = JVX_ACCESS_ROLE_DEFAULT;
	oneProperty.access_rights_create = JVX_ACCESS_ROLE_DEFAULT;
	oneProperty.access_rights_destroy = JVX_ACCESS_ROLE_DEFAULT;
	oneProperty.config_mode = JVX_CONFIG_MODE_DEFAULT;
	oneProperty.allowStateMask = (JVX_STATE_ACTIVE | JVX_STATE_PREPARED | JVX_STATE_PROCESSING);
	oneProperty.category = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
	oneProperty.format = JVX_DATAFORMAT_DATA;
	oneProperty.decoderType = JVX_PROPERTY_DECODER_NONE;
	oneProperty.contxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
	oneProperty.updateOnAssociate = false;
	oneProperty.validOnInit = true;

	oneProperty.init_set.filename_set = false;

	oneProperty.id = -1;
	oneProperty.numElements = 1;
	oneProperty.onlySelectionToConfig = false;
	oneProperty.accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
	oneProperty.generateConfigFileEntry = false;
	oneProperty.generateLinkObjectEntry = false;

	oneProperty.initValueInRange.minVal = 0.0;
	oneProperty.initValueInRange.maxVal = 1.0;
	oneProperty.initValueInRange.value = 0.0;

	oneProperty.translations.tpName.clear();
	oneProperty.translations.enumClass = false;
	oneProperty.translations.strings.clear();

	oneProperty.selection.strings.clear();
	oneProperty.selection.selected.clear();
	oneProperty.selection.exclusive.clear();

	oneProperty.selection.varname = "";
	oneProperty.selection.nummax = "";
	oneProperty.selection.numprefix = "";
	
	initPropertyAudioPlugin(oneProperty.audioPluginDescr);
}

static void initPropPass(propertiesPassThrough* propspass)
{
	propspass->duplicateSystemProperties = false;
}

/*
static bool setCrossReferences(std::vector<onePropertySection>& theLists, std::vector<std::string>& errors)
{
	jvxSize i,j,ii,jj;
	bool detectedError = false;
	for(i = 0; i < theLists.size(); i++)
	{
		for(j = 0; j < theLists[i].properties.size(); j++)
		{
			onePropertyDefinition& elm = theLists[i].properties[j];
			elm.parent = &theLists[i];
			if(elm.reference.set)
			{
				bool foundit = false;
				if(elm.reference.path.size() == 2)
				{
					for(ii = 0; ii < theLists.size(); ii++)
					if(elm.reference.path[0] == theLists[ii].name)
					{
						for(jj = 0; jj < theLists[ii].properties.size(); jj++)
						{
							if(elm.reference.path[1] == theLists[ii].properties[jj].name)
							{
								elm.reference.solved = &theLists[ii].properties[jj];
								foundit = true;
							}
						}
					}
				}
				if(!foundit)
				{
					std::string oneError = elm.reference.fname + "(" + jvx_int2String(elm.reference.lineno) + "): Error: Undefined reference to determine length of array (property " + LENGTH + ").";
					errors.push_back(oneError);
					detectedError = true;
				}
			}
		}
	}
	return(detectedError);
}

static bool setCrossReferences(std::vector<onePropertyGroup>& theGroups, std::vector<std::string>& errors)
{
	jvxSize j;
	jvxBool res = true;
	for(j = 0; j < theGroups.size(); j++)
	{
		jvxBool resL = setCrossReferences(theGroups[j].thePropertySections, errors);
		if(resL != true)
		{
			res = false;
			break;
		}
	}
	return(res);
}
*/

bool
textProcessor_core::checkForConsistency()
{
	return(true);
}


// ==========================================================================

bool
textProcessor_core::scanInputFromIr(jvxConfigData* theMainSection, IjvxConfigProcessor* theReader, 
	std::string& purefilenameOutput, jvxBool pluginSupportMode)
{
	jvxSize  i;
	std::vector<std::string> lstPrefixes;
	std::vector<std::string> lstCallbacks;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* theContent = NULL, *theSubContent = NULL;
	jvxConfigData* datTmp = NULL;
	bool foundMainSection = false;
	std::string oneentry;
	jvxInt32 oneInt = 0;
	std::vector<std::string> entrylist;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxUInt64 mask = 0;

	intermediateStruct.fileOutputMiddle = purefilenameOutput;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, OUTPUTFILE_NAME, &intermediateStruct.fileOutputMiddle), OUTPUTFILE_NAME, theContent);

	intermediateStruct.className = intermediateStruct.fileOutputMiddle;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, CLASSNAME, &intermediateStruct.className), CLASSNAME, theContent);

	intermediateStruct.callbacks.clear();
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theMainSection, MACRO_CALLBACKS, &lstCallbacks), MACRO_CALLBACKS, theMainSection);
	for (i = 0; i < lstCallbacks.size(); i++)
	{
		oneCallback elm;
		elm.theName = lstCallbacks[i];
		// Leave empty elm.thePrefix;
		intermediateStruct.callbacks.push_back(elm);
	}

	intermediateStruct.audio_plugin_register_tag = intermediateStruct.className;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, 		
		MACRO_AUDIO_PLUGIN_REGISTER, &intermediateStruct.audio_plugin_register_tag), 
		MACRO_AUDIO_PLUGIN_REGISTER, theContent);

	intermediateStruct.default_audioplugin_sync_active = "no";
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection,
		MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, &intermediateStruct.default_audioplugin_sync_active),
		MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, theContent);

	intermediateStruct.default_audioplugin_stream_in = "no";
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection,
		MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, &intermediateStruct.default_audioplugin_stream_in),
		MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, theContent);

	intermediateStruct.default_audioplugin_message_in = "no";
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection,
		MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, &intermediateStruct.default_audioplugin_message_in),
		MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, theContent);
	

/*
	intermediateStruct.prefix_component = intermediateStruct.className;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, JVX_MAT_GENERATE_PREFIX_TXT, &intermediateStruct.prefix_component), JVX_MAT_GENERATE_PREFIX_TXT, theContent);

	oneentry = jvxComponentType_txt(JVX_COMPONENT_TYPE_NONE);
	#define JVX_MAT_GENERATE_COMPONENT_TYPE "JVX_MAT_GENERATE_COMPONENT_TYPE"
	*/

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Code generation is based on the intermediate representation in the
	// following. The data type is used to browse through the sections, subsections...
	// Strings can be read and used for code generation.
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	jvxSize numSections = 0;
	theReader->getNumberEntriesCurrentSection(theMainSection, &numSections);

//	jvxInt32 valI;
	jvxApiString aStr;
	bool detectedError = false;
	bool detectedErrorL = false;
	for(i = 0; i < numSections; i++)
	{
		theContent = NULL;
		propertiesPassThrough propspass;
		initPropPass(&propspass);
		theReader->getReferenceEntryCurrentSection_id(theMainSection, &theContent, i);
		if(theContent)
		{
			onePropertyElement theElement;
			jvxConfigSectionTypes type = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
			theReader->getTypeCurrentEntry(theContent, &type);
			switch(type)
			{
				case JVX_CONFIG_SECTION_TYPE_SECTION:
					detectedErrorL = processOneGroupSection(theContent, "", theReader, 
						theElement, lstPrefixes, 
						intermediateStruct.callbacks, 
						&propspass,
						pluginSupportMode, JVX_SIZE_UNSELECTED);

					this->intermediateStruct.thePropertyElements.push_back(theElement);
					detectedError = detectedError || detectedErrorL;
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
					/*
					std::cout << "Element #" << i << " is assignment string." << std::flush;
					theReader->getNameCurrentEntry(theContent, &aStr);
					std::cout << aStr.std_str() << " = " << std::flush;
					theReader->getAssignmentString(theContent, &aStr);
					std::cout << aStr.std_str() << std::endl;*/
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
					//std::cout << "Element #" << i << " is assignment value." << std::endl;
					break;
				case JVX_CONFIG_SECTION_TYPE_VALUELIST:
					//std::cout << "Element #" << i << " is assignment value list." << std::endl;
					break;
				case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
					//std::cout << "Element #" << i << " is assignment string list." << std::endl;
					break;
				case JVX_CONFIG_SECTION_TYPE_REFERENCE:
					//std::cout << "Element #" << i << " is assignment reference." << std::endl;
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTHEXSTRING:
					//std::cout << "Element #" << i << " is assignment hex string." << std::endl;
					break;
				default:
					//std::cout << "Element #" << i << " is unknown." << std::endl;
					break;	
			}
		}//if(theContent)
	}
	if(!detectedError)
	{
		std::vector<std::string> errors;
/*		detectedError = setCrossReferences(this->intermediateStruct.thePropertyGroups, errors);
		if(detectedError)
		{
			for(i = 0; i < errors.size(); i++)
			{
				std::cerr << errors[i] << std::endl;
			}
		}
		*/
	}
	return(!detectedError);
}


bool
textProcessor_core::processOneGroupSection(jvxConfigData* theContent, std::string prefix, IjvxConfigProcessor* theReader, onePropertyElement& theElement, 
	const std::vector<std::string>& lstPrefixes, std::vector<oneCallback>& lstCallbacks, propertiesPassThrough* passthrough, jvxBool pluginSupportMode,
	jvxSize groupidArg)
{
	jvxSize j, k;
	std::vector<std::string> callbacksLoc;

	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData*theSubContent = NULL;
	jvxConfigData* datTmp = NULL;
	bool foundMainSection = false;
	jvxApiString fldStr;
	std::string oneentry;
	jvxInt32 oneInt = 0;
	std::vector<std::string> entrylist;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxCBitField16 mask = 0;
	jvxData valD;

	bool caseExplicitId = false;
	bool caseRequiresId = false;
	bool caseRequiresGeneratedId = false;
	bool caseExplitSize = false;
	bool caseExplitCategory = false;
	bool caseExplitFormat = false;
	bool caseExpliticStatic = false;
	bool caseExpliticDecoder = false;
	bool noSizeHint = false;

	bool detectedError = false;
	bool detectedErrorL = false;
	bool isGroup = false;
	
	std::string fName;
	std::string origin_str;
	jvxInt32 lineno;
	
	onePropertySection oneSection;
	std::vector<std::string> lstPrefixesLoc = lstPrefixes;

	theReader->getTypeCurrentEntry(theContent, &tp);
	if(tp != JVX_CONFIG_SECTION_TYPE_SECTION)
	{
		HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, TYPE, fName, lineno);
		std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Found non-section element type in config file." << std::endl;
		return true;
	}
	
	theReader->getGroupStatus(theContent, &isGroup);
	if(isGroup)
	{
		std::string token;
		onePropertyGroup theGroup;
		jvxSize num = 0;
		propertiesPassThrough passLoc;
		
		initPropPass(&passLoc);
		if(passthrough)
		{
			passLoc = *passthrough;
		}
		
		// Entry to find name of member variable
		std::string myPrefix = "";
		theReader->getNameCurrentEntry(theContent, &fldStr);
		myPrefix = fldStr.std_str();
		theGroup.thePrefix = myPrefix;

		lstPrefixesLoc.push_back(myPrefix);

		myPrefix = jvx_makePathExpr(prefix, myPrefix);

		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, MACRO_CALLBACKS, &callbacksLoc), MACRO_CALLBACKS, theContent);
		if(!callbacksLoc.empty())
		{
			for(j = 0; j < callbacksLoc.size(); j++)
			{
				oneCallback elm;
				elm.theName = callbacksLoc[j];
				elm.thePrefix = lstPrefixesLoc;
				lstCallbacks.push_back(elm);
			}
		}

		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_DUPLICATE_SYSTEM_PROPERTIES, &token), 
			MACRO_DUPLICATE_SYSTEM_PROPERTIES, theContent);
		if(!token.empty())
		{
			if(token == "yes")
			{
				passLoc.duplicateSystemProperties = true;
			}
		}
		jvxSize groupid = groupidArg;
		res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theContent, GROUP_ID, &groupid);
		if (res == JVX_ERROR_WRONG_SECTION_TYPE)
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, GROUP_ID, fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << GROUP_ID << " is associated to wrong type." << std::endl;
		}

		theReader->getNumberSubsectionsCurrentSection(theContent, &num);
		for(j = 0; j < num; j++)
		{
			theSubContent = NULL;
			onePropertyElement myElement;
			theReader->getReferenceSubsectionCurrentSection_id(theContent, &theSubContent, j);
			if(theContent)
			{
				detectedErrorL = processOneGroupSection(theSubContent, 
					myPrefix, theReader, myElement, 
					lstPrefixesLoc, lstCallbacks, 
					&passLoc, pluginSupportMode, groupid);

				detectedError = detectedError || detectedErrorL;
			}//if(theContent)
			theGroup.thePropertyElements.push_back(myElement);
		}
		theElement.thePropertyGroup = theGroup;
		theElement.iamagroup = true;
	}
	else
	{
		// Setup default configuration
		initSection(oneSection);

		// Entry to generate config files
		oneentry = "no";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, GENERATE_ENTRIES_CONFIG_FILE, &oneentry), GENERATE_ENTRIES_CONFIG_FILE, theContent);
		if(oneentry == YES)
		{
			oneSection.generateConfigFileEntries = true;
		}
		else
		{
			oneSection.generateConfigFileEntries = false;
		}

		// Entry to generate link object associations
		oneentry = "no";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, GENERATE_ENTRIES_LINK_OBJECTS, &oneentry), GENERATE_ENTRIES_LINK_OBJECTS, theContent);
		if(oneentry == YES)
		{
			oneSection.generateLinkObjectEntries = true;
		}
		else
		{
			oneSection.generateLinkObjectEntries = false;
		}

		// Entry to find name of member variable
		oneSection.name = "unknown";
		HjvxConfigProcessor_readEntry_nameEntry(theReader, theContent, oneSection.name);
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, NAME, &oneSection.name), NAME, theContent);

		oneSection.prefixPathList = lstPrefixes;

		// Entry to find name of member variable
		oneSection.myLocalPrefix = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, PREFIX, &oneSection.myLocalPrefix), PREFIX, theContent);
		//oneSection.prefix = jvx_makePathExpr(prefix, oneSection.prefix, true);
		if (!oneSection.myLocalPrefix.empty())
		{
			oneSection.prefixPathList.push_back(oneSection.myLocalPrefix);
			lstPrefixesLoc.push_back(oneSection.myLocalPrefix);
		}

		oneSection.description = oneSection.name;
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, DESCRIPTION, &oneSection.description), DESCRIPTION, theContent);

		oneSection.comment = oneSection.description;
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, COMMENT, &oneSection.comment), COMMENT, theContent);

		// Entry to add matlab function tags
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MATLAB_SECTION_TAG, &oneSection.mat_tag), MATLAB_SECTION_TAG, theContent);

		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, MACRO_CALLBACKS, &callbacksLoc), MACRO_CALLBACKS, theContent);
		if(!callbacksLoc.empty())
		{
			for(j = 0; j < callbacksLoc.size(); j++)
			{
				oneCallback elm;
				elm.theName = callbacksLoc[j];
				elm.thePrefix = lstPrefixesLoc;
				lstCallbacks.push_back(elm);
			}
		}

		entrylist.clear();
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, ALLOWED_STATE_MASK, &entrylist), ALLOWED_STATE_MASK, theContent);
		if (entrylist.size())
		{
			mask = 0;
			for (k = 0; k < entrylist.size(); k++)
			{
				if (entrylist[k] == "JVX_STATE_NONE")
				{
					mask |= JVX_STATE_NONE;
				}
				else if (entrylist[k] == "JVX_STATE_INIT")
				{
					mask |= JVX_STATE_INIT;
				}
				else if (entrylist[k] == "JVX_STATE_SELECTED")
				{
					mask |= JVX_STATE_SELECTED;
				}
				else if (entrylist[k] == "JVX_STATE_ACTIVE")
				{
					mask |= JVX_STATE_ACTIVE;
				}
				else if (entrylist[k] == "JVX_STATE_PREPARED")
				{
					mask |= JVX_STATE_PREPARED;
				}
				else if (entrylist[k] == "JVX_STATE_PROCESSING")
				{
					mask |= JVX_STATE_PROCESSING;
				}
				else if (entrylist[k] == "JVX_STATE_DONTCARE")
				{
					mask |= JVX_STATE_DONTCARE;
				}
				else
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, TYPE, fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property allowStateMask " <<
						entrylist[k] << " in assignment " << ALLOWED_STATE_MASK << "." << std::endl;
					detectedError = true;
				}
			}
			oneSection.allowedStateMask = mask;
		}
		// otherwise use default

		// Threding state mask
		entrylist.clear();
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, ALLOWED_THREADING_MASK, &entrylist), ALLOWED_THREADING_MASK, theContent);
		if(entrylist.size() == 0)
		{
			entrylist.push_back("JVX_THREADING_DONTCARE");
		}

		// Threading mask
		mask = 0;
		for(k = 0; k < entrylist.size(); k++)
		{
			if(entrylist[k] == "JVX_THREADING_ASYNC")
			{
				mask |= JVX_THREADING_ASYNC;
			}
			else if(entrylist[k] == "JVX_THREADING_SYNC")
			{
				mask |= JVX_THREADING_SYNC;
			}
			else if(entrylist[k] == "JVX_THREADING_DONTCARE")
			{
				mask |= JVX_THREADING_DONTCARE;
			}
			else
			{
				HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, TYPE, fName, lineno);
				std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property allowThreadingMask " <<
					entrylist[k] << " in assignment " << ALLOWED_THREADING_MASK << "." << std::endl;
				detectedError = true;
			}
		}
		oneSection.allowedThreadingMask = mask;

		oneentry = "JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, READ_WRITE_ACCESS, &oneentry), READ_WRITE_ACCESS, theSubContent);
		if(oneentry == "JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE")
		{
			oneSection.accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
		}
		else if(oneentry == "JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT")
		{
			oneSection.accessType = JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT;
		}
		else if(oneentry == "JVX_PROPERTY_ACCESS_READ_ONLY")
		{
			oneSection.accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
		}
		else if (oneentry == "JVX_PROPERTY_ACCESS_READ_ONLY_ON_START")
		{
			//std::cout << "Default access type: " << oneentry << std::endl;
			oneSection.accessType = JVX_PROPERTY_ACCESS_READ_ONLY_ON_START;
		}
		else if(oneentry == "JVX_PROPERTY_ACCESS_WRITE_ONLY")
		{
			oneSection.accessType = JVX_PROPERTY_ACCESS_WRITE_ONLY;
		}
		else
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, READ_WRITE_ACCESS, fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type for specification of property access right " << READ_WRITE_ACCESS << "." << std::endl;
			detectedError = true;
		}
			
		// Invalidate on state switch

		oneSection.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
		oneentry = "JVX_PROPERTY_INVALIDATE_INACTIVE";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, "PROPERTY_INVALIDATE_STATE_SWITCH", &oneentry), 
			"PROPERTY_INVALIDATE_STATE_SWITCH", theSubContent);
		if (oneentry == "JVX_PROPERTY_INVALIDATE_INACTIVE")
		{
			oneSection.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
		}
		else if(oneentry == "JVX_PROPERTY_INVALIDATE_CONTENT")
		{
			oneSection.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_CONTENT;
		}
		else if (oneentry == "JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT")
		{
			oneSection.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT;
		}
		else
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, "PROPERTY_INVALIDATE_STATE_SWITCH", fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type " << oneentry << " for specification of property invalidate " <<
				"PROPERTY_INVALIDATE_STATE_SWITCH" << "." << std::endl;
			detectedError = true;
		}

		// Invalidate on test

		oneSection.invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
		oneentry = "JVX_PROPERTY_INVALIDATE_INACTIVE";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, "PROPERTY_INVALIDATE_TEST", &oneentry),
			"PROPERTY_INVALIDATE_TEST", theSubContent);
		if (oneentry == "JVX_PROPERTY_INVALIDATE_INACTIVE")
		{
			oneSection.invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
		}
		else if (oneentry == "JVX_PROPERTY_INVALIDATE_CONTENT")
		{
			oneSection.invalidateOnTest = JVX_PROPERTY_INVALIDATE_CONTENT;
		}
		else if (oneentry == "JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT")
		{
			oneSection.invalidateOnTest = JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT;
		}
		else
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, "PROPERTY_INVALIDATE_TEST", fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type " << oneentry << " for specification of property invalidate " <<
				"PROPERTY_INVALIDATE_TEST" << "." << std::endl;
			detectedError = true;
		}

		oneSection.callback_set = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_CALLBACK_SET_POSTHOOK, &oneSection.callback_set), MACRO_CALLBACK_SET_POSTHOOK, theContent);
				
		oneSection.callback_get = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_CALLBACK_GET_PREHOOK, &oneSection.callback_get), MACRO_CALLBACK_GET_PREHOOK, theContent);

		oneSection.callback_set_pre = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_CALLBACK_SET_PREHOOK, &oneSection.callback_set_pre), MACRO_CALLBACK_SET_PREHOOK, theContent);
				
		oneSection.callback_get_post = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_CALLBACK_GET_POSTHOOK, &oneSection.callback_get_post), MACRO_CALLBACK_GET_POSTHOOK, theContent);

		oneentry = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, MACRO_CALLBACK_SET_GET_HOOK, &oneentry), MACRO_CALLBACK_SET_GET_HOOK, theContent);
		if (!oneentry.empty())
		{
			oneSection.callback_set = oneentry;
			oneSection.callback_get = oneentry;
		}

		oneSection.updateContextToken = "";
		SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, UPDATE_CONTEXT_TOKEN, &oneSection.updateContextToken), UPDATE_CONTEXT_TOKEN, theContent);

		res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, SHOW_CONTENT_HINTS, &oneSection.showhint.content);
		if (res == JVX_ERROR_WRONG_SECTION_TYPE)
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, SHOW_CONTENT_HINTS, fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << SHOW_CONTENT_HINTS << " is associated to wrong type." << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			oneSection.showhint.set = true;
		}
		
		oneSection.default_audioplugin_sync_active = intermediateStruct.default_audioplugin_sync_active;
		SAFE_CALL_WITH_WARNING(
			HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, 
				MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, &oneSection.default_audioplugin_sync_active),
				MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, theContent);

		oneSection.default_audioplugin_stream_in = intermediateStruct.default_audioplugin_stream_in;
		SAFE_CALL_WITH_WARNING(
			HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent,
				MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, &oneSection.default_audioplugin_stream_in),
			MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, theContent);

		oneSection.default_audioplugin_message_in = intermediateStruct.default_audioplugin_message_in;
		SAFE_CALL_WITH_WARNING(
			HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent,
				MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, &oneSection.default_audioplugin_message_in),
			MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, theContent);
			/*
		if(oneSection.showhint.set)
		{
			std::cerr << "-S>" << oneSection.showhint.content << std::endl;
		}			
		*/
		
		oneSection.groupid = groupidArg;
		res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theContent, GROUP_ID, &oneSection.groupid);
		if (res == JVX_ERROR_WRONG_SECTION_TYPE)
		{
			HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, GROUP_ID, fName, lineno);
			std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << GROUP_ID << " is associated to wrong type." << std::endl;
		}

		// Now the properties in this member variable
		jvxSize numSubSections = 0;
		theReader->getNumberSubsectionsCurrentSection(theContent, &numSubSections);

		for(j = 0; j < numSubSections; j++)
		{
			theSubContent = NULL;
			theReader->getReferenceSubsectionCurrentSection_id(theContent, &theSubContent, j);
			if(theSubContent)
			{
				jvxSize numVariants = 1;
				
				onePropertyDefinition newProp;

				// Setup default configuration
				initProperty(newProp);

				caseExplicitId = false;
				caseRequiresId = false;
				caseRequiresGeneratedId = false;
				caseExplitSize = false;
				caseExplitCategory = false;
				caseExplitFormat = false;
				caseExpliticStatic = false;
				caseExpliticDecoder = false;
				noSizeHint = false;

				newProp.name = "property_" + jvx_int2String((int)j);
				HjvxConfigProcessor_readEntry_nameEntry(theReader, theSubContent, newProp.name);
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, NAME, &newProp.name), NAME, theSubContent);

				newProp.description = newProp.name;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, DESCRIPTION, &newProp.description), DESCRIPTION, theSubContent);
				newProp.description = jvx_replaceStrInStr(newProp.description, "@SECTION", oneSection.name);
				
				newProp.callback_set = oneSection.callback_set;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MACRO_CALLBACK_SET_POSTHOOK, &newProp.callback_set), MACRO_CALLBACK_SET_POSTHOOK, theSubContent);
				
				newProp.callback_get = oneSection.callback_get;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MACRO_CALLBACK_GET_PREHOOK, &newProp.callback_get), MACRO_CALLBACK_GET_PREHOOK, theSubContent);

				newProp.callback_set_pre = oneSection.callback_set_pre;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MACRO_CALLBACK_SET_PREHOOK, &newProp.callback_set_pre), MACRO_CALLBACK_SET_PREHOOK, theSubContent);
				
				newProp.callback_get_post = oneSection.callback_get_post;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MACRO_CALLBACK_GET_POSTHOOK, &newProp.callback_get_post), MACRO_CALLBACK_GET_POSTHOOK, theSubContent);

				oneentry = "";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MACRO_CALLBACK_SET_GET_HOOK, &oneentry), MACRO_CALLBACK_SET_GET_HOOK, theSubContent);
				if (!oneentry.empty())
				{
					newProp.callback_set = oneentry;
					newProp.callback_get = oneentry;
				}

				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theSubContent,
					"VARIANTS", &numVariants);
				
				newProp.descriptor = newProp.name;

				theReader->getOriginSection(theSubContent, &fldStr, &lineno);
				origin_str = fldStr.std_str();
				origin_str = jvx_replaceStrInStr(origin_str, "\\", "/"); // <- we need to consider linux seperators
				newProp.origin.content = (std::string)origin_str + ":" + jvx_int2String(lineno);
				newProp.origin.set = true;
				
				// Overwrite descriptor if section is defined
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, DESCRIPTOR, &newProp.descriptor), DESCRIPTOR, theSubContent);

				callbacksLoc.clear();
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, MACRO_CALLBACKS, &callbacksLoc), MACRO_CALLBACKS, theSubContent);
				if(!callbacksLoc.empty())
				{
					for(k = 0; k < callbacksLoc.size(); k++)
					{
						oneCallback elm;
						elm.theName = callbacksLoc[k];
						elm.thePrefix = lstPrefixes;
						lstCallbacks.push_back(elm);
					}
				}

				datTmp = NULL;
				theReader->getReferenceEntryCurrentSection_name(theSubContent, &datTmp, ID);
				if(datTmp)
				{
					// Id is given as a system specified names
					tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
					theReader->getTypeCurrentEntry(datTmp, &tp);
					if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING)
					{
						jvxSize id = 0;
						jvxDataFormat form = JVX_DATAFORMAT_NONE;
						jvxInt32 numElements = 0;
						std::string descrStr;
						jvxPropertyDecoderHintType decHt = JVX_PROPERTY_DECODER_NONE;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, ID, &oneentry), ID, theSubContent);
						if(jvx_findPropertyDescriptor(oneentry, &id, &form, &numElements, &decHt, &descrStr) == true)
						{
							if(!passthrough->duplicateSystemProperties)
							{
								newProp.category = JVX_PROPERTY_CATEGORY_PREDEFINED;
								newProp.id = id;
							}
							else
							{
								//std::cout << "Hallo hier!" << std::endl;
								newProp.category = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
								newProp.id = -1;
							}
							newProp.format = form;
							newProp.numElements = numElements;
							newProp.dynamic = false;
							newProp.decoderType = decHt;
							newProp.descriptor =  descrStr;
							caseExplicitId = true;
							caseExplitSize = true;
							caseExplitCategory = true;
							caseExplitFormat = true;
							caseExpliticStatic = true;
							caseExpliticDecoder = true;
						}
						else
						{							
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, ID, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown specific category id name " << oneentry << " in assignment " << ID << "." << std::endl;
							detectedError = true;
						}
					}
					else if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
					{
						oneInt = -1;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theSubContent, ID, &oneInt), ID, theSubContent);
						/*
						if(oneInt >= 0)
						{*/
						newProp.id = oneInt;
						newProp.numElements = 1;
						newProp.format = JVX_DATAFORMAT_DATA;
						newProp.dynamic = false;

						caseExplicitId = true;
						/*}
						else
						{
						std::string fName;
						jvxInt32 lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, ID, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Id specification has a value lower than 0." << std::endl;
						detectedError = true;
						}*/
					}
					else
					{
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, ID, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong specification of property id, should be either a positive integer or one of the pre-defined system names." << std::endl;
						detectedError = true;
					}
				}

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, DESCRIPTOR, &newProp.descriptor), DESCRIPTOR, theSubContent);
				// No else case, we need to generate an id

				// Determine category - if not specified explicitely before
				if(!caseExplitCategory)
				{
					// If we have not seen any id yet, generate one
					if(!caseExplicitId)
					{
						// use next available id and specific cataegory
						newProp.category = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
						newProp.id = -1;
						newProp.format = JVX_DATAFORMAT_DATA;
						newProp.numElements = 1;
						newProp.dynamic = false;
						/*							oneInt = -1;
						if(!foundInPreviouslyDefinedIds(ids_specific, oneInt))
						{
						}*/
					}
					else
					{
						oneentry = "JVX_PROPERTY_CATEGORY_SPECIFIC";
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, CATEGORY, &oneentry), CATEGORY, theSubContent);

						if(oneentry == MACRO_CATEGORY_SPECIFIC)
						{
							newProp.category = JVX_PROPERTY_CATEGORY_UNSPECIFIC;
						}
						else if(oneentry ==  MACRO_CATEGORY_COMMON)
						{
							newProp.category = JVX_PROPERTY_CATEGORY_PREDEFINED;
						}
						else
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, CATEGORY, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property category " << oneentry << " in assignment " << CATEGORY << "." << std::endl;
							detectedError = true;
						}
					}
				}

				if(caseExplicitId)
				{
					// Check that id has not been used before
					if(newProp.category == JVX_PROPERTY_CATEGORY_UNSPECIFIC)
					{
						if(foundInPreviouslyDefinedIds(ids_specific, newProp.id))
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Input property id " << newProp.id << " has been used before." << std::endl;
							detectedError = true;
						}
					}
					else if(newProp.category == JVX_PROPERTY_CATEGORY_PREDEFINED)
					{
						if(foundInPreviouslyDefinedIds(ids_common, newProp.id))
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Input property id " << newProp.id << " has been used before." << std::endl;
							detectedError = true;
						}
					}
				}

				if(!caseExplitFormat)
				{
					oneentry = "JVX_DATAFORMAT_DATA";
					SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, TYPE, &oneentry), TYPE, theSubContent);

					if(oneentry == MACRO_DATATYPE_DATA)
					{
						newProp.format = JVX_DATAFORMAT_DATA;
					}
					else if(oneentry == MACRO_DATATYPE_64BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_64BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_32BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_32BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_16BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_16BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_8BIT)
					{
						newProp.format = JVX_DATAFORMAT_8BIT;
					}
					else if(oneentry == MACRO_DATATYPE_U64BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_U64BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_U32BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_U32BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_U16BIT_LE)
					{
						newProp.format = JVX_DATAFORMAT_U16BIT_LE;
					}
					else if(oneentry == MACRO_DATATYPE_U8BIT)
					{
						newProp.format = JVX_DATAFORMAT_U8BIT;
					}
					else if (oneentry == MACRO_DATATYPE_SIZE)
					{
						newProp.format = JVX_DATAFORMAT_SIZE;
					}
					else if(oneentry == MACRO_DATATYPE_STRING)
					{
						newProp.format = JVX_DATAFORMAT_STRING;
					}
					else if(oneentry == MACRO_DATATYPE_STRING_LIST)
					{
						newProp.format = JVX_DATAFORMAT_STRING_LIST;
					}
					else if(oneentry == MACRO_DATATYPE_SELECTION_LIST)
					{
						newProp.format = JVX_DATAFORMAT_SELECTION_LIST;
					}
					else if(oneentry == MACRO_DATATYPE_VALUE_IN_RANGE)
					{
						newProp.format = JVX_DATAFORMAT_VALUE_IN_RANGE;
					}
					else if (oneentry == MACRO_DATATYPE_HANDLE)
					{
						newProp.format = JVX_DATAFORMAT_HANDLE;
					}
					else if (oneentry == "JVX_DATAFORMAT_BYTE")
					{
						newProp.format = JVX_DATAFORMAT_BYTE;
					}
					else if (oneentry == "JVX_DATAFORMAT_CALLBACK")
					{
						newProp.format = JVX_DATAFORMAT_CALLBACK;
					}
					else if (oneentry == "JVX_DATAFORMAT_INTERFACE")
					{
						newProp.format = JVX_DATAFORMAT_INTERFACE;
					}

					// These entries are for backward compatible behavior
					// These format types set format AND decoder type
					else if (oneentry == "JVX_DATAFORMAT_PROPERTY_EXTENDER_INTERFACE")
					{						
						newProp.format = JVX_DATAFORMAT_INTERFACE;
						newProp.decoderType = JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE;
						caseExpliticDecoder = true;
					}
					else if (oneentry == "JVX_DATAFORMAT_LOCAL_TEXT_LOG")
					{
						newProp.format = JVX_DATAFORMAT_STRING;
						newProp.decoderType = JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG;
						caseExpliticDecoder = true;
					}

					else if (oneentry == "JVX_DATAFORMAT_BOOL")
					{
						newProp.format = JVX_DATAFORMAT_BOOL;
						newProp.decoderType = JVX_PROPERTY_DECODER_SIMPLE_ONOFF;
						caseExpliticDecoder = true;
					}
					else
					{
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, TYPE, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property dataformat " << oneentry << " in assignment " << TYPE << "." << std::endl;
						detectedError = true;
					}
				}

				// Read the settype mask parameters
				entrylist.clear();
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, ALLOWED_THREADING_MASK, &entrylist),
					ALLOWED_THREADING_MASK, theSubContent);
				if(entrylist.empty())
				{
					mask = oneSection.allowedThreadingMask;
				}
				else
				{
					mask = 0;
					for(k = 0; k < entrylist.size(); k++)
					{
						if(entrylist[k] == "JVX_THREADING_ASYNC")
						{
							mask |= JVX_THREADING_ASYNC;
						}
						else if(entrylist[k] == "JVX_THREADING_SYNC")
						{
							mask |= JVX_THREADING_SYNC;
						}
						else if(entrylist[k] == "JVX_THREADING_DONTCARE")
						{
							mask |= JVX_THREADING_DONTCARE;
						}
						else
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, TYPE, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property allowThreadingMask " <<
								entrylist[k] << " in assignment " << ALLOWED_THREADING_MASK << "." << std::endl;
							detectedError = true;
						}
					}
				}
				newProp.allowThreadingMask = mask;

				entrylist.clear();
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, ALLOWED_STATE_MASK, &entrylist),
					ALLOWED_THREADING_MASK, theSubContent);
				if(entrylist.empty())
				{
					mask = oneSection.allowedStateMask;
				}
				else
				{
					mask = 0;
					for(k = 0; k < entrylist.size(); k++)
					{
						if(entrylist[k] == "JVX_STATE_NONE")
						{
							mask |= JVX_STATE_NONE;
						}
						else if(entrylist[k] == "JVX_STATE_INIT")
						{
							mask |= JVX_STATE_INIT;
						}
						else if(entrylist[k] == "JVX_STATE_SELECTED")
						{
							mask |= JVX_STATE_SELECTED;
						}
						else if(entrylist[k] == "JVX_STATE_ACTIVE")
						{
							mask |= JVX_STATE_ACTIVE;
						}
						else if(entrylist[k] == "JVX_STATE_PREPARED")
						{
							mask |= JVX_STATE_PREPARED;
						}
						else if(entrylist[k] == "JVX_STATE_PROCESSING")
						{
							mask |= JVX_STATE_PROCESSING;
						}
						else if(entrylist[k] == "JVX_STATE_DONTCARE")
						{
							mask |= JVX_STATE_DONTCARE;
						}
						else
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, TYPE, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input property allowStateMask " <<
								entrylist[k] << " in assignment " << ALLOWED_STATE_MASK << "." << std::endl;
							detectedError = true;
						}
					}
				}
				newProp.allowStateMask = mask;

				if(!caseExplitSize)
				{
					datTmp = NULL;
					theReader->getReferenceEntryCurrentSection_name(theSubContent, &datTmp, LENGTH);
					if(datTmp)
					{
						tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
						theReader->getTypeCurrentEntry(datTmp, &tp);
						if(tp == JVX_CONFIG_SECTION_TYPE_REFERENCE)
						{
							oneentry = "";
							entrylist.clear();
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_references(theReader, theSubContent, LENGTH, oneentry, NULL, entrylist), LENGTH, theSubContent);
							newProp.dynamic = true;
							newProp.reference.absolute = true;
							newProp.reference.set = true;
							newProp.reference.key = oneentry;
							newProp.reference.path = entrylist;

							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, LENGTH, newProp.reference.fname, newProp.reference.lineno);

							caseExplitSize = true;
							caseExpliticStatic = true;
						}
						else if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
						{
							oneInt = 1;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theSubContent, LENGTH, &oneInt), LENGTH, theSubContent);
							newProp.numElements = oneInt;
							caseExplitSize = true;
						}
					}
				}

				if(!caseExpliticStatic)
				{
					oneentry = "yes";
					SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, STATIC_VARIABLE, &oneentry), STATIC_VARIABLE, theSubContent);
					if(oneentry == "yes")
					{
						newProp.dynamic = false;
					}
					else
					{
						newProp.dynamic = true;
					}
				}

				datTmp = NULL;
				newProp.init_set.values.clear();
				newProp.init_set.filename_set = false;
				theReader->getReferenceEntryCurrentSection_name(theSubContent, &datTmp, INIT_SET);
				if(datTmp)
				{
					tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
					theReader->getTypeCurrentEntry(datTmp, &tp);
					if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING)
					{
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, INIT_SET, &newProp.init_set.string), INIT_SET, theSubContent);
						newProp.init_set.filename_set = true;
					}
					else if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
					{
						newProp.init_set.filename_set = false;
						valD = 0.0;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theSubContent, INIT_SET, &valD), INIT_SET, theSubContent);
						newProp.init_set.values.push_back(valD);
					}
					else if(tp == JVX_CONFIG_SECTION_TYPE_VALUELIST)
					{
						newProp.init_set.filename_set = false;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxData>(theReader, theSubContent, INIT_SET, 0, newProp.init_set.values), INIT_SET, theSubContent);
					}
					else if(tp == JVX_CONFIG_SECTION_TYPE_STRINGLIST)
					{
						newProp.init_set.filename_set = false;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_SET, &newProp.init_set.strings), INIT_SET, theSubContent);
					}
					else
					{
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, INIT_SET, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type for specification of initial values in assignment " << INIT_SET << "." << std::endl;
						detectedError = true;
					}
				}

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, SELECTION_LIST_NAMES, &newProp.selection.strings), SELECTION_LIST_NAMES, theSubContent);

				entrylist.clear();
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, SELECTION_LIST_SELECTED, &entrylist), SELECTION_LIST_SELECTED, theSubContent);
				for(k = 0; k < entrylist.size(); k++)
				{
					if(entrylist[k] == "yes")
					{
						newProp.selection.selected.push_back(1);
					}
					else
					{
						newProp.selection.selected.push_back(0);
					}
				}

				entrylist.clear();
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, SELECTION_LIST_EXCLUSIVE, &entrylist), SELECTION_LIST_EXCLUSIVE, theSubContent);
				for(k = 0; k < entrylist.size(); k++)
				{
					if(entrylist[k] == "yes")
					{
						newProp.selection.exclusive.push_back(1);
					}
					else
					{
						newProp.selection.exclusive.push_back(0);
					}
				}

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, SELECTION_LIST_TRANSLATORS, 
					&newProp.translations.strings), SELECTION_LIST_TRANSLATORS, theSubContent);
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, SELECTION_TRANSLATOR_TYPE, 
					&newProp.translations.tpName), 
					SELECTION_TRANSLATOR_TYPE, theSubContent);

				oneentry = "no";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, SELECTION_TRANSLATOR_ENUM_CLASS,
					&oneentry),
					SELECTION_TRANSLATOR_ENUM_CLASS, theSubContent);
				newProp.translations.enumClass = (oneentry == "yes");
				
				oneentry = ""; 
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "SELECTION_LIST_VAR_NAME",
					&oneentry), "SELECTION_LIST_VAR_NAME", theSubContent);
				newProp.selection.varname = oneentry;

				oneentry = "";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "SELECTION_LIST_VAR_NUM_PREFIX",
					&oneentry), "SELECTION_LIST_VAR_NUM_PREFIX", theSubContent);
				newProp.selection.numprefix = oneentry;

				oneentry = "";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "SELECTION_LIST_VAR_POSTFIX",
					&oneentry), "SELECTION_LIST_VAR_POSTFIX", theSubContent);
				newProp.selection.varpostfix = oneentry;

				oneentry = "";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "SELECTION_LIST_VAR_NUM_MAX",
					&oneentry), "SELECTION_LIST_VAR_NUM_MAX", theSubContent);
				newProp.selection.nummax = oneentry;
				
				
				// ===============================================================================================

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theSubContent, VALUE_IN_RANGE_MIN_VALUE, &newProp.initValueInRange.minVal), VALUE_IN_RANGE_MIN_VALUE, theSubContent);

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theSubContent, VALUE_IN_RANGE_MAX_VALUE, &newProp.initValueInRange.maxVal), VALUE_IN_RANGE_MAX_VALUE, theSubContent);

				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theSubContent, VALUE_IN_RANGE_VALUE, &newProp.initValueInRange.value), VALUE_IN_RANGE_VALUE, theSubContent);

				entrylist.clear(); 
				newProp.access_rights_read = JVX_ACCESS_ROLE_DEFAULT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_ACCESS_RIGHTS_READ, &entrylist), 
					INIT_ACCESS_RIGHTS_READ, theSubContent);
				if (res == JVX_NO_ERROR)
				{
					for (k = 0; k < entrylist.size(); k++)
					{
						if (entrylist[k] == ACCESS_RIGHT_ROLE_0)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_0;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_1)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_1;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_2)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_2;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_3)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_3;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_4)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_4;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_5)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_5;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_6)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_6;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_7)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_7;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_8)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_8;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_9)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_9;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_10)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_10;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_11)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_11;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_12)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_12;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_13)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_13;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ACCESS_RIGHTS)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_CONFIG_MODE)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_CONFIG_MODE;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ALL)
						{
							newProp.access_rights_read |= JVX_ACCESS_ROLE_GROUP_ALL;
						}
						else
						{
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INIT_ACCESS_RIGHTS_READ << " is associated to wrong specification <" <<
								entrylist[k] << ">." << std::endl;
						}
					}
				}
				
				// ======================================================================================================================

				entrylist.clear();
				newProp.access_rights_write = JVX_ACCESS_ROLE_DEFAULT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_ACCESS_RIGHTS_WRITE, &entrylist),
					INIT_ACCESS_RIGHTS_WRITE, theSubContent);
				if (res == JVX_NO_ERROR)
				{
					for (k = 0; k < entrylist.size(); k++)
					{
						if (entrylist[k] == ACCESS_RIGHT_ROLE_0)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_0;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_1)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_1;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_2)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_2;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_3)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_3;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_4)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_4;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_5)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_5;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_6)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_6;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_7)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_7;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_8)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_8;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_9)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_9;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_10)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_10;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_11)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_11;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_12)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_12;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_13)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_13;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ACCESS_RIGHTS)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_CONFIG_MODE)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_CONFIG_MODE;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ALL)
						{
							newProp.access_rights_write |= JVX_ACCESS_ROLE_GROUP_ALL;
						}
						else
						{
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INIT_ACCESS_RIGHTS_READ << " is associated to wrong specification <" <<
								entrylist[k] << ">." << std::endl;
						}
					}
				}

				// ====================================================================================================

				entrylist.clear();
				newProp.access_rights_create = JVX_ACCESS_ROLE_DEFAULT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_ACCESS_RIGHTS_CREATE, &entrylist),
					INIT_ACCESS_RIGHTS_CREATE, theSubContent);
				if (res == JVX_NO_ERROR)
				{
					for (k = 0; k < entrylist.size(); k++)
					{
						if (entrylist[k] == ACCESS_RIGHT_ROLE_0)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_0;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_1)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_1;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_2)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_2;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_3)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_3;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_4)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_4;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_5)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_5;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_6)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_6;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_7)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_7;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_8)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_8;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_9)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_9;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_10)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_10;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_11)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_11;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_12)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_12;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_13)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_13;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ACCESS_RIGHTS)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_CONFIG_MODE)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_CONFIG_MODE;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ALL)
						{
							newProp.access_rights_create |= JVX_ACCESS_ROLE_GROUP_ALL;
						}
						else
						{
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INIT_ACCESS_RIGHTS_CREATE << " is associated to wrong specification <" <<
								entrylist[k] << ">." << std::endl;
						}
					}
				}

				// =============================================================================================

				entrylist.clear();
				newProp.access_rights_destroy = JVX_ACCESS_ROLE_DEFAULT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_ACCESS_RIGHTS_DESTROY, &entrylist),
					INIT_ACCESS_RIGHTS_DESTROY, theSubContent);
				if (res == JVX_NO_ERROR)
				{
					for (k = 0; k < entrylist.size(); k++)
					{
						if (entrylist[k] == ACCESS_RIGHT_ROLE_0)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_0;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_1)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_1;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_2)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_2;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_3)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_3;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_4)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_4;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_5)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_5;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_6)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_6;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_7)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_7;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_8)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_8;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_9)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_9;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_10)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_10;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_11)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_11;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_12)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_12;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_13)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_13;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ACCESS_RIGHTS)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_ACCESS_RIGHTS;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_CONFIG_MODE)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_CONFIG_MODE;
						}
						else if (entrylist[k] == ACCESS_RIGHT_ROLE_ALL)
						{
							newProp.access_rights_destroy |= JVX_ACCESS_ROLE_GROUP_ALL;
						}
						else
						{
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INIT_ACCESS_RIGHTS_READ << " is associated to wrong specification <" <<
								entrylist[k] << ">." << std::endl;
						}
					}
				}

				entrylist.clear();
				newProp.config_mode = JVX_CONFIG_MODE_DEFAULT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theSubContent, INIT_CONFIG_MODE, &entrylist), INIT_CONFIG_MODE, theSubContent);
				if (res == JVX_NO_ERROR)
				{
					for (k = 0; k < entrylist.size(); k++)
					{
						if (entrylist[k] == CONFIG_MODE_FULL)
						{
							newProp.config_mode |= JVX_CONFIG_MODE_FULL;
						}
						else if (entrylist[k] == CONFIG_MODE_OVERLAY)
						{
							newProp.config_mode |= JVX_CONFIG_MODE_OVERLAY;
						}
						else
						{
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INIT_CONFIG_MODE << " is associated to wrong specification <" <<
								entrylist[k] << ">." << std::endl;
						}
					}
				}


				/*
				oneentry = "no";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, ONLY_SELECTION_TO_CONFIG, &oneentry), ONLY_SELECTION_TO_CONFIG, theSubContent);
				if(oneentry == "yes")
				{
				newProp.onlySelectionToConfig = true;
				}
				else
				{
				newProp.onlySelectionToConfig = false;
				}
				*/
				oneentry = "no";
				res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, ONLY_CONTENT_TO_CONFIG, &oneentry); 
				if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
				{
					// Try the other used option ONLY_SELECTION_TO_CONFIG
					res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, ONLY_SELECTION_TO_CONFIG, &oneentry);
					if(res == JVX_ERROR_WRONG_SECTION_TYPE) 
					{
						std::string fName; 
						jvxInt32 lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, ONLY_SELECTION_TO_CONFIG, fName, lineno); 
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << ONLY_SELECTION_TO_CONFIG << " is associated to wrong type." << std::endl;
					}					
				}
				else
				{
					if(res == JVX_ERROR_WRONG_SECTION_TYPE) 
					{
						std::string fName; 
						jvxInt32 lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, ONLY_CONTENT_TO_CONFIG, fName, lineno); 
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << ONLY_CONTENT_TO_CONFIG << " is associated to wrong type." << std::endl;
					}
				}
				if(oneentry == "yes")
				{
					newProp.onlySelectionToConfig = true;
				}
				else
				{
					newProp.onlySelectionToConfig = false;
				}

				if(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, READ_ONLY, &oneentry) == JVX_NO_ERROR)
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, READ_ONLY, fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Old version if assignment for write access of properties " << READ_ONLY << "." << std::endl;
					detectedError = true;
				}

				oneentry = "JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE";
				res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, READ_WRITE_ACCESS, &oneentry); 
				if(res == JVX_ERROR_WRONG_SECTION_TYPE) 
				{
					std::string fName; 
					jvxInt32 lineno;
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, READ_WRITE_ACCESS, fName, lineno); 
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << READ_WRITE_ACCESS << " is associated to wrong type." << std::endl;
				}
				else if(res == JVX_ERROR_ELEMENT_NOT_FOUND) 
				{
					//std::cout << "Element not found, use section type" << std::endl;
					newProp.accessType = oneSection.accessType;
				}
				else
				{
					if(oneentry == "JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE")
					{
						newProp.accessType = JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE;
					}
					else if(oneentry == "JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT")
					{
						newProp.accessType = JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT;
					}
					else if(oneentry == "JVX_PROPERTY_ACCESS_READ_ONLY")
					{
						newProp.accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
					}
					else if (oneentry == "JVX_PROPERTY_ACCESS_READ_ONLY_ON_START")
					{
						newProp.accessType = JVX_PROPERTY_ACCESS_READ_ONLY_ON_START;
					}
					else if(oneentry == "JVX_PROPERTY_ACCESS_WRITE_ONLY")
					{
						newProp.accessType = JVX_PROPERTY_ACCESS_WRITE_ONLY;
					}
					else
					{					
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, READ_WRITE_ACCESS, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type for specification of property access right " << READ_WRITE_ACCESS << "." << std::endl;
						detectedError = true;
					}
				}

				// Invalidate on state switch
				oneentry = jvxPropertyInvalidateType_str[oneSection.invalidateOnStateSwitch].full;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "PROPERTY_INVALIDATE_STATE_SWITCH", &oneentry),
					"PROPERTY_INVALIDATE_STATE_SWITCH", theSubContent);
				if (oneentry == "JVX_PROPERTY_INVALIDATE_INACTIVE")
				{
					newProp.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
				}
				else if (oneentry == "JVX_PROPERTY_INVALIDATE_CONTENT")
				{
					newProp.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_CONTENT;
				}
				else if (oneentry == "JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT")
				{
					newProp.invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT;
				}
				else
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, "PROPERTY_INVALIDATE_STATE_SWITCH", fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type <" << oneentry << "> for specification of property invalidate " <<
						"PROPERTY_INVALIDATE_STATE_SWITCH" << "." << std::endl;
					detectedError = true;
				}

				// Invalidate on test
				oneentry = jvxPropertyInvalidateType_str[oneSection.invalidateOnTest].full;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, "PROPERTY_INVALIDATE_TEST", &oneentry),
					"PROPERTY_INVALIDATE_TEST", theSubContent);
				if (oneentry == "JVX_PROPERTY_INVALIDATE_INACTIVE")
				{
					newProp.invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
				}
				else if (oneentry == "JVX_PROPERTY_INVALIDATE_CONTENT")
				{
					newProp.invalidateOnTest = JVX_PROPERTY_INVALIDATE_CONTENT;
				}
				else if (oneentry == "JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT")
				{
					newProp.invalidateOnTest = JVX_PROPERTY_INVALIDATE_DESCRIPTOR_CONTENT;
				}
				else
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, "PROPERTY_INVALIDATE_TEST", fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Wrong assignment type <" << oneentry << "> for specification of property invalidate " <<
						"PROPERTY_INVALIDATE_TEST" << "." << std::endl;
					detectedError = true;
				}

				newProp.showhint.content = oneSection.showhint.content;
				newProp.showhint.set = oneSection.showhint.set;
				
				res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, SHOW_CONTENT_HINTS, &newProp.showhint.content);
				if (res == JVX_ERROR_WRONG_SECTION_TYPE)
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, SHOW_CONTENT_HINTS, fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << SHOW_CONTENT_HINTS << " is associated to wrong type." << std::endl;
				}
				if (res == JVX_NO_ERROR)
				{
					newProp.showhint.set = true;
				}
				
				/*
				if(newProp.showhint.set)
				{
					std::cerr << "->" << newProp.showhint.content << std::endl;
				}
				*/

				newProp.groupid = oneSection.groupid;
				res = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theSubContent, GROUP_ID, &newProp.groupid);
				if (res == JVX_ERROR_WRONG_SECTION_TYPE)
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, GROUP_ID, fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << GROUP_ID << " is associated to wrong type." << std::endl;
				}				
					
				res = HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, MORE_INFO, &newProp.moreinfo.content);
				if (res == JVX_ERROR_WRONG_SECTION_TYPE)
				{
					HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, MORE_INFO, fName, lineno);
					std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << MORE_INFO << " is associated to wrong type." << std::endl;
				}
				if (res == JVX_NO_ERROR)
				{
					newProp.moreinfo.set = true;
				}

				oneentry = JVX_PROPERTY_CONTEXT_UNKNOWN_TXT;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, CONTEXT_TXT, &oneentry), CONTEXT_TXT, theSubContent);
				if(oneentry == JVX_PROPERTY_CONTEXT_INFO_TXT)
				{
					newProp.contxt = JVX_PROPERTY_CONTEXT_INFO;
				}
				else if(oneentry == JVX_PROPERTY_CONTEXT_PARAMETER_TXT)
				{
					newProp.contxt = JVX_PROPERTY_CONTEXT_PARAMETER;
				}
				else if(oneentry == JVX_PROPERTY_CONTEXT_RESULT_TXT)
				{
					newProp.contxt = JVX_PROPERTY_CONTEXT_RESULT;
				}
				else if(oneentry == JVX_PROPERTY_CONTEXT_VIEWBUFFER_TXT)
				{
					newProp.contxt = JVX_PROPERTY_CONTEXT_VIEWBUFFER;
				}
				else
				{
					newProp.contxt = JVX_PROPERTY_CONTEXT_UNKNOWN;
				}

				oneentry = "yes";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, IGNORE_CONFIG_PROBLEMS, &oneentry), IGNORE_CONFIG_PROBLEMS, theSubContent);
				if(oneentry == "yes")
				{
					newProp.ignoreproblemsconfig = true;
				}
				else
				{
					newProp.ignoreproblemsconfig = false;
				}

				oneentry = "no";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, ASSOCIATE_EXTERNAL, &oneentry), ASSOCIATE_EXTERNAL, theSubContent);
				if(oneentry == "yes")
				{
					newProp.associateExternal = true;
				}
				else
				{
					newProp.associateExternal = false;
				}

				newProp.generateConfigFileEntry = oneSection.generateConfigFileEntries;
				if(newProp.generateConfigFileEntry)
				{
					oneentry = "yes";
				}
				else
				{
					oneentry = "no";
				}
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, GENERATE_ENTRY_CONFIG_FILE, &oneentry), GENERATE_ENTRY_CONFIG_FILE, theSubContent);
				if(oneentry == "yes")
				{
					newProp.generateConfigFileEntry = true;
				}
				else
				{
					newProp.generateConfigFileEntry = false;
				}

				oneentry = "no";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, FIELD_CONFIG_IS_OPTIONAL, &oneentry), FIELD_CONFIG_IS_OPTIONAL, theSubContent);
				if(oneentry == "yes")
				{
					newProp.fldConfigIsOptional = true;
				}
				else
				{
					newProp.fldConfigIsOptional = false;
				}


				newProp.generateLinkObjectEntry = oneSection.generateLinkObjectEntries;
				if(newProp.generateLinkObjectEntry)
				{
					oneentry = "yes";
				}
				else
				{
					oneentry = "no";
				}
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, GENERATE_ENTRY_LINK_OBJECTS, &oneentry), GENERATE_ENTRY_LINK_OBJECTS, theSubContent);
				if(oneentry == "yes")
				{
					newProp.generateLinkObjectEntry = true;
				}
				else
				{
					newProp.generateLinkObjectEntry = false;
				}

				if(!caseExpliticDecoder)
				{
					oneentry = "JVX_PROPERTY_DECODER_NONE";
					SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, CONTENT_DECODER_TYPE, &oneentry), CONTENT_DECODER_TYPE, theSubContent);
					if(oneentry == "JVX_PROPERTY_DECODER_FILENAME_OPEN")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_FILENAME_OPEN;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_DIRECTORY_SELECT")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_DIRECTORY_SELECT;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_FILENAME_SAVE")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_FILENAME_SAVE;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_IP_ADDRESS")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_IP_ADDRESS;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_BITFIELD")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_BITFIELD;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_SINGLE_SELECTION")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_SINGLE_SELECTION;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_SELECTION")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_SELECTION;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_FORMAT_IDX")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_FORMAT_IDX;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_SUBFORMAT_IDX")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_SUBFORMAT_IDX;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_NONE")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_NONE;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_PROGRESSBAR")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_PROGRESSBAR;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_INPUT_FILE_LIST")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_INPUT_FILE_LIST;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_COMMAND")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_COMMAND;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_SIMPLE_ONOFF")
					{
						newProp.format = JVX_DATAFORMAT_BOOL; // Override!
						newProp.decoderType = JVX_PROPERTY_DECODER_SIMPLE_ONOFF;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG;
					}
					else if(oneentry == "JVX_PROPERTY_DECODER_ENUM_TYPE")
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_ENUM_TYPE;
					}
					else
					{
						newProp.decoderType = JVX_PROPERTY_DECODER_NONE;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, CONTENT_DECODER_TYPE, fName, lineno);
						std::cout << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown decoder property specification " <<
							oneentry << " in assignment " << CONTENT_DECODER_TYPE << "." << std::endl;
						detectedError = true;
					}
				}
				else
				{
					// A decoder type was provided by the definition. however, we may overwrite that type
					oneentry = "JVX_PROPERTY_DECODER_NONE";
					SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, CONTENT_DECODER_TYPE, &oneentry), CONTENT_DECODER_TYPE, theSubContent);
					if(res == JVX_NO_ERROR)
					{
						if(oneentry == "JVX_PROPERTY_DECODER_FILENAME_OPEN")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_FILENAME_OPEN;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_DIRECTORY_SELECT")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_DIRECTORY_SELECT;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_FILENAME_SAVE")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_FILENAME_SAVE;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_IP_ADDRESS")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_IP_ADDRESS;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_BITFIELD")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_BITFIELD;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_SINGLE_SELECTION")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_SINGLE_SELECTION;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_SELECTION")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_SELECTION;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_FORMAT_IDX")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_FORMAT_IDX;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_NONE")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_NONE;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_PROGRESSBAR")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_PROGRESSBAR;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_INPUT_FILE_LIST")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_INPUT_FILE_LIST;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_COMMAND")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_COMMAND;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_SIMPLE_ONOFF")
						{
							newProp.format = JVX_DATAFORMAT_BOOL; // override
							newProp.decoderType = JVX_PROPERTY_DECODER_SIMPLE_ONOFF;
						}
						else if(oneentry == "JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG")
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG;
						}
						else
						{
							newProp.decoderType = JVX_PROPERTY_DECODER_NONE;
							HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, CONTENT_DECODER_TYPE, fName, lineno);
							std::cout << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown decoder property specification " <<
								oneentry << " in assignment " << CONTENT_DECODER_TYPE << "." << std::endl;
							detectedError = true;
						}
					}
				}


				oneentry = "yes";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, IS_VALID_ON_INIT, &oneentry), IS_VALID_ON_INIT, theSubContent);
				if(oneentry == "yes")
				{
					newProp.validOnInit = true;
				}
				else
				{
					newProp.validOnInit = false;
				}

				oneentry = "yes";
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, UPDATE_POINTER_ON_ASSOCIATE, &oneentry), UPDATE_POINTER_ON_ASSOCIATE, theSubContent);
				if(oneentry == "yes")
				{
					newProp.updateOnAssociate = true;
				}
				else
				{
					newProp.updateOnAssociate = false;
				}

				newProp.updateContextToken = oneSection.updateContextToken;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theSubContent, UPDATE_CONTEXT_TOKEN, &newProp.updateContextToken), UPDATE_CONTEXT_TOKEN, theSubContent);

				/* ==================================================== */
				// Modify properties if required
				if(
					(newProp.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER)||
					(newProp.decoderType == JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER))
				{
					//if(!newProp.associateExternal )
					//{
					//	HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, CONTENT_DECODER_TYPE, fName, lineno);
					//	std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << CONTENT_DECODER_TYPE << ": Circular buffer properties can only be external references, overriding user choice." << std::endl;
					//}
					newProp.associateExternal = true;

					// No config entries for associated externals
					newProp.generateConfigFileEntry = false;
				}

				if(newProp.format == JVX_DATAFORMAT_HANDLE)
				{ 
					newProp.generateConfigFileEntry = false;
				}

				// ==============================================================
				// Here, make some override modifications!!
				// The installable properties are of format INTERFACE
				// ==============================================================
				switch(newProp.decoderType)
				{
				case JVX_PROPERTY_DECODER_PROPERTY_EXTENDER_INTERFACE:
					newProp.format = JVX_DATAFORMAT_INTERFACE;
					break;
				case JVX_PROPERTY_DECODER_LOCAL_TEXT_LOG:
					newProp.format = JVX_DATAFORMAT_STRING;
					break;
				}
				
				if (pluginSupportMode)
				{
					// =================================================================
					// Audio Plugin ID
					// =================================================================
					jvxConfigData* theDataSubSub = nullptr;
					theReader->getReferenceEntryCurrentSection_name(theSubContent, &theDataSubSub, "AUDIO_PLUGIN");
					if (theDataSubSub)
					{
						newProp.audioPluginDescr.audio_plugin_id_set = false;
						newProp.audioPluginDescr.audio_plugin_id = -1;
						SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theDataSubSub,
							MACRO_AUDIO_PLUGIN_ID, &newProp.audioPluginDescr.audio_plugin_id),
							MACRO_AUDIO_PLUGIN_ID, theDataSubSub);
						if (res == JVX_NO_ERROR)
						{
							newProp.audioPluginDescr.audio_plugin_id_set = true;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_PROPERTY, &newProp.audioPluginDescr.audio_plugin_fragment_param_2_prop.expr),
								MACRO_AUDIO_PLUGIN_PARAM_PROPERTY, theDataSubSub);
							if (res == JVX_NO_ERROR)
							{
								HjvxConfigProcessor_readEntry_originEntry(theReader, theDataSubSub, MACRO_AUDIO_PLUGIN_PARAM_PROPERTY,
									newProp.audioPluginDescr.audio_plugin_fragment_param_2_prop.fName, 
									newProp.audioPluginDescr.audio_plugin_fragment_param_2_prop.lineno);
							}

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PROPERTY_PARAM, &newProp.audioPluginDescr.audio_plugin_fragment_prop_2_param.expr),
								MACRO_AUDIO_PLUGIN_PROPERTY_PARAM, theDataSubSub);
							if (res == JVX_NO_ERROR)
							{
								HjvxConfigProcessor_readEntry_originEntry(theReader, theDataSubSub, MACRO_AUDIO_PLUGIN_PROPERTY_PARAM,
									newProp.audioPluginDescr.audio_plugin_fragment_prop_2_param.fName,
									newProp.audioPluginDescr.audio_plugin_fragment_prop_2_param.lineno);
							}

							oneentry = MACRO_AUDIO_PLUGIN_PARAM_NORMALIZED;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_TYPE, &oneentry),
								MACRO_AUDIO_PLUGIN_PARAM_TYPE, theDataSubSub);
							if (oneentry == MACRO_AUDIO_PLUGIN_PARAM_NORMALIZED)
							{
								newProp.audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORMALIZED;
							}
							else if (oneentry == MACRO_AUDIO_PLUGIN_PARAM_NORM_SO)
							{
								newProp.audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_SO;
							}
							else if (oneentry == MACRO_AUDIO_PLUGIN_PARAM_NORM_STRLIST)
							{
								newProp.audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_STRLIST;
							}
							else if (oneentry == MACRO_AUDIO_PLUGIN_PARAM_NORM_BOOL)
							{
								newProp.audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_NORM_BOOL;
							}
							else if (oneentry == MACRO_AUDIO_PLUGIN_PARAM_ID_ONLY)
							{
								newProp.audioPluginDescr.audio_plugin_param_type = jvxAudioPluginParamTypes::AUDIO_PLUGIN_PARAM_ID_ONLY;
							}
							else
							{
								HjvxConfigProcessor_readEntry_originEntry(theReader, theSubContent, MACRO_AUDIO_PLUGIN_PARAM_TYPE, fName, lineno);
								std::cout << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown plugin parameter type " <<
									oneentry << " in assignment " << MACRO_AUDIO_PLUGIN_PARAM_TYPE << "." << std::endl;
								detectedError = true;
							}

							oneentry = oneSection.default_audioplugin_stream_in;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, &oneentry),
								MACRO_AUDIO_PLUGIN_PARAM_STREAM_IN, theDataSubSub);
							if (oneentry == "yes")
							{
								newProp.audioPluginDescr.audio_plugin_param_stream_in = true;
							}
							else
							{
								newProp.audioPluginDescr.audio_plugin_param_stream_in = false;
							}

							oneentry = oneSection.default_audioplugin_message_in;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, &oneentry),
								MACRO_AUDIO_PLUGIN_PARAM_MESSAGE_IN, theDataSubSub);
							if (oneentry == "yes")
							{
								newProp.audioPluginDescr.audio_plugin_param_message_in = true;
							}
							else
							{
								newProp.audioPluginDescr.audio_plugin_param_message_in = false;
							}

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NORM_OFFSET, &newProp.audioPluginDescr.norm.audio_plugin_param_norm_offset),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_OFFSET, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NORM_SCALE, &newProp.audioPluginDescr.norm.audio_plugin_param_norm_scalefac),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_SCALE, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NORM_STRINGS, &newProp.audioPluginDescr.norm.strlist.audio_plugin_param_norm_strlist),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_STRINGS, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NORM_INIT, &newProp.audioPluginDescr.norm.audio_plugin_param_norm_init),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_INIT, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NORM_UNIT, &newProp.audioPluginDescr.norm.audio_plugin_param_norm_unit),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_UNIT, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_FLAGS, &newProp.audioPluginDescr.audio_plugin_param_flags), 
								MACRO_AUDIO_PLUGIN_PARAM_FLAGS, theDataSubSub);

							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_UNIT_ID, &newProp.audioPluginDescr.audio_plugin_param_unit_id),
								MACRO_AUDIO_PLUGIN_PARAM_NORM_UNIT, theDataSubSub);
							
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_NUM_DIGITS, &newProp.audioPluginDescr.audio_plugin_param_num_digits),
								MACRO_AUDIO_PLUGIN_PARAM_NUM_DIGITS, theDataSubSub);

							oneentry = "yes";
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_PARAM_ALLOCATE, &oneentry),
								MACRO_AUDIO_PLUGIN_PARAM_ALLOCATE, theDataSubSub);
							if (oneentry == "yes")
							{
								newProp.audioPluginDescr.allocate_param = true;
							}
							else
							{
								newProp.audioPluginDescr.allocate_param = false;
							}
							
							oneentry = oneSection.default_audioplugin_sync_active;
							SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theDataSubSub,
								MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, &oneentry),
								MACRO_AUDIO_PLUGIN_SYNC_ACTIVE, theDataSubSub);
							if (oneentry == "yes")
							{
								newProp.audioPluginDescr.audio_plugin_sync_active = true;
							}
							else
							{
								newProp.audioPluginDescr.audio_plugin_sync_active = false;
							};
						}
					}
				}

				if (numVariants == 1)
				{
					// Add new property to section
					oneSection.properties.push_back(newProp);
				}
				else
				{
					std::string storeName = newProp.name;
					std::string storeDescr = newProp.descriptor;
					std::string storeDescrion = newProp.description;
					for (jvxSize i = 0; i < numVariants; i++)
					{
						newProp.name = storeName + jvx_size2String(i);
						newProp.description = jvx_replaceStrInStr(storeDescrion, "$VARIANT", jvx_size2String(i));
						newProp.descriptor = jvx_replaceStrInStr(storeDescr, "$VARIANT", jvx_size2String(i));
						oneSection.properties.push_back(newProp);
					}
				}

				

			}// if(theSubContent)
		} // for(j = 0; j < numSubSections; j++)

		theElement.thePropertySection = oneSection;
		theElement.iamagroup = false;
	}
	return(detectedError);
}