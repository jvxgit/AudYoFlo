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
	
	virtual jvxErrorType select(const std::string& fName) override;

	virtual jvxErrorType get_tag_text(jvxAudioFileTagType tp, std::string& theTag)override;
	
	virtual jvxErrorType get_file_properties(jvxSize* channels, jvxSize* length_samples,
					 jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian,
					 jvxSize* numberBitsSample, jvxSize* subtype) override;

	virtual jvxErrorType unselect() override;
	
	virtual jvxErrorType prepare(jvxSize preferredSizeRead) override;
	
	virtual jvxErrorType start() override;

	virtual jvxErrorType stop() override;
	
	virtual jvxErrorType postprocess()override;

	virtual jvxErrorType status(jvxState* stat)override;

	// =====================================================================

	virtual jvxErrorType activate(int& errCode)override;

	virtual jvxErrorType deactivate()override;

	virtual jvxErrorType set_loop(jvxBool)override;

	virtual jvxErrorType get_loop(jvxBool* loop)override;

	virtual jvxErrorType read_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize, jvxDataFormat format)override;

	virtual jvxErrorType read_one_buf_raw(jvxByte* fld_out, jvxSize num_bytes_to_read, jvxSize* num_bytes_at_read) override;

	virtual jvxErrorType rewind()override;

	virtual jvxErrorType current_progress(jvxSize* progress)override;

	virtual jvxErrorType wind_bwd(jvxSize deltaT)override;

	virtual jvxErrorType wind_fwd(jvxSize deltaT)override;

private:
	void reset();

};
	
#endif
	
