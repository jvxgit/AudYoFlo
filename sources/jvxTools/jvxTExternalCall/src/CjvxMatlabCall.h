
#ifndef _CJVXEXTERNALCALL_H__
#define _CJVXEXTERNALCALL_H__

#include <string>
#include <vector>
#include <list>

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#endif
#endif

#include "mex.h"
#include "jvx.h"
#include "CjvxCToMatlabConverter.h"
#include "common/CjvxObject.h"

#include <list>

#define rtpGarbageCollection std::list<void*>
#define rtpNewGarbage std::list<void*>

/**
 * Class to manage the call of C functions from External.
 * An application in C has tp provide and register function pointer referenes
 * which can be called from within External.
 *///=====================================================
class CjvxExternalCall: public IjvxExternalCall, public CjvxCToMatlabConverter, public CjvxObject
{
public:

	//! Element type to store the registered functions and classes
	typedef struct
	{
		//! Pointer to the object for callback
		IjvxExternalCallTarget* fObject;

		//! Registered function
		std::string objectName;

		//! Registered function
		std::string fName;

		//! Registered function
		std::string fDescription;

		std::vector<std::string> fdescrsInputParams;

		std::vector<std::string> fdescrsOutputParams;

		//! Pointer to the function
		jvxExternalFunction fPtr;
	} elementFunctions;

	//! Lookup table for registered functions
	std::vector<elementFunctions> _lst_functions;

	JVX_THREAD_ID threadIdMatlab;

public:

	//! Constructor
	JVX_CALLINGCONVENTION CjvxExternalCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor
	JVX_CALLINGCONVENTION ~CjvxExternalCall();

	// ==========
	virtual jvxErrorType JVX_CALLINGCONVENTION registerThreadId() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregisterThreadId()override;

	// =================================================================================

	//! Register a specific function to be callable from Matlab (or other external interpreter)
	virtual jvxErrorType JVX_CALLINGCONVENTION registerFunction(const char* objectName,
								    const char* fName, jvxExternalFunction fPtr,
		const char* fDescription = "", IjvxExternalCallTarget* fObject = NULL, const char** inParameters = NULL,
		int numInParameters = 0, const char** outParameters = NULL, int numOutPars = 0)override;

	//! Function to unregister a new callback
	virtual jvxErrorType JVX_CALLINGCONVENTION unregisterFunction(jvxExternalFunction fPtr)override;

	jvxErrorType obtain_reference(jvxSize* idRef) override;
	jvxErrorType release_reference(jvxSize idRef) override;

	// =================================================================================

	//! External C conversion functions, dimY x dimX matrix of different RTProc dataformat
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**,
								      const jvxHandle** fieldInput,
								      jvxInt32 dimY, jvxInt32 dimX,
								      jvxDataFormat processingFormat, jvxBool cplx = false) override;

	//! External C conversion functions, 1 x dimX matrix of different RTProc dataformat
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**,
								      const jvxHandle* fieldInput, jvxInt32 dimX,
								      jvxDataFormat processingFormat, jvxBool cplx = false) override;

	//! Convert a string (const char*) into a Matlab string
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const char* textInput) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const char** dict_tag,
								      const char** dict_value , jvxSize numEntries)override;
	
	// =================================================================================

	//! Convert external (Matlab) datatype into a C array (which must be allocated before) - here dimY x dimX matrix
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
		jvxDataFormat processingFormat, const jvxExternalDataType* ptr, const char* nameVar = "",
								      jvxBool convertFloat = false)override;

	//! Convert external (Matlab) datatype into a C array (which must be allocated before) - here 1 x dimX matrix
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(void* fieldOutput, unsigned dimX, jvxDataFormat processingFormat,
		const jvxExternalDataType* ptr, const char* nameVar = "", jvxBool convertFloat = false)override;

	//! Convert external (Matlab) string into a C string (RTProc string)
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(jvxApiString* textInput, const jvxExternalDataType* ptr, const char* nameVar = "")override;

	// =================================================================================

	//! Given an external (Matlab) datafield, get the properties of this field
	virtual jvxErrorType JVX_CALLINGCONVENTION getPropertiesVariable(const jvxExternalDataType* var, jvxDataFormat* format = NULL, bool* isString = NULL,
		jvxInt32* dimX = NULL, jvxInt32* dimY = NULL, jvxBool* convertToFloat = NULL)override;

	// =================================================================================

	//! Function to pass a variable to the external (Matlab) workspace
	virtual jvxErrorType JVX_CALLINGCONVENTION putVariableToExternal(const char* externalInformation, const char* variableName,
		jvxExternalDataType* data)override;

	//! Function to return a variable from the external (Matlab) workspace, identified by variable name
	virtual jvxErrorType JVX_CALLINGCONVENTION getVariableFromExternal(const char* externalInformation,
									   const char* variableName,
		jvxExternalDataType** data)override;

	//! Execute a command in the externally connected interpreter
	virtual jvxErrorType JVX_CALLINGCONVENTION executeExternalCommand(const char* evalString, jvxBool catchAllExceptions = true)override;

	//! Post a message to the external viewer canvas/commandwindow/whatever
	virtual jvxErrorType JVX_CALLINGCONVENTION postMessageExternal(const char* message, bool isError = true) override;

	// =================================================================================

	//! If a variable was returned by the getvariable, we need to destroy it once the content has been copied
	virtual jvxErrorType JVX_CALLINGCONVENTION destroyExternalVariable(jvxExternalDataType* data)override;

	//! If a function call failed, this function ionforms about the reason for the failure
	virtual jvxErrorType JVX_CALLINGCONVENTION getLastErrorReason(jvxApiString* errStr)override;

	//! If an jvxString has been returned, the returned string can be released by calling this function
	//virtual jvxErrorType JVX_CALLINGCONVENTION releaseString(jvxString* errStr);

	virtual jvxErrorType JVX_CALLINGCONVENTION rtpPrintf(const char* text)override;


	virtual jvxErrorType JVX_CALLINGCONVENTION external_call( jvxInt32 nlhs, jvxExternalDataType **plhs, jvxInt32 nrhs, const jvxExternalDataType **prhs, int offset_nlhs, int offset_nrhs )override;

	virtual jvxErrorType JVX_CALLINGCONVENTION fill_empty(jvxExternalDataType** plhs, jvxSize nlhs, jvxSize offset)override;

		//virtual bool JVX_CALLINGCONVENTION convertMatlabToC(mxArray* ptrMxArray, TrtpGenericRuntime* templ);

	//! Function to send a runtime message package to the algorithm component, returns false if not supported
	bool checkThread();

	//! Helper: Convert string into Matlab type object
	//bool mexReturnString(mxArray*& plhs, const std::string& str);

	//! Helper: Convert list of strings into Matlab type object
	//bool mexReturnStringCell(mxArray*& plhs, const std::vector<std::string>& lstStr);

	//! Convert a registered function into a Matlab type struct.
	bool mexReturnStructFunction(mxArray*& plhs,const std::string& objectName,
								const std::string& fName,
								const std::string& fDescription,
								const std::vector<std::string>& strListIn,
								const std::vector<std::string>& strListOut);

	//! Convert all registered functions into a cell array of structs
	bool mexReturnStructsFunctions(mxArray*& plhs, const std::vector<elementFunctions>& lstFcts);

	//! Fill all lhs parameters with empty fields
	bool mexFillEmpty(jvxSize nlhs, mxArray* plhs[]);

	// ==============================================================
	// This a cross reference to the Matlab host in order to reuse the
	// code for generic configuration
	// ==============================================================



	// ==============================================================
#include "codeFragments/simplify/jvxObjects_simplify.h"


};

//! API: Function to get access to the Matlab Call handler
//matlabCallHandler* getReferenceMatlabCallHandler();

#endif
