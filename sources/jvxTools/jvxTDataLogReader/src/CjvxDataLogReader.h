#ifndef __CJVXDATALOGREADER_H__
#define __CJVXDATALOGREADER_H__

#include <string>
#include <vector>

#include "jvx.h"

#include "common/CjvxObject.h"

class CjvxDataLogReader: public IjvxDataLogReader, public CjvxObject
{
private:

	//! Typedefs for states in reading statemachine
	typedef enum
	{
		//! Start reading
		BEFORE_START_SESSION,
		
		//! The header that indicates that a session has been started has been read
		START_SESSION_SCANNED,

		//! The number of available datasets has been scanned
		NUMBER_SETS_SCANNED,

		//! The datasets have been completely scanned
		SETS_SCANNED

	} statesScanFile;

	//! For each dataset there is the dataset itself and a position pointer stored
	typedef struct
	{
		//! Stire the dataset itself
		jvxLogFileDataSetHeader* ptrDataSet;

		//! Store the offset from beginning of file to store file access pointer
		jvxInt64 offsetNextChunkCurrentDataSet;

		//! ID of next datachunk (current position counter!)
		jvxInt64 idNextChunk;

		//! Number of datachunks with this ID
		jvxSize numDatachunksSetID;
	} structDataSet;

	//! Store data with each data session
	typedef struct
	{
		//! Store all datasets
		std::vector<structDataSet> dataSets;
	
		//! Store offset position to rescan datasets if required
		jvxInt64 offsetPositionDatasets;

		//! Offset position to rewiund to beginning of datachunks
		jvxInt64 offsetDataChunksStart;

		//! Current position when scanning for datachunks without dataset specification
		jvxInt64 offsetDataChunksCurrent;

		long idNextChunk;

		//! Store the number of chunks in this session
		long numDataChunks;

		std::vector<oneTagEntry> theTags;

	} structSession;
	
	/** Struct for file handling
	 *///=====================================================
	struct
	{
		//! Pointer to the file
		FILE* fPtr;

		//! Name of the file
		std::string fName;

		jvxInt64 fSize;

		jvxData nextReportProgress;

		jvxUInt8 floatingPointId;
	} fileHandle;

	
	//! Store list of sessions
	std::vector<structSession> sessionList;
	
	jvxDataRecoveryCallback eventCallbackLocal;
	

public:

	//! Constructor
	CjvxDataLogReader(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	~CjvxDataLogReader(void);

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(jvxDataRecoveryCallback eventCallback)override;

	//! Step-1: Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

	//! Step2: Open file to read from
	virtual jvxErrorType JVX_CALLINGCONVENTION activate(const char* filename)override;

	//! Copy the name of the file opened
	virtual jvxErrorType JVX_CALLINGCONVENTION filename(jvxApiString* fName)override;

	//! Step-2: Close open file, this results in unscanning
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	/** Step3: Scann for all available sessions. After this user may return 
	*number of sessions/datasets for sessions. File rewind to first chunk for each session*/
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	//! Opposite of scan, forget all sessions etc..
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION floating_point_id_file(jvxUInt8* id)override;

	//! Return the number of available sessions
	virtual jvxErrorType JVX_CALLINGCONVENTION number_sessions(jvxSize* numSess)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_tags_session(jvxSize idSess, jvxSize* numSets)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION name_value_tags_session(jvxSize idSess, jvxSize idTag, jvxApiString* name, jvxApiString* value)override; 

	//! Return the number of available datasets
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datasets_session(jvxSize idSess, jvxSize* numSets)override;

	//! Return the size for a dataset. Datasets may have different size
	virtual jvxErrorType JVX_CALLINGCONVENTION size_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxSize* szSet)override;

	//! Copy the dataset to a pre allocated field from application
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxLogFileDataSetHeader* ptrHandle, jvxSize szFld)override;

	//! Return the number of datachunks in session (no dataset id)
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datachunks_session(jvxSize idSession, jvxSize* numChunks)override;

	//! Return the number of datachunks in session (no dataset id)
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datachunks_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxSize* numChunks)override;

	//! Rewind the global file position
	virtual jvxErrorType JVX_CALLINGCONVENTION rewind_dataChunks_session(jvxSize idSession)override;

	//! Rewind the dataset file position
	virtual jvxErrorType JVX_CALLINGCONVENTION rewind_dataChunks_dataset_session(jvxSize idSession, jvxSize idDataSet)override;

	//! Return the next data chunk in file with the right dataset id. The position is at the beginning of this chunk!
	virtual jvxErrorType JVX_CALLINGCONVENTION find_size_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxSize* szNextChunk)override;

	//! Return the next data chunk in file. The position is at the beginning of this chunk!
	virtual jvxErrorType JVX_CALLINGCONVENTION find_size_next_datachunk_session(jvxSize idSession, jvxSize* szFld)override;

	//! Copy the next datachunk. It is assumed that the position is right before the next available. If id is wrong, nothing is copied
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet,
										       jvxLogFileDataChunkHeader* newFld, jvxSize szFld)override;

	//! Copy next datachunk
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_next_datachunk_session(jvxSize idSession, jvxLogFileDataChunkHeader* newFld, jvxSize szFld)override;

	//! Offset of current datachunk in session
	virtual jvxErrorType JVX_CALLINGCONVENTION position_session(jvxSize idSession, jvxInt64 *idDatachunkOffset)override;

	//! Offset of current datachunk in session for specified dataset
	virtual jvxErrorType JVX_CALLINGCONVENTION position_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxInt64 *idDatachunkOffset)override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

/*	//! Step1: Initialize this module
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION initialize(rtpDataRecoveryCallback eventCallback);
	
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION isInitialized(bool* retVal);

	//! Step-1: Terminate
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION terminate();

	//! Step2: Open file to read from
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION openFile(const char* filename);

	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION isFileOpen(bool* retval);

	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getLengthFilename(size_t* szFld);

	//! Copy the name of the file opened
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getFilename(char* fld, size_t szFld);

	//! Step-2: Close open file, this results in unscanning
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION closeFile();

	/** Step3: Scann for all available sessions. After this user may return 
	 *number of sessions/datasets for sessions. File rewind to first chunk for each session* /
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION scanForSessions();

	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION isScanned(bool* retVal);

	//! Opposite of scan, forget all sessions etc..
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION unscanSessions();

	//! Return the number of available sessions
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getNumberSessions(unsigned* numSess);

	//! Return the number of available datasets
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getNumberDatasetsSession(unsigned idSession, unsigned* numSets);

	//! Return the size for a dataset. Datasets may have different size
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getSizeDatasetSession(unsigned idSession, unsigned idDataSet, size_t* szSet);

	//! Copy the dataset to a pre allocated field from application
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION copyDatasetSession(unsigned idSession, unsigned idDataSet, rtpLogFileDataSetHeader* ptrHandle, size_t szFld);

	//! Return the number of datachunks in session (no dataset id)
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getNumberDataChunksSession(unsigned idSession, unsigned* numChunks);

	//! Return the number of datachunks in session (no dataset id)
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getNumberDataChunksSessionDataset(unsigned idSession, unsigned idSet, unsigned* numChunks);

	//! Return the timing divisor (dots per second!)
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getDivisorTimingSession(unsigned idSession, unsigned long long* divisor);

	//! Rewind the global file position
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION rewindDataChunksSession(unsigned idSession);

	//! Rewind the dataset file position
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION rewindDataChunksSessionDataset(unsigned idSession, unsigned idSet);

	//! Return the next data chunk in file with the right dataset id. The position is at the beginning of this chunk!
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION findAndGetSizeNextDataChunkSessionDataset(unsigned idSession, unsigned idSet, size_t* szFld);

	//! Return the next data chunk in file. The position is at the beginning of this chunk!
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION findAndGetSizeNextDataChunkSession(unsigned idSession, size_t* szFld);

	//! Copy the next datachunk. It is assumed that the position is right before the next available. If id is wrong, nothing is copied
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION copyNextDataChunkSessionDataset(unsigned idSession, unsigned idSet, rtpLogFileDataChunkHeader* newFld, size_t szFld);

	//! Copy next datachunk
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION copyNextDataChunkSession(unsigned idSession, rtpLogFileDataChunkHeader* newFld, size_t szFld);

	//! Offset of current datachunk in session
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getDataPositionSession(unsigned idSession, long *idDatachunkOffset);

	//! Offset of current datachunk in session for specified dataset
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getDataPositionSessionDataset(unsigned idSession, unsigned idSet, long *idDatachunkOffset);

	//! Offset of current datachunk in session for specified dataset
	JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION returnDataPositionSessionDataset(unsigned idSession, unsigned idSet, long *idDatachunkOffset);


		//! IrtpObjects API: Function to return an objects name
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getName(rtpString** name_argout);

	//! IrtpObjects API: Function to return an objects description
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getDescription(rtpString** name_argout);

	//! IrtpObjects API: Function to return an error code if a member function has recently failed.
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getLastError(rtpError** err_argout);

	//! IrtpObjects API: Function to return a reference to upper level objects
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION getInterfaceSpecialization(rtpHandle** link, rtpInterfaceType* type);

	//! IrtpObjects API: Deallocation function for data type, deallocation of the side of the dynamic library
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION deallocate(rtpString* tobeRemoved);
	
	//! IrtpObjects API: Deallocation function for data type, deallocation of the side of the dynamic library
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION deallocate(rtpError* tobeRemoved);

	//! Function that returns wether object is a unique global object (request always returns the same object) or not
	virtual JVX_API_RETURN_TYPE JVX_CALLINGCONVENTION isUniqueGlobalObject(rtpBool* isUnique);* /
	~dataRecovery(void);*/
};

#endif
