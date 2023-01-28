#ifndef __CJVXRTAUDIOBUFFERS_H__
#define __CJVXRTAUDIOBUFFERS_H__

#include "jvx.h"
#include "CjvxOneAudioBuffer.h"
#include <vector>

class CjvxRtAudioBuffers
{
private:
	jvxState theState;
	JVX_MUTEX_HANDLE safeAccessEntries;
	struct
	{
		jvxSize numberChannels;
		jvxInt32 rate;
		jvxBool loop;
		std::string sessionName;
		std::string sessionName_tmp;
	} config;

	struct
	{
		CjvxOneAudioBuffer* entryRead;
		CjvxOneAudioBuffer* entryDraw;
		CjvxOneAudioBuffer* entryWrite;
		jvxBool offlineMode;
		jvxInt32 idSelect;
	} runtime;

	std::vector<CjvxOneAudioBuffer*> entries;

public:

	CjvxRtAudioBuffers();
	~CjvxRtAudioBuffers();

	jvxErrorType configure_type(jvxSize numberChannels, jvxInt32 rate);
	jvxErrorType configure_session(const char* session_name, const char* session_name_tmp);

	jvxErrorType number_entries(jvxSize& num);
	jvxErrorType tag_entry(jvxSize id, std::string& tag);
	jvxErrorType edit_tag_entry(jvxInt32 id, std::string tag);
	//jvxErrorType import_file(const char* fName);
	//jvxErrorType export_to_file(const char* fName, jvxSize idEntry);
	jvxErrorType fill_view_buffer(jvxData** buffersY, jvxData** buffersX, jvxSize bLength);
	jvxErrorType fill_view_buffer_online(jvxData** buffersY, jvxData** buffersX, jvxSize bLength, jvxInt32 maxNumberSamplesShow);


	jvxErrorType prepare_read();
	jvxErrorType read_frame(jvxData** data, jvxSize number_samples);
	jvxErrorType read_frame_position(jvxData** data, jvxSize number_samples, jvxData progress);
	jvxErrorType postprocess_read();

	jvxErrorType prepare_write(jvxSize, jvxInt32, jvxData, const char*);
	jvxErrorType write_frame(jvxData** data, jvxSize number_samples);
	jvxErrorType write_frame_position(jvxData** data, jvxSize number_samples, jvxData progress);
	jvxErrorType postprocess_write();

	jvxErrorType save_session(IjvxConfigProcessor* proc);
	jvxErrorType open_session(IjvxConfigProcessor* proc);

	jvxErrorType addEntry(CjvxOneAudioBuffer* newBuffer);
	jvxErrorType addRecordedEntry();

	jvxErrorType selection(jvxInt32& id);
	jvxErrorType set_selection(jvxInt32 id);

	jvxErrorType entry_type(jvxSize id, CjvxOneAudioBuffer::jvxAudioBuffertype& tp);
	jvxErrorType entry_description(jvxSize id, std::string& description);
	jvxErrorType entry_filename(jvxSize id, std::string& filename);
	jvxErrorType entry_length_seconds(jvxSize id, jvxData& valD);

	jvxErrorType borrowActiveEntry(CjvxOneAudioBuffer** theBuf);
	jvxErrorType returnActiveEntry(CjvxOneAudioBuffer* theBuf);

	void lock()
	{
		JVX_LOCK_MUTEX(safeAccessEntries);
	};
	void unlock()
	{
		JVX_UNLOCK_MUTEX(safeAccessEntries);
	};

	void set_offline_mode(jvxBool offlineMode)
	{
		runtime.offlineMode = offlineMode;		
	};

	jvxBool loop(){ return(config.loop);};
	void set_loop(jvxBool l){ config.loop = l;};
};

#endif




