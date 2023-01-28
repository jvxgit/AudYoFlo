#ifndef __CJVXDATALOGGER_H__
#define __CJVXDATALOGGER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvx_threads.h"

namespace _myJvxTools {

class CjvxDataLogger: public IjvxDataLogger, public CjvxObject
{
public:

	/** Enum for the datatype for stored datachunk fields, can be enhanced in the future
	 *///===================================================
private:

	struct
	{
		jvxSize sizeDataFieldWriteAtOnce;
		jvxSize bytesBuffer;
		std::vector<jvxLogFileDataSetHeader*> listDataSets;
	} config;

	struct
	{
		jvxByte* buffer;
		jvxSize idxRead;
		jvxSize fillHeight;
		jvxSize bytesBuffer;

	} memory;


	/** Struct for file handling
	 *///=====================================================
	struct
	{
		//! Pointer to the file
		FILE* fPtr;

		//! Name of the file
		std::string fName;

	} fileHandle;

	struct
	{
		jvx_thread_handler thread_handler;
		jvxHandle* thread_hdl;

		//jvxBool loopInThread;

		//! ID of started thread
		//JVX_THREAD_ID idThread;

		//! Handle to the second thread
		//JVX_THREAD_HANDLE handleThread;

		//jvxInt64 startCounter_us;

		jvxSize numBytesWrittenToLogger;
		jvxSize numBytesWrittenToFile;

		jvxTimeStampData tStampRef;

		//! Critical section when accessing byte counter during read operation ( there is always only one read thread!)
		JVX_MUTEX_HANDLE updateFillHeight;

		//! Event to notify new data to be written and end of writing
		//JVX_NOTIFY_HANDLE eventsNotify;

		//JVX_NOTIFY_HANDLE started;

		//! Header to be written for each session
		jvxSessionHeader headerStruct;

	} runtime;

	std::vector<oneTagEntry> theTags;

public:

	//! Constructor: Do nothing
	CjvxDataLogger(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	// =====================================================================================
	// Interface API
	// =====================================================================================

	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(jvxSize numberBytesBuffer, jvxSize szDataFieldWriteAtOnce = -1, IjvxHiddenInterface* hostRef = NULL) override;

	//! Function to return the current value for timing
	virtual jvxErrorType JVX_CALLINGCONVENTION timestamp_us(jvxInt64* timing) override;

	//! Open the file for data logging
	virtual jvxErrorType JVX_CALLINGCONVENTION activate(const char* fileNameOutput, jvxBool append = false) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION filename(jvxApiString* fName) override;

	//! Add another data handle. Must be added before starting to record.
	virtual jvxErrorType JVX_CALLINGCONVENTION associate_datahandle(jvxLogFileDataSetHeader* ptrOutside) override;

	//! Deassociate another data handle. Must be added before starting to record.
	virtual jvxErrorType JVX_CALLINGCONVENTION deassociate_datahandle(jvxLogFileDataSetHeader* ptrOutside)override;

	//! Start recording: Set up parallel thread and be ready for incoming datachunks 
	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxBool applyPrioBoost = true)override;

	//! Pass a new data set to written to harddrive
	virtual jvxErrorType JVX_CALLINGCONVENTION pass_new(jvxLogFileDataChunkHeader* addData, jvxBool noBlock)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION buffer_fillheight(jvxData* bFillHeight)override;

	//! Stop recording: Stop parallel thread, flush remaining data and leave file open
	virtual jvxErrorType JVX_CALLINGCONVENTION stop(jvxSize* byteCounterFromAppl = NULL, jvxSize* byteCounterToFile = NULL, jvxInt32 timeoutStop_ms = 2000)override;

	//! Close the file for datalogging
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	//! Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION addSessionTag(const char* tagName, const char* tagValue)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION removeAllSessionTags()override;

	// =====================================================================================

	//! Destructor
	~CjvxDataLogger(void);

	//! Thread callback for parallel thread to wait for incoming data
	JVX_THREAD_RETURN_TYPE awaitIncomingData();

	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_FUNC_IP(wokeup);

#include "codeFragments/simplify/jvxObjects_simplify.h"

};

} // namespace

#endif
