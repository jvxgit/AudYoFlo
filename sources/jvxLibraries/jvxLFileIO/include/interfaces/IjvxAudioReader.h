#ifndef __JVXAUDIOREADER_H__
#define __JVXAUDIOREADER_H__

#include "interfaces/IjvxAudio.h"

JVX_INTERFACE IjvxAudioReader: public IjvxAudio
{
public:
	IjvxAudioReader(){};
	virtual ~IjvxAudioReader() {};
	virtual jvxErrorType set_loop(jvxBool) = 0;

	virtual jvxErrorType get_loop(jvxBool* loop) = 0;

	virtual jvxErrorType read_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize, jvxDataFormat format) = 0;

	virtual jvxErrorType read_one_buf_raw(jvxByte* fld_out, jvxSize num_bytes_to_read, jvxSize* num_bytes_at_read) = 0;

	virtual jvxErrorType current_progress(jvxSize* progress_samples) = 0;

	virtual jvxErrorType rewind() = 0;

	virtual jvxErrorType wind_bwd(jvxSize deltaT) = 0;

	virtual jvxErrorType wind_fwd(jvxSize deltaT) = 0;


};

#endif
