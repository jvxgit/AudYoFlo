#ifndef __JVXWAVCOMMON_H__
#define __JVXWAVCOMMON_H__

#pragma pack(push, 1)
struct wavGUID
{
	jvxInt32 Data1;
	jvxInt16  Data2;
	jvxInt16  Data3;
	jvxByte  Data4[8];
};
#pragma pack(pop)

#define JVX_COMPARE_GUID(one, two) \
	((one.Data1 == two.Data1) && (one.Data2 == two.Data2) && (one.Data3 == two.Data3) && (one.Data4[0] == two.Data4[0]) && (one.Data4[1] == two.Data4[1]) && (one.Data4[2] == two.Data4[2]) && (one.Data4[3] == two.Data4[3])&& (one.Data4[4] == two.Data4[4])&& (one.Data4[5] == two.Data4[5])&& (one.Data4[6] == two.Data4[6])&& (one.Data4[7] == two.Data4[7])) ? true: false

// RIFF
static const wavGUID riffGUID =
{
	(jvxInt32)0x66666972, (jvxInt16)0x912E, (jvxInt16)0x11CF,{ (jvxByte)0xA5, (jvxByte)0xD6, (jvxByte)0x28, (jvxByte)0xDB, (jvxByte)0x04, (jvxByte)0xC1, (jvxByte)0x00, (jvxByte)0x00 }
};

// WAVE
static const wavGUID waveGUID =
{
	(jvxInt32)0x65766177, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00,(jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// LIST
static const wavGUID listGUID =
{
	(jvxInt32)0x7473696C, (jvxInt16)0x912F,(jvxInt16)0x11CF,{ (jvxByte)0xA5, (jvxByte)0xD6, (jvxByte)0x28, (jvxByte)0xDB, (jvxByte)0x04, (jvxByte)0xC1, (jvxByte)0x00, (jvxByte)0x00 }
};

// FMT 
static const wavGUID fmtGUID =
{
	(jvxInt32)0x20746D66, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// FACT
static const wavGUID factGUID =
{
	(jvxInt32)0x74636166, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// DATA
static const wavGUID dataGUID =
{
	(jvxInt32)0x61746164, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// LEVL
static const wavGUID levlGUID =
{
	(jvxInt32)0x6C76656C, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// JUNK
static const wavGUID junkGUID =
{
	(jvxInt32)0x6b6E756A, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4f, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// BEXT
static const wavGUID bextGUID =
{
	(jvxInt32)0x74786562, (jvxInt16)0xACF3, (jvxInt16)0x11D3,{ (jvxByte)0x8C, (jvxByte)0xD1, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// MARKER
static const wavGUID markerGUID =
{
	(jvxInt32)0xABF76256, (jvxInt16)0x392D, (jvxInt16)0x11D2,{ (jvxByte)0x86, (jvxByte)0xC7, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// SUMMARYLIST
static const wavGUID summarylistlGUID =
{
	(jvxInt32)0x925F94BC, (jvxInt16)0x525A, (jvxInt16)0x11D2,{ (jvxByte)0x86, (jvxByte)0xDC, (jvxByte)0x00, (jvxByte)0xC0, (jvxByte)0x4F, (jvxByte)0x8E, (jvxByte)0xDB, (jvxByte)0x8A }
};

// ==================================================================================

class jvxWavCommon
{

protected:

	typedef struct
	{
		std::string tagName;
		std::string tagValue;
	} oneTagEntry;

	std::string fName;
	jvxState state_object;
	
	struct
	{
		FILE* fHandle = nullptr;
		jvxUInt64 fLength = 0;
		jvxUInt64 offset_start_data = 0;
		jvxUInt64 offset_stop_data_p1 = 0;
		jvxUInt64 length_data_samples = 0;
		jvxUInt64 data_size_bytes = 0;
		jvxSize channels = 0;
		jvxInt32 samplerate = 0;
		jvxInt32 byte_rate = 0;
		jvxInt16 block_align = 0;
		jvxInt16 bits_per_sample = 0;
		std::vector<oneTagEntry> all_tag;
		std::string bext;
		std::string id3;
		jvxFileFormat fformat = JVX_FILEFORMAT_NONE;
		jvxUInt32 channel_mask = 0;
		jvxUInt32 guid[5];
		//jvxBool isRf64;
		jvxEndpointSubType_wav wav_mode;

	} params_file;
	
	struct
	{
		jvxSize szOneSampleChannels_bytes = 0;
		jvxSize numSamplesBuffer = 0;
		jvxSize szOneSampleBufferChannels_bytes = 0;
		jvxBool loop = false;
	} params_processing;

	struct
	{
		jvxByte* oneChunk = nullptr;
		jvxUInt64 position = 0;
		jvxUInt64 maxPositionP1 = 0;
	} runtime;

public:

	jvxWavCommon()
    {
        runtime.oneChunk = NULL;
        state_object = JVX_STATE_INIT;
    };

	jvxErrorType _select(std::string fNameI, jvxBool doOpenForWrite)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object == JVX_STATE_INIT)
		{
			fName = fNameI;
			if(doOpenForWrite)
			{
				JVX_FOPEN(params_file.fHandle, fName.c_str(), "wb");
			}
			else
			{
				JVX_FOPEN(params_file.fHandle, fName.c_str(), "rb");
			}
			if(params_file.fHandle)
			{
				state_object = JVX_STATE_SELECTED;
			}
			else
			{
				res = JVX_ERROR_OPEN_FILE_FAILED;
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}

		return(res);
	};

	jvxErrorType _activate()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object == JVX_STATE_SELECTED)
		{
			state_object = JVX_STATE_ACTIVE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	jvxErrorType _deactivate()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object == JVX_STATE_ACTIVE)
		{
			state_object = JVX_STATE_SELECTED;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	jvxErrorType _unselect()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object == JVX_STATE_SELECTED)
		{
			JVX_FCLOSE(params_file.fHandle);
			this->reset();
			state_object = JVX_STATE_INIT;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	jvxErrorType _get_tag_text(jvxAudioFileTagType tp, std::string& theTag)
	{
		int i;
		std::string tagLookFor = "----";
		theTag = "--";
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		if(state_object >= JVX_STATE_ACTIVE)
		{
			tagLookFor = jvxAudioFileTagType_txt[jvx_bitfieldSelection2Id(tp, JVX_NUMBER_AUDIO_FILE_TAGS)];

			for(i = 0; i < params_file.all_tag.size(); i++)
			{
				if(params_file.all_tag[i].tagName == tagLookFor)
				{
					theTag = params_file.all_tag[i].tagValue;
					res = JVX_NO_ERROR;
					break;
				}
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	jvxErrorType _get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numBitsSample, jvxSize* extended)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object >= JVX_STATE_ACTIVE)
		{
			if(channels)
			{
				*channels = params_file.channels;
			}
			if(length_samples)
			{
				*length_samples = params_file.length_data_samples;
			}
			if(srate)
			{
				*srate = params_file.samplerate;
			}
			
			if(fformat)
			{
				*fformat = params_file.fformat;
			}
			if (numBitsSample)
			{
				*numBitsSample = params_file.bits_per_sample;
			}
			if (extended)
			{
				*extended = (jvxSize)params_file.wav_mode;
			}
			if (littleEndian)
			{
				*littleEndian = true; 
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	jvxErrorType _status(jvxState* stat)
	{
		if(stat)
		{
			*stat = state_object;
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType _postprocess()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if(state_object == JVX_STATE_PREPARED)
		{
            if(runtime.oneChunk)
            {
                JVX_SAFE_DELETE_FLD(runtime.oneChunk, jvxByte*);
            }
			state_object = JVX_STATE_ACTIVE;
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	void reset()
	{
		fName = "unknown";
		state_object = JVX_STATE_NONE;
		params_file.fHandle = NULL;
		params_file.offset_start_data = 0;
		params_file.length_data_samples = 0;
		params_file.channels = 0;
		params_file.samplerate = 0;
		params_file.all_tag.clear();
		params_file.fLength = 0;
		params_file.data_size_bytes = 0;
		params_file.byte_rate = 0;
		params_file.block_align = 0;
		params_file.bits_per_sample = 0;
		params_file.fformat = JVX_FILEFORMAT_NONE;
		params_file.bits_per_sample = 0;
		params_file.wav_mode = JVX_WAV_UNKNOWN;

		params_file.all_tag.clear();

		params_processing.numSamplesBuffer = 0;
		params_processing.szOneSampleBufferChannels_bytes = 0;
		params_processing.szOneSampleChannels_bytes = 0;
		params_processing.loop = false;


		runtime.oneChunk = NULL;
		runtime.position = 0;
		runtime.maxPositionP1 = 0;

	};

};

#endif

