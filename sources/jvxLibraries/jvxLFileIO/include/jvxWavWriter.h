#ifndef __JVXWAVWRITER_H__
#define __JVXWAVWRITER_H__

#include "jvx.h"
#include "jvx_dsp_base.h"
#include "typedefs/jvxTools/TjvxAudioFileIo.h"

#include "interfaces/IjvxAudioWriter.h"
#include "jvxWavCommon.h"

class jvxWavWriter : public IjvxAudioWriter, public jvxWavCommon
{
	typedef struct
	{
		jvxUInt64 offset_write;
		jvxUInt64 offset_count;
		jvxUInt64 value;
	} oneCrossLink;

	typedef struct
	{
		jvxUInt64 offset_write;
		jvxUInt64 offset_count;
		jvxUInt64 value;
	} oneCrossLink64;

		struct
	{
		oneCrossLink fLength;
		oneCrossLink dLength;
		oneCrossLink64 ds64Tag;
	} onClose;

public:
	jvxWavWriter();
	virtual ~jvxWavWriter();

	jvxErrorType select(const std::string& fName);

	jvxErrorType activate(int& errCode);

	jvxErrorType deactivate();

	jvxErrorType add_tag_text(jvxAudioFileTagType tp, std::string theTag);

	jvxErrorType get_tag_text(jvxAudioFileTagType tp, std::string& theTag);

	jvxErrorType set_file_properties(jvxSize* channels, jvxInt32* srate, jvxFileFormat* fformatn, jvxSize* numberBitsSample, jvxSize* subtype);

	jvxErrorType get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformatn, jvxBool* littleEndian, jvxSize* numberBitsSample, jvxSize* subtype);

	jvxErrorType select();

	jvxErrorType unselect();

	jvxErrorType terminate();

	jvxErrorType prepare(jvxSize preferredSizeRead);

	jvxErrorType start();

	jvxErrorType write_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize, jvxDataFormat format);

	jvxErrorType write_one_buf_raw(jvxByte* fld_out, jvxSize num_bytes_to_write, jvxSize* num_bytes_written);

	jvxErrorType stop();

	jvxErrorType postprocess();

	jvxErrorType status(jvxState* stat);

private:

	jvxErrorType init_write(int& errCode);

	jvxErrorType terminate_write();

};

#endif
