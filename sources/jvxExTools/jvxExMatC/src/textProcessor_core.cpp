#include "scan_token_defines.h"
#include "textProcessor_core.h"
#include "jvx-helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define OFFSET__NUMBER_NRHS 3
#define SAFE_CALL_WITH_WARNING(a,b,c) res = a; if(res == JVX_ERROR_WRONG_SECTION_TYPE) {std::string fName; jvxInt32 lineno;HjvxConfigProcessor_readEntry_originEntry(theReader, c, b, fName, lineno); std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << b << " is associated to wrong type." << std::endl;}

bool
textProcessor_core::scanInputFromIr(jvxConfigData* theMainSection, IjvxConfigProcessor* theReader, std::string& purefilenameOutput)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* theContent = NULL;
	jvxConfigData* datTmp = NULL;
	bool foundMainSection = false;


	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Code generation is based on the intermediate representation in the
	// following. The data type is used to browse through the sections, subsections...
	// Strings can be read and used for code generation.
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, OUTPUTFILE_NAME, &purefilenameOutput), OUTPUTFILE_NAME, theContent);
	fileOutputMiddle = purefilenameOutput;

	std::string propertyFld;
	jvxInt32 valI = 0;
	jvxData valD = 0;
	jvxSize i;

	allFs.nameReferenceClass = DEFAULT_NAME_REF_CLASS;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, REF_CLASS_TOKEN, &allFs.nameReferenceClass), REF_CLASS_TOKEN, theContent);;

	allFs.friendlyNameReferenceClass = allFs.nameReferenceClass;
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, REF_CLASS_TOKEN_FRIENDLY, &allFs.friendlyNameReferenceClass), REF_CLASS_TOKEN_FRIENDLY, theContent);;

	allFs.generateForLibraryUse = false;
	propertyFld = "NO";
	SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theMainSection, USE_MATLAB_CALL_AS_LIBRARY, &propertyFld), USE_MATLAB_CALL_AS_LIBRARY, theContent);;
	if(propertyFld == "YES")
	{
		allFs.generateForLibraryUse = true;
	}

	jvxSize numSubSecs = 0;
	theReader->getNumberSubsectionsCurrentSection(theMainSection, &numSubSecs);

	bool detectedError = false;

	for(i = 0; i < numSubSecs; i++)
	{
		theContent = NULL;
		theReader->getReferenceSubsectionCurrentSection_id(theMainSection, &theContent, i);
		if(theContent)
		{
			oneFunction singleFct;

			//=========================
			// Setup the default values
			//=========================
			singleFct.acceptInputMax = 0;
			singleFct.acceptInputMin = 0;
			singleFct.acceptOutputMax = 0;
			singleFct.acceptOutputMin = 0;
			singleFct.generateBackwardLink = false;
			singleFct.descriptionFunctionExpose = "No description available.";

			singleFct.nameFunctionC = "undefined";
			HjvxConfigProcessor_readEntry_nameEntry(theReader, theContent, singleFct.nameFunctionC);
			singleFct.nameFunctionCpp = singleFct.nameFunctionC;
			singleFct.nameFunctionExpose = singleFct.nameFunctionC;

			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, NAME, &singleFct.nameFunctionExpose), NAME, theContent);

			singleFct.descriptionFunctionExpose = singleFct.nameFunctionExpose;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, DESCRIPTION, &singleFct.descriptionFunctionExpose), DESCRIPTION, theContent);;

			singleFct.nameFunctionCpp = singleFct.nameFunctionC;//DEFAULT_PREFIX_FUNCTION "__" + int2String(i);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, REFERENCE_FUNCTION_CLASS, &singleFct.nameFunctionCpp), REFERENCE_FUNCTION_CLASS, theContent);;

			propertyFld = "NO";
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentString(theReader, theContent, BACKWARDLINK, &propertyFld), BACKWARDLINK, theContent);;

			if(propertyFld == "YES")
			{
				singleFct.generateBackwardLink = true;
			}

			//=============================================
			// Scanning properties of input arguments
			//=============================================
			std::vector<std::string> lstNewIn;
			std::vector<jvxInt32> iList_dimX;
			std::vector<jvxInt32> iList_dimY;
			std::vector<std::string> sList_dim;
			std::vector<std::string> descrList_dim;

			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, ACCEPT_INPUT_TYPES, &lstNewIn), ACCEPT_INPUT_TYPES, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, ACCEPT_INPUT_DIM_X, 0, iList_dimX), ACCEPT_INPUT_DIM_X, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, ACCEPT_INPUT_DIM_Y, 0, iList_dimY), ACCEPT_INPUT_DIM_Y, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, DIMENSION_INPUT_FIELD, &sList_dim), DIMENSION_INPUT_FIELD, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, DESCRIPTION_INPUT_PARAMS, &descrList_dim), DESCRIPTION_INPUT_PARAMS, theContent);;

			jvxSize numInputParams = 0;
			numInputParams = JVX_MAX(numInputParams, lstNewIn.size());
			numInputParams = JVX_MAX(numInputParams, iList_dimX.size());
			numInputParams = JVX_MAX(numInputParams, iList_dimY.size());
			numInputParams = JVX_MAX(numInputParams, sList_dim.size());
			numInputParams = JVX_MAX(numInputParams, descrList_dim.size());

			singleFct.acceptInputMin = (jvxInt32)numInputParams;
			singleFct.acceptInputMax = (jvxInt32)numInputParams;

			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theContent, ACCEPT_INPUT_NUMBER_MIN, &singleFct.acceptInputMin), ACCEPT_INPUT_NUMBER_MIN, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theContent, ACCEPT_INPUT_NUMBER_MAX, &singleFct.acceptInputMax), ACCEPT_INPUT_NUMBER_MAX, theContent);;

			for(unsigned int i = 0; i < numInputParams; i++)
			{
				//=============================================
				// Set the default values
				//=============================================
				oneArgumentIn argumentIn;
				argumentIn.dimX = 0;
				argumentIn.dimY = 0;
				argumentIn.dimension = MAT_DIM_2D;
				argumentIn.type = MAT_TYPE_NONE;
				argumentIn.description = "Input Parameter " + jvx_int2String(i);

				//=============================================
				// Check the type of the input argument
				//=============================================
				if(i < lstNewIn.size())
				{
					if(lstNewIn[i] == INHERIT_TOKEN)
					{
						argumentIn.type = MAT_TYPE_INHERIT;
					}
					else if(lstNewIn[i] == MXARRAY_TOKEN)
					{
						argumentIn.type = MAT_TYPE_MXARRAY;
					}
					else if(lstNewIn[i] == DATA_TOKEN)
					{
						argumentIn.type = MAT_TYPE_DATA;
					}
					else if(lstNewIn[i] == SIZE_TOKEN)
					{
						argumentIn.type = MAT_TYPE_SIZE;
					}
					
					else if(lstNewIn[i] == INT64_TOKEN)
					{
						argumentIn.type = MAT_TYPE_INT64;
					}
					else if(lstNewIn[i] == INT32_TOKEN)
					{
						argumentIn.type = MAT_TYPE_INT32;
					}
					else if(lstNewIn[i] == INT16_TOKEN)
					{
						argumentIn.type = MAT_TYPE_INT16;
					}
					else if(lstNewIn[i] == INT8_TOKEN)
					{
						argumentIn.type = MAT_TYPE_INT8;
					}
					
					else if(lstNewIn[i] == UINT64_TOKEN)
					{
						argumentIn.type = MAT_TYPE_UINT64;
					}
					else if(lstNewIn[i] == UINT32_TOKEN)
					{
						argumentIn.type = MAT_TYPE_UINT32;
					}
					else if(lstNewIn[i] == UINT16_TOKEN)
					{
						argumentIn.type = MAT_TYPE_UINT16;
					}
					else if(lstNewIn[i] == UINT8_TOKEN)
					{
						argumentIn.type = MAT_TYPE_UINT8;
					}
					
					else if(lstNewIn[i] == STRING_TOKEN)
					{
						argumentIn.type = MAT_TYPE_STRING;
					}
					else
					{
						std::string fName;
						jvxInt32 lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, ACCEPT_INPUT_TYPES, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input variable datatype " << lstNewIn[i] << " in assignment " << ACCEPT_INPUT_TYPES << "." << std::endl;
						detectedError = true;
					}

					//=============================================
					// Set the dimension in x-dimension, -1 to inherit
					//=============================================
					argumentIn.dimX = -1;
					if(i < iList_dimX.size())
					{
						argumentIn.dimX = iList_dimX[i];
					}

					//=============================================
					// Set the dimension in y-dimension, -1 to inherit
					//=============================================
					argumentIn.dimY = -1;
					if(i < iList_dimY.size())
					{
						argumentIn.dimY = iList_dimY[i];
					}

					//=============================================
					// Get the dimension, 2D, 1D or 0D
					//=============================================
					if(i < sList_dim.size())
					{
						if(sList_dim[i] == DIM2_TOKEN)
						{
							argumentIn.dimension = MAT_DIM_2D;
						}
						else if(sList_dim[i] == DIM1_TOKEN)
						{
							// Set dimension y to 1 in case of 1D
							argumentIn.dimY = 1;
							argumentIn.dimension = MAT_DIM_1D;
						}
						else if(sList_dim[i] == DIM0_TOKEN)
						{
							// Set dimension x and y to 1 and 1 for 0D
							argumentIn.dimY = 1;
							argumentIn.dimX = 1;
							argumentIn.dimension = MAT_DIM_0D;
						}
						else
						{
							std::string fName;
							int lineno;
							HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, DIMENSION_INPUT_FIELD, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown input variable dimension " << sList_dim[i] << " in assignment " << DIMENSION_INPUT_FIELD << "." << std::endl;
							detectedError = true;
						}
					}

					// Get the input parameter description
					if(i < descrList_dim.size())
					{
						argumentIn.description = descrList_dim[i];
					}
					singleFct.inputArguments.push_back(argumentIn);
				}
			} // for(unsigned int i = 0; i < numInputParams; i++)


			//=============================================
			// Scanning properties of output arguments
			//=============================================
			std::vector<std::string> lstNewOut;
			iList_dimX.clear();
			iList_dimY.clear();
			std::vector<jvxInt32> refList_dimX2X;
			std::vector<jvxInt32> refList_dimX2Y;
			std::vector<jvxInt32> refList_dimY2Y;
			std::vector<jvxInt32> refList_dimY2X;
			std::vector<jvxInt32> refList_dimV2Y;
			std::vector<jvxInt32> refList_dimV2X;
			std::vector<jvxInt32> refList_Type;
			sList_dim.clear();
			descrList_dim.clear();

			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, PRODUCE_OUTPUT_TYPES, &lstNewOut), PRODUCE_OUTPUT_TYPES, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, ACCEPT_OUTPUT_DIM_X, 0, iList_dimX), ACCEPT_OUTPUT_DIM_X, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, ACCEPT_OUTPUT_DIM_Y, 0, iList_dimY), ACCEPT_OUTPUT_DIM_Y, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, DIMENSION_OUTPUT_FIELD, &sList_dim), DIMENSION_OUTPUT_FIELD, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignmentStringList(theReader, theContent, DESCRIPTION_OUTPUT_PARAMS, &descrList_dim), DESCRIPTION_OUTPUT_PARAMS, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X2Y, 0, refList_dimX2Y), INPUT_OUTPUT_CROSS_REFERENCE_X2Y, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y2X, 0, refList_dimY2X), INPUT_OUTPUT_CROSS_REFERENCE_Y2X, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2Y, 0, refList_dimV2Y), INPUT_OUTPUT_CROSS_REFERENCE_V2Y, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2X, 0, refList_dimV2X), INPUT_OUTPUT_CROSS_REFERENCE_V2X, theContent);
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_TYPE, 0, refList_Type), INPUT_OUTPUT_CROSS_REFERENCE_TYPE, theContent);

			jvxBool readX = false;
			jvxBool readX2X = true;
			res = HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X2X, 0, refList_dimX2X); 
			if(res == JVX_ERROR_WRONG_SECTION_TYPE) 
			{
				std::string fName; 
				jvxInt32 lineno;
				HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X2X, fName, lineno); 
				std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INPUT_OUTPUT_CROSS_REFERENCE_X2X << " is associated to wrong type." << std::endl;
			}
			else if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
			{
				readX = true;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X, 0, refList_dimX2X), INPUT_OUTPUT_CROSS_REFERENCE_X, theContent);	
			}
			// SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X, 0, refList_dimX2X), INPUT_OUTPUT_CROSS_REFERENCE_X2X, theContent);	
			
			jvxBool readY = false;
			jvxBool readY2Y = true;
			res = HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y2Y, 0, refList_dimY2Y); 
			if(res == JVX_ERROR_WRONG_SECTION_TYPE) 
			{
				std::string fName; 
				jvxInt32 lineno;
				HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y2Y, fName, lineno); 
				std::cerr << fName << "(" << jvx_int2String(lineno) << "): Warning: Entry " << INPUT_OUTPUT_CROSS_REFERENCE_Y2Y << " is associated to wrong type." << std::endl;
			}
			else if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
			{
				readY = true;
				SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y, 0, refList_dimY2Y), INPUT_OUTPUT_CROSS_REFERENCE_Y, theContent);	
			}
			//SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_1dList<jvxInt32>(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y, 0, refList_dimY2Y), INPUT_OUTPUT_CROSS_REFERENCE_Y2Y, theContent);
			
			jvxSize numOutputParams = 0;
			numOutputParams = JVX_MAX(numOutputParams, lstNewOut.size());
			numOutputParams = JVX_MAX(numOutputParams, iList_dimX.size());
			numOutputParams = JVX_MAX(numOutputParams, iList_dimY.size());
			numOutputParams = JVX_MAX(numOutputParams, sList_dim.size());
			numOutputParams = JVX_MAX(numOutputParams, descrList_dim.size());
			numOutputParams = JVX_MAX(numOutputParams, refList_dimX2X.size());
			numOutputParams = JVX_MAX(numOutputParams, refList_dimY2Y.size());
			numOutputParams = JVX_MAX(numOutputParams, refList_dimX2Y.size());
			numOutputParams = JVX_MAX(numOutputParams, refList_dimY2X.size());
			numOutputParams = JVX_MAX(numOutputParams, refList_Type.size());

			singleFct.acceptOutputMin = (jvxInt32)numOutputParams;
			singleFct.acceptOutputMax = (jvxInt32)numOutputParams;

			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theContent, ACCEPT_OUTPUT_NUMBER_MIN, &singleFct.acceptOutputMin), ACCEPT_OUTPUT_NUMBER_MIN, theContent);;
			SAFE_CALL_WITH_WARNING(HjvxConfigProcessor_readEntry_assignment<jvxInt32>(theReader, theContent, ACCEPT_OUTPUT_NUMBER_MAX, &singleFct.acceptOutputMax), ACCEPT_OUTPUT_NUMBER_MAX, theContent);;

			for(unsigned int i = 0; i < numOutputParams; i++)
			{
				//=============================================
				// Setting the default values
				//=============================================
				oneArgumentOut argumentOut;
				argumentOut.dimX = 0;
				argumentOut.dimY = 0;
				argumentOut.dimension = MAT_DIM_2D;
				argumentOut.type = MAT_TYPE_NONE;
				argumentOut.refX2X = i;
				argumentOut.refY2Y = i;
				argumentOut.refX2Y = -1;
				argumentOut.refY2X = -1;
				argumentOut.refV2Y = -1;
				argumentOut.refV2X = -1;
				argumentOut.refType = i;
				argumentOut.description = "Output Parameter " + jvx_int2String(i);

				if(i < lstNewOut.size())
				{
					// Scan for type
					if(lstNewOut[i] == INHERIT_TOKEN)
					{
						argumentOut.type = MAT_TYPE_INHERIT;
					}
					else if(lstNewOut[i] == MXARRAY_TOKEN)
					{
						argumentOut.type = MAT_TYPE_MXARRAY;
					}
					else if(lstNewOut[i] == DATA_TOKEN)
					{
						argumentOut.type = MAT_TYPE_DATA;
					}
					else if(lstNewOut[i] == SIZE_TOKEN)
					{
						argumentOut.type = MAT_TYPE_SIZE;
					}

					else if(lstNewOut[i] == INT64_TOKEN)
					{
						argumentOut.type = MAT_TYPE_INT64;
					}
					else if(lstNewOut[i] == INT32_TOKEN)
					{
						argumentOut.type = MAT_TYPE_INT32;
					}
					else if(lstNewOut[i] == INT16_TOKEN)
					{
						argumentOut.type = MAT_TYPE_INT16;
					}
					else if(lstNewOut[i] == INT8_TOKEN)
					{
						argumentOut.type = MAT_TYPE_INT8;
					}

					else if(lstNewOut[i] == UINT64_TOKEN)
					{
						argumentOut.type = MAT_TYPE_UINT64;
					}
					else if(lstNewOut[i] == UINT32_TOKEN)
					{
						argumentOut.type = MAT_TYPE_UINT32;
					}
					else if(lstNewOut[i] == UINT16_TOKEN)
					{
						argumentOut.type = MAT_TYPE_UINT16;
					}
					else if(lstNewOut[i] == UINT8_TOKEN)
					{
						argumentOut.type = MAT_TYPE_UINT8;
					}

					else if(lstNewOut[i] == STRING_TOKEN)
					{
						argumentOut.type = MAT_TYPE_STRING;
					}
					else
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, PRODUCE_OUTPUT_TYPES, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown output variable datatype " << lstNewOut[i] << " in assignment " << PRODUCE_OUTPUT_TYPES << "." << std::endl;
						detectedError = true;
					}
				}

				// Set the dimension
				argumentOut.dimX = -1;
				if(i < iList_dimX.size())
				{
					argumentOut.dimX = iList_dimX[i];
				}

				// Set the dimension
				argumentOut.dimY = -1;
				if(i < iList_dimY.size())
				{
					argumentOut.dimY = iList_dimY[i];
				}

				// Set variable dimensionality
				if(i < sList_dim.size())
				{
					if(sList_dim[i] == DIM2_TOKEN)
					{
						argumentOut.dimension = MAT_DIM_2D;
					}
					else if(sList_dim[i] == DIM1_TOKEN)
					{
						argumentOut.dimY = 1;
						argumentOut.dimension = MAT_DIM_1D;
					}
					else if(sList_dim[i] == DIM0_TOKEN)
					{
						argumentOut.dimY = 1;
						argumentOut.dimX = 1;
						argumentOut.dimension = MAT_DIM_0D;
					}
					else
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, DIMENSION_OUTPUT_FIELD, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Unknown output variable dimension " << sList_dim[i] << " in assignment " << DIMENSION_OUTPUT_FIELD << "." << std::endl;
						detectedError = true;
					}
				}

				// Create the references for dimension X
				if(i < refList_dimX2X.size())
				{
					if(refList_dimX2X[i] >= 0)
					{
						argumentOut.refX2X = refList_dimX2X[i];
					}

					// Make sure that there is a reference to a valid input argument
					if(argumentOut.refX2X >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						if(readX)
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for X-dimension, value " << refList_dimX2X[i]
								<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_X << "." << std::endl;
							detectedError = true;
						}
						else
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X2X, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for X-dimension, value " << refList_dimX2X[i]
								<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_X2X << "." << std::endl;
							detectedError = true;							
						}
					}
				}
				if(i < refList_dimY2Y.size())
				{
					if(refList_dimY2Y[i] >= 0)
					{
						argumentOut.refY2Y = refList_dimY2Y[i];
					}
					if(argumentOut.refY2Y >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						if(readY)
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, value " << refList_dimY2Y[i]
								<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_Y << "." << std::endl;
							detectedError = true;
						}
						else
						{
							HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y2Y, fName, lineno);
							std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, value " << refList_dimY2Y[i]
								<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_Y2Y << "." << std::endl;
							detectedError = true;							
						}
					}
				}
				
				// If we have XY corss references...
				if(i < refList_dimX2Y.size())
				{
					if(refList_dimX2Y[i] >= 0)
					{
						argumentOut.refX2Y = refList_dimX2Y[i];
					}

					// Make sure that there is a reference to a valid input argument
					if(argumentOut.refX2Y >= (int)lstNewIn.size())
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_X2Y, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, value " << refList_dimX2Y[i]
							<< ", is out of bounds, it should be between 0 and " << lstNewIn.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_X2Y << "." << std::endl;
						detectedError = true;
					}
				}
				if(i < refList_dimY2X.size())
				{
					if(refList_dimY2X[i] >= 0)
					{
						argumentOut.refY2X = refList_dimY2X[i];
					}
					if(argumentOut.refY2X >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_Y2X, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for X-dimension, value " << refList_dimY2X[i]
							<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_Y2X << "." << std::endl;
						detectedError = true;
					}
				}
				
				// If we have XY corss references...
				if(i < refList_dimV2Y.size())
				{
					if(refList_dimV2Y[i] >= 0)
					{
						argumentOut.refV2Y = refList_dimV2Y[i];
					}

					// Make sure that there is a reference to a valid input argument
					if(argumentOut.refV2Y >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2Y, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, value " << refList_dimV2Y[i]
							<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_V2Y << "." << std::endl;
						detectedError = true;
					}
					else
					{
						if(argumentOut.refV2Y >= 0)
						{
							if(
								(singleFct.inputArguments[argumentOut.refV2Y].type != MAT_TYPE_SIZE) ||
								(singleFct.inputArguments[argumentOut.refV2Y].dimension != MAT_DIM_0D))
							{
								std::string fName;
								int lineno;
								HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2Y, fName, lineno);
								std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, referenced value must be <0D> and of type JVX_DATAFORMAT_SIZE in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_V2Y << "." << std::endl;
								detectedError = true;
							}
						}
					}
				}
				if(i < refList_dimV2X.size())
				{
					if(refList_dimV2X[i] >= 0)
					{
						argumentOut.refV2X = refList_dimV2X[i];
					}
					if(argumentOut.refV2X >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2X, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for Y-dimension, value " << refList_dimV2X[i]
							<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_V2X << "." << std::endl;
						detectedError = true;
					}
					else
					{
						if(argumentOut.refV2X >= 0)
						{
							if(
								(singleFct.inputArguments[argumentOut.refV2X].type != MAT_TYPE_SIZE) ||
								(singleFct.inputArguments[argumentOut.refV2X].dimension != MAT_DIM_0D))
							{
								std::string fName;
								int lineno;
								HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_V2X, fName, lineno);
								std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for X-dimension, referenced value must be <0D> and of type JVX_DATAFORMAT_SIZE in assignment " << 
									INPUT_OUTPUT_CROSS_REFERENCE_V2X << "." << std::endl;
								detectedError = true;
							}
						}
					}
				}
				
				if(i < refList_Type.size())
				{
					if(refList_Type[i] >= 0)
					{
						argumentOut.refType = refList_Type[i];
					}
					if(refList_Type[i] >= (int)singleFct.inputArguments.size())
					{
						std::string fName;
						int lineno;
						HjvxConfigProcessor_readEntry_originEntry(theReader, theContent, INPUT_OUTPUT_CROSS_REFERENCE_TYPE, fName, lineno);
						std::cerr << fName << "(" << jvx_int2String(lineno) << "): Error: Crossreference for type, value " << refList_Type[i]
							<< ", is out of bounds, it should be between 0 and " << singleFct.inputArguments.size()-1 << " in assignment " << INPUT_OUTPUT_CROSS_REFERENCE_TYPE << "." << std::endl;
						detectedError = true;
					}
				}

				// Check that type reference is not to a string type
				/*						if(argumentOut.type == MAT_TYPE_INHERIT)
				{
				if(!((singleFct.inputArguments[argumentOut.refType].type == MAT_TYPE_DOUBLE)||
				(singleFct.inputArguments[argumentOut.refType].type == MAT_TYPE_FLOAT)||
				(singleFct.inputArguments[argumentOut.refType].type == MAT_TYPE_32BIT)||
				(singleFct.inputArguments[argumentOut.refType].type == MAT_TYPE_DOUBLE)))
				{
				std::string fName;
				int lineno;
				fldTmp.getLocationStringList(fName, lineno, DIMENSION_OUTPUT_FIELD);									std::cout << fName << "(" << int2String(lineno)
				<< "): Error: Crossreference for type refers to input field id #"
				<< argumentOut.refType << " which is not of right type, it should be "
				<< " DOUBLE, FLOAT, INT64, INT32,INT16, or INT8." << std::endl;
				}
				}
				*/
				// Get the input parameter description
				if(i < descrList_dim.size())
				{
					argumentOut.description = descrList_dim[i];
				}

				singleFct.outputArguments.push_back(argumentOut);
			} // for(unsigned int i = 0; i < numOutputParams; i++)

			// Add function definition to list
			allFs.lstFunctions.push_back(singleFct);
		}
	}
	return(!detectedError);
}


