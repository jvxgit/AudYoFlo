/* 
 *****************************************************
 * Filename: mexFileReader.h
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

#ifndef __MEXFILEREADER_H__
#define __MEXFILEREADER_H__

#include <vector>
#include "jvx.h"
#include "CjvxCToMatlabConverter.h"
/**
 * Object to wrap the fileReading functionality. This class dispatches 
 * MEX functioncalls to the underlying library component for fileReading
 *///=================================================================
class mexFileReader: public CjvxCToMatlabConverter
{
	//! Member variable: is component initilalized
	bool isInitialized;

	//! Member variable: has filename been specified
	bool filenameSpecified;

	//! Member variable: has file been scanned for sessions etc.
	bool isScanned;

	jvxUInt8 floatingPointIdReader;
	jvxUInt8 floatingPointIdFile;

	//! Reference to underlying fileReader functionality module
	struct
	{
		IjvxDataLogReader* theHdl;
		IjvxObject* theObj;
	} dataLogReader;
public:

	//! Constructor
	mexFileReader(void);

	//! Destructor
	~mexFileReader(void);

	//! Main wrapper function to scan for MEX input arguments
	bool parseInput(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]);
	

	//! For command OPENFILE: Open the input file
	jvxErrorType activate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command SCAN SESSIONS: Scan for sessions etc.
	jvxErrorType prepare(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NUMBER SESSIONS: Return the number of available sessions
	jvxErrorType returnNumberSessions(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	jvxErrorType returnTagsSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NUMBER DATASETS FOR SESSION: Return the number of available datasets for a session
	jvxErrorType returnNumberDataSetsSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NUMBER DATACHUNKS FOR SESSION
	jvxErrorType returnNumberDataChunksSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NUMBER DATACHUNKS FOR SESSION FOR DATASET
	jvxErrorType returnNumberDataChunksSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN DATASET
	jvxErrorType returnDataSetSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN TIMING DIVISOR
	jvxErrorType returnTimingDivisorSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NEXT DATACHUNK SESSION
	jvxErrorType returnNextDatachunkSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN NEXT DATACHUNK SESSION FOR DATASET ID
	jvxErrorType returnNextDatachunkSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN DATACHUNK FIELDS FOR SESSION FOR DATASET ID (FAST ACCESS)
	jvxErrorType returnNextDatachunkSessionOneSetConstantSize(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command REWIND DATA ACCESS
	jvxErrorType rewindSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command REWIND DATA ACCESS FOR ONE DATASET
	jvxErrorType rewindSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN PROGRESS
	jvxErrorType returnProgressSession(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command RETURN PROGRESS FOR ONE SET
	jvxErrorType returnProgressSessionOneSet(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command UNSCAN SESSIONS
	jvxErrorType postprocess(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	//! For command CLOSE INPUT FILE
	jvxErrorType deactivate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);


	//! Print usage of MATLAB module
	void printUsage();

	//! Return data in MEX format
	void mexReturnStructSetup(mxArray*& plhs);

	//! Return data in MEX format
	void mexReturnStructDatachunk(mxArray*& plhs, jvxLogFileDataChunkHeader* headerDChunk);

	//! Return data in MEX format
	void mexReturnStructDataset(mxArray*& plhs, jvxLogFileDataSetHeader* headerDSet);

	void mexReturnStructTagsSession(mxArray*& plhs, std::vector<oneTagEntry> lst );
	
	void mexReturnStructOneTag(mxArray*& plhs, const std::string& tagName, const std::string& tagValue);

};

#endif
