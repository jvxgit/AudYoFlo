#include "jvx.h"

#include <iostream>
#include <string>
#include <vector>

#include "scan_token_defines.h"
#include "jvxTconfigProcessor.h"

#include "textProcessor_core.h"

void parseInput(char* argv[], int argc, std::string& inputFName, std::string& outFileName_h, std::string& outFileName_hp)
{
	int cnt = 0;
	bool cont = true;

	inputFName = "";
	outFileName_h = "";
	outFileName_hp = "";

	//std::cerr << "Hallo!" << std::endl;
	while(cont && (cnt < argc))
	{
		std::string nextToken = argv[cnt++];
		std::string tmp;

		//std::cerr << nextToken << std::endl;

		if(nextToken == "--out-h")
		{
			tmp = "";
			if(cnt < argc)
			{
				outFileName_h = argv[cnt++];
			}

		}
		else if(nextToken == "--out-hp")
		{
			if(cnt < argc)
			{
				outFileName_hp = argv[cnt++];
			}
		}
		else
		{
			inputFName = nextToken;
		}
	}
}
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
	textProcessor_core textProcessor;

	if(argc < 2)
	{
		std::cout << "Jvx " << argv[0] << " code generator, developed by Javox Solutions GmbH" << std::endl;
		std::cout << "Usage: " << argv[0] << " <input0> [-out-h outh] [--out-hp outhp]" << std::endl;
		return(1);
	}

	std::string inputFName;
	std::string outFileName_h;
	std::string outFileName_hp;

	parseInput(&argv[1], argc-1, inputFName, outFileName_h, outFileName_hp);

	//================================================================
	// Determine the name of the file to generate source code to
	//================================================================
	std::string outFilename = inputFName;
	std::string outFilenameH;
	std::string outFilenameHP;
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

	outFilenameHP = outFilename + "_" + POSTFIX_PROTOTYPES + POSTFIX_H;
	if(!outFileName_hp.empty())
	{
		outFilenameHP = outFileName_hp;
	}


	//	std::cout << "Outputfile: " << outFilename << std::endl;

	//================================================================
	//================================================================



	IjvxObject* theObject = NULL;
	IjvxConfigProcessor* theReader = NULL;
	jvxErrorType res = jvxTconfigProcessor_init(&theObject);
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;

	if( !((res == JVX_NO_ERROR) && theObject))
	{
		std::cerr << "Failed to access jvx object for config processing." << std::endl;
		exit(0);
	}
	theObject->request_specialization(reinterpret_cast<jvxHandle**>(&theReader), &tp, NULL);
	if(!((tp.tp == JVX_COMPONENT_CONFIG_PROCESSOR) && theReader))
	{
		std::cerr << "Failed to access config processor component." << std::endl;
		exit(0);
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
		std::cerr << "Error Reason: " << err.errorDescription.std_str() << std::endl;
		exit(0);
	}

	// If we are here, parsing was successful
	jvxConfigData* theMainSection = NULL;

	//================================================================
	// obtain on intermediate representation for access to read input
	//================================================================
	res = theReader->getConfigurationHandle(&theMainSection);
	assert(res == JVX_NO_ERROR);

	// Store all input data in here..

	if(textProcessor.scanInputFromIr(theMainSection, theReader, purefilenameOutput))
	{
		textProcessor.generateCode(outFilenameH, outFilenameHP);
	}

	return(0);
}
