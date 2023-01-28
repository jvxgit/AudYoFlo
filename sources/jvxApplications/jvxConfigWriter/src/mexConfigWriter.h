#ifndef __MEXFILEWRITER_H__
#define __MEXFILEWRITER_H__

#include <vector>
#include "jvx.h"
#include "mexConstants.h"
#include "CjvxCToMatlabConverter.h"

/**
 * Object to wrap the fileReading functionality. This class dispatches 
 * MEX functioncalls to the underlying library component for fileReading
 *///=================================================================
class mexConfigWriter: public CjvxCToMatlabConverter
{
	struct
	{
		IjvxConfigProcessor* theHdl;
		IjvxObject* theObj;
	} theWriter;


public:

	//! Constructor
	mexConfigWriter(void);

	jvxErrorType init();
	//! Destructor
	~mexConfigWriter(void);

	jvxErrorType terminate();

	jvxErrorType produceFile(std::string fName, const mxArray* inputStruct, mxArray*& returnStruct);
	
	jvxErrorType writeStruct(const mxArray* inputStruct, IjvxConfigProcessor* process, jvxConfigData* theSectionAll);
	
	jvxErrorType writeNumeric(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionWriteTo, std::string name);

	jvxErrorType writeCharacter(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionWriteTo, std::string name);

	jvxErrorType writeCell(const mxArray* inputStruct, IjvxConfigProcessor* process, jvxConfigData*& theSectionWriteTo, std::string name);


	jvxErrorType decideType(const mxArray* inputStruct, IjvxConfigProcessor* processor, jvxConfigData*& theSectionProduce, std::string name);

};

#endif
