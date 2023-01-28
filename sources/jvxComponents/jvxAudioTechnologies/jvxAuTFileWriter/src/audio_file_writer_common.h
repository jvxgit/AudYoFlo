#ifndef JVX_AUDIO_FILE_WRITER_COMMON_H__
#define JVX_AUDIO_FILE_WRITER_COMMON_H__

struct fileprops_wav_base
{
	jvxFileFormat fformat = JVX_FILEFORMAT_NONE;
	jvxBool lendian = true;
	jvxSize bitssample = 16;
	jvxSize subtype = 0;
};

#endif
