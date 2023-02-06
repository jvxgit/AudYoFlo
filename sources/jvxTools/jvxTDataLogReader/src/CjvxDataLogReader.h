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

};

#endif
