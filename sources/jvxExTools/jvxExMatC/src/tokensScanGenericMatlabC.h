/* 
 *****************************************************
 * Filename: tokensScanGenericMatlabC.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/


// This file contains a list of keywords to search for in the
// configuration files. The grammar is fixed, the keywords
// can be easily modified.
#define MXARRAY_TOKEN "JVX_DATAFORMAT_MXARRAY"
#define INHERIT_TOKEN "JVX_DATAFORMAT_INHERIT"
#define DOUBLE_TOKEN "JVX_DATAFORMAT_DATA"
#define FLOAT_TOKEN "JVX_DATAFORMAT_DATA"
#define INT64_TOKEN "JVX_DATAFORMAT_64BIT_LE"
#define INT32_TOKEN "JVX_DATAFORMAT_32BIT_LE"
#define INT16_TOKEN "JVX_DATAFORMAT_16BIT_LE"
#define INT8_TOKEN "JVX_DATAFORMAT_8BIT"
#define DIM2_TOKEN "2D"
#define DIM1_TOKEN "1D"
#define DIM0_TOKEN "0D"
#define STRING_TOKEN "JVX_DATAFORMAT_STRING"
#define MAIN_SEC_TOKEN "MATLAB_EXPORTS"
#define REF_CLASS_TOKEN "REFERENCE_CLASS"
#define REF_CLASS_TOKEN_FRIENDLY "REFERENCE_CLASS_FRIENDLY_NAME"
#define OUTPUTFILE_NAME "OUTPUTFILE_NAME"

#define	NAME "NAME"
#define	DESCRIPTION "DESCRIPTION"
#define	REFERENCE_FUNCTION_CLASS "REFERENCE_FUNCTION_CLASS"
#define	BACKWARDLINK "BACKWARDLINK"
#define	ACCEPT_INPUT_TYPES "ACCEPT_INPUT_TYPES"
#define	ACCEPT_INPUT_DIM_X "ACCEPT_INPUT_DIM_X"
#define	ACCEPT_INPUT_DIM_Y "ACCEPT_INPUT_DIM_Y"
#define	ACCEPT_INPUT_NUMBER_MIN "ACCEPT_INPUT_NUMBER_MIN"
#define	ACCEPT_INPUT_NUMBER_MAX "ACCEPT_INPUT_NUMBER_MAX"
#define	PRODUCE_OUTPUT_TYPES "PRODUCE_OUTPUT_TYPES"
#define	ACCEPT_OUTPUT_NUMBER_MIN "ACCEPT_OUTPUT_NUMBER_MIN"
#define	ACCEPT_OUTPUT_NUMBER_MAX "ACCEPT_OUTPUT_NUMBER_MAX"
#define	ACCEPT_OUTPUT_DIM_X "ACCEPT_OUTPUT_DIM_X"
#define	ACCEPT_OUTPUT_DIM_Y "ACCEPT_OUTPUT_DIM_Y"
#define DIMENSION_INPUT_FIELD "DIMENSION_INPUT_FIELD"
#define DIMENSION_OUTPUT_FIELD "DIMENSION_OUTPUT_FIELD"
#define DESCRIPTION_INPUT_PARAMS "DESCRIPTION_INPUT_PARAMETERS"
#define DESCRIPTION_OUTPUT_PARAMS "DESCRIPTION_OUTPUT_PARAMETERS"
#define USE_MATLAB_CALL_AS_LIBRARY "USE_MATLAB_CALL_AS_LIBRARY"

#define DEFAULT_NAME_REF_CLASS "classRTProc"
#define DEFAULT_PREFIX_FUNCTION "rtpFunctionMatlabCall"
#define INPUT_OUTPUT_CROSS_REFERENCE_X "INPUT_OUTPUT_CROSS_REFERENCE_X"
#define INPUT_OUTPUT_CROSS_REFERENCE_Y "INPUT_OUTPUT_CROSS_REFERENCE_Y"
#define INPUT_OUTPUT_CROSS_REFERENCE_TYPE "INPUT_OUTPUT_CROSS_REFERENCE_TYPE"

#define CLASSNAME "CLASSNAME"
#define COMMENT "COMMENT"
#define POSTFIXALLOCATE "POSTFIXALLOCATE"
#define POSTFIXDEALLOCATE "POSTFIXDEALLOCATE"
#define OUTPUTFILE "OUTPUTFILE"
#define FUNCTIONNAME "FUNCTIONNAME"
#define TYPE "TYPE"
#define ID "ID"
#define NAME "NAME"
#define DESCRIPTION "DESCRIPTION"
#define LENGTH "LENGTH"
#define VALUE "VALUE"
#define INCLUDE_VALUE "INCLUDE_VALUE"
#define STR_TYPE "STR_TYPE"
#define STR_OVERALL_TYPE "STR_OVERALL_TYPE"
#define STR_DESCRIPTION "STR_DESCRIPTION"
#define STR_TEXT "STR_TEXT"
#define SEL_TEXT "SEL_TEXT"
#define SEL_DESCRIPTION "SEL_DESCRIPTION"
#define SEL_ISSELECTED "SEL_ISSELECTED"
#define SEL_ISEXCLUSIVE "SEL_ISEXCLUSIVE"
#define MIN_VALUE "MIN_VALUE"
#define MAX_VALUE "MAX_VALUE"
#define STEPS "STEPS"
#define CAN_SET "CAN_SET"
#define CAN_GET "CAN_GET"
#define NAME_PTRTEMP "ptrTemp"
#define NAME_VARTEMP "varTemp"
#define POSTFIX_CPP ".cpp"
#define POSTFIX_H ".h"
#define POSTFIX_ELEMENT "elm"
#define SUBFIELD "SUBFIELD"
#define NAME_PTR_RETURN "ptrReturn"
#define ID_PTR_RETURN "id"
#define BOOL_TERM_RETURN "terminateBool"
#define INDICATION_RESULT "resSuccess"
#define NUMBER_ELEMENTS "numElem"
#define GET_POINTER_PREFIX "getPointer"
#define SET_POINTER_PREFIX "setPointer"

#define POSTFIX_PROTOTYPES "prototypes"