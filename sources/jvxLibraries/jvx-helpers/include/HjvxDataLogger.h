#ifndef __HJVXDATALOGGER_H__
#define __HJVXDATALOGGER_H__

#include "jvx.h"

class HjvxDataLogger
{
	private:

	typedef struct
	{
		jvxDataFormat format;
		jvxInt32 numElements;
		std::string descriptionLine;
		jvxInt64 numEntriesAdded;
		jvxInt64 numEntriesFailed;
		jvxInt32 descriptorUser;
	} oneLine;

	jvxState _theState;

	IjvxDataLogger* _loggerData;

	std::vector<oneLine> _activeLines;

	jvxSize _numFramesMemory;

	jvxInt32 _timeoutStop_ms;

	jvxBool _nonBlockingWrite;

	std::string _filename;

	struct
	{
		jvxLogFileDataSetHeader** headerSetPtrsLines;
		jvxLogFileDataChunkHeader** headerChunkPtrsLines;
	} _runtime;
public:
	HjvxDataLogger();
	~HjvxDataLogger();

	jvxErrorType _state(jvxState& stateReturn);

	// Start in state JVX_STATE_INIT
	jvxErrorType _activate(IjvxDataLogger* logFileRef, jvxSize numFramesMemory, jvxBool nonBlockingWrite, jvxInt32 timeoutstop_ms);
	jvxErrorType _add_line(const std::string description, jvxSize numElements, jvxDataFormat formatElements, jvxInt32 dataDescriptorUser, jvxSize& theId);
	jvxErrorType _number_lines(jvxSize& numOnReturn);
	jvxErrorType _description_line(std::string& descOnReturn, jvxSize lineNo);
	jvxErrorType _number_elements_line(jvxInt32& numElements, jvxSize lineNo);
	jvxErrorType _format_elements_line(jvxDataFormat& format, jvxSize lineNo);
	jvxErrorType _number_entries_line(jvxInt64& numAddedDataChunks, jvxSize lineNo);
	jvxErrorType _number_entries_failed_line(jvxInt64& numDatachunksFailed, jvxSize lineNo);
	jvxErrorType _start(const std::string& filename, std::vector<std::string>& errMessages, IjvxHiddenInterface* hostRef);

	// All available in state JVX_STATE_PROCESSING
	jvxErrorType _field_reference(jvxSize idxLine, jvxByte** field, jvxSize* numElms = NULL);
	jvxErrorType _add_data_reference(jvxSize idxLine, jvxInt32 idUser, jvxInt32 subIdUser, std::string& errDescr, bool noBlock = false);
	jvxErrorType _stop(jvxSize* numBytesWrittenToBuffer, jvxSize* numBytesWrittenToFile, std::vector<std::string>& errorDescrs);
	jvxErrorType _clean_all();
	jvxErrorType _deactivate();
	jvxErrorType _handle_logfile(IjvxDataLogger** hdlOnReturn);
	jvxErrorType _fill_height(jvxData& fHeight);
	jvxErrorType _add_session_tag(std::string tagName, std::string tagValue);
	jvxErrorType _remove_session_tags();
};

#endif

