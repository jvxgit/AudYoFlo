#include "jvx-helpers.h"
#include "jvxWavWriter.h"

jvxWavWriter::jvxWavWriter(): jvxWavCommon()
{
}
	
jvxWavWriter::~jvxWavWriter()
{
}

jvxErrorType 
jvxWavWriter::select(const std::string& fName)
{
	return(_select(fName, true));
}
	
jvxErrorType 
jvxWavWriter::unselect()
{
	return(_unselect());
}
	
jvxErrorType 
jvxWavWriter::prepare(jvxSize preferredSizeWrite)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object == JVX_STATE_ACTIVE)
	{	
		params_file.block_align = JVX_SIZE_INT16((params_file.bits_per_sample >> 3) * params_file.channels);
		params_file.byte_rate= params_file.block_align * params_file.samplerate;
		params_processing.szOneSampleChannels_bytes = params_file.channels * (params_file.bits_per_sample >> 3);

		params_processing.numSamplesBuffer = preferredSizeWrite;
		params_processing.szOneSampleBufferChannels_bytes = params_processing.numSamplesBuffer * params_processing.szOneSampleChannels_bytes;

		runtime.position = 0;
        runtime.oneChunk = NULL;
        
		if (JVX_CHECK_SIZE_SELECTED(params_processing.numSamplesBuffer))
		{
			if (params_processing.szOneSampleBufferChannels_bytes)
			{
				JVX_SAFE_NEW_FLD(runtime.oneChunk, jvxByte, params_processing.szOneSampleBufferChannels_bytes);
				memset(runtime.oneChunk, 0, sizeof(jvxByte) * params_processing.szOneSampleBufferChannels_bytes);
			}
		}

		state_object = JVX_STATE_PREPARED;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
jvxWavWriter::postprocess()
{
	return(_postprocess());
}
	
jvxErrorType 
jvxWavWriter::status(jvxState* stat)
{
	return(_status(stat));
}

jvxErrorType 
jvxWavWriter::activate(int& errCode)
{
	return(_activate());
}

jvxErrorType 
jvxWavWriter::deactivate()
{
	return(_deactivate());
}

jvxErrorType
jvxWavWriter::get_file_properties(jvxSize*  channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numBitsSample, jvxSize* subtype)
{
	return(_get_file_properties(channels,length_samples, srate, fformat, littleEndian, numBitsSample, subtype));
}

#define FWRITE_WAV_STR(cptr, LSTR) fwrite((void*)cptr, sizeof(char), LSTR, params_file.fHandle); byteCnt += sizeof(char)*LSTR
#define FWRITE_WAV_64(i64ptr) fwrite(i64ptr, sizeof(jvxInt64), 1, params_file.fHandle); byteCnt += sizeof(jvxInt64)

#ifdef JVX_OS_ANDROID
	// Completely unclear why the following gives a compiler error
	// fwrite(ui64ptr, sizeof(jvxUInt32), 2, params_file.fHandle); byteCnt += sizeof(jvxUInt64)
	#define FWRITE_WAV_U64(ui64ptr) assert(0); 
#else
	#define FWRITE_WAV_U64(ui64ptr) fwrite(ui64ptr, sizeof(jvxUInt64), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt64)
#endif

#define FWRITE_WAV_32(i32ptr) fwrite(i32ptr, sizeof(jvxInt32), 1, params_file.fHandle); byteCnt += sizeof(jvxInt32)
#define FWRITE_WAV_U32(ui32ptr) fwrite(ui32ptr, sizeof(jvxUInt32), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt32)
#define FWRITE_WAV_16(i16ptr) fwrite(i16ptr, sizeof(jvxInt16), 1, params_file.fHandle); byteCnt += sizeof(jvxInt16)
#define FWRITE_WAV_U16(ui16ptr) fwrite(ui16ptr, sizeof(jvxUInt16), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt16)

jvxErrorType 
jvxWavWriter::init_write(int& errCode)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string token;
	jvxUInt32 valUI32;
	jvxUInt32 valUI64;
	jvxUInt16 valUI16;
	jvxBool parseError = false;
	jvxUInt64 byteCnt = 0;
	jvxUInt64 byteCnt_info = 0;
	std::string txt;
	int i;
	jvxSize read = 0;
	oneCrossLink fillInLater_1;
	jvxBool setInfoSize = false;

	if(state_object == JVX_STATE_PREPARED)
	{
		// File pointer to "begin"
		JVX_FSEEK(params_file.fHandle, 0, SEEK_SET);
		byteCnt = 0;
		
		switch (params_file.wav_mode)
		{ 
		case JVX_WAV_64:
			token = "RF64";
			valUI32 = 0xFFFFFFFF;
			break;
		case JVX_WAV_32:
			token = "RIFF";
			valUI32 = 0;
			break;
		default:
			assert(0);
		}

		FWRITE_WAV_STR(token.c_str(), 4);
		onClose.fLength.offset_write = byteCnt;

		// to be filled in later..
		FWRITE_WAV_U32(&valUI32);
		onClose.fLength.offset_count = byteCnt;
		onClose.fLength.value = 0;

		token = "WAVE";
		FWRITE_WAV_STR(token.c_str(), 4);

		if (params_file.wav_mode == JVX_WAV_64)
		{
			token = "ds64";
			FWRITE_WAV_STR(token.c_str(), 4);

			valUI32 = 28;
			FWRITE_WAV_U32(&valUI32);

			onClose.ds64Tag.offset_write = byteCnt;
			onClose.ds64Tag.value = 0;
			valUI64 = 0;

			// The three 64 bit lengths
			FWRITE_WAV_U64(&valUI64);
			FWRITE_WAV_U64(&valUI64);
			FWRITE_WAV_U64(&valUI64);
			valUI32 = 0;
			FWRITE_WAV_U32(&valUI32);

		}
		// Header complete here..
		// Write format field
		token = "fmt ";
		FWRITE_WAV_STR(token.c_str(), 4);

		if (params_file.wav_mode == JVX_WAV_64)
		{
			valUI32 = 40;
			FWRITE_WAV_U32(&valUI32);
			valUI16 = 0xFFFE;
		}
		else
		{
			valUI32 = 16;
			FWRITE_WAV_U32(&valUI32);

			switch (params_file.fformat)
			{
			case JVX_FILEFORMAT_PCM_FIXED:
				valUI16 = 1;
				break;
			case JVX_FILEFORMAT_IEEE_FLOAT:
				valUI16 = 3;
				break;
			default:
				assert(0);
			}
		}
		FWRITE_WAV_U16(&valUI16); // Format PCM

		valUI16 = JVX_SIZE_UINT16(params_file.channels);
		FWRITE_WAV_U16(&valUI16); // Number of channels

		valUI32 = params_file.samplerate;
		FWRITE_WAV_U32(&valUI32); // Samplerate

		valUI32 = params_file.byte_rate;
		FWRITE_WAV_U32(&valUI32); // Byterate

		valUI16 = params_file.block_align;
		FWRITE_WAV_U16(&valUI16); // Block align

		valUI16 = JVX_SIZE_UINT16(params_file.bits_per_sample);
		FWRITE_WAV_U16(&valUI16); // Block align
		
		if (params_file.wav_mode == JVX_WAV_64)
		{
			valUI32 = 0x00180016;
			FWRITE_WAV_U32(&valUI32); // Channel Mask

			valUI32 = 0x00000000;
			FWRITE_WAV_U32(&valUI32); 
			valUI32 = 0x00000001;
			FWRITE_WAV_U32(&valUI32); 
			valUI32 = 0x00100000;
			FWRITE_WAV_U32(&valUI32);
			valUI32 = 0xAA000080;
			FWRITE_WAV_U32(&valUI32); 
			valUI32 = 0x719B3800;
			FWRITE_WAV_U32(&valUI32); 
		}
		// Format section complete

		//if (!params_file.isRf64)
		//{
			// Tag section if desired
			if (params_file.all_tag.size() > 0)
			{
				token = "LIST";
				FWRITE_WAV_STR(token.c_str(), 4);

				fillInLater_1.offset_write = byteCnt;
				valUI32 = 0;
				FWRITE_WAV_U32(&valUI32);
				fillInLater_1.offset_count = byteCnt;
				byteCnt_info = byteCnt;

				token = "INFO";
				FWRITE_WAV_STR(token.c_str(), 4);

				for (i = 0; i < params_file.all_tag.size(); i++)
				{
					token = params_file.all_tag[i].tagName;
					FWRITE_WAV_STR(token.c_str(), JVX_SIZE_INT32(token.size()));

					token = params_file.all_tag[i].tagValue;

					// Could it be that these entries must be 16 bit aligned??
					if (token.size() % 2)
					{
						token += " ";
					}

					valUI32 = JVX_SIZE_UINT32(token.size());
					FWRITE_WAV_U32(&valUI32);
					FWRITE_WAV_STR(token.c_str(), JVX_SIZE_INT32(token.size()));
				}
				fillInLater_1.value = byteCnt - byteCnt_info;
				setInfoSize = true;
			}
		//}
		token = "data";
		FWRITE_WAV_STR(token.c_str(), 4);

		onClose.dLength.offset_write = byteCnt;

		if (params_file.wav_mode == JVX_WAV_64)
		{
			valUI32 = 0xFFFFFFFF;
		}
		else
		{
			valUI32 = 0;
		}
		FWRITE_WAV_U32(&valUI32);

		onClose.dLength.offset_count = byteCnt;
		onClose.dLength.value = 0;

		onClose.fLength.value  = JVX_SIZE_INT32(byteCnt - onClose.fLength.offset_count);

		params_file.offset_start_data = byteCnt;

		if(setInfoSize)
		{
			JVX_FSEEK(params_file.fHandle,  fillInLater_1.offset_write, SEEK_SET);
			valUI32 = fillInLater_1.value;
			FWRITE_WAV_U32(&valUI32);

			JVX_FSEEK(params_file.fHandle, 0, SEEK_END); // Should be the place where to start writing
		}

		state_object = JVX_STATE_PROCESSING;
	} // if(state_file_reader == JVX_STATE_SELECTED)
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
jvxWavWriter::terminate_write()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxUInt64 valUI64 = 0;
	jvxUInt32 valUI32 = 0;
	jvxSize byteCnt = 0;

	if(state_object == JVX_STATE_PROCESSING)
	{
        onClose.fLength.value += runtime.position * (jvxUInt64)params_processing.szOneSampleChannels_bytes;
        onClose.dLength.value += runtime.position * (jvxUInt64)params_processing.szOneSampleChannels_bytes;
        
		if (params_file.wav_mode == JVX_WAV_64)
		{
			JVX_FSEEK(params_file.fHandle, onClose.ds64Tag.offset_write, SEEK_SET);

			valUI64 = onClose.fLength.value;
			FWRITE_WAV_U64(&valUI64);

			valUI64 = onClose.dLength.value;
			FWRITE_WAV_U64(&valUI64);

			valUI64 = onClose.ds64Tag.value;
			FWRITE_WAV_U64(&valUI64);

		}
		else
		{
			// In Rf64 mode, the length is already set to 0xFFFE
			JVX_FSEEK(params_file.fHandle, onClose.fLength.offset_write, SEEK_SET);
			
			if (onClose.fLength.value > 0xFFFFFFFF)
			{
				std::cout << "Warning: max size of wav file has been exceded. The file <" << fName << "> may be unreadible." << std::endl;
			}
			valUI32 = onClose.fLength.value;
			FWRITE_WAV_U32(&valUI32);

			JVX_FSEEK(params_file.fHandle, onClose.dLength.offset_write, SEEK_SET);
			if (onClose.dLength.value > 0xFFFFFFFF)
			{
				std::cout << "Warning: max size of wav file has been exceded. The file <" << fName << "> may be unreadible." << std::endl;
			}
			valUI32 = onClose.dLength.value;
			FWRITE_WAV_U32(&valUI32);
		}

		state_object = JVX_STATE_PREPARED;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
jvxWavWriter::start()
{
	int errCode;
	jvxErrorType res = JVX_NO_ERROR;

	this->add_tag_text(JVX_AUDIO_FILE_TAG_BTFLD_TITLE, "JVX-RECORDED");
	this->add_tag_text(JVX_AUDIO_FILE_TAG_BTFLD_ARTIST, "JVX-RTPROC");
	this->add_tag_text(JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA, JVX_DATE() + "-" + JVX_TIME());

	res = this->init_write(errCode);
	if(res == JVX_NO_ERROR)
	{
		jvxUInt64 numBytesMax = ((jvxUInt64)1 << 63) - (onClose.fLength.value + 8);
		jvxUInt64 numFramesMax = numBytesMax / params_processing.szOneSampleChannels_bytes;
		if (params_file.wav_mode == JVX_WAV_32)
		{
			numBytesMax = ((jvxUInt64)1 << 32) - (onClose.fLength.value + 8);
			numFramesMax = numBytesMax / params_processing.szOneSampleChannels_bytes;
		}

		runtime.maxPositionP1 = numFramesMax; 

		// state_object = JVX_STATE_PROCESSING; // <- state set in init_write!
	}
	return(res);
}

jvxErrorType 
jvxWavWriter::write_one_buf(jvxHandle** fld_in, jvxSize num_chans_write, jvxSize buffersize, jvxDataFormat format)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize write = 0;
	jvxSize i;
	jvxSize inOutCnt = 0;

	if(state_object == JVX_STATE_PROCESSING)
	{
        if(params_processing.numSamplesBuffer == 0)
        {
            // Direct write without any conversion!!
            // Ignore input arg num_chans_write, ignore input arg format, just use direct pointer and buffersize
            jvxHandle* bufInterleaved = (jvxHandle*)fld_in;
            jvxSize numBytes = buffersize * (params_file.bits_per_sample >> 3) * params_file.channels;
            write = fwrite(bufInterleaved, sizeof(char), numBytes, params_file.fHandle);
			if (write != numBytes)
			{
				res = JVX_ERROR_INTERNAL;
			}
			else
			{
				runtime.position += (jvxUInt64)buffersize;
			}
        }
        else
        {
            jvxSize writeSize = JVX_MIN(params_processing.numSamplesBuffer, buffersize);
            jvxSize stilltowrite = buffersize;
            
            while(stilltowrite)
            {
                // Read in data
                writeSize = JVX_MIN(stilltowrite, (jvxSize)(runtime.maxPositionP1 - runtime.position));
                
                
                for(i = 0; i < num_chans_write; i++)
				{
					switch(format)
					{
					case JVX_DATAFORMAT_DATA:
						switch(params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
									(jvxData*)fld_in[i],
									(jvxInt16*)runtime.oneChunk,
									writeSize,
									JVX_MAX_INT_16_DATA,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 24:
								jvx_convertSamples_from_flp_norm_to_bytes<jvxData, jvxInt32>(
									(jvxData*)fld_in[i],
									(jvxByte*)runtime.oneChunk,
									writeSize,
									JVX_MAX_INT_24_DATA, 3,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 32:
								jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
									(jvxData*)fld_in[i],
									(jvxInt32*)runtime.oneChunk,
									writeSize,
									JVX_MAX_INT_32_DATA,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch(params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_data_to_float<float>(
									(jvxData*)fld_in[i],
									(float*)runtime.oneChunk,
									writeSize,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						default:
							assert(0);
						}
						break;
					case JVX_DATAFORMAT_8BIT:
						assert(0);
						/*
						switch(params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.dformat)
							{
							case JVX_DATAFORMAT_16BIT_LE:
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt16, jvxInt16>(
									(jvxInt8*)fld_in[i],
									(jvxInt16*)runtime.oneChunk,
									writeSize, 8,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt8, jvxInt32, jvxInt32>(
									(jvxInt8*)fld_in[i],
									(jvxInt32*)runtime.oneChunk,
									writeSize, 24,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch(params_file.dformat)
							{
							case JVX_DATAFORMAT_16BIT_LE:
								assert(0);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt8, float>(
									(jvxInt8*)fld_in[i],
									(float*)runtime.oneChunk,
									writeSize, 
									(float)JVX_MAX_INT_8_DIV,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						default: 
							assert(0);
						}*/
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						switch(params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
									(jvxInt16*)fld_in[i],
									(jvxInt16*)runtime.oneChunk, 
									writeSize, 
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 24:
								jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
									(jvxInt16*)fld_in[i],
									(jvxByte*)runtime.oneChunk,
									writeSize, 3, 8,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 32:
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
									(jvxInt16*)fld_in[i], 
									(jvxInt32*)runtime.oneChunk, 
									writeSize, 16,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch(params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, float>(
									(jvxInt16*)fld_in[i],
									(float*)runtime.oneChunk,
									writeSize, 
									(float)JVX_MAX_INT_16_DIV,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						default: 
							assert(0);
						}
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						switch(params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
									(jvxInt32*)fld_in[i], 
									(jvxInt16*)runtime.oneChunk, 
									writeSize, 16,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 24:
								jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
									(jvxInt32*)fld_in[i], 
									(jvxByte*)runtime.oneChunk, 
									writeSize, 3, 8, 
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case 32:
								jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
									(jvxInt32*)fld_in[i],
									(jvxInt32*)runtime.oneChunk,
									writeSize,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch(params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, float>(
									(jvxInt32*)fld_in[i],
									(float*)runtime.oneChunk,
									writeSize, 
									(float)JVX_MAX_INT_32_DIV,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						default:
							assert(0);
						}
						break;

					case JVX_DATAFORMAT_64BIT_LE:
						assert(0);
						/*
						switch(params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.dformat)
							{
							case JVX_DATAFORMAT_16BIT_LE:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
									(jvxInt64*)fld_in[i], 
									(jvxInt16*)runtime.oneChunk, 
									writeSize, 48,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
									(jvxInt64*)fld_in[i], 
									(jvxInt32*)runtime.oneChunk, 
									writeSize, 32,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch(params_file.dformat)
							{
							case JVX_DATAFORMAT_16BIT_LE:
								assert(0);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt64, float>(
									(jvxInt64*)fld_in[i],
									(float*)runtime.oneChunk,
									writeSize, 
									JVX_MAX_INT_64_DIV,
									inOutCnt, 1,
									i, params_file.channels);
								break;
							default:
								assert(0);
							}
							break;
						default:
							assert(0);
						}*/
						break;
					default:
						assert(0);
					}
                }
                write = fwrite(runtime.oneChunk, params_processing.szOneSampleChannels_bytes, writeSize, params_file.fHandle);
                
                runtime.position += (jvxUInt64)write;
                

				if (params_file.wav_mode == JVX_WAV_64)
				{
					onClose.ds64Tag.value += writeSize;
				}

				if (write != writeSize)
				{
					return JVX_ERROR_END_OF_FILE;
				}
                
                if(runtime.maxPositionP1 == runtime.position)
                {
                    // We may open a new file here but I do not think we need it
                    res = JVX_ERROR_END_OF_FILE;
                    stilltowrite = 0;
                    break;
                }
                else
                {
                    stilltowrite -= write;
                    inOutCnt += write;
                }
            }
        }
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavWriter::write_one_buf_raw(jvxByte* fld_in, jvxSize num_bytes_to_write, jvxSize* num_bytes_written)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize writtenAll = 0;

	if (state_object == JVX_STATE_PROCESSING)
	{
		jvxSize numBytesMaxWrite = params_processing.numSamplesBuffer * params_processing.szOneSampleChannels_bytes;
		jvxSize writeSizeMax = JVX_MIN(numBytesMaxWrite, num_bytes_to_write);
		jvxSize stillToGo = num_bytes_to_write;

		jvxByte* writeOffsetPtr = fld_in;
		while (stillToGo > 0)
		{
			jvxSize writenow = JVX_MIN(writeSizeMax, stillToGo);
			jvxSize written = fwrite(writeOffsetPtr, sizeof(jvxByte), writenow, params_file.fHandle);
			writeOffsetPtr += written;
			writtenAll += written;
			stillToGo -= written;
		}

		runtime.position += (jvxUInt64)num_bytes_to_write;
		if (num_bytes_written)
		{
			*num_bytes_written = writtenAll;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType 
jvxWavWriter::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = terminate_write();
	return(res);
}

jvxErrorType 
jvxWavWriter::get_tag_text(jvxAudioFileTagType tp, std::string& theTag)
{
	int i;
	std::string tagLookFor = "----";
	theTag = "--";
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if(state_object >= JVX_STATE_ACTIVE)
	{
		switch(tp)
		{
		case JVX_AUDIO_FILE_TAG_BTFLD_ARTIST:
			tagLookFor = "IART";
			break;
		case JVX_AUDIO_FILE_TAG_BTFLD_TITLE:
			tagLookFor = "INAM";
			break;
		case JVX_AUDIO_FILE_TAG_BTFLD_CREATION_DATA:
			tagLookFor = "ICRD";
			break;
		case JVX_AUDIO_FILE_TAG_BTFLD_GENRE:
			tagLookFor = "IGNR";
			break;
		}

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
}

jvxErrorType 
jvxWavWriter::add_tag_text(jvxAudioFileTagType tp, std::string theTag)
{
	int i;
	std::string tagLookFor = "----";
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if(state_object >= JVX_STATE_ACTIVE)
	{
		jvxSize sel = jvx_cbitfieldSelection2Id((jvxCBitField)tp);
		if (sel < JVX_NUMBER_AUDIO_FILE_TAGS)
		{
			tagLookFor = jvxAudioFileTagType_txt[sel];
		}

		for(i = 0; i < params_file.all_tag.size(); i++)
		{
			if(params_file.all_tag[i].tagName == tagLookFor)
			{
				theTag = params_file.all_tag[i].tagValue;
				res = JVX_NO_ERROR;
				break;
			}
		}

		if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			oneTagEntry newEntry;
			newEntry.tagName = tagLookFor;
			newEntry.tagValue = theTag;
			params_file.all_tag.push_back(newEntry);
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}


jvxErrorType 
jvxWavWriter::set_file_properties(jvxSize* channels, jvxInt32* srate,  jvxFileFormat* fformat, jvxSize* numberBitsSample, jvxSize* subtype)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object >= JVX_STATE_ACTIVE)
	{
		if(channels)
		{
			params_file.channels = *channels;
		}
		if(srate)
		{
			params_file.samplerate = *srate;
		}
		
		if(fformat)
		{
			params_file.fformat = *fformat;
		}
		if (numberBitsSample)
		{
			params_file.bits_per_sample = (jvxInt16)*numberBitsSample;
		}
		if (subtype)
		{
			if (
				(*subtype > JVX_WAV_UNKNOWN) &&
				(*subtype < JVX_WAV_LIMIT))
			{
				params_file.wav_mode = (jvxEndpointSubType_wav)*subtype;
			}
			else
			{
				assert(0);
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

