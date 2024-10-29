#ifndef _IJVXEXTERNALHANDLER_H__
#define _IJVXEXTERNALHANDLER_H__

JVX_INTERFACE IjvxExternalCall: public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxExternalCall(){};

	// ==========
	virtual jvxErrorType JVX_CALLINGCONVENTION registerThreadId() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregisterThreadId() = 0;

	// =================================================================================

	// Function entry for Matlab hosts
	virtual jvxErrorType JVX_CALLINGCONVENTION external_call( jvxInt32 nlhs, jvxExternalDataType **plhs, jvxInt32 nrhs, const jvxExternalDataType **prhs, int offset_nlhs, int offset_nrhs  ) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION fill_empty(jvxExternalDataType** plhs, jvxSize nlhs, jvxSize offset) = 0;

	// =================================================================================

	//! Register a specific function to be callable from Matlab (or other external interpreter)
	virtual jvxErrorType JVX_CALLINGCONVENTION registerFunction(const char* objectName, const char* fName, jvxExternalFunction fPtr,
		const char* fDescription = "", IjvxExternalCallTarget* fObject = NULL, const char** inParameters = NULL,
		int numInParameters = 0, const char** outParameters = NULL, int numOutPars = 0) = 0;

	//! Function to unregister a new callback
	virtual jvxErrorType JVX_CALLINGCONVENTION unregisterFunction(jvxExternalFunction fPtr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION obtain_reference(jvxSize* idRef) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION release_reference(jvxSize idRef) = 0;

	// =================================================================================

	//! External C conversion functions, dimY x dimX matrix of different RTProc dataformat
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool cplx = false) = 0;

	//! External C conversion functions, 1 x dimX matrix of different RTProc dataformat
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const jvxHandle* fieldInput, jvxInt32 dimX, jvxDataFormat processingFormat, jvxBool cplx = false)=0;

	//! Convert a string (const char*) into a Matlab string
	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const char* textInput) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION convertCToExternal(jvxExternalDataType**, const char** dict_tag, const char** dict_value, jvxSize numEntries) = 0;

	// =================================================================================

	//! Convert external (Matlab) datatype into a C array (which must be allocated before) - here dimY x dimX matrix
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
		jvxDataFormat processingFormat, const jvxExternalDataType* ptr, const char* nameVar = "", jvxBool convFloat = false, jvxBool outputComplex = false)=0;

	//! Convert external (Matlab) datatype into a C array (which must be allocated before) - here 1 x dimX matrix
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(void* fieldOutput, unsigned dimX, jvxDataFormat processingFormat,
		const jvxExternalDataType* ptr, const char* nameVar = "", jvxBool convFloat = false, jvxBool outputComplex = false)=0;

	//! Convert external (Matlab) string into a C string (RTProc string)
	virtual jvxErrorType JVX_CALLINGCONVENTION convertExternalToC(jvxApiString* textInput, const jvxExternalDataType* ptr, const char* nameVar = "") = 0;

	// =================================================================================

	//! Given an external (Matlab) datafield, get the properties of this field
	virtual jvxErrorType JVX_CALLINGCONVENTION getPropertiesVariable(const jvxExternalDataType* var, jvxDataFormat* format = NULL, bool* isString = NULL,
		jvxInt32* dimX = NULL, jvxInt32* dimY = NULL, jvxBool* convertFloat = NULL) = 0;

	// =================================================================================

	//! Function to pass a variable to the external (Matlab) workspace
	virtual jvxErrorType JVX_CALLINGCONVENTION putVariableToExternal(const char* externalInformation, const char* variableName,
		jvxExternalDataType* data) = 0;

	//! Function to return a variable from the external (Matlab) workspace, identified by variable name
	virtual jvxErrorType JVX_CALLINGCONVENTION getVariableFromExternal(const char* externalInformation, const char* variableName,
		jvxExternalDataType** data) = 0;

	//! Execute a command in the externally connected interpreter
	virtual jvxErrorType JVX_CALLINGCONVENTION executeExternalCommand(const char* evalString, jvxBool catchAllExceptions = true) = 0;

	//! Post a message to the external viewer canvas/commandwindow/whatever
	virtual jvxErrorType JVX_CALLINGCONVENTION postMessageExternal(const char* message, bool isError = true) = 0;

	// =================================================================================

	//! If a variable was returned by the getvariable, we need to destroy it once the content has been copied
	virtual jvxErrorType JVX_CALLINGCONVENTION destroyExternalVariable(jvxExternalDataType* data) = 0;

	//! If a function call failed, this function ionforms about the reason for the failure
	virtual jvxErrorType JVX_CALLINGCONVENTION getLastErrorReason(jvxApiString* errStr) = 0;

	//! If an jvxString has been returned, the returned string can be released by calling this function
	virtual jvxErrorType JVX_CALLINGCONVENTION rtpPrintf(const char* text) = 0;

};

#endif
