#ifndef _IJVXDATALOGGER_H__
#define _IJVXDATALOGGER_H__

JVX_INTERFACE IjvxDataLogger: public IjvxObject
{
public:
  virtual JVX_CALLINGCONVENTION ~IjvxDataLogger(){};

	/** List of currently supported datatypes. May be subject to
	 * further datatypes in the future, therefore using IDs.*/

	// =====================================================================================
	// Interface API
	// =====================================================================================

	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(jvxSize numberBytesBuffer, jvxSize szDataFieldWriteAtOnce, IjvxHiddenInterface* hostRef = NULL) = 0;

	//! Function to return the current value for timing
	virtual jvxErrorType JVX_CALLINGCONVENTION timestamp_us(jvxInt64* timing) = 0;

	//! Open the file for data logging
	virtual jvxErrorType JVX_CALLINGCONVENTION activate(const char* fileNameOutput, jvxBool append = false) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION filename(jvxApiString* fName) = 0;

	//! Add another data handle. Must be added before starting to record.
	virtual jvxErrorType JVX_CALLINGCONVENTION associate_datahandle(jvxLogFileDataSetHeader* ptrOutside) = 0;

	//! Deassociate another data handle. Must be added before starting to record.
	virtual jvxErrorType JVX_CALLINGCONVENTION deassociate_datahandle(jvxLogFileDataSetHeader* ptrOutside) = 0;

	//! Start recording: Set up parallel thread and be ready for incoming datachunks 
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxBool applyPrioBoost = true) = 0;

	//! Pass a new data set to written to harddrive
	virtual jvxErrorType JVX_CALLINGCONVENTION pass_new(jvxLogFileDataChunkHeader* addData, jvxBool noBlock) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION buffer_fillheight(jvxData* bFillHeight) = 0;

	//! Stop recording: Stop parallel thread, flush remaining data and leave file open
	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxSize* byteCounterFromAppl = NULL, jvxSize* byteCounterToFile = NULL, jvxInt32 timeoutStop_ms = 2000) = 0;

	//! Close the file for datalogging
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;

	//! Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addSessionTag(const char* tagName, const char* tagValue) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION removeAllSessionTags() = 0;

};

#endif
