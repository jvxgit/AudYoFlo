#include "jvx.h"

#include <iostream>
#include <string>
#include <vector>

#include "scan_token_defines.h"
#include "jvxTconfigProcessor.h"

#include "textProcessor_core.h"

#ifdef JVX_PROP_MATLAB_OUTPUT
void parseInput(char* argv[], int argc, std::string& inputFName, std::string& outDir, std::string& outCompText, 
		std::string& outCompPrefix, std::vector<std::string>& includePaths, 
		jvxBool& dependencyMode, std::vector<std::string>& ifdefs, jvxSize& maxLength)
{
	int cnt = 0;
	bool cont = true;

	inputFName = "";
	outDir = "";
	outCompText = "";
	outCompPrefix = "";

	maxLength = JVX_SIZE_UNSELECTED;

	//std::cerr << "Hallo!" << std::endl;
	while(cont && (cnt < argc))
	{
		std::string nextToken = argv[cnt++];
		std::string tmp;

		//std::cerr << nextToken << std::endl;

		if(nextToken == "-o")
		{
			tmp = "";
			if(cnt < argc)
			{
				outDir = argv[cnt++];
			}

		}
		else if(nextToken == "-ctp")
		{
			tmp = "";
			if(cnt < argc)
			{
				outCompText = argv[cnt++];
			}

		}
		else if(nextToken == "-cpf")
		{
			tmp = "";
			if(cnt < argc)
			{
				outCompPrefix = argv[cnt++];
			}

		}
		else if(nextToken == "-maxl")
		{
			tmp = "";
			if(cnt < argc)
			{
				jvxBool err = false;
				maxLength = jvx_string2Size(argv[cnt], err);
				if(err != false)
				{
					std::cout << __FUNCTION__ << ": Warning: Failed to read option <maxl> which should be followed by a number. Instead, I found <" << argv[cnt] << ">." << std::endl;
					maxLength = JVX_SIZE_UNSELECTED;
				}
				cnt++;
			}

		}
		else if(nextToken == "-M")
		{
			dependencyMode	= true;
		}
		else if(nextToken == "-I")
		{
			tmp = "";
			if(cnt < argc)
			{
				includePaths.push_back(argv[cnt++]);
			}

		}
		else if(nextToken == "-D")
		{
			tmp = "";
			if(cnt < argc)
			{
				ifdefs.push_back(argv[cnt++]);
			}

		}
		else
		{
			inputFName = nextToken;
		}
	}
}
#else
void parseInput(char* argv[], int argc, std::string& inputFName, std::string& outName, 
	std::vector<std::string>& includePaths, jvxBool& dependencyMode, 
	jvxBool& pluginSupport, jvxBool& generateLineInfo, std::vector<std::string>& ifdefs,
	std::string& assClassName)
{
	int cnt = 0;
	bool cont = true;

	inputFName = "";
	outName = "";

	//std::cerr << "Hallo!" << std::endl;
	while(cont && (cnt < argc))
	{
		std::string nextToken = argv[cnt++];
		std::string tmp;

		//std::cerr << nextToken << std::endl;

		if(nextToken == "-o")
		{
			tmp = "";
			if(cnt < argc)
			{
				outName = argv[cnt++];
			}

		}
		else if(nextToken == "-M")
		{
			dependencyMode	= true;
		}
		else if(nextToken == "-I")
		{
			tmp = "";
			if(cnt < argc)
			{
				includePaths.push_back(argv[cnt++]);
			}

		}
		else if(nextToken == "-D")
		{
			tmp = "";
			if(cnt < argc)
			{
				ifdefs.push_back(argv[cnt++]);
			}

		}
		else if (nextToken == "-C")
		{
			tmp = "";
			if (cnt < argc)
			{
				assClassName = argv[cnt++];
			}
		}
		else if (nextToken == "-P")
		{
			pluginSupport = true;
		}
		else if (nextToken == "-L")
		{
			generateLineInfo = true;
		}
		else
		{
			inputFName = nextToken;
		}
	}
}
#endif

//================================================================================
// GENERIC RUNTIME CONFGURATION COMPILER (GRCC)
//================================================================================

/**
* Main function: Read in a text file and generate .cpp and .h for the parsed input
* as long there are no errors returned during parse.
* The output is first stored in a string stream. Right at the end this string
* stream will be generated to a file.
*///================================================================================
int
main(int argc, char** argv)
{
	jvxSize i;
	textProcessor_core textProcessor;
	jvxBool dependencyMode = false;
	jvxBool pluginSupportMode = false;
	jvxBool generateLineInfo = false;

#ifdef JVX_PROP_MATLAB_OUTPUT
	if(argc < 2)
	{
		std::cout << "Jvx " << argv[0] << " code generator, developed by Javox Solutions GmbH" << std::endl;
		std::cout << "Usage: " << argv[0] << " <input0> [-o outDIr] [-I includeDir] [-M]" << std::endl;
		return(1);
	}
#else
	if(argc < 2)
	{
		std::cout << "Jvx " << argv[0] << " code generator, developed by Javox Solutions GmbH" << std::endl;
		std::cout << "Usage: " << argv[0] << " <input0> [-o outfile]" << std::endl;
		return(1);
	}
#endif

	std::string inputFName;
	std::vector<std::string> includePaths;
	std::vector<std::string> ifdefs;
	jvxSize maxLength = JVX_SIZE_UNSELECTED;
	std::string assClassName;

#ifdef JVX_PROP_MATLAB_OUTPUT
	std::string outDir;
	std::string outCptp;
	std::string outCppf;

	parseInput(&argv[1], argc-1, inputFName, outDir, outCptp, outCppf, includePaths, dependencyMode, ifdefs, maxLength);

	//================================================================
	// Determine the name of the file to generate source code to
	//================================================================
	std::string outFilename = inputFName;
	std::string outFilenameH;
	std::string purefilenameOutput = "UNKNOWN";

	std::string::size_type indDot = outFilename.rfind(".");
	if(indDot != std::string::npos)
		outFilename = outFilename.substr(0, indDot);

	std::string::size_type indfilename = outFilename.rfind(JVX_SEPARATOR_DIR);
	if(indfilename != std::string::npos)
		purefilenameOutput = outFilename.substr(indfilename+1, std::string::npos);
	else
		purefilenameOutput = outFilename;

#ifdef JVX_OS_WINDOWS
	indfilename = purefilenameOutput.rfind(JVX_SEPARATOR_DIR_THE_OTHER);
	if(indfilename != std::string::npos)
		purefilenameOutput = purefilenameOutput.substr(indfilename+1, std::string::npos);
	else
		purefilenameOutput = purefilenameOutput;
#endif
#else

	std::string outFileName_h;
	parseInput(&argv[1], argc-1, inputFName, outFileName_h, includePaths, dependencyMode, pluginSupportMode, generateLineInfo, ifdefs, assClassName);

	//================================================================
	// Determine the name of the file to generate source code to
	//================================================================
	std::string outFilename = inputFName;
	std::string outFilenameH;
#ifndef JVX_PROP_MATLAB_OUTPUT
	std::string outFilenameDirectCpp;
	std::string outFilenameDirectH;
#endif
	std::string purefilenameOutput = "UNKNOWN";

	std::string::size_type indDot = outFilename.rfind(".");
	if(indDot != std::string::npos)
		outFilename = outFilename.substr(0, indDot);

	std::string::size_type indfilename = outFilename.rfind(JVX_SEPARATOR_DIR);
	if(indfilename != std::string::npos)
		purefilenameOutput = outFilename.substr(indfilename+1, std::string::npos);
	else
		purefilenameOutput = outFilename;

#ifdef JVX_OS_WINDOWS
	indfilename = purefilenameOutput.rfind(JVX_SEPARATOR_DIR_THE_OTHER);
	if(indfilename != std::string::npos)
		purefilenameOutput = purefilenameOutput.substr(indfilename+1, std::string::npos);
	else
		purefilenameOutput = purefilenameOutput;
#endif

	outFilenameH = outFilename + POSTFIX_H;
	if(!outFileName_h.empty())
	{
		outFilenameH = outFileName_h;
	}

#ifndef JVX_PROP_MATLAB_OUTPUT
	auto idxBwd = outFilenameH.rfind('.');
	assert(idxBwd != std::string::npos);
	outFilenameDirectCpp = outFilenameH.substr(0, idxBwd) + "__assoc_cpp" + POSTFIX_H;
	outFilenameDirectH = outFilenameH.substr(0, idxBwd) + "__assoc__h" + POSTFIX_H;
#endif

	//	std::cout << "Outputfile: " << outFilename << std::endl;
#endif
	//================================================================
	//================================================================



	IjvxObject* theObject = NULL;
	IjvxConfigProcessor* theReader = NULL;
	jvxErrorType res = jvxTconfigProcessor_init(&theObject);
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;

	if( !((res == JVX_NO_ERROR) && theObject))
	{
		std::cerr << "Failed to access jvx object for config processing." << std::endl;
		return(-1);
	}
	theObject->request_specialization(reinterpret_cast<jvxHandle**>(&theReader), &tp, NULL);
	if(!((tp.tp == JVX_COMPONENT_CONFIG_PROCESSOR) && theReader))
	{
		std::cerr << "Failed to access config processor component." << std::endl;
		return(-1);
	}

	//================================================================
	//================================================================
	for(i = 0; i < includePaths.size(); i++)
	{
		theReader->addIncludePath(includePaths[i].c_str());
	}

	//================================================================
	//================================================================
	for(i = 0; i < ifdefs.size(); i++)
	{
		theReader->addIfdefToken(ifdefs[i].c_str());
	}
	//================================================================
	// Parse input
	//================================================================
	// FILEREADER LIBRARY: Initialize
	res = theReader->parseFile(inputFName.c_str());
	if(res != JVX_NO_ERROR)
	{
		jvxApiError err;
		std::cerr << "Parsing input file " + inputFName + " failed!" << std::endl;
		theReader->getParseError(&err);
		std::cerr << err.errorDescription.std_str() << std::endl;
		return(-1);
	}

	// If we are here, parsing was successful
	jvxConfigData* theMainSection = NULL;

	//================================================================
	// obtain on intermediate representation for access to read input
	//================================================================
	res = theReader->getConfigurationHandle(&theMainSection);
	assert(res == JVX_NO_ERROR);

	//jvxString* fldStr = NULL;
	//theReader->printConfiguration(theMainSection, &fldStr);
	//std::cout << fldStr->bString << std::endl;
	// Store all input data in here..

	if(textProcessor.scanInputFromIr(theMainSection, theReader, purefilenameOutput, pluginSupportMode, assClassName))
	{
		if(textProcessor.checkForConsistency())
		{
			if(dependencyMode)
			{
				jvxSize numberIncludes = 0;
				jvxApiString fldStr;
				theReader->getNumberIncludes(&numberIncludes);
				for(i= 0; i < numberIncludes; i++)
				{
					theReader->getInclude_id(&fldStr, i);
					std::cout << fldStr.std_str() << ";";
				}
			}
			else
			{
#ifdef JVX_PROP_MATLAB_OUTPUT
				//std::cerr << "-->" << outDir << std::endl;
				textProcessor.generateCode_mat(outDir, outCptp, outCppf, maxLength);
#else
				textProcessor.generateCode_c(outFilenameH/*outFilename*/, 
					outFilenameDirectCpp, 
					outFilenameDirectH, 
					generateLineInfo);
#endif
			}
			return(0);
		}
	}
	return(-2);
}
