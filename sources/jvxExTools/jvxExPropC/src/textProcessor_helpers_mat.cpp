#include "scan_token_defines.h"
#include "textProcessor_core.h"
#include <iostream>
#include <fstream>
#include <sstream>

// static int cntGen = 1;

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

void
textProcessor_core::generateCode_oneElement_mat(onePropertyElement& theElm, const std::string& outDir, std::string& Cptp, std::string& Cppf, 
	std::string funcPrefix, std::string propPrefix, jvxSize maxNumberCharacters)
{
	jvxSize j,k,l;
	std::string tag;
	std::string funcName_get;
	std::string funcName_set;
	std::string fileName;
	std::string autoTypecast = "";

	if(theElm.iamagroup)
	{
		funcPrefix = funcPrefix + theElm.thePropertyGroup.thePrefix + "_";
		propPrefix = jvx_makePathExpr(propPrefix, theElm.thePropertyGroup.thePrefix, true);

		for(l = 0; l < theElm.thePropertyGroup.thePropertyElements.size(); l++)
		{
			generateCode_oneElement_mat(theElm.thePropertyGroup.thePropertyElements[l], outDir, Cptp, Cppf, funcPrefix, propPrefix, maxNumberCharacters);
		}
	}
	else
	{
		std::vector<std::string> locPrefixList = theElm.thePropertySection.prefixPathList;
		std::string prefix = prefixListToString(locPrefixList, "/");
		// std::string prefix_func = prefixListToString(locPrefixList, "_");
		std::string prefix_func;
		if(!theElm.thePropertySection.myLocalPrefix.empty())
		{
			prefix_func = theElm.thePropertySection.myLocalPrefix + "_";
		}
		
		for(j = 0; j < theElm.thePropertySection.properties.size(); j++)
		{
			funcName_get = "";
			tag = "";

			if(!theElm.thePropertySection.mat_tag.empty())
			{
				tag = theElm.thePropertySection.mat_tag + "_";
			}

			if(Cppf.empty())
			{
				funcName_get = "get_" JVX_PRODUCT_CODEGEN_TAG + tag + funcPrefix + prefix_func + theElm.thePropertySection.properties[j].name;
				funcName_set = "set_" JVX_PRODUCT_CODEGEN_TAG + tag + funcPrefix + prefix_func + theElm.thePropertySection.properties[j].name;
			}
			else
			{
				funcName_get = "get_" JVX_PRODUCT_CODEGEN_TAG  + Cppf + "_" + tag + funcPrefix + prefix_func + theElm.thePropertySection.properties[j].name;
				funcName_set = "set_" JVX_PRODUCT_CODEGEN_TAG  + Cppf + "_" + tag + funcPrefix + prefix_func + theElm.thePropertySection.properties[j].name;
			}

			if(JVX_CHECK_SIZE_SELECTED(maxNumberCharacters))
			{
				jvxBool doneShorten = false;
				// Remove 6 cars in the middle
				if(maxNumberCharacters > 8)
				{
					jvxSize numCharsOrig = funcName_get.size();
					jvxSize numCharsParts = (maxNumberCharacters - 6)/2;
					if(numCharsOrig > maxNumberCharacters) 
					{
						std::string fNameGetNewFirst = funcName_get.substr(0, numCharsParts);
						std::string fNameGetNewSecond = funcName_get.substr(funcName_get.size() - numCharsParts);
						funcName_get = fNameGetNewFirst + "_" + jvx_size2String(numCharsOrig - fNameGetNewFirst.size() - fNameGetNewSecond.size()) + "_" + fNameGetNewSecond;
						doneShorten = true;
					}				

					numCharsOrig = funcName_set.size();
					numCharsParts = (maxNumberCharacters - 6)/2;
					if(numCharsOrig > maxNumberCharacters)
					{
						std::string fNameSetNewFirst = funcName_set.substr(0, numCharsParts);
						std::string fNameCut = funcName_get.substr(numCharsParts, funcName_get.size() - 2 * numCharsParts);
						std::string fNameSetNewSecond = funcName_set.substr(funcName_set.size() - numCharsParts);
						funcName_set = fNameSetNewFirst + "_" + jvx_size2String(numCharsOrig - fNameSetNewFirst.size() - fNameSetNewSecond.size()) + "_" + fNameSetNewSecond;
						doneShorten = true;
					}
				}
				if(doneShorten)
				{
					// cntGen++;
				}
			}
			
				
			// Function to GET property
			fileName = outDir + JVX_SEPARATOR_DIR + funcName_get + ".m";
			//std::cout << "-XXXX-> " << fileName << "::" <<  tag << "::" <<  funcPrefix << std::endl;

			{
				// Put this into braces to recreate variables. streamContent.clear() seems not to work
				std::ofstream osOutFile;
				osOutFile.open(fileName.c_str(), std::ios_base::out);
				if(!osOutFile.is_open())
				{
					std::cout << "Unable to write to file " << fileName << std::endl;
				}
				else
				{

					std::ostringstream streamContent;

					streamContent << "% Matlab file for accessing (get) property " << theElm.thePropertySection.properties[j].name << std::endl;
					streamContent << "% Property related to entry in property description file " << std::flush;
					streamContent << theElm.thePropertySection.properties[j].reference.fname << ", starting in line " <<
						theElm.thePropertySection.properties[j].reference.lineno << std::endl;

					if(theElm.thePropertySection.properties[j].format == JVX_DATAFORMAT_SELECTION_LIST)
					{
						for(k = 0; k < theElm.thePropertySection.properties[j].selection.strings.size(); k++)
						{
							streamContent << "% Selection 0x" << jvx_intx2String((1 << k)) << "(" << jvx_int2String((1 << k)) << "): " << theElm.thePropertySection.properties[j].selection.strings[k] << std::endl;
						}
					}


					streamContent << "function [out] = " << funcName_get << "(prop_tag)" << std::endl;
					streamContent << "\tout = [];" << std::endl;
					streamContent << "\tprop_search_tag = '" << Cptp << "';" << std::endl;
					streamContent << "\tif(nargin >= 1)" << std::endl;
					streamContent << "\t\tprop_search_tag = prop_tag;" << std::endl;
					streamContent << "\tend" << std::endl;
					streamContent << "\tglobal jvx_host_call_global;" << std::endl;
					streamContent << "\ttry" << std::endl;
					
					
					if(prefix.empty())
					{
						streamContent << "\t\t%% " << "propPrefix = <" << propPrefix << ">, Descriptor = <" 
							<< theElm.thePropertySection.properties[j].descriptor << ">" << std::endl; 
						streamContent << "\t\t[p0 p1] = jvx_host_call_global('get_property_descriptor', prop_search_tag, '" 
							<< jvx_makePathExpr("", theElm.thePropertySection.properties[j].descriptor) << "');" << std::endl;
					}
					else
					{
						streamContent << "\t\t%% " <<  "Prefix = <" << prefix << ">, propPrefix = <" 
							<< propPrefix << ">, Descriptor = <" << theElm.thePropertySection.properties[j].descriptor << ">" << std::endl; 
						streamContent << "\t\t[p0 p1] = jvx_host_call_global('get_property_descriptor', prop_search_tag, '" 
							<< jvx_makePathExpr(prefix, theElm.thePropertySection.properties[j].descriptor) << "');" << std::endl;
					}
	
					streamContent << "\t\tif(~p0)" << std::endl;
					streamContent << "\t\t\twarning(['Error in get_property: ', " << "p1.DESCRIPTION_STRING]);" << std::endl;
					streamContent << "\t\telse" << std::endl;
					streamContent << "\t\t\tout = p1;" << std::endl;
					streamContent << "\t\tend" << std::endl;
					streamContent << "\tcatch ME" << std::endl;
					streamContent << "\t\tp0 = functions(jvx_host_call_cb);" << std::endl;
					streamContent << "\t\twarning(['Function <' p0.function '> failed, reason: ' ME.message]);" << std::endl;
					streamContent << "\tend" << std::endl;

					//std::cerr << "--> " << fileName << std::endl;
					//std::cerr << (std::string)streamContent.str() << std::flush;
					osOutFile << (std::string)streamContent.str() << std::flush;
					osOutFile.close();

					// Do we want to execute an external command for conversion to p-file??
				}
			}

			// Function to SET property
			fileName = outDir + JVX_SEPARATOR_DIR + funcName_set + ".m";

			{
				std::ofstream osOutFile;
				osOutFile.open(fileName.c_str(), std::ios_base::out);
				if(!osOutFile.is_open())
				{
					std::cout << "Unable to write to file " << fileName << std::endl;
				}
				else
				{
					autoTypecast = "";
					switch(theElm.thePropertySection.properties[j].format)
					{
					case JVX_DATAFORMAT_DATA:
						autoTypecast = JVX_DATA_DESCRIPTOR;
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						autoTypecast = "int64";
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						autoTypecast = "int32";
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						autoTypecast = "int16";
						break;
					case JVX_DATAFORMAT_8BIT:
						autoTypecast = "int8";
						break;

					}
					std::ostringstream streamContent;

					streamContent << "% Matlab file for accessing (set) property " << theElm.thePropertySection.properties[j].name << std::endl;
					streamContent << "% Property related to entry in property description file " << std::flush;
					streamContent << theElm.thePropertySection.properties[j].reference.fname << ", starting in line " <<
						theElm.thePropertySection.properties[j].reference.lineno << std::endl;

					if(theElm.thePropertySection.properties[j].format == JVX_DATAFORMAT_SELECTION_LIST)
					{
						if(theElm.thePropertySection.properties[j].format == JVX_DATAFORMAT_SELECTION_LIST)
						{
							for(k = 0; k < theElm.thePropertySection.properties[j].selection.strings.size(); k++)
							{
								streamContent << "% Selection 0x" << jvx_intx2String((1 << k)) << "(" << jvx_int2String((1 << k)) << "): " << theElm.thePropertySection.properties[j].selection.strings[k] << std::endl;
							}
						}
					}

					streamContent << "function " << funcName_set << "(in, prop_tag)" << std::endl;
					streamContent << "\tprop_search_tag = '" << Cptp << "';" << std::endl;
					streamContent << "\tif(nargin >= 2)" << std::endl;
					streamContent << "\t\tprop_search_tag = prop_tag;" << std::endl;
					streamContent << "\tend" << std::endl;
					streamContent << "\tglobal jvx_host_call_global;" << std::endl;
					streamContent << "\ttry" << std::endl;

					if (prefix.empty())
					{
						streamContent << "\t\t%% " << "propPrefix = <" << propPrefix << ">, Descriptor = <"
							<< theElm.thePropertySection.properties[j].descriptor << ">" << std::endl;
						streamContent << "\t\t[p0 p1] = jvx_host_call_global('set_property_descriptor', prop_search_tag, '"
							<< jvx_makePathExpr("", theElm.thePropertySection.properties[j].descriptor) 
							<< "', " << autoTypecast << "(in));" << std::endl;
					}
					else
					{
						streamContent << "\t\t%% " << "Prefix = <" << prefix << ">, propPrefix = <"
							<< propPrefix << ">, Descriptor = <" << theElm.thePropertySection.properties[j].descriptor << ">" << std::endl;
						streamContent << "\t\t[p0 p1] = jvx_host_call_global('set_property_descriptor', prop_search_tag, '"
							<< jvx_makePathExpr(prefix, theElm.thePropertySection.properties[j].descriptor) 
							<< "', " << autoTypecast << "(in));" << std::endl;
					}

					streamContent << "\t\tif(~p0)" << std::endl;
					streamContent << "\t\t\twarning(['Error in set_property: ', " << "p1.DESCRIPTION_STRING]);" << std::endl;
					streamContent << "\t\tend" << std::endl;
					streamContent << "\tcatch ME" << std::endl;
					streamContent << "\t\tp0 = functions(jvx_host_call_cb);" << std::endl;
					streamContent << "\t\twarning(['Function <' p0.function '> failed, reason: ' ME.message]);" << std::endl;
					streamContent << "\tend" << std::endl;

					//std::cerr << "--> " << fileName << std::endl;
					//std::cerr << (std::string)streamContent.str() << std::flush;
					osOutFile << (std::string)streamContent.str() << std::flush;
					osOutFile.close();

					// Do we want to execute an external command for conversion to p-file??
				}
			}
		}
	}
}

void
textProcessor_core::generateCode_mat(const std::string& outDir, std::string& Cptp, std::string& Cppf, jvxSize numCharsParts)
{
	int i;

	for(i = 0; i < intermediateStruct.thePropertyElements.size(); i++)
	{
		generateCode_oneElement_mat(intermediateStruct.thePropertyElements[i], outDir, Cptp, Cppf, "", "", numCharsParts);
	}
}
