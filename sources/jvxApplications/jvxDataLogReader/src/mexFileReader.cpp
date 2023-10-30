#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

#include <string>

//#include <matrix.h>
#include <cassert>

#include "mexFileReader.h"
#include "mexCommandsFileReader.h"
#include "HjvxMex2CConverter.h"
#include "jvx-helpers.h"
#include "CjvxCToMatlabConverter.h"

#define ERROR_MESSAGE_REPORT(fct) ((std::string)"Internal error when using jvxDataLogReader, member function " + fct + (std::string)", error code: ")

// Define the entryx points for static linkage of dynamic tools
#include "jvxTDataLogReader.h"

#define MAXSTRING 256

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

/**
 * Function to create a C string in MEX format.
 *///=========================================================0


static void dataRecoveryCallback(TjvxDatarecoveryEventType tp, jvxHandle* content, jvxSize szFld)
{
	int progressPer = 0;
	switch(tp)
	{
	case JVX_DATATRECOVERY_EVENT_TYPE_PROGRESS:
		progressPer = JVX_DATA2INT32(*((jvxData*)content) * 100.0);
		//mexPrintf("Progress: %i\n", progressPer);
	}
}

//=========================================================
/**
 * Constructor: Set all values to initialized/uninitialized values,
 * Constructor opens the used fileWriter/Reader library and sets all variables to initial.
 *///========================================================
mexFileReader::mexFileReader(void)
{
	jvxErrorType res = JVX_NO_ERROR;
	isInitialized = false;
	filenameSpecified = false;
	isScanned = false;
	dataLogReader.theHdl = NULL;
	dataLogReader.theObj = NULL;
	jvxComponentIdentification tp = JVX_COMPONENT_UNKNOWN;

	res = jvxTDataLogReader_init(&dataLogReader.theObj);
	if(dataLogReader.theObj)
	{
		res = dataLogReader.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&dataLogReader.theHdl), &tp, NULL);
		if(res == JVX_NO_ERROR)
		{
			res = dataLogReader.theHdl->initialize(dataRecoveryCallback);
		}
	}

	if(res == JVX_NO_ERROR)
	{
		mexPrintf("##--> Jvx DataLogReader is ready.\n");
	}
	else
	{
		mexPrintf("##--> Jvx DataLogReader is NOT ready.\n");
	}
	floatingPointIdReader = 2;
	floatingPointIdFile = 2;
}

//===========================================================
// ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY
//===========================================================

/** 
 * Main entry function: The most common response is to return a struct that contains all available information 
 * getCurrentSetup.
 * This function is nothing else than a dispatcher that converts the input commands
 * into calls to predefined functions.
 *///=======================================================================================================
bool 
mexFileReader::parseInput(int nlhs, mxArray *plhs[],
 					int nrhs, const mxArray *prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	jvxSize i;

	// If calling the function without input arguments, the setup is returned
	if(nrhs == 0)
	{
		printUsage();
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if(nlhs > 1)
			this->mexReturnStructSetup(plhs[1]);
		return(JVX_NO_ERROR);
	}
	else
	{
		// We have at least one input argument, this must be an ID
		JVX_FIO_COMMAND command;
	
		// Step I: Extract command ID from input argument
		arr = prhs[0];
		if(mxIsDouble(arr))
		{
			command = (JVX_FIO_COMMAND)((int)*((double*)mxGetData(arr)));
		}
		else if(mxIsSingle(arr))
		{
			command = (JVX_FIO_COMMAND)((int)*((float*)mxGetData(arr)));
		}
		else if(mxIsInt32(arr))
		{
			command = (JVX_FIO_COMMAND)*((int*)mxGetData(arr));
		}
		else if(mxIsChar(arr))
		{
			std::string token = jvx_mex_2_cstring(arr);
			bool foundit = false;
			for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
			{
				if(mapperCommands[i].cmdToken == token)
				{
					foundit = true;
					command = mapperCommands[i].cmdId;
					break;
				}
			}

			if(!foundit)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], "Input argument command specification " + token + " is unknown.", JVX_ERROR_INVALID_ARGUMENT);
				}
				return(JVX_NO_ERROR);
			}
		}
		else
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for function ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}
	
		// Depending on the ID, different actions are performed:
		//==================================================================
		switch(command)
		{
			// ID for init: Init this class
		case JVX_FIO_COMMAND_ACTIVATE: 
			res = this->activate(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknown";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break; 
		case JVX_FIO_COMMAND_PREPARE : 
			res = this->prepare(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break; 
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NUMBER_SESSIONS:
			res = this->returnNumberSessions(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_TAGS_SESSION:
			res = this->returnTagsSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NUMBER_DATASETS_SESSION:
			res = this->returnNumberDataSetsSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_DATASET_SESSION:
			res = this->returnDataSetSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_SESSION:
			res = this->returnNumberDataChunksSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NUMBER_DATACHUNKS_DATASET_SESSION:
			res = this->returnNumberDataChunksSessionOneSet(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_SESSION:
			res = this->returnNextDatachunkSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION:
			res = this->returnNextDatachunkSessionOneSet(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_NEXT_DATACHUNK_DATASET_SESSION_COMPLETE_FIELD_DATA:
			res = this->returnNextDatachunkSessionOneSetConstantSize(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;

		case JVX_FIO_COMMAND_REWIND_SESSION:
			res = this->rewindSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_REWIND_DATASET_SESSION:
			res = this->rewindSessionOneSet(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_PROGRESS_SESSION:
			res = this->returnProgressSession(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_RETURN_PROGRESS_DATASET_SESSION:
			res = this->returnProgressSessionOneSet(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_POSTPROCESS:
			res = this->postprocess(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		case JVX_FIO_COMMAND_DEACTIVATE:
			res = this->deactivate(nlhs, plhs, nrhs, prhs);
			if(res != JVX_NO_ERROR)
			{
				std::string comToken = "unknwo";
				for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
				{
					if(mapperCommands[i].cmdId == command)
					{
						comToken = mapperCommands[i].cmdToken;
						break;
					}
				}
				mexPrintf(("Unexpected Error in <jvxDataLogReader>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
				return(false);
			}
			break;
		default:

			// Out of range of fileReaderMatlab commands
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument does not hold a valid ID.", JVX_ERROR_ELEMENT_NOT_FOUND);
		}
	}
	return(JVX_NO_ERROR);
}


/**
 * Function to print usage of the fileReaderMatlab as documentation reference.
 * This message is printed whenever the fileReaderMatlab is called without additional command argument.
 *///============================================================================
void
mexFileReader::printUsage()
{
	jvxSize i;
	mexPrintf("-->Jvx reader for data logging tool created files. Developed by Javox Solutions GmbH\n");
	mexPrintf("-->The following commands can be specified:\n");
	
	for(i = 0; i < JVX_FIO_COMMAND_LIMIT; i++)
	{
		std::string line = "----> " + jvx_int2String(mapperCommands[i].cmdId) + ": " + mapperCommands[i].cmdToken + ": " + mapperCommands[i].cmdDescription;
		mexPrintf("%s\n", line.c_str());
	}

/*	mexPrintf("File reader for logiles, use >> [a b ] = fileReaderMatlab(in1, in2, in3) <<\n");
	mexPrintf("a: Return bool for success, b: Return function specific value.\n");
	mexPrintf("in1: Functional id (see below), in2, in3: Function specific parameters.\n");


	mexPrintf("Command:JVX_FIO_MX_INITIALIZE, id=0, Initialization\n");
	mexPrintf("Command:JVX_FIO_MX_OPENFILE, id=1, Open file, in2: filename\n");
	mexPrintf("Command:JVX_FIO_MX_SCANSESSIONS, id=2, Scan for sessions in file\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NUMBER_SESSIONS, id=3, Retrurn number of available sessions\n");
	mexPrintf("Command:JVX_FIO_MX_SELECT_TIMING_DIVISOR_SESSION, id=4, Return timing divisor value for session, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NUMBER_DATASETS_SESSION, id=5, Return the number of datasets in session, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_DATASET_SESSION, id=6, Return struct for a dataset, in2: Session id (0..), in3: Dataset id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NUMBER_DATACHUNKS_SESSION, id=7, Return number of datachunks in session, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NUMBER_DATACHUNKS_SESSION_DATASET, id=8, Return the number of datachunks for a session and a specific dataset, in2: Session id (0..), in3: Dataset id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NEXT_DATACHUNK_SESSION, id=9, Return the next datachunk in session, if files, reached end of session, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NEXT_DATACHUNK_SESSION_DATASET, id=10, Return the next datachunk in session for a specific dataset id, if files, reached end of session, in2: Session id (0..), in3: Dataset id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_NEXT_DATACHUNK_SESSION_DATASET_COMPLETE_FIELD_DATA, id=11, \n");
	mexPrintf("		Copy fields in rectangular matrix. This functioncall will return more than one datachunk in one function call for speed-up.\n");
	mexPrintf("     Input argument 1: session ID, input argument 2: Dataset ID, input argument 3: Number of datachunks to be returned in one field, input argument 4: command sub ID: What to return in the field:\n");
	mexPrintf("     argument[4] == 0: Return the datafield: The input must contain a constant number of elements for each chunk, also specified in dataset (numberElementsIfConstant != -1)\n");
	mexPrintf("						  If the datachunks have different size than specified in the dataset, function will fail.\n");
	mexPrintf("     argument[4] == 1: Return the ID\n");
	mexPrintf("     argument[4] == 2: Return the datatype\n");
	mexPrintf("     argument[4] == 3: Return the user ID\n");
	mexPrintf("     argument[4] == 4: Return the user sub-ID\n");
	mexPrintf("     argument[4] == 5: Return the timing information\n");
	mexPrintf("     argument[4] == 6: Return the number of elements\n");
	mexPrintf("Command:JVX_FIO_MX_REWIND_SESSION, id=12, Rewind datachunk pointer in session, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_REWIND_SESSION_DATASET, id=13, Rewind datachunk pointer in session, in2: Session id (0..), in3: Dataset id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_PROGRESS_SESSION, id=14, Return progress of session pointer, in2: Session id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_RETURN_PROGRESS_SESSION_DATASET, id=15, Return progress of session pointer, in2: Session id (0..), in3: Dataset id (0..)\n");
	mexPrintf("Command:JVX_FIO_MX_UNSCANSESSIONS, id=16, unscan sessions, inverse to scan.\n");
	mexPrintf("Command:JVX_FIO_MX_CLOSE_FILE, id=17, close the input file.\n");
	mexPrintf("Command:JVX_FIO_MX_TERMINATE, ID=18, terminate.\n");
	*/
}




/**
 * Open the specified input filename for reading
 *///====================================================0
jvxErrorType
mexFileReader::activate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	int errCode = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::activate");
	if(nrhs > 1)
	{
		const mxArray* pa = prhs[1];
		std::string nmFile;
		if(mxIsChar(pa))
		{
			size_t strLen = mxGetN(pa)+1;
			char* fld = new char[strLen+1];
			memset(fld, 0, sizeof(char) * strLen);
			mxGetString(pa, fld, strLen);
			nmFile = fld;
			delete[](fld);
			res = dataLogReader.theHdl->activate(nmFile.c_str());
			if(res == JVX_NO_ERROR)
			{
				if(nlhs > 0)
				{
					mexReturnBool(plhs[0], true);
				}	
				if(nlhs > 1)
				{
					this->mexReturnStructSetup(plhs[1]);
				}
				return(res);
			}
			else
			{
				internalErrorMessage += jvxErrorType_descr(res);
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
				return(JVX_NO_ERROR);
			}
		}					
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for filename is of wrong type.", JVX_ERROR_INVALID_ARGUMENT);
		}
		return(JVX_NO_ERROR);
	}
	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
	{
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for filename is missing.", JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_NO_ERROR);
}

/**
 * Whenever the input is open right now, scan for number of sessions in input file.
 *///================================================================================0
jvxErrorType
mexFileReader::prepare(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::prepare");
	res = dataLogReader.theHdl->prepare();

	if(res == JVX_NO_ERROR)
	{
		floatingPointIdReader = JVX_FLOAT_DATAFORMAT_ID; // Default id is that of the current compilation version
		dataLogReader.theHdl->floating_point_id_file(&floatingPointIdFile );
		
		if((floatingPointIdFile == JVX_FLOAT_DATAFORMAT_ID) || (floatingPointIdFile == JVX_FLOAT_DATAFORMAT_ID_OTHER))
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnStructSetup(plhs[1]);
		}
		else
		{
			internalErrorMessage += "System version conflict: The log file was written involving a writer compiled for another type of floating point arithmetic.";

			if (nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if (nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
	}
	else
	{
		internalErrorMessage += jvxErrorType_descr(res);
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
	}
	return(JVX_NO_ERROR);
}

/**
 * Return the number of sessions in the input file according to previous scan process.
 * Convert number of sessions into MEX format.
 *///===================================================================================
jvxErrorType
mexFileReader::returnNumberSessions(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNumberSessions");
	jvxSize numSess;
	res = dataLogReader.theHdl->number_sessions(&numSess);
	if(res == JVX_NO_ERROR)
	{
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if(nlhs > 1)
			this->mexReturnInt32(plhs[1], (jvxInt32)numSess);
	}
	else
	{
		internalErrorMessage += jvxErrorType_descr(res);
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexFileReader::returnTagsSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNumberDataSetsSession");

	jvxSize numTags;
	jvxSize i;
	jvxApiString fldName;
	jvxApiString fldValue;
	jvxSize idSess = 0;
	std::vector<oneTagEntry> theTags;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}
		res = dataLogReader.theHdl->number_tags_session(idSess, &numTags);
		for(i = 0; i < numTags; i++)
		{
			oneTagEntry theE;
			res = dataLogReader.theHdl->name_value_tags_session(idSess, i, &fldName, &fldValue);
			theE.tagName = fldName.std_str();

			theE.tagValue = fldValue.std_str();
			theTags.push_back(theE);
		}

		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
			{
				this->mexReturnStructTagsSession(plhs[1], theTags);
			}
		}
		else
		{
			
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
	{
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_NO_ERROR);
}

/**
 * Return the number of datasets in the current session in MEX format.
 *///============================================================================
jvxErrorType
mexFileReader::returnNumberDataSetsSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNumberDataSetsSession");

	jvxSize numSets;
	jvxSize idSess = 0;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}
		res = dataLogReader.theHdl->number_datasets_session(idSess, &numSets);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnInt32(plhs[1], JVX_SIZE_INT32(numSets));
		}
		else
		{
			
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
	{
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_NO_ERROR);
}

/**
 * Return the number of datachunks in this session (independent from dataset ID)
 *///=============================================================================
jvxErrorType
mexFileReader::returnNumberDataChunksSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNumberDataChunksSession");
	jvxSize numSets;
	jvxSize idSess = 0;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		res = dataLogReader.theHdl->number_datachunks_session(idSess, &numSets);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnInt32(plhs[1], JVX_SIZE_INT32(numSets));
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);

			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
	{
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_NO_ERROR);
}

/**
 * Return the number of datachunks in this session depending on dataset ID
 *///=============================================================================
jvxErrorType
mexFileReader::returnNumberDataChunksSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNumberDataChunksSessionOneSet");
	jvxSize numSets;
	jvxSize idSess = 0;
	jvxSize idSet = 0;
	
	if(nrhs > 2)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}
		
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}

		res = dataLogReader.theHdl->number_datachunks_dataset_session(idSess, idSet, &numSets);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnInt32(plhs[1], JVX_SIZE_INT32(numSets));
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
	{
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing" , JVX_ERROR_INVALID_ARGUMENT);
	}
	return(JVX_NO_ERROR);
}


/**
 * return the dataset for the current session and the ID specified in MEX format.
 *///==============================================================================
jvxErrorType
mexFileReader::returnDataSetSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnDataSetSession");
	jvxLogFileDataSetHeader* headerDSet = NULL;
	jvxSize idSess = 0;
	jvxSize idSet = 0;
	if(nrhs > 2)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{

				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return(JVX_NO_ERROR);
		}
		
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{

			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}

		size_t szDataset;

		res = dataLogReader.theHdl->size_dataset_session(idSess, idSet, &szDataset);
		if(res == JVX_NO_ERROR)
		{
			JVX_SAFE_NEW_FLD_CVRT(headerDSet, jvxByte, szDataset, jvxLogFileDataSetHeader*);
			res = dataLogReader.theHdl->copy_dataset_session(idSess, idSet,headerDSet, szDataset);
			if(res == JVX_NO_ERROR)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				if(nlhs > 1)
					this->mexReturnStructDataset(plhs[1], headerDSet);
			}
			else
			{
				internalErrorMessage += jvxErrorType_descr(res);
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
			}
			JVX_SAFE_DELETE_FLD_CVRT(headerDSet, jvxByte*)
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Two input arguments are rquired for this function.", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Function to return the next datachunk for the session, independent from dataset ID.
 *///==================================================================================
jvxErrorType
mexFileReader::returnNextDatachunkSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNextDatachunkSession");
	int errCode = 0;
	jvxLogFileDataSetHeader* headerDSet = NULL;
	jvxLogFileDataChunkHeader* headerDChunk = NULL;
	jvxSize idSess = 0;
	unsigned numberDatasets = 0;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		
		size_t szFld;

		res = dataLogReader.theHdl->find_size_next_datachunk_session(idSess, &szFld);
		if(res == JVX_NO_ERROR)
		{
			JVX_SAFE_NEW_FLD_CVRT(headerDChunk, jvxByte, szFld, jvxLogFileDataChunkHeader*);
			res = dataLogReader.theHdl->copy_next_datachunk_session(idSess, headerDChunk, szFld);
			if(res == JVX_NO_ERROR)
				{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				if(nlhs > 1)
					this->mexReturnStructDatachunk(plhs[1], headerDChunk);
			}
			else
			{
				internalErrorMessage += jvxErrorType_descr(res);
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
			}
			JVX_SAFE_DELETE_FLD_CVRT(headerDChunk, jvxByte*);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type.", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/** 
 * Function to return the next datachunk(s) for the current session and set.
 * this function can be used to speed up read process by returning the datafields for
 * as many datachunks as willing to be returned.
 * The data for multiple datachunks can be grouped into a 2-dimensional array of dimension MxN
 * while M is the number of datachunks combined and N is the number of dataelements for each datachunk:
 * The matrix:
 * | c(0,0)			c(0,1)		c(0,2)		c(0,3)		c(0,4)		....		c(0, (N-1) |
 * | c(1,0)			c(1,1)		c(1,2)		c(1,3)		c(1,4)		....		c(1, (N-1) |
 * | ....			....		....		  ...		 ....		....		 ........  |
 * | c((M-1),0)		c((M-1),1)	c((M-1),2)	c((M-1),3)	c((M-1),4)	....	c((M-1), (N-1) |
 * holds the datachunks c(i,..) and the elements for each datachunk c(..,j), while c(i-1,..) and
 * c(i,..) are stored sequentially in the file.
 * For this method to function propperly, it is required that each datachunk holds a constant number
 * of data elements (for the square matrix) and that the type is also constant. For this purpose,
 * the numberElementsIfConstant field in the associated dataset definition must be greater equal 1.
 * Besides the data elements for a specific number of datachunks, also other fields from the datachunk
 * header can be returned. Those will form a (Mx1) matrix on return with as many rows as datachunks have been
 * combined.
 * The third MEX argument specifies the number of chunks to group together while
 * the fourth argument can be used to specify which elemet should be grouped:
 * JVX_FIO_MX_COPY_DATA = 0: Grouped data elements in datachunks (copy data is also default for 3 arguments)
 * JVX_FIO_MX_COPY_ID_CHUNK = 1: Grouped chunk IDs, could be for example numbers in sequentiel order
 * JVX_FIO_MX_COPY_ID_DATA_TYPE = 2: Group the datatypes for the datachunk elements
 * JVX_FIO_MX_COPY_ID_USER = 3: Group the IDs specified by user when writing the file
 * JVX_FIO_MX_COPY_SUB_ID_USER = 4: Group the sub IDs from the user
 * JVX_FIO_MX_COPY_TIMING = 5: Group the timing information for all the datachunks in a sequence of array.
 * JVX_FIO_MX_COPY_NUMBER_ELEMENTS = 6: Group the number of elements, in general this will be constant.
 *///==============================================================================
jvxErrorType
mexFileReader::returnNextDatachunkSessionOneSetConstantSize(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNextDatachunkSessionOneSetConstantSize");
	jvxLogFileDataSetHeader* headerDSet = NULL;
	jvxLogFileDataChunkHeader* headerDChunk = NULL;
	jvxSize idSess = 0;
	jvxSize idSet;
	unsigned numberDatasets = 0;
	jvxInt32 numChunksAtOnce = 1;
	JVX_FIO_COPY_TYPE returnModeID = JVX_FIO_COPY_DATA;

	// Collect the three arguments session, dataset id and number of chunks returned in one field
	if(nrhs > 3)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}


		arr = prhs[3];
		res = mexArgument2Index<jvxInt32>(numChunksAtOnce, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for number of datachunks at once is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}

		//=====================================================================

		// Additional argument if different functionality is desired
		
		// Default return mode is copy data
		returnModeID = JVX_FIO_COPY_DATA;
		jvxInt32 valI;
		if(nrhs > 4)
		{
			arr = prhs[4];
			res = mexArgument2Index<jvxInt32>(valI, &arr, 0, 1);
			if(res != JVX_NO_ERROR)
			{
		
				jvxSize i;
				std::string token = jvx_mex_2_cstring(arr);
				bool foundit = false;
				for(i = 0; i < JVX_FIO_COPY_LIMIT; i++)
				{
					if(mapperDataCopyTypes[i].cmdToken == token)
					{
						foundit = true;
						valI = mapperCommands[i].cmdId;
						break;
					}
				}

				if(!foundit)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], "Input argument copy type specification " + token + " is unknown.", JVX_ERROR_INVALID_ARGUMENT);
					}
					return(JVX_NO_ERROR);
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
				return(JVX_NO_ERROR);
			}

			if(valI < JVX_FIO_COPY_LIMIT)
			{
				returnModeID = (JVX_FIO_COPY_TYPE)valI;
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is not a valid id", JVX_ERROR_ID_OUT_OF_BOUNDS);
				return(JVX_NO_ERROR);
			}
		}

		size_t szFld;
		size_t szDataset;

		jvxLogFileDataSetHeader* headerDSet = NULL;
		int numberElementsConstantSize = 0;
		jvxDataFormat dataTypeStore = JVX_DATAFORMAT_NONE;
		bool isTiming = false;

		//================================================================
		// Prepare different functionalities
		//================================================================
		switch(returnModeID)
		{
		case JVX_FIO_COPY_DATA:
			res = dataLogReader.theHdl->size_dataset_session(idSess, idSet, &szDataset);
			if(res == JVX_NO_ERROR)
			{
				JVX_SAFE_NEW_FLD_CVRT(headerDSet, jvxByte, szDataset, jvxLogFileDataSetHeader*);
				res = dataLogReader.theHdl->copy_dataset_session(idSess, idSet,headerDSet, szDataset);
				if(res == JVX_NO_ERROR)
				{
					numberElementsConstantSize = headerDSet->numberElementsIfConstant;
					dataTypeStore = (jvxDataFormat)headerDSet->dataTypeGlobal;
				}
				else
				{
					internalErrorMessage += jvxErrorType_descr(res);
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs	> 1)
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					return(JVX_NO_ERROR);
				}
				JVX_SAFE_DELETE_FLD_CVRT(headerDSet, jvxByte*);
			}
			else
			{
				internalErrorMessage += jvxErrorType_descr(res);
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
				return(JVX_NO_ERROR);
			}
			break;
		case JVX_FIO_COPY_ID_CHUNK:
		case JVX_FIO_COPY_ID_DATA_TYPE:
		case JVX_FIO_COPY_ID_USER:
		case JVX_FIO_COPY_SUB_ID_USER:
		case JVX_FIO_COPY_NUMBER_ELEMENTS:
			numberElementsConstantSize = 1;
			dataTypeStore = JVX_DATAFORMAT_32BIT_LE;
			break;
		case JVX_FIO_COPY_TIMING:
			numberElementsConstantSize = 1;
			
			// Introduce type for 64 bit signed for timing information
			isTiming = true;
			break;
		}

		jvxSize numChunks = 0;
		res = dataLogReader.theHdl->number_datachunks_dataset_session(idSess, idSet, &numChunks);
		if(numChunksAtOnce < 0)
		{
			numChunksAtOnce = JVX_SIZE_INT32(numChunks);
		}

		numChunksAtOnce = JVX_SIZE_INT32(JVX_MIN(numChunks, numChunksAtOnce));


		// Found out up to this location:
		// Number of constant elements in array and number of elements to return (MxN matrix)
		if(numberElementsConstantSize > 0)
		{


			// Allocate square matrix of right type
			SZ_MAT_TYPE ndim = 2;
			SZ_MAT_TYPE dims[2] = {1, 1};
			dims[1] = (int)numberElementsConstantSize;
			dims[0] = (int)numChunksAtOnce;

			jvxSize numBytesSample = 0;
			mxArray* retArr = NULL;

			if(isTiming)
			{
				// Special case: INT64 type, only used for timing information
				retArr = mxCreateNumericArray(2, dims, mxINT64_CLASS, mxREAL);
				numBytesSample = sizeof(long long);
			}
			else
			{
				// Find out the datatype to allocate according to dataset
				switch(dataTypeStore)
				{

				case JVX_DATAFORMAT_SIZE:
					retArr = mxCreateNumericArray(2, dims, mxUINT64_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[JVX_DATAFORMAT_U64BIT_LE];
					break;
				case JVX_DATAFORMAT_DATA:
					retArr = mxCreateNumericArray(2, dims, JVX_DATA_MEX_CONVERT, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;

				case JVX_DATAFORMAT_16BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxINT16_CLASS, mxREAL);
					numBytesSample = 2;
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;
				case JVX_DATAFORMAT_64BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxINT64_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;
				case JVX_DATAFORMAT_8BIT:
					retArr = mxCreateNumericArray(2, dims, mxINT8_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;

				case JVX_DATAFORMAT_U16BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxUINT16_CLASS, mxREAL);
					numBytesSample = 2;
					break;
				case JVX_DATAFORMAT_U32BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;
				case JVX_DATAFORMAT_U64BIT_LE:
					retArr = mxCreateNumericArray(2, dims, mxUINT64_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;
				case JVX_DATAFORMAT_U8BIT:
					retArr = mxCreateNumericArray(2, dims, mxINT8_CLASS, mxREAL);
					numBytesSample = jvxDataFormat_size[dataTypeStore];
					break;

				default:
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					if(nlhs > 1)
						this->mexReturnAnswerNegativeResult(plhs[1], (std::string)"Tried to read data for unsupported type: " + 
							jvxDataFormat_txt(dataTypeStore), JVX_ERROR_UNSUPPORTED);
					return(JVX_NO_ERROR);
				}
			}

			// Set pointer to allocated load field from MEX
			jvxData* tFldDouble = (jvxData*)mxGetData(retArr);
			jvxInt8* tFld8bit = (jvxInt8*)mxGetData(retArr);
			jvxInt16* tFldInt16 = (jvxInt16*)mxGetData(retArr);
			jvxInt32* tFldInt32 = (jvxInt32*)mxGetData(retArr);
			jvxInt64* tFldInt64 = (jvxInt64*)mxGetData(retArr);
			jvxUInt8* tFldU8bit = (jvxUInt8*)mxGetData(retArr);
			jvxUInt16* tFldUInt16 = (jvxUInt16*)mxGetData(retArr);
			jvxUInt32* tFldUInt32 = (jvxUInt32*)mxGetData(retArr);
			jvxUInt64* tFldUInt64 = (jvxUInt64*)mxGetData(retArr);

			// Copy the data from field to allocated mex type
			bool allCouldBeCopied = true;
			int idElementFailedCopyFirst = 0;
			std::string errTxt;
			jvxBool warnOnly = false;
			// For as many chunks as there are to be copied afterwards
			for(int ii = 0; ii < numChunksAtOnce; ii++)
			{
				res = dataLogReader.theHdl->find_size_next_datachunk_dataset_session(idSess, idSet, &szFld);
				if(res == JVX_NO_ERROR)
				{
					JVX_SAFE_NEW_FLD_CVRT(headerDChunk, jvxByte, szFld, jvxLogFileDataChunkHeader*);
					res = dataLogReader.theHdl->copy_next_datachunk_dataset_session(idSess, idSet, headerDChunk, szFld);
					if(res == JVX_NO_ERROR)
					{
						// Case I: copy the data from a number of datachunks
						if(returnModeID == JVX_FIO_COPY_DATA)
						{
							// Check that datatype matches for chunk and dataset
							if(((jvxLogFileDataChunkHeader*)headerDChunk)->dataTypeGlobal == dataTypeStore)
							{
								// Check that really a constant number of data elements is stored for each chunk
								if(((jvxLogFileDataChunkHeader*)headerDChunk)->numberElements == numberElementsConstantSize)
								{
									jvxData* fFldData = (jvxData*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
									//jvxData* fFldFloat = (jvxData*)(((byte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
									float* fFldOther = (float*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
#else
									double* fFldOther = (double*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
#endif
									jvxSize* fFldSize = (jvxSize*)(((jvxByte*)headerDChunk) + sizeof(jvxLogFileDataChunkHeader));

									jvxInt16* fFldInt16 = (jvxInt16*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
									jvxInt32* fFldInt32 = (jvxInt32*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
									jvxInt64* fFldInt64 = (jvxInt64*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));
									jvxInt8* fFld8bit = (jvxInt8*)(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader));									

									jvxUInt16* fFldUInt16 = (jvxUInt16*)(((jvxByte*)headerDChunk) + sizeof(jvxLogFileDataChunkHeader));
									jvxUInt32* fFldUInt32 = (jvxUInt32*)(((jvxByte*)headerDChunk) + sizeof(jvxLogFileDataChunkHeader));
									jvxUInt64* fFldUInt64 = (jvxUInt64*)(((jvxByte*)headerDChunk) + sizeof(jvxLogFileDataChunkHeader));
									jvxUInt8* fFldU8bit = (jvxUInt8*)(((jvxByte*)headerDChunk) + sizeof(jvxLogFileDataChunkHeader));

									// Copy the data elements according to used type: NOTE THAT MATLAB REQUIRES A RE-MAPPING
									switch(dataTypeStore)
									{
									case JVX_DATAFORMAT_8BIT:
										for(int i = 0; i < numberElementsConstantSize; i++)
										{	
											idElementFailedCopyFirst++;
											tFld8bit[i*numChunksAtOnce+ii] = *fFld8bit++;
										}
										break;
									case JVX_DATAFORMAT_16BIT_LE:
										for(int i = 0; i < numberElementsConstantSize; i++)
										{	
											idElementFailedCopyFirst++;
											tFldInt16[i*numChunksAtOnce+ii] = *fFldInt16++;
										}
										break;
									case JVX_DATAFORMAT_32BIT_LE:
										for(int i = 0; i < numberElementsConstantSize; i++)
										{	
											idElementFailedCopyFirst++;
											tFldInt32[i*numChunksAtOnce+ii] = *fFldInt32++;
										}
										break;
									case JVX_DATAFORMAT_64BIT_LE:
										for(int i = 0; i < numberElementsConstantSize; i++)
										{	
											idElementFailedCopyFirst++;
											tFldInt64[i*numChunksAtOnce+ii] = *fFldInt64++;
										}
										break;

									case JVX_DATAFORMAT_U8BIT:
										for (int i = 0; i < numberElementsConstantSize; i++)
										{
											idElementFailedCopyFirst++;
											tFldU8bit[i*numChunksAtOnce + ii] = *fFldU8bit++;
										}
										break;
									case JVX_DATAFORMAT_U16BIT_LE:
										for (int i = 0; i < numberElementsConstantSize; i++)
										{
											idElementFailedCopyFirst++;
											tFldUInt16[i*numChunksAtOnce + ii] = *fFldUInt16++;
										}
										break;
									case JVX_DATAFORMAT_U32BIT_LE:
										for (int i = 0; i < numberElementsConstantSize; i++)
										{
											idElementFailedCopyFirst++;
											tFldUInt32[i*numChunksAtOnce + ii] = *fFldUInt32++;
										}
										break;
									case JVX_DATAFORMAT_U64BIT_LE:
										for (int i = 0; i < numberElementsConstantSize; i++)
										{
											idElementFailedCopyFirst++;
											tFldUInt64[i*numChunksAtOnce + ii] = *fFldUInt64++;
										}
										break;

									case JVX_DATAFORMAT_SIZE:
										for (int i = 0; i < numberElementsConstantSize; i++)
										{
											idElementFailedCopyFirst++;
											tFldUInt64[i*numChunksAtOnce + ii] = *fFldSize++;
										}
										break;
									case JVX_DATAFORMAT_DATA:
										if(floatingPointIdFile != floatingPointIdReader)
										{
											for(int i = 0; i < numberElementsConstantSize; i++)
											{	
												idElementFailedCopyFirst++;
												tFldDouble[i*numChunksAtOnce+ii] = (jvxData) *fFldOther++;
											}
										}
										else
										{
											for(int i = 0; i < numberElementsConstantSize; i++)
											{	
												idElementFailedCopyFirst++;
												tFldDouble[i*numChunksAtOnce+ii] = *fFldData++;
											}
										}
										break;
									}
								}
								else
								{
									allCouldBeCopied = false;
									errTxt = "Size of datachunk buffer does not match the size of the dataset specificiation.";
									break;
								}
							}
							else
							{
								allCouldBeCopied = false;
								errTxt = "Type of datachunk buffer elements does not match the type of the dataset specificiation.";
								break;
							}
						}
						else
						{

							// For the modes other than COPY DATA
							jvxInt32* fFldInt32 = NULL;
							jvxInt64* fFldInt64 = NULL;
							switch(returnModeID)
							{
							case JVX_FIO_COPY_ID_CHUNK:
								fFldInt32 = (jvxInt32*)&((jvxLogFileDataChunkHeader*)headerDChunk)->idDataChunk;
								break;
							case JVX_FIO_COPY_ID_DATA_TYPE:
								fFldInt32 = (jvxInt32*)&((jvxLogFileDataChunkHeader*)headerDChunk)->dataTypeGlobal;
								break;
							case JVX_FIO_COPY_ID_USER:
								fFldInt32 = (jvxInt32*)&((jvxLogFileDataChunkHeader*)headerDChunk)->idUser;
								break;
							case JVX_FIO_COPY_SUB_ID_USER:
								fFldInt32 = (jvxInt32*)&((jvxLogFileDataChunkHeader*)headerDChunk)->subIdUser;
								break;
							case JVX_FIO_COPY_NUMBER_ELEMENTS:
								fFldInt32 = (jvxInt32*)&((jvxLogFileDataChunkHeader*)headerDChunk)->numberElements;
								break;
							case JVX_FIO_COPY_TIMING:
								fFldInt64 = (jvxInt64*)&((jvxLogFileDataChunkHeader*)headerDChunk)->ticktime_us;
								break;
							}

							// Copy one element for each datachunk
							idElementFailedCopyFirst++;
							switch(returnModeID)
							{
							case JVX_FIO_COPY_ID_CHUNK:
							case JVX_FIO_COPY_ID_DATA_TYPE:
							case JVX_FIO_COPY_ID_USER:
							case JVX_FIO_COPY_SUB_ID_USER:
							case JVX_FIO_COPY_NUMBER_ELEMENTS:
								tFldInt32[ii] = *fFldInt32;
								break;
							case JVX_FIO_COPY_TIMING:
								tFldInt64[ii] = *fFldInt64;
								break;
							}
						}
						idElementFailedCopyFirst++;
					}
					else
					{					
						internalErrorMessage += jvxErrorType_descr(res);
						allCouldBeCopied = false;
						warnOnly = true;
						errTxt = "Failed to copy datachunk buffer elements from file, element with failure is index " + jvx_size2String(ii) + "/" + jvx_size2String(numChunksAtOnce) + ".";
						break;  
					}
					JVX_SAFE_DELETE_FLD_CVRT(headerDChunk, jvxByte*);
				}
				else
				{
					internalErrorMessage += jvxErrorType_descr(res);
					allCouldBeCopied = false;
					errTxt = "Failed to find next datachunk buffer element in file.";
					break;
				}
			}

			if(allCouldBeCopied)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				if(nlhs > 1)
					plhs[1] = retArr;
			}
			else
			{
				if (warnOnly)
				{
					mexPrintf("WARNING: %s\n", ("Failed to copy multiple elements:" + errTxt + internalErrorMessage).c_str());
					
					// We copy back the content however
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					if (nlhs > 1)
						plhs[1] = retArr;
				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
						this->mexReturnAnswerNegativeResult(plhs[1], "Failed to copy multiple elements, first problem arised in element " + jvx_int2String(idElementFailedCopyFirst) + ", ERRTEXT:" + errTxt + internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
				}
			}
			return(JVX_NO_ERROR);
		}
		else
		{			
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Instructed to copy zero elements.", JVX_ERROR_INVALID_ARGUMENT);
		}
		return(JVX_NO_ERROR);
	}
	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Three input argument required for session dataset datachunk.", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Return the next datachunk for current session and dataset ID.
 *///===============================================================
jvxErrorType
mexFileReader::returnNextDatachunkSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnNextDatachunkSessionOneSet");
	jvxLogFileDataSetHeader* headerDSet = NULL;
	jvxLogFileDataChunkHeader* headerDChunk = NULL;
	jvxSize idSess = 0;
	jvxSize idSet;
	unsigned numberDatasets = 0;
	if(nrhs > 2)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}

		size_t szFld;

		
		res = dataLogReader.theHdl->find_size_next_datachunk_dataset_session(idSess, idSet, &szFld);
		if(res == JVX_NO_ERROR)
		{
			JVX_SAFE_NEW_FLD_CVRT(headerDChunk, jvxByte, szFld, jvxLogFileDataChunkHeader*);
			
			res = dataLogReader.theHdl->copy_next_datachunk_dataset_session(idSess, idSet, headerDChunk, szFld);
			if(res == JVX_NO_ERROR)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				if(nlhs > 1)
					this->mexReturnStructDatachunk(plhs[1], headerDChunk);
			}
			else
			{
				internalErrorMessage += jvxErrorType_descr(res);
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
			}
			JVX_SAFE_DELETE_FLD_CVRT(headerDChunk, jvxByte*);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Two input argument required for dataset datachunk.", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Rewind pointer into file that has no specific dataset ID association
 *///======================================================================
jvxErrorType
mexFileReader::rewindSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::rewindSession");
	jvxSize idSess = 0;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		
		res = dataLogReader.theHdl->rewind_dataChunks_session(idSess);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnStructSetup(plhs[1]);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	
	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Rewind pointer into file that has the specified dataset ID association
 *///======================================================================
jvxErrorType
mexFileReader::rewindSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::rewindSessionOneSet");
	jvxSize idSess = 0;
	jvxSize idSet = 0;
	if(nrhs > 2)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}

		res = dataLogReader.theHdl->rewind_dataChunks_dataset_session(idSess, idSet);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnStructSetup(plhs[1]);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Return the progress while reading out data for a session (no dataset ID)
 *///==========================================================================
jvxErrorType
mexFileReader::returnProgressSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnProgressSession");
	jvxSize idSess = 0;
	jvxInt64 offset;
	if(nrhs > 1)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		res = dataLogReader.theHdl->position_session(idSess, &offset);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnInt64(plhs[1], offset);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}


	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Return the progress while reading out data for a session and a specific dataset ID.
 *///==========================================================================
jvxErrorType
mexFileReader::returnProgressSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::returnProgressSessionOneSet");

	jvxSize idSess = 0;
	jvxSize idSet = 0;
	jvxInt64 offset;
	if(nrhs > 2)
	{
		const mxArray* arr = prhs[1];
		res = mexArgument2Index<jvxSize>(idSess, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}
		arr = prhs[2];
		res = mexArgument2Index<jvxSize>(idSet, &arr, 0, 1);
		if(res != JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for dataset ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			return(JVX_NO_ERROR);
		}

		res = dataLogReader.theHdl->position_dataset_session(idSess, idSet, &offset);
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
				this->mexReturnInt64(plhs[1], offset);
		}
		else
		{
			internalErrorMessage += jvxErrorType_descr(res);
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		return(JVX_NO_ERROR);
	}

	if(nlhs > 0)
		this->mexReturnBool(plhs[0], false);
	if(nlhs > 1)
		this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for session ID is missing", JVX_ERROR_INVALID_ARGUMENT);
	return(JVX_NO_ERROR);
}

/**
 * Unscan the sessions
 *///================================================================================0
jvxErrorType
mexFileReader::postprocess(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::postprocess");
	res = dataLogReader.theHdl->postprocess();
	if(res == JVX_NO_ERROR)
	{
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if(nlhs > 1)
			this->mexReturnStructSetup(plhs[1]);
	}
	else
	{
		internalErrorMessage += jvxErrorType_descr(res);
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
	}
	return(JVX_NO_ERROR);
}

/**
 * Close the file
 *///==============================================================================0
jvxErrorType
mexFileReader::deactivate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxDataLogReader::activate");


	res = dataLogReader.theHdl->deactivate();
	if(res == JVX_NO_ERROR)
	{
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if(nlhs > 1)
			this->mexReturnStructSetup(plhs[1]);
	}
	else
	{
		internalErrorMessage += jvxErrorType_descr(res);
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage, JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
	}
	return(JVX_NO_ERROR);
}


//======================================================================
//======================================================================
// MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS
//======================================================================
//======================================================================

/**
 * Return a struct for a datachunk
 *///===============================================================
void 
mexFileReader::mexReturnStructTagsSession(mxArray*& plhs, std::vector<oneTagEntry> lst )
{
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;

	dims[0] = 1;
	dims[1] = (int)lst.size();
	plhs = mxCreateCellArray(ndim, dims);
	for(i = 0; i < lst.size(); i++)
	{
		mexReturnStructOneTag(arr, lst[i].tagName, lst[i].tagValue);
		mxSetCell(plhs, i, arr);
		arr = NULL;
	}
}

void 
mexFileReader::mexReturnStructOneTag(mxArray*& plhs, const std::string& tagName, const std::string& tagValue)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;

	const char** fld = new const char*[2];
	std::string fldName1 = FLD_NAME_SESSION_TAG_NAME;
	std::string fldName2 = FLD_NAME_SESSION_TAG_VALUE;

	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();
	
	plhs = mxCreateStructArray(ndim, dims, 2, fld);
	delete[](fld);

	//===================================================

	this->mexReturnString(arr, tagName);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	this->mexReturnString(arr, tagValue);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;
}

/**
 * Return a struct for a datachunk
 *///===============================================================
void 
mexFileReader::mexReturnStructDatachunk(mxArray*& plhs, jvxLogFileDataChunkHeader* headerDChunk)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;

	const char** fld = new const char*[6];
	std::string fldName1 = FLD_NAME_DATATYPE;
	std::string fldName2 = FLD_NAME_ID_DATASET;
	std::string fldName3 = FLD_NAME_USERID;
	std::string fldName4 = FLD_NAME_USERSUBID;
	std::string fldName5 = FLD_NAME_TICKTIME;
	std::string fldName6 = FLD_NAME_DATAFIELD;

	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();
	fld[2] = fldName3.c_str();
	fld[3] = fldName4.c_str();
	fld[4] = fldName5.c_str();
	fld[5] = fldName6.c_str();

	plhs = mxCreateStructArray(ndim, dims, 6, fld);
	delete[](fld);

	//===================================================

	this->mexReturnInt32(arr, headerDChunk->dataTypeGlobal);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDChunk->idUser);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDChunk->idDataChunk);
	mxSetFieldByNumber(plhs, 0, 2, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDChunk->subIdUser);
	mxSetFieldByNumber(plhs, 0, 3, arr);
	arr = NULL;

	this->mexReturnInt64(arr, headerDChunk->ticktime_us);
	mxSetFieldByNumber(plhs, 0, 4, arr);
	arr = NULL;

	char* fldTxt;
	std::string txt;

	switch((jvxDataFormat)headerDChunk->dataTypeGlobal)
	{
	case JVX_DATAFORMAT_STRING:
		fldTxt = new char[headerDChunk->numberElements+1];
		memset(fldTxt, 0, sizeof(char)*(headerDChunk->numberElements+1));
		memcpy(fldTxt, (((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), sizeof(char)*(headerDChunk->numberElements));
		txt = fldTxt;
		this->mexReturnString(arr, txt);
		delete[](fldTxt);
		break;
	case JVX_DATAFORMAT_8BIT:
		this->mexReturnInt8List(arr, reinterpret_cast<jvxInt8*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
		break;
	case JVX_DATAFORMAT_DATA:
		
		if(floatingPointIdReader == JVX_FLOAT_DATAFORMAT_ID)
		{
			this->mexReturnDataList(arr, reinterpret_cast<jvxData*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
		}
		else
		{
#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
			this->mexReturnFloatList(arr, reinterpret_cast<float*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
#else
			this->mexReturnDoubleList(arr, reinterpret_cast<double*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
#endif
		}

		break;
	case JVX_DATAFORMAT_16BIT_LE:
		this->mexReturnInt16List(arr, reinterpret_cast<jvxInt16*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		this->mexReturnInt32List(arr, reinterpret_cast<jvxInt32*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		this->mexReturnInt64List(arr, reinterpret_cast<jvxInt64*>(((jvxByte*)headerDChunk)+sizeof(jvxLogFileDataChunkHeader)), headerDChunk->numberElements);
		break;
	default:
		this->mexReturnString(plhs, "<DATATYPE NOT SUPPORTED>");
	}
	mxSetFieldByNumber(plhs, 0, 5, arr);
	arr = NULL;

}

/**
 * Return a struct for a dataset
 *///===============================================================
void 
mexFileReader::mexReturnStructDataset(mxArray*& plhs, jvxLogFileDataSetHeader* headerDSet)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;

	const char** fld = new const char*[5];
	std::string fldName1 = FLD_NAME_DESCRIPTION_DATASET;
	std::string fldName2 = FLD_NAME_DATASET_DATATYPE;
	std::string fldName3 = FLD_NAME_DATASET_DATATYPE_USER;
	std::string fldName4 = FLD_NAME_DATASET_ID;
	std::string fldName5 = FLD_NAME_DATASET_LENGTH;

	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();
	fld[2] = fldName3.c_str();
	fld[3] = fldName4.c_str();
	fld[4] = fldName5.c_str();

	plhs = mxCreateStructArray(ndim, dims, 5, fld);
	delete[](fld);

	//===================================================

	this->mexReturnString(arr, headerDSet->noteOnType);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDSet->dataTypeGlobal);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDSet->dataDescriptorUser);
	mxSetFieldByNumber(plhs, 0, 2, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDSet->idDataChunk);
	mxSetFieldByNumber(plhs, 0, 3, arr);
	arr = NULL;

	this->mexReturnInt32(arr, headerDSet->numberElementsIfConstant);
	mxSetFieldByNumber(plhs, 0, 4, arr);
	arr = NULL;


}
/**
 * Function to return the main setup struct which is formed by calling subfunctions to return all fields.
 *///===========================================================
void 
mexFileReader::mexReturnStructSetup(mxArray*& plhs)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;
	jvxState theState = JVX_STATE_NONE;

	const char** fld = new const char*[7];
	std::string fldName0 = FLD_NAME_COPYRIGHT;
	std::string fldName1 = FLD_NAME_VERSION;
	std::string fldName2 = FLD_NAME_MODULENAME;
	std::string fldName3 = FLD_NAME_STATE;
	std::string fldName4 = FLD_NAME_FILENAME;

	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();
	fld[3] = fldName3.c_str();
	fld[4] = fldName4.c_str();

	plhs = mxCreateStructArray(ndim, dims, 5, fld);
	delete[](fld);

	//===================================================

	this->mexReturnString(arr, EXPRESSION_COPYRIGHT);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	this->mexReturnString(arr, EXPRESSION_VERSION);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;

	this->mexReturnString(arr, EXPRESSION_MODULENAME);
	mxSetFieldByNumber(plhs, 0, 2, arr);
	arr = NULL;

	bool isInit = false;
	this->dataLogReader.theObj->state(&theState);
	this->mexReturnInt32(arr, isInit);
	mxSetFieldByNumber(plhs, 0, 3, arr);
	
	arr = NULL;
	if(theState >= JVX_STATE_ACTIVE)
	{
		jvxApiString fName;
		std::string nm = "unknown";
		jvxErrorType res = dataLogReader.theHdl->filename(&fName);
		if(res == JVX_NO_ERROR)
		{
			nm = fName.std_str();
		}
		this->mexReturnString(arr, nm);
	}
	mxSetFieldByNumber(plhs, 0, 4, arr);
}

//==========================================================================================
// MEX RETURN GENERIC RUNTIME CONFIGURATION MEX RETURN GENERIC RUNTIME CONFIGURATION MEX RETURN GENERIC RUNTIME CONFIGURATION
//==========================================================================================
// ================================================================================
// ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR
// ================================================================================

/**
 * Destructor: Set all values to initial, unload the library for fileReading and leave.
 *///======================================================================================
mexFileReader::~mexFileReader(void)
{
	isInitialized = false;
	filenameSpecified = false;
	isScanned = false;
	if(dataLogReader.theHdl)
	{
		jvxTDataLogReader_terminate(dataLogReader.theObj);
	}
	dataLogReader.theObj = NULL;
	dataLogReader.theHdl = NULL;
}
