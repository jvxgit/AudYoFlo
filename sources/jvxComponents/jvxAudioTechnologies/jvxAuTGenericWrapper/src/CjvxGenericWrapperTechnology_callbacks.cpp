#include "CjvxGenericWrapperTechnology.h"

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_input_file_set)
{
	assert(JVX_CHECK_SIZE_SELECTED(inPropCallId));
	if (
		(genGenericWrapper_technology::properties_selected_input_file.lookahead.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.lookahead.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].common.idxNumberBuffersLookahead = genGenericWrapper_technology::properties_selected_input_file.lookahead.value;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.boost_prio.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.boost_prio.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].common.boostPrio= (genGenericWrapper_technology::properties_selected_input_file.boost_prio.value == c_true);
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].common.pauseOnStart = 
			(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.value == c_true);
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.loop.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.loop.category == ident.cat)
		)
	{
		if (genGenericWrapper_technology::properties_selected_input_file.loop.value == c_true)
		{
			theFiles.theInputFiles[inPropCallId].ctTp = JVX_FILECONTINUE_LOOP;
		}
		else
		{
			theFiles.theInputFiles[inPropCallId].ctTp = JVX_FILECONTINUE_STOPEND;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.triggerFwd.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.triggerFwd.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].flag_fwd = c_true;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.triggerPlay.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.triggerPlay.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].flag_play = c_true;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.triggerPause.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.triggerPause.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].flag_pause = c_true;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.triggerRestart.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.triggerRestart.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].flag_restart = c_true;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.triggerBwd.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.triggerBwd.category == ident.cat)
		)
	{
		theFiles.theInputFiles[inPropCallId].flag_bwd = c_true;
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	inPropCallId = JVX_SIZE_UNSELECTED;
	return JVX_NO_ERROR;
}

// ======================================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_input_file_set_pre)
{
	inPropCallId = tune.offsetStart;
	tune.offsetStart = 0;
	return JVX_NO_ERROR;
}

// ======================================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_input_file_get)
{
	jvxSize i;
	// Prefilter get operations
	if (
		(genGenericWrapper_technology::properties_selected_input_file.friendly_name.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.friendly_name.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_input_file.friendly_name.value =
				theFiles.theInputFiles[tune.offsetStart].common.friendlyName;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.file_length.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.file_length.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_input_file.file_length.value =
				theFiles.theInputFiles[tune.offsetStart].common.length;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.samplerate.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.samplerate.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_input_file.samplerate.value =
				theFiles.theInputFiles[tune.offsetStart].common.samplerate;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.mode.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.mode.category == ident.cat)
		)
	{
		jvxSize idSel = JVX_SIZE_UNSELECTED;
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			// Microsoft bug workaround: invalid RF64 headers lead to crash of explorer. Invalid means
			// other than 24 bits resolution. Therefore, I deactivate 16 and 32 bit resolutions
			// <- this bug seem to have been removed!
			
			/*
			 * "PCM-16BIT-RF64" - 4, 
			 * "PCM-24BIT-RF64" - 5, 
			 * "PCM-32BIT-RF64" - 6, 
			 * "PCM-32BIT-float-RF64" - 7
			 */
			switch(theFiles.theInputFiles[tune.offsetStart].common.subformat)
			{
			case JVX_WAV_64:
				if (theFiles.theInputFiles[tune.offsetStart].common.fform_file == JVX_FILEFORMAT_IEEE_FLOAT)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT_RF64;
				}
				//else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 24)
				if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 24)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT_RF64;					
				}
				else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 16)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT_RF64;
				}
				else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 32)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_RF64;
				}
				else
				{
					assert(0);
				}
				break;
			case JVX_WAV_32:

				/*
				 * "PCM-16BIT" - 0,
				 * "PCM-24BIT" - 1,
				 * "PCM-32BIT" - 2,
				 * "PCM-32BIT-float" - 3,
				 */

				if (theFiles.theInputFiles[tune.offsetStart].common.fform_file == JVX_FILEFORMAT_IEEE_FLOAT)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT;

				}
				else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 24)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT;
				}
				else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 16)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT;
				}
				else if (theFiles.theInputFiles[tune.offsetStart].common.numBitsSample == 32)
				{
					idSel = (int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT;
				}
				else
				{
					assert(0);
				}
				break;
			default:
				assert(0);
			}

			assert(JVX_CHECK_SIZE_SELECTED(idSel));
			jvx_bitZSet(
				genGenericWrapper_technology::properties_selected_input_file.mode.value.selection(),
				idSel);
			if(rawPtr->ftype() == JVX_DATAFORMAT_STRING)
			{
				// If we request a string from a selection list we need to encode the selection in the offset to read out the value directly
				tune.offsetStart = idSel;
			}
			else
			{
				tune.offsetStart = 0;
			}
		}		
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.lookahead.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.lookahead.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			genGenericWrapper_technology::properties_selected_input_file.lookahead.value =
				theFiles.theInputFiles[tune.offsetStart].common.idxNumberBuffersLookahead;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.boost_prio.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.boost_prio.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			if (theFiles.theInputFiles[tune.offsetStart].common.boostPrio)
			{
				genGenericWrapper_technology::properties_selected_input_file.lookahead.value = c_true;
			}
			else
			{
				genGenericWrapper_technology::properties_selected_input_file.lookahead.value = c_false;
			}
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.loop.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.loop.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			if (theFiles.theInputFiles[tune.offsetStart].ctTp == JVX_FILECONTINUE_LOOP)
			{
				genGenericWrapper_technology::properties_selected_input_file.loop.value = c_true;
			}
			else
			{
				genGenericWrapper_technology::properties_selected_input_file.loop.value = c_false;
			}
			tune.offsetStart = 0;
		}
	}

	else if (
	(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.globalIdx == ident.id) &&
	(genGenericWrapper_technology::properties_selected_input_file.pause_on_start.category == ident.cat)
	)
	{
	if (tune.offsetStart < theFiles.theInputFiles.size())
	{
		if (theFiles.theInputFiles[tune.offsetStart].common.pauseOnStart)
		{
			genGenericWrapper_technology::properties_selected_input_file.pause_on_start.value = c_true;
		}
		else
		{
			genGenericWrapper_technology::properties_selected_input_file.pause_on_start.value = c_false;
		}
		tune.offsetStart = 0;
	}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_input_file.progress.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_input_file.progress.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_input_file.progress.value =
				theFiles.theInputFiles[tune.offsetStart].progress;
			tune.offsetStart = 0;
		}
	}
	else if (
	(genGenericWrapper_technology::properties_selected_input_file.num_channels.globalIdx == ident.id) &&
	(genGenericWrapper_technology::properties_selected_input_file.num_channels.category == ident.cat)
	)
	{
		if (tune.offsetStart < theFiles.theInputFiles.size())
		{
			genGenericWrapper_technology::properties_selected_input_file.num_channels.value =
				theFiles.theInputFiles[tune.offsetStart].common.number_channels;
			tune.offsetStart = 0;
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_NO_ERROR;
}

// ==========================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_output_file_set)
{
	assert(JVX_CHECK_SIZE_SELECTED(inPropCallId));
	if (
		(genGenericWrapper_technology::properties_selected_output_file.lookahead.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.lookahead.category == ident.cat)
		)
	{
		theFiles.theOutputFiles[inPropCallId].common.idxNumberBuffersLookahead = genGenericWrapper_technology::properties_selected_output_file.lookahead.value;
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.boost_prio.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.boost_prio.category == ident.cat)
		)
	{
		theFiles.theOutputFiles[inPropCallId].common.boostPrio = (genGenericWrapper_technology::properties_selected_output_file.boost_prio.value == c_true);
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.samplerate.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.samplerate.category == ident.cat)
		)
	{
	  theFiles.theOutputFiles[inPropCallId].common.samplerate = JVX_SIZE_INT32(genGenericWrapper_technology::properties_selected_output_file.samplerate.value);
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.num_channels.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.num_channels.category == ident.cat)
		)
	{
		theFiles.theOutputFiles[inPropCallId].common.number_channels = genGenericWrapper_technology::properties_selected_output_file.num_channels.value;
		this->updateAllDevicesOtherThan_lock(NULL);
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.mode.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.mode.category == ident.cat)
		)
	{
		// Make sure, use of this property also is successful if no output file has been selected
		if (inPropCallId < theFiles.theOutputFiles.size())
		{
			// Microsoft bug workaround: invalid RF64 headers lead to crash of explorer. Invalid means
			// other than 24 bits resolution. Therefore, I deactivate 16 and 32 bit resolutions

			/*
			 * "PCM-16BIT" - 0,
			 * "PCM-24BIT" - 1,
			 * "PCM-32BIT" - 2,
			 * "PCM-32BIT-float" - 3,
			 */

			if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_32;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 16;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_32;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 24;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_32;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 32;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_IEEE_FLOAT;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_32;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 32;
			}

			/*
			 * "PCM-16BIT-RF64" - 4,
			 * "PCM-24BIT-RF64" - 5,
			 * "PCM-32BIT-RF64" - 6,
			 * "PCM-32BIT-float-RF64" - 7
			 */


			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT_RF64))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_64;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 16;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT_RF64))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_64;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 24;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_RF64))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_PCM_FIXED;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_64;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 32;
			}
			else if (jvx_bitTest(
				genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
				(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT_RF64))
			{
				theFiles.theOutputFiles[inPropCallId].common.fform_file = JVX_FILEFORMAT_IEEE_FLOAT;
				theFiles.theOutputFiles[inPropCallId].common.subformat = JVX_WAV_64;
				theFiles.theOutputFiles[inPropCallId].common.numBitsSample = 32;
			}
			else
			{
				assert(0);
			}
		}
	}
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	inPropCallId = JVX_SIZE_UNSELECTED;
	return JVX_NO_ERROR;
}

// ======================================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_output_file_set_pre)
{
	inPropCallId = tune.offsetStart;
	tune.offsetStart = 0;
	return JVX_NO_ERROR;
}

// ======================================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_output_file_get)
{
	jvxSize i;
	// Prefilter get operations
	if (
		(genGenericWrapper_technology::properties_selected_output_file.samplerate.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.samplerate.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theOutputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_output_file.samplerate.value =
				theFiles.theOutputFiles[tune.offsetStart].common.samplerate;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.num_channels.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.num_channels.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theOutputFiles.size())
		{
			// Filter the request to output the right property
			genGenericWrapper_technology::properties_selected_output_file.num_channels.value =
				theFiles.theOutputFiles[tune.offsetStart].common.number_channels;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.mode.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.mode.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theOutputFiles.size())
		{
			// Microsoft bug workaround: invalid RF64 headers lead to crash of explorer. Invalid means
			// other than 24 bits resolution. Therefore, I deactivate 16 and 32 bit resolutions

			switch(theFiles.theOutputFiles[tune.offsetStart].common.subformat)
			{
			case JVX_WAV_64:
				if (theFiles.theOutputFiles[tune.offsetStart].common.fform_file == JVX_FILEFORMAT_IEEE_FLOAT)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT_RF64);

				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 32)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_RF64);
				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 24)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT_RF64);
				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 16)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT_RF64);
				}
				else
				{
					assert(0);
				}
				break;
			case JVX_WAV_32:
				if (theFiles.theOutputFiles[tune.offsetStart].common.fform_file == JVX_FILEFORMAT_IEEE_FLOAT)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT);
				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 32)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_32BIT);
				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 24)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_24BIT);
				}
				else if (theFiles.theOutputFiles[tune.offsetStart].common.numBitsSample == 16)
				{
					jvx_bitZSet(
						genGenericWrapper_technology::properties_selected_output_file.mode.value.selection(),
						(int)jvxWavFileType::JVX_WAV_FILE_TYPE_PCM_16BIT);
				}
				else
				{
					assert(0);
				}
				break;
			default: 
				assert(0);
			}
			tune.offsetStart = 0;
		}
		// Set the friendly
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.lookahead.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.lookahead.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theOutputFiles.size())
		{
			genGenericWrapper_technology::properties_selected_output_file.lookahead.value =
				theFiles.theOutputFiles[tune.offsetStart].common.idxNumberBuffersLookahead;
			tune.offsetStart = 0;
		}
	}

	else if (
		(genGenericWrapper_technology::properties_selected_output_file.boost_prio.globalIdx == ident.id) &&
		(genGenericWrapper_technology::properties_selected_output_file.boost_prio.category == ident.cat)
		)
	{
		if (tune.offsetStart < theFiles.theOutputFiles.size())
		{
			if (theFiles.theOutputFiles[tune.offsetStart].common.boostPrio)
			{
				genGenericWrapper_technology::properties_selected_output_file.lookahead.value = c_true;
			}
			else
			{
				genGenericWrapper_technology::properties_selected_output_file.lookahead.value = c_false;
			}
			tune.offsetStart = 0;
		}
	}

	
	else
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperTechnology, cb_prop_selected_technology_set)
{
	skipConfigAudioTech = true;
	return JVX_NO_ERROR;
}