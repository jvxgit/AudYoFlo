#include "jvxAudioFixedFftN1Filter/CjvxAudioFixedFftN1Filter.h"

CjvxAudioFixedFftN1Filter::CjvxAudioFixedFftN1Filter()
{
}

CjvxAudioFixedFftN1Filter::~CjvxAudioFixedFftN1Filter()
{
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::clear_config()
{
	jvxSize i;
	if (stat == JVX_STATE_ACTIVE)
	{
		if (numChannelsIn)
		{
			if (proc_data.fir_in)
			{
				if (firSize)
				{
					for (i = 0; i < numChannelsIn; i++)
					{
						JVX_DSP_SAFE_DELETE_FIELD(proc_data.fir_in[i]);
					}
				}
				JVX_DSP_SAFE_DELETE_FIELD(proc_data.fir_in);
			}
		}

		numChannelsIn = JVX_SIZE_UNSELECTED;
		stat = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::allocate_config(
		IjvxConfigProcessor* theConfigHdl, 
		jvxConfigData* theLocalSection, 
		const std::string& token_scan,
		std::string& errorDescr)
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
		firSize = 0;

		theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntry, "in");
		if (theLocalLocalEntry)
		{
			resL = theConfigHdl->getAssignmentValue(theLocalLocalEntry, &val);
			if (resL == JVX_NO_ERROR)
			{
				// Check order 
				val.toContent(&sz);
				if (JVX_CHECK_SIZE_UNSELECTED(numChannelsIn))
				{
					numChannelsIn = sz;
				}
				else
				{
					if (sz != numChannelsIn)
					{
						errorDescr = "N1 Filter output number mismatch, ";
						errorDescr += jvx_size2String(sz) + " detected in file whereas ";
						errorDescr += jvx_size2String(numChannelsIn) + " detected in current configuration.";
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

		theLocalLocalEntryFir = NULL;
		theConfigHdl->getReferenceEntryCurrentSection_name(theLocalSection, &theLocalLocalEntryFir, token_scan.c_str());
		if (theLocalLocalEntryFir)
		{
			theConfigHdl->getNumberValueLists(theLocalLocalEntryFir, &sz);
			if (sz != numChannelsIn)
			{
				errorDescr = "Filter N1 fir number mismatch, ";
				errorDescr += jvx_size2String(sz) + " detected in file whereas ";
				errorDescr += jvx_size2String(numChannelsIn) + " detected in current configuration.";
				resL = JVX_ERROR_INVALID_SETTING;
			}
			else
			{
				theConfigHdl->getMaxSizeValueLists(theLocalLocalEntryFir, &firSize);
			}
		}
		else
		{
			// Ok, just no fir specified
		}

		if (resL == JVX_NO_ERROR)
		{
			// Allocate the data 
			if (numChannelsIn)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.fir_in, jvxData*, numChannelsIn);
				if (firSize)
				{
					for (i = 0; i < numChannelsIn; i++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_data.fir_in[i], jvxData, firSize);
					}
				}

				for (i = 0; i < numChannelsIn; i++)
				{
					theConfigHdl->getValueList_id(theLocalLocalEntryFir, &lstVals_fir, i);
					for (j = 0; j < lstVals_fir.ll(); j++)
					{
						lstVals_fir.elm_at(j).toContent(&proc_data.fir_in[i][j]);
					}
				}
			}

			stat = JVX_STATE_ACTIVE;
			resL = JVX_NO_ERROR;
		}
		else
		{
			// If there was an error, deactivate functionality
			numChannelsIn = JVX_SIZE_UNSELECTED;
			firSize = 0;
		}
	}
	return resL;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::prepare_N1Filter(jvxSize bsize)
{
	jvxSize i;
	if (stat == JVX_STATE_ACTIVE)
	{
		proc_variables.bsize = bsize;
		if (numChannelsIn)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_variables.theFftFilt, jvx_firfft, numChannelsIn);
			firFftSize = JVX_SIZE_UNSELECTED;

			for (i = 0; i < numChannelsIn; i++)
			{
				// ========================================================================================
				jvxErrorType resL = jvx_firfft_initCfg(&proc_variables.theFftFilt[i]);
				proc_variables.theFftFilt[i].init.bsize = bsize;
				proc_variables.theFftFilt[i].init.lFir = firSize;
				proc_variables.theFftFilt[i].init.type = JVX_FIRFFT_FLEXIBLE_FIR;
				proc_variables.theFftFilt[i].init.fir = proc_data.fir_in[i];
				if (JVX_CHECK_SIZE_SELECTED(this->firFftSize))
				{
					proc_variables.theFftFilt[i].init.lFft = this->firFftSize;
				}

				// We may specify a delay value here if the delay is not known from the 
				// type
				if (JVX_CHECK_SIZE_SELECTED(this->delayFir))
				{
					proc_variables.theFftFilt[i].init.delayFir = this->delayFir;
				}
				resL = jvx_firfft_init(&proc_variables.theFftFilt[i]);
				assert(resL == JVX_DSP_NO_ERROR);

				proc_variables.theFftFilt[i].init.fir = NULL; // this is only for initialization and will afterwards not be used
			}
		}

		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_variables.out_buf, jvxData, bsize);

		// ==========================
		// ==========================

		stat = JVX_STATE_PROCESSING;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::process_N1Filter(jvxData** bufs_in, jvxData* buf_out, jvxSize numChannels, jvxSize bsize)
{
	jvxSize i;
	jvxBool isFirst = true;
	jvxSize numChansMin = JVX_MIN(numChannelsIn, numChannels);

	for (i = 0; i < numChansMin; i++)
	{
		jvxDspBaseErrorType resD = jvx_firfft_process_mix(&proc_variables.theFftFilt[i], bufs_in[i], buf_out, isFirst, proc_variables.out_buf);
		assert(resD == JVX_DSP_NO_ERROR);
		isFirst = false;
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::postprocess_N1Filter()
{
	jvxSize i;
	if (stat == JVX_STATE_PROCESSING)
	{
		JVX_DSP_SAFE_DELETE_FIELD(proc_variables.out_buf);
		if (numChannelsIn)
		{
			for (i = 0; i < numChannelsIn; i++)
			{
				// ========================================================================================
				jvx_firfft_terminate(&proc_variables.theFftFilt[i]);
			}
			JVX_DSP_SAFE_DELETE_FIELD(proc_variables.theFftFilt);
		}

		stat = JVX_STATE_ACTIVE;
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::number_channels(jvxSize* num)
{
	if (num)
	{
		*num = 0;
	}
	
	if (stat == JVX_STATE_ACTIVE)
	{
		if (num)
		{
			*num = numChannelsIn;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxAudioFixedFftN1Filter::current_fir_hrtf(jvxSize idChannel, std::string& fir_eq_str)
{
	std::string strRet = "[]";
	if (stat == JVX_STATE_ACTIVE)
	{
		if (idChannel < numChannelsIn)
		{
			jvxData* fldEq = proc_data.fir_in[idChannel];
			strRet = jvx_valueList2String(fldEq, JVX_DATAFORMAT_DATA, firSize, JVX_DATA_2STRING_CONST_FORMAT_G);
			fir_eq_str = strRet;
			return JVX_NO_ERROR;
		}
		
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxAudioFixedFftN1Filter::access_fir_hrtf(jvxSize idChannel, jvxData** ptr_fir, jvxSize* len_fir)
{
	if (stat == JVX_STATE_ACTIVE)
	{
		if (idChannel < numChannelsIn)
		{
			if(ptr_fir)
				*ptr_fir = proc_data.fir_in[idChannel];
			if (len_fir)
				*len_fir = firSize;
			return JVX_NO_ERROR;
		}

		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}