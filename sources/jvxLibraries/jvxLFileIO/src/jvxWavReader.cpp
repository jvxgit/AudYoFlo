#include "jvx-helpers.h"
#include "jvxWavReader.h"

jvxWavReader::jvxWavReader(): jvxWavCommon()
{
}

jvxWavReader::~jvxWavReader()
{
}

jvxErrorType
jvxWavReader::select(const std::string& fName)
{
	return(_select(fName, false));
}

jvxErrorType
jvxWavReader::unselect()
{
	return(_unselect());
}

jvxErrorType
jvxWavReader::prepare(jvxSize preferredSizeRead)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object == JVX_STATE_ACTIVE)
	{

		params_processing.szOneSampleChannels_bytes = params_file.block_align;
		params_processing.numSamplesBuffer = preferredSizeRead;
		params_processing.szOneSampleBufferChannels_bytes = params_processing.numSamplesBuffer * params_processing.szOneSampleChannels_bytes;

		runtime.position = 0;
		runtime.maxPositionP1 = runtime.position + params_file.length_data_samples;

		jvxSize checkme = (params_file.offset_stop_data_p1 - params_file.offset_start_data);
		checkme = checkme / params_file.block_align;
		assert(checkme == runtime.maxPositionP1);

		if (JVX_CHECK_SIZE_SELECTED(params_processing.numSamplesBuffer))
		{
			JVX_SAFE_NEW_FLD(runtime.oneChunk, jvxByte, params_processing.szOneSampleBufferChannels_bytes);
			memset(runtime.oneChunk, 0, sizeof(jvxByte) * params_processing.szOneSampleBufferChannels_bytes);
		}

		JVX_FSEEK(params_file.fHandle, params_file.offset_start_data, SEEK_SET);
		state_object = JVX_STATE_PREPARED;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavReader::postprocess()
{
	return(_postprocess());
}

jvxErrorType
jvxWavReader::status(jvxState* stat)
{
	return(_status(stat));
}

jvxErrorType
jvxWavReader::get_tag_text(jvxAudioFileTagType tp, std::string& theTag)
{
	return(_get_tag_text(tp, theTag));
}

jvxErrorType
jvxWavReader::get_file_properties(jvxSize* channels, jvxSize* length_samples, jvxInt32* srate, jvxFileFormat* fformat, jvxBool* littleEndian, jvxSize* numBitsSample, jvxSize* subtype)
{
	return(_get_file_properties(channels, length_samples, srate,  fformat, littleEndian, numBitsSample, subtype));
}

// =======================================================================
// Specific functionality
// =======================================================================

#define FREAD_WAV_C(cptr) fread(cptr, sizeof(char), 1, params_file.fHandle); byteCnt += sizeof(char)
#define FREAD_WAV_STR(cptr) fread(cptr, sizeof(char), 4, params_file.fHandle); byteCnt += sizeof(char)*4
#define FREAD_WAV_32(i32ptr) fread(i32ptr, sizeof(jvxInt32), 1, params_file.fHandle); byteCnt += sizeof(jvxInt32)
#define FREAD_WAV_U32(ui32ptr) fread(ui32ptr, sizeof(jvxUInt32), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt32)
#define FREAD_WAV_64(i64ptr) fread(i64ptr, sizeof(jvxInt64), 1, params_file.fHandle); byteCnt += sizeof(jvxInt64)
#define FREAD_WAV_U64(ui64ptr) fread(ui64ptr, sizeof(jvxUInt64), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt64)
#define FREAD_WAV_16(i16ptr) fread(i16ptr, sizeof(jvxInt16), 1, params_file.fHandle); byteCnt += sizeof(jvxInt16)
#define FREAD_WAV_U16(ui16ptr) fread(ui16ptr, sizeof(jvxUInt16), 1, params_file.fHandle); byteCnt += sizeof(jvxUInt16)
#define FREAD_WAV_RN(cptr, N) fread(cptr, sizeof(char), N, params_file.fHandle); byteCnt += sizeof(char) * N
#define FREAD_WAV_GUID(cptr) fread(cptr, sizeof(wavGUID), 1, params_file.fHandle); byteCnt += sizeof(wavGUID)


// ==================================================================================================

jvxErrorType
jvxWavReader::activate(int& errCode)
{
	jvxErrorType res = JVX_NO_ERROR;
	char str_c[5];
	char oneFrag[2];
	jvxUInt32 valUI32;
	jvxUInt64 valUI64;
	jvxUInt16 valUI16;
	jvxBool parseError = false;
	jvxInt64 byteCnt = 0;
	jvxInt64 byteCnt_info = 0;
	std::string txt;
	int i;
	jvxSize read = 0;
	jvxUInt32 tmpI32;
	wavGUID myGUID;
	jvxBool noError = true;

	if(state_object == JVX_STATE_SELECTED)
	{
		memset(str_c, 0, sizeof(char)*5);
		memset(oneFrag, 0, sizeof(char)*2);

		params_file.data_size_bytes = JVX_SIZE_UNSELECTED;
		params_file.fLength = JVX_SIZE_UNSELECTED;
		params_file.length_data_samples = JVX_SIZE_UNSELECTED;
		params_file.bext = "";
		params_file.id3 = "";
		params_file.all_tag.clear();


		JVX_FSEEK(params_file.fHandle, 0L, SEEK_END);
		jvxSize sz = JVX_FTELL(params_file.fHandle);

		// File pointer to "begin"
		JVX_FSEEK(params_file.fHandle, 0, SEEK_SET);

		read = FREAD_WAV_STR(str_c);
		if (read == 4)
		{
			if (
				((std::string)str_c == "riff")
				)
			{

				memcpy((void*)&myGUID, str_c, 4);
				read = FREAD_WAV_RN(((jvxByte*)&myGUID + 4), sizeof(wavGUID) - 4);
				if (JVX_COMPARE_GUID(myGUID, riffGUID))
				{
					params_file.wav_mode = JVX_WAV_64_SONY;
				}
				else
				{
					noError = false;
				}
			}
			else if (
				((std::string)str_c == "RF64") ||
				((std::string)str_c == "RIFF"))
			{
				if ((std::string)str_c == "RF64")
				{
					params_file.wav_mode = JVX_WAV_64;
				}
				else
				{
					params_file.wav_mode = JVX_WAV_32;
				}
			}
			else
			{
				noError = false;
			}
		}
		if (noError)
		{
			if (
				(params_file.wav_mode == JVX_WAV_64) ||
				(params_file.wav_mode == JVX_WAV_32))
			{
				read = FREAD_WAV_U32(&valUI32);
				if (read == 1)
				{
					if (valUI32 == 0xFFFFFFFF)
					{
						params_file.fLength = JVX_SIZE_UNSELECTED;
					}
					else
					{
						params_file.fLength = valUI32 + 8; // First 8 bytes are neglected by that field..
					}
				}
				else
				{
					noError = false;
				}
			}
			else
			{
				read = FREAD_WAV_U64(&valUI64);
				if (read == 1)
				{
					params_file.fLength = valUI64; // First 8 bytes are neglected by that field..
				}
				else
				{
					noError = false;
				}
			}
		}

		if (noError)
		{
			if (
				(params_file.wav_mode == JVX_WAV_64) ||
				(params_file.wav_mode == JVX_WAV_32))
			{
				read = FREAD_WAV_STR(str_c);
				if (read == 4)
				{
					if ((std::string)str_c != "WAVE")
					{
						noError = false;
					}
				}
				else
				{
					noError = false;
				}
			}
			else
			{
				read = FREAD_WAV_GUID(&myGUID);
				if (read == 1)
				{
					if (!JVX_COMPARE_GUID(myGUID, waveGUID))
					{
						noError = false;
					}
				}
				else
				{
					noError = false;
				}
			}
		}

		if (noError)
		{
			while (1)
			{
				// On chunks: http://netghost.narod.ru/gff/vendspec/micriff/ms_riff.txt

				jvxBool found_ds64 = false;
				jvxBool found_fmt = false;
				jvxBool found_bext = false;
				jvxBool found_list = false;
				jvxBool found_data = false;
				jvxBool found_marker = false;
				jvxBool found_pad = false;
				jvxBool found_junk = false;
				jvxBool found_levl = false;
				jvxBool found_summarylist = false;
				jvxBool found_id3 = false;
				jvxBool found_xmp = false;
				jvxBool found_disp = false;
				jvxBool found_fact = false;
				jvxBool found_peak = false;
				std::string unknown_type;

				if (
					(params_file.wav_mode == JVX_WAV_64) ||
					(params_file.wav_mode == JVX_WAV_32))
				{
					read = FREAD_WAV_STR(str_c);
					if (read == 4)
					{
						if ((std::string)str_c == "ds64")
						{
							found_ds64 = true;
						}
						else if ((std::string)str_c == "fmt ")
						{
							found_fmt = true;
						}
						else if ((std::string)str_c == "bext")
						{
							found_bext = true;
						}
						else if ((std::string)str_c == "data")
						{
							found_data = true;
						}
						else if ((std::string)str_c == "LIST")
						{
							found_list = true;
						}
						else if ((std::string)str_c == "PAD ")
						{
							found_pad = true;
						}
						else if (
							((std::string)str_c == "JUNK") || 
							((std::string)str_c == "junk")
							)
						{
							found_junk = true;
						}
						else if ((std::string)str_c == "id3 ")
						{
							found_id3 = true;
						}
						else if ((std::string)str_c == "_PMX")
						{
							found_xmp = true;
						}
						else if ((std::string)str_c == "DISP")
						{
							found_disp = true;
						}
						else if ((std::string)str_c == "fact")
						{
							found_fact = true;
						}
						else if ((std::string)str_c == "PEAK")
						{
							found_peak = true;
						}
						else
						{
							unknown_type = str_c;
							// std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Unknown chunk type <" << str_c << "> found." << std::endl;
						}
					}
					else
					{
						// No more data chunk read, this means that file ended ok
						if (read != 0)
						{
							std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": File did not end on byte content." << std::endl;
						}
						break;
					}
				}
				else
				{
					read = FREAD_WAV_GUID(&myGUID);
					if (read == 1)
					{
						if (JVX_COMPARE_GUID(myGUID, fmtGUID))
						{
							found_fmt = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, listGUID))
						{
							found_list = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, fmtGUID))
						{
							found_fmt = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, factGUID))
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Found unsupported chunk type GUID <fact>." << std::endl;
							noError = false;
							break;
						}
						else if (JVX_COMPARE_GUID(myGUID, dataGUID))
						{
							found_data = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, levlGUID))
						{
							found_levl = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, junkGUID))
						{
							found_junk = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, bextGUID))
						{
							found_bext = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, markerGUID))
						{
							found_marker = true;
						}
						else if (JVX_COMPARE_GUID(myGUID, summarylistlGUID))
						{
							found_summarylist = true;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Found unexpected chunk type GUID." << std::endl;
							noError = false;
							break;
						}
					}
					else
					{
						// No more data chunk read, this means that file ended ok
						break;
					}
				}
					
				// Read chunk size
				jvxUInt64 sofarRead = 0;
				jvxUInt64 shouldRead = 0;
				jvxBool isWaveEx = false;

				// ===============================================================
				// FORMAT SECTION
				// ===============================================================
				if (
					(params_file.wav_mode == JVX_WAV_64) ||
					(params_file.wav_mode == JVX_WAV_32))
				{
					read = FREAD_WAV_U32(&valUI32);
					if (read == 1)
					{
						
						if (valUI32 == 0xFFFFFFFF)
						{
							shouldRead = (jvxUInt64)-1;
						}
						else
						{
							shouldRead = (jvxUInt64)(valUI32);


							// Be careful with following
							if (shouldRead % 2)
								shouldRead++;
						}
						/*
						// Not required due to previous typecasting
						if (valI32 == -1)
						{
							// Extend 32 bit -1 to 64 bit -1
							shouldRead = -1;
						}*/
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - wave format." << std::endl;
						noError = false;
						break;
					}
				}
				else
				{
					read = FREAD_WAV_U64(&valUI64);
					if (read == 1)
					{
						shouldRead = valUI64 - sizeof(wavGUID) - sizeof(jvxUInt64);

						// Align to 8 byte bounds
						shouldRead = ((shouldRead + 7) >> 3) << 3;
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - wave format." << std::endl;
						noError = false;
						break;
					}
				}

				if (found_ds64)//(std::string)str_c == "ds64")
				{
					read = FREAD_WAV_U64(&valUI64);
					if (read == 1)
					{
						params_file.fLength = valUI64 + 8;
						sofarRead += sizeof(valUI64);
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read ds64 - entry0." << std::endl;
						parseError = true;
						break;
					}

					read = FREAD_WAV_U64(&valUI64);
					if (read == 1)
					{
						params_file.data_size_bytes = valUI64;
						sofarRead += sizeof(valUI64);
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read ds64 - entry1." << std::endl;
						parseError = true;
						break;
					}

					read = FREAD_WAV_U64(&valUI64);
					if (read == 1)
					{
						params_file.length_data_samples = valUI64;
						sofarRead += sizeof(valUI64);
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read ds64 - entry2." << std::endl;
						parseError = true;
						break;
					}

					/*
					 * Currently ignore what comes after
					read = FREAD_WAV_32(&valI32);
					if (read == 1)
					{
						tLength = valI32;
					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << ": Failed to read ds64 - table size." << std::endl;
						parseError = true;
						break;
					}
					for (i = 0; i < tLength; i++)
					{
						read = FREAD_WAV_32(&valI32);
						if (read != 1)
						{
							std::cout << "Error Position " << __COUNTER__ << ": Failed to read ds64 - table entry #" << i << "." << std::endl;
							parseError = true;
							break;
						}
					}*/
				}
				else if (found_fmt)//(std::string)str_c == "fmt ")
				{
					if (shouldRead >= 16) // The format section should be 16 bytes
					{

						read = FREAD_WAV_U16(&valUI16);
						if (read == 1)
						{
							sofarRead += 2;
							if (valUI16 == 0xFFFE)
							{
								// extensible format. I think that this format is always PCM..
								params_file.fformat = JVX_FILEFORMAT_PCM_FIXED;
								isWaveEx = true;
							}
							else
							{
								params_file.fformat = JVX_FILEFORMAT_NONE;
								if (valUI16 == 1)
								{
									params_file.fformat = JVX_FILEFORMAT_PCM_FIXED;
								}
								if (valUI16 == 3)
								{
									params_file.fformat = JVX_FILEFORMAT_IEEE_FLOAT;
								}
							}
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - wave format." << std::endl;
							parseError = true;
							break;
						}
						read = FREAD_WAV_U16(&valUI16);
						if (read == 1)
						{
							sofarRead += 2;

							// Number channels
							params_file.channels = valUI16;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - number channels." << std::endl;
							parseError = true;
							break;
						}
						read = FREAD_WAV_U32(&valUI32);
						if (read == 1)
						{
							sofarRead += 4;
							params_file.samplerate = valUI32;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - samplerate." << std::endl;
							parseError = true;
							break;
						}

						read = FREAD_WAV_U32(&valUI32);
						if (read == 1)
						{
							sofarRead += 4;
							params_file.byte_rate = valUI32;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - byte rate." << std::endl;
							parseError = true;
							break;
						}

						read = FREAD_WAV_U16(&valUI16);
						if (read == 1)
						{
							sofarRead += 2;
							params_file.block_align = valUI16;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - block align." << std::endl;
							parseError = true;
							break;
						}

						read = FREAD_WAV_U16(&valUI16);
						if (read == 1)
						{
							sofarRead += 2;
							params_file.bits_per_sample = valUI16;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - bits per sample." << std::endl;
							parseError = true;
							break;
						}

						if (isWaveEx)
						{
							// Wave format extensible
							read = FREAD_WAV_U32(&valUI32);
							if (read == 1)
							{
								sofarRead += 4;
								params_file.channel_mask = valUI32;
							}
							else
							{
								std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - channel mask." << std::endl;
								parseError = true;
								break;
							}

							/* These words are not important and will be skipped in the general forward skip to follow
							for (i = 0; i < 5; i++)
							{
								read = FREAD_WAV_32(&valI32);
								if (read == 1)
								{
									sofarRead += 4;
								}
								else
								{
									std::cout << "Error Position " << __COUNTER__ << ": Failed to read fmt - channel mask." << std::endl;
									parseError = true;
									break;
								}
							}*/
							if (parseError)
							{
								break;
							}
						}

					}
					else
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read fmt - channel mask." << std::endl;
						parseError = true;
						break;
					}
					// ===============================================================
					// END FORMAT SECTION
					// ===============================================================
				}
				else if (found_bext)//(std::string)str_c == "bext")
				{
					params_file.bext = "";
					for (i = 0; i < shouldRead; i++)
					{
						memset(str_c, 0, 2 * sizeof(char));
						read = FREAD_WAV_C(str_c);
						if (read == 1)
						{
							params_file.bext += str_c;
							sofarRead += 1;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Unexpected end of file in bext chunk." << std::endl;
							parseError = true;
							break;
						}
					}
					if (parseError)
					{
						break;
					}
				}
				else if (found_id3)
				{
					params_file.id3 = "";
					for (i = 0; i < shouldRead; i++)
					{
						memset(str_c, 0, 2 * sizeof(char));
						read = FREAD_WAV_C(str_c);
						if (read == 1)
						{
							params_file.id3 += str_c;
							sofarRead += 1;
						}
						else
						{
							std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Unexpected end of file in bext chunk." << std::endl;
							parseError = true;
							break;
						}
					}
					if (parseError)
					{
						break;
					}
				}					
				else if (found_data)//(std::string)str_c == "data")
				{
					// ===============================================================
					// DATA SECTION
					// ===============================================================
					if (shouldRead == -1)
					{
						shouldRead = params_file.data_size_bytes;
					}
					else
					{
						// In RIFF64 this value should have been defined before
						params_file.data_size_bytes = shouldRead;
					}
					if (JVX_CHECK_SIZE_UNSELECTED(params_file.data_size_bytes))
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read size of data section." << std::endl;
						parseError = true;
						break;
					}
					params_file.offset_start_data = byteCnt;
					int retVal = JVX_FSEEK(params_file.fHandle, params_file.data_size_bytes, SEEK_CUR);
					if (retVal != 0)
					{
						std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Unexpected end of file in data length scan." << std::endl;
						parseError = true;
						break;
					}
					byteCnt += params_file.data_size_bytes;
					sofarRead = params_file.data_size_bytes;
					params_file.offset_stop_data_p1 = byteCnt;
					// ===============================================================
					// END DATA SECTION
					// ===============================================================
				}
				else if (found_list)//(std::string)str_c == "LIST")
				{
					// ===============================================================
					// LIST SECTION
					// ===============================================================
					read = FREAD_WAV_STR(str_c);
					if (read == 4)
					{
						sofarRead += 4;
						byteCnt_info -= sizeof(char) * 4;
						if ((std::string)str_c == "INFO")
						{
							while (1)
							{
								read = FREAD_WAV_STR(str_c);
								if (read == 4)
								{
									sofarRead += 4;
									txt = "";

									read = FREAD_WAV_U32(&valUI32);
									if (read == 1)
									{
										sofarRead += 4;

										// Word alignment
										if (valUI32 % 2)
										{
											valUI32++;
										}
										for (i = 0; i < (int)valUI32; i++)
										{
											read = FREAD_WAV_C(oneFrag);
											if (read == 1)
											{
												sofarRead += 1;
												txt += oneFrag;
											}
											else
											{
												errCode = 11;
												parseError = true;
												break;
											}
										}
										if (!parseError)
										{
											oneTagEntry newEntry;
											newEntry.tagName = (std::string)str_c;
											newEntry.tagValue = txt;
											params_file.all_tag.push_back(newEntry);
										}
										if (sofarRead == shouldRead)
										{
											// If the lst tags are complete...
											break;
										}
									}
									else
									{
										std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read size of list entry section." << std::endl;
										parseError = true;
										break;
									}
								}
								else
								{
									std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": Failed to read list entry header." << std::endl;
									parseError = true;
									break;
								}
							}// while(1)
						}
						else
						{
							errCode = 14;
							parseError = true;
							break;
						}
					}
					else
					{
						errCode = 15;
						parseError = true;
						break;
					}
				}
				else if (found_marker)//(std::string)str_c == "LIST")
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <marker> chunk." << std::endl;
				}
				else if (found_pad)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <pad> chunk." << std::endl;
				}
				else if (found_junk)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <junk> chunk." << std::endl;
				}
				else if (found_levl)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <levl> chunk." << std::endl;
				}
				else if (found_summarylist)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << ": Do not know what to do with the <summarylist> chunk." << std::endl;
				}
				else if (found_xmp)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <xmp> chunk." << std::endl;
				}
				else if (found_disp)
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <DISP> chunk." << std::endl;
				}
				else if (found_fact)//(std::string)str_c == "LIST")
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <fact> chunk." << std::endl;
				}
				else if (found_peak)//(std::string)str_c == "LIST")
				{
					// Marker requires to end on a 64 bit bound..
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Do not know what to do with the <PEAK> chunk." << std::endl;
				}
				else
				{

					// Unknown sections
					std::cout << "Warning Position " << __COUNTER__ << " reading file " << fName << ": Unknown chunk type <" << unknown_type << "< found." << std::endl;

				}

				// Skip all unknown fields
				shouldRead -= sofarRead;

				if (shouldRead)
				{
					int retVal = JVX_FSEEK(params_file.fHandle, shouldRead, SEEK_CUR);
					if (retVal != 0)
					{
						noError = false;
						break;
					}
					byteCnt += shouldRead;
				}


			} // while (1) ->  All loops is over, let us postproces the collected data

		}// if(fread(&valI32, sizeof(jvxInt32), 1, params_file.fHandle) == 1)

		if (!noError)
		{
			res = JVX_ERROR_PARSE_ERROR;
		}

		if (res == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(params_file.fLength))
			{
				std::cout << "Error Position " << __COUNTER__ << " reading file " << fName << ": RIFF64 file without ds64 chunk sectin." << std::endl;
				res = JVX_ERROR_INVALID_SETTING;
			}
		}
		
		if(res == JVX_NO_ERROR)
		{
			params_file.length_data_samples = params_file.data_size_bytes / params_file.channels / (params_file.bits_per_sample >> 3); // 8 bits per sample
			JVX_FSEEK(params_file.fHandle, 0, SEEK_SET); // <- rewind


			state_object = JVX_STATE_ACTIVE;
		}
	} // if(state_object == JVX_STATE_SELECTED)
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavReader::deactivate()
{
	return(_deactivate());
}

jvxErrorType
jvxWavReader::set_loop(jvxBool loop)
{
	params_processing.loop = loop;
	return(JVX_NO_ERROR);
}

jvxErrorType
jvxWavReader::get_loop(jvxBool* loop)
{
	if(loop)
	{
		*loop = params_processing.loop;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
jvxWavReader::start()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object == JVX_STATE_PREPARED)
	{
		state_object = JVX_STATE_PROCESSING;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavReader::read_one_buf(jvxHandle** fld_out, jvxSize num_chans_read, jvxSize buffersize, jvxDataFormat format)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize read = 0;
	jvxSize i;
	jvxSize inOutCnt = 0;

	if(state_object == JVX_STATE_PROCESSING)
	{
		jvxSize readSizeMax = JVX_MIN(params_processing.numSamplesBuffer, buffersize);
		jvxSize stilltoread = buffersize;

		while(stilltoread)
		{
			// Read in data

			// Constraint due to end of file
			jvxSize readSize = JVX_MIN(readSizeMax, runtime.maxPositionP1 - runtime.position);
			
			// Constraint due to what is needed to be read
			readSize = JVX_MIN(stilltoread, readSize);

			if (readSize)
			{
				read = fread(runtime.oneChunk, params_processing.szOneSampleChannels_bytes, readSize, params_file.fHandle);
				assert(read == readSize);

				/* Do the actual data copy from runtime.oneChunk*/
				for (i = 0; i < num_chans_read; i++)
				{
					switch (format)
					{
					case JVX_DATAFORMAT_DATA:

						switch (params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch (params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
									(jvxInt16*)runtime.oneChunk,
									(jvxData*)fld_out[i],
									read,
									JVX_MAX_INT_16_DIV,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 24:
								jvx_convertSamples_from_bytes_shift_norm_to_flp<jvxInt32, jvxData>(
									(jvxByte*)runtime.oneChunk,
									(jvxData*)fld_out[i],
									read, 8,
									JVX_MAX_INT_32_DIV, 3,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 32:
								jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
									(jvxInt32*)runtime.oneChunk,
									(jvxData*)fld_out[i],
									read,
									JVX_MAX_INT_32_DIV,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch (params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_float_to_data<float>(
									(float*)runtime.oneChunk,
									(jvxData*)fld_out[i],
									read,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 64:
								jvx_convertSamples_from_to<jvxData>(
									(jvxData*)runtime.oneChunk,
									(jvxData*)fld_out[i],
									read,
									i, params_file.channels,
									inOutCnt, 1);
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
						switch (params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch(params_file.dformat)
							{
							case JVX_DATAFORMAT_16BIT_LE:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt16, jvxInt8, jvxInt16>(
									(jvxInt16*)runtime.oneChunk,
									(jvxInt8*)fld_out[i],
									read, 8,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt8, jvxInt32>(
									(jvxInt32*)runtime.oneChunk,
									(jvxInt8*)fld_out[i],
									read, 24,
									i, params_file.channels,
									inOutCnt, 1);
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
								jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt8>(
									(float*)runtime.oneChunk,
									(jvxInt8*)fld_out[i],
									read, JVX_MAX_INT_8_DATA,
									i, params_file.channels,
									inOutCnt, 1);
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
						switch (params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch (params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
									(jvxInt16*)runtime.oneChunk,
									(jvxInt16*)fld_out[i],
									read,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 24:
								jvx_convertSamples_from_bytes_shiftright_to_fxp<jvxInt16, jvxInt32>(
									(jvxByte*)runtime.oneChunk,
									(jvxInt16*)fld_out[i],
									read, 3, 8,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 32:
								jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
									(jvxInt32*)runtime.oneChunk,
									(jvxInt16*)fld_out[i],
									read, 16,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch (params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt16>(
									(float*)runtime.oneChunk,
									(jvxInt16*)fld_out[i],
									read, JVX_MAX_INT_16_DATA,
									i, params_file.channels,
									inOutCnt, 1);
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
						switch (params_file.fformat)
						{
						case JVX_FILEFORMAT_PCM_FIXED:
							switch (params_file.bits_per_sample)
							{
							case 16:
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
									(jvxInt16*)runtime.oneChunk,
									(jvxInt32*)fld_out[i],
									read, 16,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 24:
								jvx_convertSamples_from_bytes_shiftleft_to_fxp<jvxInt32, jvxInt32>(
									(jvxByte*)runtime.oneChunk,
									(jvxInt32*)fld_out[i],
									read, 3, 8,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case 32:
								jvx_convertSamples_from_fxp_noshift_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
									(jvxInt32*)runtime.oneChunk,
									(jvxInt32*)fld_out[i],
									read,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							default:
								assert(0);
							}
							break;
						case JVX_FILEFORMAT_IEEE_FLOAT:
							switch (params_file.bits_per_sample)
							{
							case 32:
								jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt32>(
									(float*)runtime.oneChunk,
									(jvxInt32*)fld_out[i],
									read, (float)JVX_MAX_INT_32_DATA,
									i, params_file.channels,
									inOutCnt, 1);
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
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt64, jvxInt64>(
									(jvxInt16*)runtime.oneChunk,
									(jvxInt64*)fld_out[i],
									read, 48,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							case JVX_DATAFORMAT_32BIT_LE:
								jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt32, jvxInt64, jvxInt64>(
									(jvxInt32*)runtime.oneChunk,
									(jvxInt64*)fld_out[i],
									read, 32,
									i, params_file.channels,
									inOutCnt, 1);
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
								jvx_convertSamples_from_flp_norm_to_fxp<float, jvxInt64>(
									(float*)runtime.oneChunk,
									(jvxInt64*)fld_out[i],
									read, JVX_MAX_INT_64_DATA,
									i, params_file.channels,
									inOutCnt, 1);
								break;
							default:
								assert(0);
							}
							break;
						default:
							assert(0);
						}
						*/
						break;
					default:
						assert(0);
					}
				}
			}
			else
			{
				read = 0;
			}

			runtime.position += read;
			inOutCnt += read;

			if (runtime.maxPositionP1 == runtime.position)
			{
				if (params_processing.loop)
				{
					this->rewind();
				}
				else
				{
					res = JVX_ERROR_END_OF_FILE;
					stilltoread = 0;
					break;
				}
			}


			stilltoread -= read;
		}

		// Set remaining output to zero
		jvxSize stilltofill = buffersize - inOutCnt;
		if(stilltofill)
		{
			for(i = 0; i < num_chans_read; i++)
			{
				jvxByte* oneBuf = (jvxByte*)fld_out[i];
				oneBuf += inOutCnt * jvxDataFormat_size[format];

				memset(oneBuf, 0, jvxDataFormat_size[format]*stilltofill);
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
jvxWavReader::read_one_buf_raw(jvxByte* fld_out, jvxSize num_bytes_to_read, jvxSize* num_bytes_at_read)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize read = 0;
	jvxSize readAll = 0;
	jvxSize inOutCnt = 0;

	if (state_object == JVX_STATE_PROCESSING)
	{
		jvxSize numBytesMaxRead = params_processing.numSamplesBuffer * params_processing.szOneSampleChannels_bytes;
		jvxSize numBytesRemain = (runtime.maxPositionP1 - runtime.position) * params_processing.szOneSampleChannels_bytes;
		jvxSize readSizeMax = JVX_MIN(numBytesMaxRead, num_bytes_to_read);
		readSizeMax = JVX_MIN(numBytesRemain, readSizeMax);
		jvxSize stillToGo = num_bytes_to_read;

		jvxByte* readOffsetPtr = fld_out;
		while(stillToGo > 0)
		{
			jvxSize readnow = JVX_MIN(readSizeMax, stillToGo);
			read = fread(readOffsetPtr, sizeof(jvxByte), readnow, params_file.fHandle);
			readOffsetPtr += read;
			readAll += read;
			jvxSize samplesOut = read / params_processing.szOneSampleChannels_bytes;
			runtime.position += samplesOut;
			stillToGo -= read;

			if (runtime.maxPositionP1 == runtime.position)
			{
				if (params_processing.loop)
				{
					this->rewind();
				}
				else
				{
					res = JVX_ERROR_END_OF_FILE;
					break;
				}
			}
		}
		
		if (num_bytes_at_read)
		{
			*num_bytes_at_read = readAll;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavReader::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object == JVX_STATE_PROCESSING)
	{
		state_object = JVX_STATE_PREPARED;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}


jvxErrorType
jvxWavReader::rewind()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(state_object == JVX_STATE_PROCESSING)
	{
		JVX_FSEEK(params_file.fHandle, params_file.offset_start_data, SEEK_SET);
		runtime.position = 0;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

jvxErrorType
jvxWavReader::current_progress(jvxSize* progress_samples)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize posiBytes = 0;
	jvxSize posiSamples = 0;
	if(state_object > JVX_STATE_ACTIVE)
	{
		if(progress_samples)
		{
			*progress_samples = runtime.position;
		}
		return(JVX_NO_ERROR);
	}
	if(progress_samples)
	{
		*progress_samples = 0;
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
jvxWavReader::wind_bwd(jvxSize deltaT_samples)
{
	jvxInt64 posiBytes = 0;
	jvxSize posiSamples = 0;
	jvxSize decSamples = 0;
	jvxSize decBytes = 0;

	jvxSize maxValForward = 0;
	if(state_object > JVX_STATE_ACTIVE)
	{
		posiBytes = JVX_FTELL(params_file.fHandle);
		assert(posiBytes >= 0);
		posiSamples = (jvxSize)posiBytes / params_processing.szOneSampleChannels_bytes;
		decSamples = deltaT_samples;
		decSamples = JVX_MIN(decSamples, posiSamples);
		decBytes = decSamples * params_processing.szOneSampleChannels_bytes;
		JVX_FSEEK(params_file.fHandle, -(jvxInt64)decBytes, SEEK_CUR); // Be careful: (-jvxSize) -> jvxSize!!
		posiBytes = JVX_FTELL(params_file.fHandle);
		assert(posiBytes >= 0);
		runtime.position -= decSamples;

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
jvxWavReader::wind_fwd(jvxSize deltaT_samples)
{
	jvxSize posiBytes = 0;
	jvxSize posiSamples = 0;
	jvxSize incSamples = 0;
	jvxSize incBytes = 0;

	jvxSize maxValForward = 0;
	if(state_object > JVX_STATE_ACTIVE)
	{
		posiBytes = JVX_FTELL(params_file.fHandle);
		posiSamples = posiBytes / params_processing.szOneSampleChannels_bytes;
		incSamples = deltaT_samples;
		incSamples = JVX_MIN(incSamples, params_file.length_data_samples - posiSamples);
		incBytes = incSamples * params_processing.szOneSampleChannels_bytes;
		JVX_FSEEK(params_file.fHandle, incBytes, SEEK_CUR);
		runtime.position += incSamples;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}
