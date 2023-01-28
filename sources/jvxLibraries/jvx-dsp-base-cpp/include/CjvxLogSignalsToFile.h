#ifndef __CJVXLOGSIGNALS_FILE_H__
#define __CJVXLOGSIGNALS_FILE_H__

#include "jvx.h"
#include "jvx-helpers.h"

class CjvxLogSignalsToFile: public HjvxDataLogger
{
private:

	typedef struct
	{
		jvxSize numChannels;
		jvxSize bsize;
		jvxDataFormat form;
		std::string channelName;
		jvxInt32 dataDescriptor;
		jvxSize offsetChannels;
	} oneLogFileLane;

	IjvxObject* theDataLogger_obj;
	IjvxDataLogger* theDataLogger_hdl;
	IjvxToolsHost* theToolsHost;		
	IjvxHiddenInterface* theHost;

	std::vector<oneLogFileLane> theStoreLanes;

	jvxHandle** fldWrite_frames_channels;
	jvxSize* idxWrite_frames_channels;

	jvxState status_file;
	jvxState status_module;

	std::string folder;
	std::string fName_prefix;

	std::string nmFile;
	JVX_MUTEX_HANDLE safeAccess;
	
	jvxSize numFramesMemory;
	jvxSize stopTimeout_msec;
	jvxSize numChannelsAll;

public:

	CjvxLogSignalsToFile();
	~CjvxLogSignalsToFile();
	
	jvxErrorType init_logging(IjvxHiddenInterface* theHostIn, jvxSize numFramesMemory, jvxSize stopTimeout_msec);

	jvxErrorType add_one_lane(jvxSize numChans, jvxSize bSize, jvxDataFormat form, const std::string& channelName, jvxInt32 dataDescriptor, jvxSize& uniqueId);
	jvxErrorType remove_all_lanes();

	jvxErrorType activate_logging(std::string folder, std::string fName_prefix);
	jvxErrorType deactivate_logging();
	jvxErrorType reactivate_logging();

	jvxErrorType add_tag(const char* tagName, const char* tagValue);
	jvxErrorType remove_all_tags();

	jvxErrorType start_processing(	);
	jvxErrorType stop_processing();
	
	jvxErrorType lock_logging();
	jvxErrorType try_lock_logging();
	jvxErrorType unlock_logging();

	//! Use a locked version if you want to activate/deactivate logging while processing. If logging is started in prepare and stopped
	//! there is no need for the log since in postprocessing and prepare, processing is always inactive.
	//! Note that there is another lock in the core logfile writer to secure access to the internal circular buffer. This cannot
	//! be bypassed since it would break the functionality.
	jvxErrorType process_logging_lock(jvxSize unqiueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent);
	jvxErrorType process_logging_try(jvxSize unqiueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent);
	jvxErrorType process_logging_nolock(jvxSize unqiueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent);

	jvxErrorType terminate_logging();

private:
	jvxErrorType logfile_select_nolock(std::string fname_path, std::string fname_prefix);
	jvxErrorType logfile_start_nolock();
	jvxErrorType logfile_stop_nolock();
	jvxErrorType logfile_unselect_nolock();
	void reportText(std::string txt);
};

#endif
