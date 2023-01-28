#ifndef _IJVXDATALOGREADER_H__
#define _IJVXDATALOGREADER_H__


JVX_INTERFACE IjvxDataLogReader: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxDataLogReader(){};

	/** List of currently supported datatypes. May be subject to
	* further datatypes in the future, therefore using IDs.*/

	// =====================================================================================
	// Interface API
	// =====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(jvxDataRecoveryCallback eventCallback) = 0;

	//! Step-1: Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	//! Step2: Open file to read from
	virtual jvxErrorType JVX_CALLINGCONVENTION activate(const char* filename) = 0;

	//! Copy the name of the file opened
	virtual jvxErrorType JVX_CALLINGCONVENTION filename(jvxApiString* fName) = 0;

	//! Step-2: Close open file, this results in unscanning
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;

	/** Step3: Scann for all available sessions. After this user may return 
	*number of sessions/datasets for sessions. File rewind to first chunk for each session*/
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() = 0;

	//! Opposite of scan, forget all sessions etc..
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION floating_point_id_file(jvxUInt8* id) = 0;

	//! Return the number of available sessions
	virtual jvxErrorType JVX_CALLINGCONVENTION number_sessions(jvxSize* numSess) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_tags_session(jvxSize idSess, jvxSize* numSets) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION name_value_tags_session(jvxSize idSess, jvxSize idTag, jvxApiString* name, jvxApiString* value) = 0; 

	//! Return the number of available datasets
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datasets_session(jvxSize idSess, jvxSize* numSets) = 0;

	//! Return the size for a dataset. Datasets may have different size
	virtual jvxErrorType JVX_CALLINGCONVENTION size_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxSize* szSet) = 0;

	//! Copy the dataset to a pre allocated field from application
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxLogFileDataSetHeader* ptrHandle, jvxSize sz) = 0;

	//! Return the number of datachunks in session (no dataset id)
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datachunks_session(jvxSize idSession, jvxSize* numChunks) = 0;

	//! Return the number of datachunks in session (no dataset id)
	virtual jvxErrorType JVX_CALLINGCONVENTION number_datachunks_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxSize* numChunks) = 0;

	//! Rewind the global file position
	virtual jvxErrorType JVX_CALLINGCONVENTION rewind_dataChunks_session(jvxSize idSession) = 0;

	//! Rewind the dataset file position
	virtual jvxErrorType JVX_CALLINGCONVENTION rewind_dataChunks_dataset_session(jvxSize idSession, jvxSize idDataSet) = 0;

	//! Return the next data chunk in file with the right dataset id. The position is at the beginning of this chunk!
	virtual jvxErrorType JVX_CALLINGCONVENTION find_size_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxSize* szNextChunk) = 0;

	//! Return the next data chunk in file. The position is at the beginning of this chunk!
	virtual jvxErrorType JVX_CALLINGCONVENTION find_size_next_datachunk_session(jvxSize idSession, jvxSize* szFld) = 0;

	//! Copy the next datachunk. It is assumed that the position is right before the next available. If id is wrong, nothing is copied
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxLogFileDataChunkHeader* newFld, jvxSize szFld) = 0;

	//! Copy next datachunk
	virtual jvxErrorType JVX_CALLINGCONVENTION copy_next_datachunk_session(jvxSize idSession, jvxLogFileDataChunkHeader* newFld, jvxSize szFld) = 0;

	//! Offset of current datachunk in session
	virtual jvxErrorType JVX_CALLINGCONVENTION position_session(jvxSize idSession, jvxInt64 *idDatachunkOffset) = 0;

	//! Offset of current datachunk in session for specified dataset
	virtual jvxErrorType JVX_CALLINGCONVENTION position_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxInt64 *idDatachunkOffset) = 0;

};

#endif
