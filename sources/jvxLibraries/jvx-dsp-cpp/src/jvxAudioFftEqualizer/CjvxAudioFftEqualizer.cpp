#include "jvxAudioFftEqualizer/CjvxAudioFftEqualizer.h"

CjvxAudioFftEqualizer::CjvxAudioFftEqualizer()
{
	numChannels = JVX_SIZE_UNSELECTED;
	firEqSize = 0;
	firEqFftSize = 0;
	delayFir = JVX_SIZE_UNSELECTED;

	stat = JVX_STATE_INIT;
	proc_data.fir_eq = NULL;
	proc_data.gainFac = NULL;
	proc_data.delayChan = NULL;
	proc_data.delayEng = false;
	proc_data.gainEng = false;
	proc_data.firEng = false;
}

CjvxAudioFftEqualizer::~CjvxAudioFftEqualizer()
{
	if (stat == JVX_STATE_ACTIVE)
	{
		clear_config();
	}
}

jvxErrorType 
CjvxAudioFftEqualizer::clear_config()
{
	jvxSize i;
	if (stat == JVX_STATE_ACTIVE)
	{
		if (numChannels)
		{
			if (proc_data.firEng)
			{
				if (firEqSize)
				{
					for (i = 0; i < numChannels; i++)
					{
						JVX_DSP_SAFE_DELETE_FIELD(proc_data.fir_eq[i]);
					}
				}
				JVX_DSP_SAFE_DELETE_FIELD(proc_data.fir_eq);
			}
			proc_data.firEng = false;

			if (proc_data.gainEng)
			{
				JVX_DSP_SAFE_DELETE_FIELD(proc_data.gainFac);
			}
			proc_data.gainEng = false;

			if (proc_data.delayEng)
			{
				JVX_DSP_SAFE_DELETE_FIELD(proc_data.delayChan);
			}
			proc_data.delayEng = false;
		}

		numChannels = JVX_SIZE_UNSELECTED;
		stat = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAudioFftEqualizer::allocate_config(IjvxConfigProcessor* theConfigHdl, jvxConfigData* theLocalSection, 
	std::string& errorDescr, jvxSize delay_max_arg)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize sz;
	jvxSize i, j;
	jvxValue val;
	jvxApiValueList lstVals_fir;
	jvxApiValueList lstVals_delay;
	jvxApiValueList lstVals_gain;

	jvxConfigData* theLocalLocalEntry = NULL;
	jvxConfigData* theLocalLocalEntryFir = NULL;
	if (stat == JVX_STATE_INIT)
	{
		proc_data.delayEng = false;
		proc_data.gainEng = false;
		proc_data.firEng = false;
		firEqFftSize = 0;
		firEqSize = 0;


		theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "out");
		if (theLocalLocalEntry)
		{
			resL = theConfigHdl->getAssignmentValue(theLocalLocalEntry, &val);
			if (resL == JVX_NO_ERROR)
			{
				// Check order 
				val.toContent(&sz);
				if (JVX_CHECK_SIZE_UNSELECTED(numChannels))
				{
					numChannels = sz;
				}
				else
				{
					if (sz != numChannels)
					{
						errorDescr = "Equalizer output number mismatch, ";
						errorDescr += jvx_size2String(sz) + " detected in file whereas ";
						errorDescr += jvx_size2String(numChannels) + " detected in current configuration.";
						return JVX_ERROR_INVALID_SETTING;
					}
				}
			}
			else
			{
				errorDescr = "Failed to read assignment value for number outputs decoder.";
				return JVX_ERROR_INVALID_SETTING;
			}
		}
		else
		{
			errorDescr = "Assignment value for number outputs decoder is not present.";
			return JVX_ERROR_INVALID_SETTING;
		}

		theLocalLocalEntry = NULL;
		theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "delay_samples");
		if (theLocalLocalEntry)
		{
			resL = theConfigHdl->getValueList_id(theLocalLocalEntry, &lstVals_delay, 0);
			if (resL == JVX_NO_ERROR)
			{
				if (lstVals_delay.ll() != numChannels)
				{
					errorDescr = "Equalizer delay_samples number mismatch, ";
					errorDescr += jvx_size2String(lstVals_delay.ll()) + " detected in file whereas ";
					errorDescr += jvx_size2String(numChannels) + " detected in current configuration.";
					resL = JVX_ERROR_INVALID_SETTING;
				}
				else
				{
					proc_data.delayEng = true;
				}
			}
			/*
			else
			{
				errorDescr = "Failed to read assignment value for channel delay.";
				resL = JVX_ERROR_INVALID_SETTING;
			}
			THIS IS A VALID CONFIGURATION
			*/ 
		}
		else
		{
			// Ok, just no gains specified
		}

		if (resL == JVX_NO_ERROR)
		{			
			theLocalLocalEntry = NULL;
			theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "gain");
			if (theLocalLocalEntry)
			{
				resL = theConfigHdl->getValueList_id(theLocalLocalEntry, &lstVals_gain, 0);
				if (resL == JVX_NO_ERROR)
				{
					if (lstVals_gain.ll() != numChannels)
					{
						errorDescr = "Equalizer gain number mismatch, ";
						errorDescr += jvx_size2String(lstVals_gain.ll()) + " detected in file whereas ";
						errorDescr += jvx_size2String(numChannels) + " detected in current configuration.";
						resL = JVX_ERROR_INVALID_SETTING;
					}
					else
					{
						proc_data.gainEng = true;
					}
				}
				/*
				else
				{
					errorDescr = "Failed to read assignment value for gain decoder.";
					resL = JVX_ERROR_INVALID_SETTING;
				}
				THIS IS A VALID CONFIGURATION
				*/
			}
			else
			{
				// Ok, just no gains specified
			}
		}

		if (resL == JVX_NO_ERROR)
		{			
			theLocalLocalEntry = NULL;
			theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntryFir, "fir_eq");
			if (theLocalLocalEntryFir)
			{
				theConfigHdl->getNumberValueLists(theLocalLocalEntryFir, &sz);
				if (sz != numChannels)
				{
					errorDescr = "Equalizer eq fir number mismatch, ";
					errorDescr += jvx_size2String(sz) + " detected in file whereas ";
					errorDescr += jvx_size2String(numChannels) + " detected in current configuration.";
					resL = JVX_ERROR_INVALID_SETTING;
				}
				else
				{
					theConfigHdl->getMaxSizeValueLists(theLocalLocalEntryFir, &firEqSize);
					proc_data.firEng = true;
				}
			}
			else
			{
				// Ok, just no fir specified
			}

			if (proc_data.firEng)
			{				
				theLocalLocalEntry = NULL;
				theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "fftsize_eq");
				if (theLocalLocalEntry)
				{
					resL = theConfigHdl->getAssignmentValue(theLocalLocalEntry, &val);
					if (resL == JVX_NO_ERROR)
					{
						val.toContent(&firEqFftSize);
					}
					else
					{
						errorDescr = "Equalizer eq fir fft size type mismatch.";
						resL = JVX_ERROR_INVALID_SETTING;
					}
				}
			}

			if (resL == JVX_NO_ERROR)
			{
				delayFir = JVX_SIZE_UNSELECTED;
				theLocalLocalEntry = NULL;
				theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "delay_eq");
				if (theLocalLocalEntry)
				{
					resL = theConfigHdl->getAssignmentValue(theLocalLocalEntry, &val);
					if (resL == JVX_NO_ERROR)
					{
						val.toContent(&delayFir);
					}
					else
					{
						errorDescr = "Equalizer eq fir fft size type mismatch.";
						resL = JVX_ERROR_INVALID_SETTING;
					}
				}
			}
		}

		if (resL == JVX_NO_ERROR)
		{
			// Allocate the data 
			if (numChannels)
			{
				if (proc_data.gainEng)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.gainFac, jvxData, numChannels);
					for (i = 0; i < numChannels; i++)
					{
						lstVals_gain.elm_at(i).toContent(&proc_data.gainFac[i]);
					}
				}
				
				if (proc_data.delayEng)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.delayChan, jvxSize, numChannels);
					for (i = 0; i < numChannels; i++)
					{
						lstVals_delay.elm_at(i).toContent(&proc_data.delayChan[i]);
					}
				}

				if (proc_data.firEng)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.fir_eq, jvxData*, numChannels);
					if (firEqSize)
					{
						for (i = 0; i < numChannels; i++)
						{
							JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.fir_eq[i], jvxData, firEqSize);
						}
					}

					for (i = 0; i < numChannels; i++)
					{
						theConfigHdl->getValueList_id(theLocalLocalEntryFir, &lstVals_fir, i);
						for (j = 0; j < lstVals_fir.ll(); j++)
						{
							lstVals_fir.elm_at(j).toContent(&proc_data.fir_eq[i][j]);
						}
					}
				}
			}

			delay_max = delay_max_arg;
			stat = JVX_STATE_ACTIVE;
			resL = JVX_NO_ERROR;
		}
		else
		{
			// If there was an error, deactivate functionality
			numChannels = JVX_SIZE_UNSELECTED;
			proc_data.delayEng = false;
			proc_data.gainEng = false;
			proc_data.firEng = false;
			firEqFftSize = 0;
			firEqSize = 0;
		}
	}
	return resL;
}


jvxErrorType 
CjvxAudioFftEqualizer::prepare_eq(jvxSize bsize)
{
	jvxSize i;

	if (stat == JVX_STATE_ACTIVE)
	{
		proc_variables.bsize = bsize;
		if (numChannels)
		{
			if (proc_data.firEng)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_variables.theFftFilt, jvx_firfft, numChannels);

				for (i = 0; i < numChannels; i++)
				{
					// ========================================================================================
					jvxErrorType resL = jvx_firfft_initCfg(&proc_variables.theFftFilt[i]);
					proc_variables.theFftFilt[i].init.bsize = bsize;
					proc_variables.theFftFilt[i].init.lFir = firEqSize;
					proc_variables.theFftFilt[i].init.type = JVX_FIRFFT_FLEXIBLE_FIR;
					proc_variables.theFftFilt[i].init.fir = proc_data.fir_eq[i];
					if (JVX_CHECK_SIZE_SELECTED(this->firEqFftSize))
					{
						proc_variables.theFftFilt[i].init.lFft = this->firEqFftSize;
					}
					if (JVX_CHECK_SIZE_SELECTED(this->delayFir))
					{
						proc_variables.theFftFilt[i].init.delayFir = this->delayFir;
					}
					resL = jvx_firfft_init(&proc_variables.theFftFilt[i]);
					assert(resL == JVX_DSP_NO_ERROR);

					proc_variables.theFftFilt[i].init.fir = NULL; // this is only for initialization and will afterwards not be used
				}
			}
			if (proc_data.delayEng)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_variables.delayline, jvx_circbuffer*, numChannels);
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_variables.current_delay, jvxSize, numChannels);

				// ==========================
				// Max delay computation
				// ==========================

				proc_variables.delay_max = delay_max;
				if (JVX_CHECK_SIZE_UNSELECTED(proc_variables.delay_max))
				{
					proc_variables.delay_max = 0;
				}

				for (i = 0; i < numChannels; i++)
				{
					proc_variables.delay_max = JVX_MAX(proc_variables.delay_max, proc_data.delayChan[i]);
				}

				for (i = 0; i < numChannels; i++)
				{

					// ========================================================================================
					jvx_circbuffer_allocate_1chan(&proc_variables.delayline[i], proc_variables.bsize + proc_variables.delay_max);
					proc_variables.current_delay[i] = proc_data.delayChan[i];
					jvx_circbuffer_fill(proc_variables.delayline[i], 0, proc_variables.current_delay[i]);
				}
			}

			if (proc_data.gainEng)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_variables.current_gain, jvxData, numChannels);
				for (i = 0; i < numChannels; i++)
				{
					// ========================================================================================
					proc_variables.current_gain[i] = proc_data.gainFac[i];
				}
			}
		}
		// ==========================
		// ==========================
			
		stat = JVX_STATE_PROCESSING;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFftEqualizer::postprocess_eq()
{
	jvxSize i;
	if (stat == JVX_STATE_PROCESSING)
	{
		if (numChannels)
		{
			if (proc_data.gainEng)
			{
				for (i = 0; i < numChannels; i++)
				{
					proc_variables.current_gain[i] = 0;
				}
				JVX_DSP_SAFE_DELETE_FIELD(proc_variables.current_gain);
			}

			if (proc_data.firEng)
			{
				for (i = 0; i < numChannels; i++)
				{
					// ========================================================================================
					jvx_firfft_terminate(&proc_variables.theFftFilt[i]);
				}
				JVX_DSP_SAFE_DELETE_FIELD(proc_variables.theFftFilt);
			}

			if (proc_data.delayEng)
			{
				for (i = 0; i < numChannels; i++)
				{
					// ========================================================================================
					jvx_circbuffer_deallocate(proc_variables.delayline[i]);
				}

				JVX_DSP_SAFE_DELETE_FIELD(proc_variables.current_delay);
				JVX_DSP_SAFE_DELETE_FIELD(proc_variables.delayline);

			}
			proc_variables.delay_max = JVX_SIZE_UNSELECTED;
		}
		stat = JVX_STATE_ACTIVE;
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioFftEqualizer::process_eq_iplace(jvxData** bufs, jvxSize numBufs, jvxSize bsize)
{
	jvxSize i,j;
	jvxSize numChansMin = JVX_MIN(numChannels, numBufs);

	if (proc_data.delayEng)
	{
		for (i = 0; i < numChansMin; i++)
		{
			jvx_circbuffer_write_update_1chan(proc_variables.delayline[i],
				(jvxData*)bufs[i], bsize);
			jvx_circbuffer_read_update_1chan(proc_variables.delayline[i],
				(jvxData*)bufs[i], bsize);
		}
	}

	if (proc_data.gainEng)
	{
		for (i = 0; i < numChansMin; i++)
		{
			for (j = 0; j < proc_variables.bsize; j++)
			{
				(bufs[i])[j] *= proc_variables.current_gain[i];
			}
		}
	}

	if (proc_data.firEng)
	{
		for (i = 0; i < numChansMin; i++)
		{
			jvxDspBaseErrorType resD = jvx_firfft_process_iplace(&proc_variables.theFftFilt[i], bufs[i]);
			assert(resD == JVX_DSP_NO_ERROR);
		}
	}

	return JVX_NO_ERROR;
}

// ===============================================================================================

jvxErrorType
CjvxAudioFftEqualizer::number_channels(jvxSize* num)
{
	if (stat == JVX_STATE_ACTIVE)
	{
		if (num)
		{
			*num = numChannels;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioFftEqualizer::fft_order(jvxSize* order)
{
	if (stat == JVX_STATE_ACTIVE)
	{
		if (order)
		{
			*order = firEqFftSize;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioFftEqualizer::current_delay(jvxSize idChannel, jvxSize* delay)
{
	jvxSize delayRet = 0.0; // neutral
	if (stat == JVX_STATE_ACTIVE)
	{
		if (idChannel < numChannels)
		{
			if (proc_data.gainEng)
			{
				delayRet = proc_data.delayChan[idChannel];
			}
			if (delay)
			{
				*delay = delayRet;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}


jvxErrorType 
CjvxAudioFftEqualizer::current_gain(jvxSize idChannel, jvxData* gain)
{
	jvxData gainRet = 1.0;
	if (stat == JVX_STATE_ACTIVE)
	{
		if (idChannel < numChannels)
		{
			if (proc_data.gainEng)
			{
				gainRet = proc_data.gainFac[idChannel];
			}
			if (gain)
			{
				*gain = gainRet;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioFftEqualizer::current_fir_eq(jvxSize idChannel, std::string& fir_eq_str)
{
	std::string strRet = "[]";
	if (stat == JVX_STATE_ACTIVE)
	{
		if (idChannel < numChannels)
		{
			if (proc_data.firEng)
			{
				jvxData* fldEq = proc_data.fir_eq[idChannel];
				strRet = jvx_valueList2String(fldEq, JVX_DATAFORMAT_DATA, firEqSize, JVX_DATA_2STRING_CONST_FORMAT_G);
			}
			fir_eq_str = strRet;

			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}
