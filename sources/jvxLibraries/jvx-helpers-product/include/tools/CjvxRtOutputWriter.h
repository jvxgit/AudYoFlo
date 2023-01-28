#ifndef __CJVXRTOUTPUTWRITER_H__
#define __CJVXRTOUTPUTWRITER_H__

#include "jvx.h"

// Parmaters set: 
#define JVX_OUTPUTWRITER_SHIFT_PARAMETERSET 1
#define JVX_OUTPUTWRITER_SHIFT_TRIGGERSTART 2

class CjvxRtOutputWriter
{
protected:
	std::string fNamePrefix;
	JVX_MUTEX_HANDLE lock;
	IjvxRtAudioFileWriter* writer;
	IjvxObject* writer_obj;
	jvxSize buffersize;
	jvxSize samplerate;
	jvxSize num_channels;
	jvxDataFormat format_app;

	jvxSize wavFileResolution;
	jvxSize wavFileType;
	jvxSize numBufs = 10;

	jvxCBitField state_outfile;

	IjvxToolsHost* tHost;
	IjvxHiddenInterface* hHost;

public:
	CjvxRtOutputWriter();

	jvxErrorType initialize(IjvxToolsHost* tHost, IjvxHiddenInterface* hHost);
	jvxErrorType terminate();

	jvxErrorType set_fname_prefix(const std::string& fnamep);

	jvxErrorType set_parameters(jvxSize buffersizeArg, jvxSize samplerateArg, jvxSize numchannelsArg, jvxDataFormat form_appl);
	jvxErrorType reset_parameters();
	
	jvxErrorType set_wav_props(jvxSize wavFileResolution, jvxSize wavFileType, jvxSize numBufsArg);

	jvxErrorType trigger_start();
	jvxErrorType trigger_stop();

	jvxErrorType write_data(jvxHandle** bufs);
private:
	jvxErrorType start_file_writer_lock();
	jvxErrorType stop_file_writer_lock();
};

#endif
