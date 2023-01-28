#ifndef __JVXAUDIO_H__
#define __JVXAUDIO_H__

JVX_INTERFACE IjvxAudio
{
public:

	IjvxAudio(){};

	virtual jvxErrorType select(const std::string& fName) = 0;

	virtual jvxErrorType activate(int& errCode) = 0;

	virtual jvxErrorType deactivate() = 0;

	virtual jvxErrorType get_tag_text(jvxAudioFileTagType tp, std::string& theTag) = 0;
	
	virtual jvxErrorType get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numberBitsSample, jvxSize* endpointSubType) = 0;

	virtual jvxErrorType unselect() = 0;
	
	virtual jvxErrorType prepare(jvxSize preferredSizeRead) = 0;
	
	virtual jvxErrorType start() = 0;

	virtual jvxErrorType postprocess() = 0;

	virtual jvxErrorType stop() = 0;

	virtual jvxErrorType status(jvxState* stat) = 0;

};

#endif