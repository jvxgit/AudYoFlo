/* 
 *****************************************************
 * Filename: mexConstants.h
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

#define MEX_REPORT_ERROR_CODE_1 "Standard Exception (Segmentation Violation)\nTO: Add DEBUG CAPABILITY.", 1
#define MEX_REPORT_ERROR_CODE_2 "Command argument in#0 is of wrong type (should be a character string)", 2
#define MEX_REPORT_ERROR_CODE_3 ("Command argument " + commandToken + " is not valid."), 3
#define MEX_REPORT_ERROR_CODE_4a ("Command 'initialize' (socket TCP): Expect 3 or 4 input parameters."), 4
#define MEX_REPORT_ERROR_CODE_5a ("Command 'initialize' (socket TCP): Expect type int16 for in#1."), 5
#define MEX_REPORT_ERROR_CODE_6a ("Command 'initialize' (socket TCP): Expect type char string for in#2."), 6
#define MEX_REPORT_ERROR_CODE_7a ("Command 'initialize' (socket TCP): Expect type int32 for in#3."), 7
#define MEX_REPORT_ERROR_CODE_4b ("Command 'initialize' (socket RS232): Expect 5 input parameters."), 4
#define MEX_REPORT_ERROR_CODE_5b ("Command 'initialize' (socket RS232): Expect type int16 for in#1."), 5
#define MEX_REPORT_ERROR_CODE_6b ("Command 'initialize' (socket RS232): Expect type int32 string for in#2."), 6
#define MEX_REPORT_ERROR_CODE_7b ("Command 'initialize' (socket RS232): Expect type int16 for in#3."), 7
#define MEX_REPORT_ERROR_CODE_65 ("Command 'initialize' (socket RS232): Expect type int16 for in#4."), 65
#define MEX_REPORT_ERROR_CODE_8 ("Command 'initialize': C-api function call of 'init' failed."), 8
#define MEX_REPORT_ERROR_CODE_64 ("Command 'isConnected': C-api function call of 'isConnected' failed."), 64
#define MEX_REPORT_ERROR_CODE_9 ("Command 'remoteRequest': Expect type int32 for in#1."), 9
#define MEX_REPORT_ERROR_CODE_10 ("Command 'remoteRequest': Expect type char string for in#2."), 10
#define MEX_REPORT_ERROR_CODE_11 ("Command 'remoteRequest': Expect type int32 for in#3."), 11
#define MEX_REPORT_ERROR_CODE_12 ("Command 'remoteRequest': Expect type int32 for in#4."), 12
#define MEX_REPORT_ERROR_CODE_13 ("Command 'waitforPendingEvent': Expect type int32 for in#1."), 13
#define MEX_REPORT_ERROR_CODE_14(var) ("Command 'remoteRequest': Remote input parameter #" + int2String(var) + " is of wrong type."), 14
#define MEX_REPORT_ERROR_CODE_15(var) ("Command 'remoteRequest': Remote output parameter #" + int2String(var) + " is of wrong type."), 15
#define MEX_REPORT_ERROR_CODE_16(err) ("Command 'remoteRequest': C-api function call of 'remoteRequest' failed, reason: Errcode = " + int2String(err) + "."), 16
#define MEX_REPORT_ERROR_CODE_16a ("Command 'remoteRequest': C-api function call of 'remoteRequest' failed, reason: TIMEOUT EXPIRED."), 16
#define MEX_REPORT_ERROR_CODE_16b ("Command 'remoteRequest': C-api function call of 'remoteRequest' failed, reason: ANOTHER PROCESS IS ALREADY PROCESSED."), 16
#define MEX_REPORT_ERROR_CODE_17 ("Command 'requestPendingEvent': C-api function call of 'requestPendingEvent' failed."), 17
#define MEX_REPORT_ERROR_CODE_18 ("Command 'requestPendingError': C-api function call of 'requestPendingError' failed."), 18 
#define MEX_REPORT_ERROR_CODE_19 ("Command 'lockPendingAction': C-api function call of 'lockPendingAction' failed."), 19 
#define MEX_REPORT_ERROR_CODE_20 ("Command 'reportPendingAction': C-api function call of 'reportPendingAction' failed."), 20
#define MEX_REPORT_ERROR_CODE_21  ("Command 'parameterDescriptorsPendingRequest': C-api function call of 'parameterDescriptorsPendingRequest' failed."), 21 
#define MEX_REPORT_ERROR_CODE_22 ("Command 'inputParametersPendingRequest': C-api function call of 'parameterDescriptorsPendingRequest' failed."), 22 
#define MEX_REPORT_ERROR_CODE_23 ("Command 'isLockedPendingAction': C-api function call of 'isLockedPendingAction' failed."), 23
#define MEX_REPORT_ERROR_CODE_24 ("Command 'inputParametersPendingRequest': C-api function call of 'allocateParameterList' failed."), 24 
#define MEX_REPORT_ERROR_CODE_25 ("Command 'inputParametersPendingRequest': C-api function call of 'parametersPendingRequest' failed."), 25 
#define MEX_REPORT_ERROR_CODE_26 ("Command 'specifyOutputParameters': C-api function call of 'parameterDescriptorsPendingRequest' failed."), 26 
//#define MEX_REPORT_ERROR_CODE_27 ("Command 'specifyOutputParameters': ."), 27
#define MEX_REPORT_ERROR_CODE_28 ("Command 'specifyOutputParameters': C-api function call of 'allocateParameterList' failed.."), 28 
#define MEX_REPORT_ERROR_CODE_29 ("Command 'specifyOutputParameters': C-api function call of 'parametersPendingRequest' failed.."), 29 
#define MEX_REPORT_ERROR_CODE_30 ("Command 'specifyOutputParameters': C-api function call of 'reportCompletePendingAction' failed.."), 30 
#define MEX_REPORT_ERROR_CODE_31 ("Command 'reportExecutionError': : Expect type int16 for in#1."), 31
#define MEX_REPORT_ERROR_CODE_32 ("Command 'reportExecutionError': C-api function call of 'reportCompletePendingAction' failed."), 32 
#define MEX_REPORT_ERROR_CODE_33 ("Command 'unlockPendingAction': C-api function call of 'unlockPendingAction' failed."), 33 
#define MEX_REPORT_ERROR_CODE_34 ("Command 'setSocketType': Id for input argument #1 is invalid."), 34 
#define MEX_REPORT_ERROR_CODE_35 ("Command 'setSocketType': Expect type int16 for in#1."), 35 
#define MEX_REPORT_ERROR_CODE_36 ("Command 'setSocketType': Expect 2 input parameters."), 36 
#define MEX_REPORT_ERROR_CODE_37 ("Command 'getSocketType': Input or output type id is invalid."), 37 
#define MEX_REPORT_ERROR_CODE_38 ("Command 'getSocketType': Expect type int16 for in#1."), 38 
#define MEX_REPORT_ERROR_CODE_39 ("Command 'getSocketType': Expect 2 input parameters."), 39

//#define MEX_REPORT_ERROR_CODE_32 ("Command argument " + commandToken + " is not valid."), 3

#define FLD_NAME_ERROR_DESCRIPTION "DESCRIPTION"
#define FLD_NAME_ERROR_ID "ID"