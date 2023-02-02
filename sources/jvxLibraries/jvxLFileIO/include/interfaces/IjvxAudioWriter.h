#ifndef __JVXAUDIOWRITER_H__
#define __JVXAUDIOWRITER_H__

#include "interfaces/IjvxAudio.h"

JVX_INTERFACE IjvxAudioWriter: public IjvxAudio
{
public:

	IjvxAudioWriter(){};
	virtual ~IjvxAudioWriter() {};

	virtual jvxErrorType add_tag_text(jvxAudioFileTagType tp, std::string theTag) = 0;

	virtual jvxErrorType set_file_properties(jvxSize* channels, jvxInt32* srate, jvxFileFormat* fformat,
						 jvxSize* numberBytesSample, jvxSize* subtype) = 0;

	virtual jvxErrorType write_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize,
					   jvxDataFormat format) = 0;

};

#endif
