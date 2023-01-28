#ifndef __TJVXAUDIOFILE_H__
#define __TJVXAUDIOFILE_H__

//namespace IrtpAudioFile
//{
typedef enum
{
	JVX_FILETYPE_NONE = -1,

	//! Standard Windows WAV file
	JVX_FILETYPE_WAV = 0,

	//! Raw audio file
	JVX_FILETYPE_RAW = 1
} jvxEndpointClass;

typedef enum
{
	JVX_WAV_UNKNOWN,
	JVX_WAV_64_SONY,
	JVX_WAV_64,
	JVX_WAV_32,
	JVX_WAV_LIMIT
} jvxEndpointSubType_wav;

typedef enum
{
	JVX_FILEFORMAT_NONE = 0,
	JVX_FILEFORMAT_PCM_FIXED = 1,
	JVX_FILEFORMAT_IEEE_FLOAT = 2
} jvxFileFormat;

//! Type of behavior in case the file has been completely read
typedef enum
{
	JVX_FILECONTINUE_NONE = -1,
	//! Stop at the end
	JVX_FILECONTINUE_STOPEND = 0,

	//! Restart the file for infinite playback
	JVX_FILECONTINUE_LOOP = 1
} jvxFileContinueType;

//! Operation mode when using rtFileReader/Writer: Realtime or batch mode
typedef enum
{
	JVX_FILEOPERATION_NONE = -1,

	//! Used buffered input/output 
	JVX_FILEOPERATION_REALTIME = 0,

	//! Read samples directly from file in batch mode
	JVX_FILEOPERATION_BATCH = 1
} jvxFileOperationType;
//};

struct jvxFileDescriptionEndpoint_open
{
	jvxSize numberChannels;
	jvxInt32 samplerate;
	jvxBool littleEndian;
	//jvxDataFormat dFormatFile;
	jvxSize lengthFile;
	jvxFileFormat fFormatFile;
	jvxSize numberBitsSample;
	jvxSize subtype;

	jvxFileDescriptionEndpoint_open()
	{
		fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
		numberBitsSample = 16;
		samplerate = 48000;
		littleEndian = true;
		numberChannels = 2;
		lengthFile = JVX_SIZE_UNSELECTED;
		subtype = JVX_WAV_32;
	}

	static jvxFileDescriptionEndpoint_open createRead()
	{
		jvxFileDescriptionEndpoint_open fileDescrO;
		fileDescrO.fFormatFile = JVX_FILEFORMAT_NONE;
		fileDescrO.numberBitsSample = 0;
		fileDescrO.samplerate = 48000;
		fileDescrO.littleEndian = false;
		fileDescrO.numberChannels = 0;
		fileDescrO.lengthFile = JVX_SIZE_UNSELECTED;
		fileDescrO.subtype = JVX_WAV_UNKNOWN;
		return fileDescrO;
	}

	static jvxFileDescriptionEndpoint_open createWrite()
	{
		jvxFileDescriptionEndpoint_open fileDescrO;
		return fileDescrO;
	}
} ;

typedef struct
{
	jvxDataFormat dFormatAppl;
	jvxInt32 numSamplesFrameMax_moveBgrd;

} jvxFileDescriptionEndpoint_prepare;

#define JVX_NUMBER_AUDIO_FILE_TAGS 5

typedef enum
{
	JVX_AUDIO_FILE_TAG_BTFLD_ARTIST = 0x1,
	JVX_AUDIO_FILE_TAG_BTFLD_TITLE = 0x2,
	JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA = 0x4,
	JVX_AUDIO_FILE_TAG_BTFLD_GENRE = 0x8,
	JVX_AUDIO_FILE_TAG_BTFLD_NORM = 0x10
} jvxAudioFileTagType;

static const char* jvxAudioFileTagType_txt[JVX_NUMBER_AUDIO_FILE_TAGS] =
{
	"IART",
	"INAM",
	"ICRD",
	"IGNR",
	"INRM"
};

#endif