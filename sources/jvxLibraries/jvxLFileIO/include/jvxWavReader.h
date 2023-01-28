#ifndef __JVXWAVREADER_H__
#define __JVXWAVREADER_H__

#include "jvx.h"
#include "jvx_dsp_base.h"
#include "typedefs/jvxTools/TjvxAudioFileIo.h"

#include "interfaces/IjvxAudioReader.h"
#include "jvxWavCommon.h"

class jvxWavReader : public IjvxAudioReader, public jvxWavCommon
{
	

public:
	jvxWavReader();
	virtual ~jvxWavReader();
	
	jvxErrorType select(const std::string& fName) override;

	jvxErrorType get_tag_text(jvxAudioFileTagType tp, std::string& theTag);
	
	jvxErrorType add_tag_text(jvxAudioFileTagType tp, std::string theTag) ;

	jvxErrorType get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numberBitsSample, jvxSize* subtype);

	jvxErrorType set_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numberBitsSample, jvxSize* subtype);

	jvxErrorType unselect() override;
	
	jvxErrorType prepare(jvxSize preferredSizeRead) override;
	
	jvxErrorType start() override;

	jvxErrorType stop() override;
	
	jvxErrorType postprocess()override;

	jvxErrorType status(jvxState* stat)override;

	// =====================================================================

	jvxErrorType activate(int& errCode)override;

	jvxErrorType deactivate()override;

	jvxErrorType set_loop(jvxBool)override;

	jvxErrorType get_loop(jvxBool* loop)override;

	jvxErrorType read_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize, jvxDataFormat format)override;

	jvxErrorType read_one_buf_raw(jvxByte* fld_out, jvxSize num_bytes_to_read, jvxSize* num_bytes_at_read);

	jvxErrorType rewind()override;

	jvxErrorType current_progress(jvxSize* progress)override;

	jvxErrorType wind_bwd(jvxSize deltaT)override;

	jvxErrorType wind_fwd(jvxSize deltaT)override;

private:
	void reset();

};
	
#endif
	