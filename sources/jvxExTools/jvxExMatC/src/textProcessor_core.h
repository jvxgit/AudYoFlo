#ifndef __TEXTPROCESSOR_CORE_H__
#define __TEXTPROCESSOR_CORE_H__

#include "jvx.h"

class textProcessor_core
{
private:
	typedef enum
	{
		MAT_TYPE_DATA,
		MAT_TYPE_SIZE,
		MAT_TYPE_INT64,
		MAT_TYPE_INT32,
		MAT_TYPE_INT16,
		MAT_TYPE_INT8,
		MAT_TYPE_UINT64,
		MAT_TYPE_UINT32,
		MAT_TYPE_UINT16,
		MAT_TYPE_UINT8,
		MAT_TYPE_STRING, 
		MAT_TYPE_INHERIT, 
		MAT_TYPE_MXARRAY, 
		MAT_TYPE_NONE
	} dataTypes;

	typedef enum
	{
		MAT_DIM_2D,
		MAT_DIM_1D,
		MAT_DIM_0D
	} dimensionField;

	typedef struct
	{
		dataTypes type;
		dimensionField dimension;
		int dimX;
		int dimY;
		std::string description;
	} oneArgumentIn;

	typedef struct
	{
		dataTypes type;
		dimensionField dimension;
		int dimX;
		int dimY;
		int refX2X;
		int refY2Y;
		int refX2Y;
		int refY2X;
		int refV2Y;
		int refV2X;
		int refType;
		std::string description;
	} oneArgumentOut;

	typedef struct
	{
		std::string nameFunctionC;
		std::string nameFunctionExpose;
		std::string descriptionFunctionExpose;
		std::string nameFunctionCpp;
		std::vector<oneArgumentIn> inputArguments;
		jvxInt32 acceptInputMin;
		jvxInt32 acceptInputMax;
		std::vector<oneArgumentOut> outputArguments;
		jvxInt32 acceptOutputMin;
		jvxInt32 acceptOutputMax;
		bool generateBackwardLink;
	}oneFunction;

	typedef struct
	{
		bool generateForLibraryUse;
		std::string nameReferenceClass;
		std::string friendlyNameReferenceClass;
		std::vector<oneFunction> lstFunctions;
	} allFunctions;

	allFunctions allFs;
	std::string fileOutputMiddle;
public:

	textProcessor_core(){};
	bool scanInputFromIr(jvxConfigData* theMainSection, IjvxConfigProcessor* theReader, std::string& purefilenameOutput);
	void generateCode(const std::string& outFilenameH, const std::string& outFilenameHP);

};
#endif