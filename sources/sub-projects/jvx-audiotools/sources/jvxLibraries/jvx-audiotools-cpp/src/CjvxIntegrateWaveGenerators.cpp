#include "CjvxIntegrateWaveGenerators.h"
#include "pcg_CjvxIntegrateWaveGenerators.h"

#define JVX_GENWAVE_NAME_ONE_GENERATOR_PREFIX "JVX_ONE_GENERATOR_"
#define JVX_GENWAVE_NAME_ALL_GENERATORS "JVX_ALL_GENERATORS"

jvxErrorType jvx_factory_allocate_jvxIntegrateWaveGenerators(IjvxIntegrateWaveGenerators** returnedObj)
{
	CjvxIntegrateWaveGenerators* retObj = NULL;
	JVX_SAFE_NEW_OBJ(retObj, CjvxIntegrateWaveGenerators);
	*returnedObj = static_cast<IjvxIntegrateWaveGenerators*>(retObj);
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_factory_deallocate_jvxIntegrateWaveGenerators(IjvxIntegrateWaveGenerators* returnedObj)
{
	JVX_SAFE_DELETE_OBJ(returnedObj);
	return(JVX_NO_ERROR);
}

// =================================================================================

CjvxIntegrateWaveGenerators::CjvxIntegrateWaveGenerators()
{
	params.rate = 48000;
	params.bsize= 0;
	params.numchannels = 0;
	theState = JVX_STATE_INIT;
	updateSyncPending = false;
	uniqueId = 0;
}

CjvxIntegrateWaveGenerators::~CjvxIntegrateWaveGenerators()
{
}

jvxErrorType
CjvxIntegrateWaveGenerators::activate(CjvxProperties* theProps, const char* prefix)
{
	this->propRef = theProps;
	this->propPrefix = prefix;
	IntegrateWaveGenerators::init__properties_active();
	IntegrateWaveGenerators::allocate__properties_active();
	IntegrateWaveGenerators::register__properties_active(theProps, prefix);

	IntegrateWaveGenerators::init__properties_active_higher_all_generators();
	IntegrateWaveGenerators::allocate__properties_active_higher_all_generators();
	IntegrateWaveGenerators::register__properties_active_higher_all_generators(theProps, prefix);

	IntegrateWaveGenerators::init__properties_active_higher_one_generator();
	IntegrateWaveGenerators::allocate__properties_active_higher_one_generator();
	IntegrateWaveGenerators::register__properties_active_higher_one_generator(theProps, prefix);

	myGenerators.selectionId = JVX_SIZE_UNSELECTED;
	theState = JVX_STATE_ACTIVE;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxIntegrateWaveGenerators::deactivate()
{

	params.rate = 48000;
	params.bsize= 0;
	params.numchannels = 0;
	theState = JVX_STATE_INIT;
	updateSyncPending = false;

	IntegrateWaveGenerators::unregister__properties_active(propRef);
	IntegrateWaveGenerators::deallocate__properties_active();
	IntegrateWaveGenerators::unregister__properties_active_higher_one_generator(propRef);
	IntegrateWaveGenerators::deallocate__properties_active_higher_one_generator();
	IntegrateWaveGenerators::unregister__properties_active_higher_all_generators(propRef);
	IntegrateWaveGenerators::deallocate__properties_active_higher_all_generators();

	propRef = NULL;
	propPrefix = "";
	myGenerators.selectionId = JVX_SIZE_UNSELECTED;

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxIntegrateWaveGenerators::set_property_local(
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	jvxSize propId, jvxPropertyCategoryType category,
	jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
	jvxBool* updateUi)
{
	std::string command;
	std::string f_expr;
	std::string descr;
	std::vector<std::string> args;
	jvxBool takeOverParamsFromExtern = true;

	jvxErrorType res;

	if(JVX_PROPERTY_CHECK_ID_CAT(propId, category, IntegrateWaveGenerators::properties_active.specifyCommand))
	{
		// Interpret command!
		command = IntegrateWaveGenerators::properties_active.specifyCommand.value;

		res = jvx_parseCommandIntoToken(command, f_expr, args);
		if(res == JVX_NO_ERROR)
		{
			takeOverParamsFromExtern = false;
			if(f_expr == "addGenerator")
			{
				oneGenerator newgenerator;
				if(args.size() == 1)
				{
					descr = args[0];
				}

				if(descr.empty())
				{
					descr = "Generator Wave #" + jvx_size2String(uniqueId);
				}
				uniqueId ++;
				newgenerator.theGenerator.tpWave = JVX_GENERATOR_WAVE_SINE_UNBUFFERED;
				jvx_generatorwave_initConfig(&newgenerator.theGenerator);
				jvx_generatorwave_activate(&newgenerator.theGenerator);
				newgenerator.description = descr;
				newgenerator.outChannelMask = 0;
				myGenerators.lstGenerators.push_back(newgenerator);
				myGenerators.selectionId = myGenerators.lstGenerators.size()-1;
				if(updateUi)
				{
					*updateUi = true;
				}
			}
			if(f_expr == "removeGenerator")
			{
				if(args.size() == 1)
				{
					descr = args[0];
				}
				else
				{
					if(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId))
					{
						descr = myGenerators.lstGenerators[myGenerators.selectionId].description;
					}
				}
				std::vector<oneGenerator>::iterator elm = myGenerators.lstGenerators.begin();
				for(; elm != myGenerators.lstGenerators.end(); elm++)
				{
					if(descr == elm->description)
					{
						break;
					}
				}

				if(elm != myGenerators.lstGenerators.end())
				{
					//jvx_generatorwave_deactivate(&elm->theGenerator);
					myGenerators.lstGenerators.erase(elm);
				}
				if(myGenerators.selectionId >= myGenerators.lstGenerators.size())
				{
					myGenerators.selectionId = myGenerators.lstGenerators.size()-1;
				}
				if(updateUi)
				{
					*updateUi = true;
				}
			}
			if(f_expr == "removeAll")
			{
				std::vector<oneGenerator>::iterator elm = myGenerators.lstGenerators.begin();
				for(; elm != myGenerators.lstGenerators.end(); elm++)
				{
					//jvx_generatorwave_deactivate(&elm->theGenerator);
				}
				myGenerators.lstGenerators.clear();
				if(myGenerators.selectionId >= myGenerators.lstGenerators.size())
				{
					myGenerators.selectionId = myGenerators.lstGenerators.size()-1;
				}
				if(updateUi)
				{
					*updateUi = true;
				}
			}
			IntegrateWaveGenerators::properties_active.specifyCommand.value = "";

		}
	}

	if(JVX_PROPERTY_CHECK_ID_CAT(propId, category,IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator))
	{
		myGenerators.selectionId = jvx_bitfieldSelection2Id(IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.selection(),
			IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.entries.size()) - 1;
		takeOverParamsFromExtern = false;
		if(updateUi)
		{
			*updateUi = true;
		}
	}

	res = takeOverParametersAndUpdateExposure(takeOverParamsFromExtern, updateUi, propId, category);

	return(res);
}

jvxErrorType
CjvxIntegrateWaveGenerators::takeOverParametersAndUpdateExposure(jvxBool takeOverParamsFromExtern, jvxBool* updateUi, jvxSize propId, jvxPropertyCategoryType category)
{
	jvxBool updateAsync = false;
	jvxBool triggerUpdateSync = false;

	jvxErrorType res = JVX_NO_ERROR;
	if(takeOverParamsFromExtern)
	{
		res = updateParams_toInside(updateUi);
	}

	res = classifyPropertySetType(updateAsync, triggerUpdateSync, propId, category);
	if(res == JVX_NO_ERROR)
	{
		if(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId))
		{
			if(theState >= JVX_STATE_ACTIVE)
			{
				if(updateAsync)
				{
					jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_ASYNC, true);
					jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_ASYNC, false);
				}

				if(theState >= JVX_STATE_PREPARED)
				{
					if(triggerUpdateSync)
					{
						this->updateSyncPending = true;
						lstPendingUpdates.push_back(JVX_SIZE_INT(myGenerators.selectionId));
					}
				}
				else
				{
					jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_ASYNC, true);
					jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_ASYNC, false);
				}
			}
		}

		res = updateParams_toOutside(updateUi);
	}
	return(res);
}

jvxErrorType
CjvxIntegrateWaveGenerators::setParameters(jvxInt32 rate_param, jvxSize bSize_param, jvxInt32 numchans_param, jvxDataFormat format, jvxBool* updateUi )
{
	params.rate = rate_param;
	params.bsize = bSize_param;
	params.numchannels = numchans_param;
	params.format = format;

	jvxErrorType res = takeOverParametersAndUpdateExposure(false, updateUi, -1, JVX_PROPERTY_CATEGORY_UNKNOWN);
	return res;
}

jvxErrorType
CjvxIntegrateWaveGenerators::startAll()
{
	jvxSize i;
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

	IntegrateWaveGenerators::properties_active_higher_one_generator.iterationCount_activeGenerator.value = 0;
	if(theState == JVX_STATE_ACTIVE)
	{
		for(i = 0; i < myGenerators.lstGenerators.size(); i++)
		{
			res = jvx_generatorwave_activate(&myGenerators.lstGenerators[i].theGenerator);
			assert(res == JVX_DSP_NO_ERROR);
			res = jvx_generatorwave_prepare(&myGenerators.lstGenerators[i].theGenerator);
			myGenerators.lstGenerators[i].oneBuf = NULL;
			JVX_SAFE_NEW_FLD(myGenerators.lstGenerators[i].oneBuf, jvxData, params.bsize);
			memset(myGenerators.lstGenerators[i].oneBuf, 0, sizeof(jvxData) * params.bsize);
			assert(res == JVX_DSP_NO_ERROR);
			res = jvx_generatorwave_update(&myGenerators.lstGenerators[i].theGenerator, JVX_DSP_UPDATE_SYNC, false);
			assert(res == JVX_DSP_NO_ERROR);
			res = jvx_generatorwave_update(&myGenerators.lstGenerators[i].theGenerator, JVX_DSP_UPDATE_ASYNC, false);
			assert(res == JVX_DSP_NO_ERROR);
		}
		updateParams_toOutside(NULL);
		theState = JVX_STATE_PROCESSING;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxIntegrateWaveGenerators::process(jvxHandle** fldsFill, jvxSize& numFilled, jvxBool& allStopped)
{
	jvxSize i,j,k;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(theState == JVX_STATE_PROCESSING)
	{
		for(i = 0; i < (jvxSize)params.numchannels; i++)
		{
			memset(fldsFill[i], 0, sizeof(jvxData)*params.bsize);
		}

		for(i = 0; i < myGenerators.lstGenerators.size(); i++)
		{
			res = jvx_generatorwave_process(&myGenerators.lstGenerators[i].theGenerator, myGenerators.lstGenerators[i].oneBuf, params.bsize, &numFilled, nullptr);
			assert(res == JVX_DSP_NO_ERROR);

			// Get the state to indicate all stopped
			jvx_generatorwave_update(&myGenerators.lstGenerators[i].theGenerator, JVX_DSP_UPDATE_USER_OFFSET, false);

			if(myGenerators.lstGenerators[i].theGenerator.prm_async.sineWave.periodic.common.theStatus != JVX_GENERATOR_WAVE_STARTED)
			{
				allStopped = true;
			}
			if(i == myGenerators.selectionId)
			{
				if(myGenerators.lstGenerators[i].theGenerator.tpWave == JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED)
				{
					IntegrateWaveGenerators::properties_active_higher_one_generator.progress_activeGenerator.value =
						myGenerators.lstGenerators[i].theGenerator.prm_sync.perfSweepWave.buffered.progress;
					IntegrateWaveGenerators::properties_active_higher_one_generator.iterationCount_activeGenerator.value =
						JVX_SIZE_INT32(myGenerators.lstGenerators[i].theGenerator.prm_sync.perfSweepWave.itCount);
				}
				else if(myGenerators.lstGenerators[i].theGenerator.tpWave == JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED)
				{
					IntegrateWaveGenerators::properties_active_higher_one_generator.progress_activeGenerator.value =
						myGenerators.lstGenerators[i].theGenerator.prm_sync.wavWave.buffered.progress;
					IntegrateWaveGenerators::properties_active_higher_one_generator.iterationCount_activeGenerator.value =
						JVX_SIZE_INT32(myGenerators.lstGenerators[i].theGenerator.prm_sync.wavWave.itCount);
				}
				else if(
					(myGenerators.lstGenerators[i].theGenerator.tpWave == JVX_GENERATOR_WAVE_LINEARSWEEP) ||
					(myGenerators.lstGenerators[i].theGenerator.tpWave == JVX_GENERATOR_WAVE_LOGSWEEP))
				{
					IntegrateWaveGenerators::properties_active_higher_one_generator.progress_activeGenerator.value =
						myGenerators.lstGenerators[i].theGenerator.prm_sync.linlogSweepWave.unbuffered.progress;
					IntegrateWaveGenerators::properties_active_higher_one_generator.iterationCount_activeGenerator.value =
						JVX_SIZE_INT32(myGenerators.lstGenerators[i].theGenerator.prm_sync.linlogSweepWave.itCount);
				}
				else
				{
					IntegrateWaveGenerators::properties_active_higher_one_generator.progress_activeGenerator.value = 0.0;
					IntegrateWaveGenerators::properties_active_higher_one_generator.iterationCount_activeGenerator.value = 0;
				}
			}

			switch(params.format)
			{
			case JVX_DATAFORMAT_DATA:

				for(j = 0; j < (jvxSize)params.numchannels; j++)
				{
					if(jvx_bitTest(myGenerators.lstGenerators[i].outChannelMask, j))
					{
						jvxData* in = myGenerators.lstGenerators[i].oneBuf;
						jvxData* out = (jvxData*)fldsFill[j];
						for(k = 0; k < params.bsize; k++)
						{
							*out += *in;
							out++;
							in++;
						}
					}
				}
				break;
			default:
				std::cout << __FUNCTION__ << ":" << __LINE__ << ": Data formats other than jvxData not yet implemented" << std::endl;
				assert(0);
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxIntegrateWaveGenerators::stopAll()
{
	jvxSize i;

	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	if(theState == JVX_STATE_PROCESSING)
	{
		for(i = 0; i < myGenerators.lstGenerators.size(); i++)
		{

			res = jvx_generatorwave_postprocess(&myGenerators.lstGenerators[i].theGenerator);
			assert(res == JVX_DSP_NO_ERROR);

			res = jvx_generatorwave_deactivate(&myGenerators.lstGenerators[i].theGenerator);
			assert(res == JVX_DSP_NO_ERROR);
		}
		theState = JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}
// ==============================================================

jvxErrorType
CjvxIntegrateWaveGenerators::updateParams_toInside(jvxBool* updateUi)
{
	jvxSize tt = JVX_SIZE_UNSELECTED;
	jvx_generatorWave* selPtr = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	if(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId))
	{
		selPtr = &myGenerators.lstGenerators[myGenerators.selectionId].theGenerator;
		assert(selPtr);
	}

	if(selPtr)
	{
		jvxGeneratorWaveType tp = JVX_GENERATOR_WAVE_NONE;
		tt = jvx_bitfieldSelection2Id(IntegrateWaveGenerators::properties_active_higher_one_generator.type_activeGenerator.value.selection(),
			IntegrateWaveGenerators::properties_active_higher_one_generator.type_activeGenerator.value.entries.size());
		if(JVX_CHECK_SIZE_SELECTED(tt))
		{
			tp = (jvxGeneratorWaveType)(tt+1);
		}
		else
		{
			assert(0);
		}

		if(selPtr->tpWave != tp)
		{
			//jvx_generatorwave_deactivate(selPtr);
			selPtr->tpWave = tp;
			jvx_generatorwave_initConfig(selPtr);
			//jvx_generatorwave_activate(selPtr);
			//updateParams_toOutside(theProps, NULL);
			if(updateUi)
				*updateUi = true;
		}

		assert(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId));

		myGenerators.lstGenerators[myGenerators.selectionId].outChannelMask = IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.selection();

		myGenerators.lstGenerators[myGenerators.selectionId].description = IntegrateWaveGenerators::properties_active_higher_one_generator.description_activeGenerator.value;

		switch(selPtr->tpWave)
		{
		case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:

			selPtr->prm_init.sineWave.periodic.common.samplerate = params.rate;
			selPtr->prm_async.sineWave.periodic.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			selPtr->prm_async.sineWave.periodic.frequency = IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value;
			break;

		case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:

			selPtr->prm_init.rectWave.periodic.common.samplerate = params.rate;
			selPtr->prm_async.rectWave.periodic.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			selPtr->prm_async.rectWave.periodic.frequency = IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value;
			selPtr->prm_async.rectWave.pulsewidth = IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.value;
			break;

		case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:

			selPtr->prm_init.rampWave.periodic.common.samplerate = params.rate;
			selPtr->prm_async.rampWave.periodic.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			selPtr->prm_async.rampWave.periodic.frequency = IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value;
			selPtr->prm_async.rampWave.pulsewidth1 = IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.value;
			selPtr->prm_async.rampWave.pulsewidth2 = IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth2_activeGenerator.value;
			break;
		case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
			selPtr->prm_init.noiseWave.common.samplerate = params.rate;
			selPtr->prm_init.noiseWave.seed = IntegrateWaveGenerators::properties_active_higher_one_generator.seed_activeGenerator.value;
			selPtr->prm_async.noiseWave.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			selPtr->prm_async.noiseWave.noiseType = (jvxGeneratorWaveNoiseType)jvx_bitfieldSelection2Id(
				IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.value.selection(),
				IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.value.entries.size());
			break;

		case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:
			selPtr->prm_init.perfSweepWave.common.samplerate = params.rate;
			selPtr->prm_async.perfSweepWave.lengthSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.value;
			selPtr->prm_async.perfSweepWave.silenceStartSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.value;
			selPtr->prm_async.perfSweepWave.silenceStopSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.value;
			selPtr->prm_async.perfSweepWave.loopCount = IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value;
			selPtr->prm_async.perfSweepWave.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			break;

		case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:
			selPtr->prm_init.wavWave.common.samplerate = params.rate;
			selPtr->prm_async.wavWave.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			selPtr->prm_async.wavWave.loopCount = IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value;
			break;
		case JVX_GENERATOR_WAVE_LOGSWEEP:
		case JVX_GENERATOR_WAVE_LINEARSWEEP:
			selPtr->prm_init.linlogSweepWave.common.samplerate = params.rate;
			selPtr->prm_async.linlogSweepWave.lengthSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.value;
			selPtr->prm_async.linlogSweepWave.silenceStartSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.value;
			selPtr->prm_async.linlogSweepWave.silenceStopSeconds = IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.value;
			selPtr->prm_async.linlogSweepWave.loopCount = IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value;
			selPtr->prm_async.linlogSweepWave.common.amplitude = IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value;
			break;
		default:
			assert(0);
		}
	}
	return(res);
}

jvxErrorType
CjvxIntegrateWaveGenerators::updateParams_toOutside(jvxBool* updateUi)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_generatorWave* selPtr = NULL;
	jvxSize i;

	if(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId))
	{
		selPtr = &myGenerators.lstGenerators[myGenerators.selectionId].theGenerator;
		assert(selPtr);
	}

	propRef->_lock_properties_local();

	if(IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.entries.size() != myGenerators.lstGenerators.size()+1)
	{
		IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.entries.clear();
		IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.entries.push_back("None");
		for(i = 0; i < myGenerators.lstGenerators.size(); i++)
		{
			IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.entries.push_back(myGenerators.lstGenerators[i].description);
		}
	}
	jvx_bitZSet(IntegrateWaveGenerators::properties_active_higher_all_generators.selectGenerator.value.selection(), (myGenerators.selectionId +1));

	IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth2_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.seed_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.type_activeGenerator.isValid = false;
	IntegrateWaveGenerators::properties_active_higher_one_generator.description_activeGenerator.isValid = false;

	if(selPtr)
	{

		if(IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.entries.size() != (jvxSize)params.numchannels)
		{
			IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.entries.clear();
			for(i = 0; i < (jvxSize)params.numchannels; i++)
			{
				IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.entries.push_back("Output #" + jvx_size2String(i));
			}
		}
		IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.selection() = myGenerators.lstGenerators[myGenerators.selectionId].outChannelMask;
		IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.selection() &= ((jvxBitField)1 << (params.numchannels))-1;

		// Update wave generator type
		IntegrateWaveGenerators::properties_active_higher_one_generator.type_activeGenerator.value.selection() = (jvxBitField)1 << ((int)selPtr->tpWave-1);
		IntegrateWaveGenerators::properties_active_higher_one_generator.type_activeGenerator.isValid = true;

		// Update wave generator description
		IntegrateWaveGenerators::properties_active_higher_one_generator.description_activeGenerator.value = myGenerators.lstGenerators[myGenerators.selectionId].description;
		IntegrateWaveGenerators::properties_active_higher_one_generator.description_activeGenerator.isValid = true;

		// Since it is always the second element in struct, we may address it via sine wave struct in ALL cases!!
		IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.value = selPtr->prm_async.sineWave.periodic.common.amplitude;
		IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.value = -1;
		IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.isValid = false;
		switch(selPtr->tpWave)
		{
		case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value = selPtr->prm_async.sineWave.periodic.frequency;

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.isValid = true;
			break;

		case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value = selPtr->prm_async.rectWave.periodic.frequency;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.value = selPtr->prm_async.rectWave.pulsewidth;

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.isValid = true;
			break;

		case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.value = selPtr->prm_async.rampWave.periodic.frequency;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.value = selPtr->prm_async.rampWave.pulsewidth1;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth2_activeGenerator.value = selPtr->prm_async.rampWave.pulsewidth2;

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth2_activeGenerator.isValid = true;
			break;
		case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.seed_activeGenerator.value = selPtr->prm_init.noiseWave.seed;
			if(selPtr->prm_async.noiseWave.noiseType == JVX_GENERATOR_NOISE_UNIFORM)
			{
				IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.value.selection() = 0x1;
			}
			else
			{
				IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.value.selection() = 0x2;
			}
			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.seed_activeGenerator.isValid = true;

			break;

		case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.value = selPtr->prm_async.perfSweepWave.lengthSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.value = selPtr->prm_async.perfSweepWave.silenceStartSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.value = selPtr->prm_async.perfSweepWave.silenceStopSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value = selPtr->prm_async.perfSweepWave.loopCount;
			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.value = JVX_SIZE_INT32(selPtr->prm_sync.perfSweepWave.buffered.length);

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.isValid = true;
			break;

		case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:

			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.value = JVX_SIZE_INT32(selPtr->prm_sync.wavWave.buffered.length);
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value = selPtr->prm_async.wavWave.loopCount;

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.isValid = true;

			break;
		case JVX_GENERATOR_WAVE_LOGSWEEP:
		case JVX_GENERATOR_WAVE_LINEARSWEEP:

			IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.value = selPtr->prm_async.linlogSweepWave.lengthSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.value = selPtr->prm_async.linlogSweepWave.silenceStartSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.value = selPtr->prm_async.linlogSweepWave.silenceStopSeconds;
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.value = selPtr->prm_async.linlogSweepWave.loopCount;
			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.value = JVX_SIZE_INT32(selPtr->prm_sync.linlogSweepWave.unbuffered.length);

			IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.isValid = true;
			IntegrateWaveGenerators::properties_active_higher_one_generator.simulationLength_activeGenerator.isValid = true;
			break;
		default:
			assert(0);
		}
	}
	propRef->_unlock_properties_local();
	return(res);
}

jvxErrorType
CjvxIntegrateWaveGenerators::classifyPropertySetType(jvxBool& setAsync, jvxBool& triggerSetSync, jvxSize propId, jvxPropertyCategoryType category)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx_generatorWave* selPtr = NULL;

	if(category == JVX_PROPERTY_CATEGORY_UNSPECIFIC)
	{
		if(JVX_CHECK_SIZE_SELECTED(myGenerators.selectionId))
		{
			selPtr = &myGenerators.lstGenerators[myGenerators.selectionId].theGenerator;
			assert(selPtr);
		}

		if(selPtr)
		{
			switch(selPtr->tpWave)
			{
			case JVX_GENERATOR_WAVE_SINE_UNBUFFERED:
				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}
				break;

			case JVX_GENERATOR_WAVE_RECT_UNBUFFERED:

				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}
				break;

			case JVX_GENERATOR_WAVE_RAMP_UNBUFFERED:

				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth1_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.pulseWidth2_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}
				break;
			case JVX_GENERATOR_WAVE_NOISE_UNBUFFERED:
				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.noiseType_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}

				break;

			case JVX_GENERATOR_WAVE_PERFECTSWEEP_BUFFERED:

				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStartSeconds_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.silenceStopSeconds_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}

				break;

			case JVX_GENERATOR_WAVE_WAVPLAYER_BUFFERED:

				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.lengthSeconds_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.loopCount_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}
				break;
			case JVX_GENERATOR_WAVE_LOGSWEEP:
			case JVX_GENERATOR_WAVE_LINEARSWEEP:

				if(
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.amplitude_activeGenerator.globalIdx)||
					(propId == IntegrateWaveGenerators::properties_active_higher_one_generator.frequency_activeGenerator.globalIdx))
				{
					setAsync = true;
				}
				break;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxIntegrateWaveGenerators::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, 
								     const char* filename, jvxInt32 lineno)
{
	jvxConfigData* theSection = NULL, *theSubSection = NULL;
	jvxSize num = 0;
	jvxSize i;
	std::vector<std::string> warns;

	processor->getReferenceSubsectionCurrentSection_name(sectionToContainAllSubsectionsForMe, &theSection, JVX_GENWAVE_NAME_ALL_GENERATORS);
	if(theSection)
	{
		// Delete all possibly previously configured wave generators
		std::vector<oneGenerator>::iterator elm = myGenerators.lstGenerators.begin();
		for(; elm != myGenerators.lstGenerators.end(); elm++)
		{
			//jvx_generatorwave_deactivate(&elm->theGenerator);
		}
		myGenerators.lstGenerators.clear();
		if(myGenerators.selectionId >= myGenerators.lstGenerators.size())
		{
			myGenerators.selectionId = myGenerators.lstGenerators.size()-1;
		}

		processor->getNumberEntriesCurrentSection(theSection, &num);
		for(i = 0; i < num; i++)
		{
			processor->getReferenceEntryCurrentSection_id(theSection, &theSubSection, i);
			if(theSubSection)
			{
				IntegrateWaveGenerators::put_configuration__properties_active_higher_one_generator(callConf, processor, theSubSection, &warns);

				oneGenerator newgenerator;
				uniqueId ++;
				newgenerator.theGenerator.tpWave = JVX_GENERATOR_WAVE_NONE;
				jvx_generatorwave_initConfig(&newgenerator.theGenerator);
				newgenerator.description = IntegrateWaveGenerators::properties_active_higher_one_generator.description_activeGenerator.value;
				newgenerator.outChannelMask = IntegrateWaveGenerators::properties_active_higher_one_generator.outputChannel_activeGenerator.value.selection();

				myGenerators.lstGenerators.push_back(newgenerator);
				myGenerators.selectionId = myGenerators.lstGenerators.size()-1;
				updateParams_toInside(NULL);

				// Take over remaining parameters
				IntegrateWaveGenerators::put_configuration__properties_active_higher_one_generator(callConf, processor, theSubSection, &warns);
				updateParams_toInside(NULL);

				// Pass to internal memory of generator
				jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_ASYNC, true);
				jvx_generatorwave_update(&myGenerators.lstGenerators[myGenerators.selectionId].theGenerator, JVX_DSP_UPDATE_SYNC, false);
			}
		}
	}

	updateParams_toOutside(NULL);
	IntegrateWaveGenerators::put_configuration__properties_active_higher_all_generators(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxIntegrateWaveGenerators::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	jvxConfigData* theSection = NULL, *theSubSection = NULL;
	jvxSize selectionOld = myGenerators.selectionId;

	IntegrateWaveGenerators::get_configuration__properties_active_higher_all_generators(callConf, processor, sectionWhereToAddAllSubsections);
	processor->createEmptySection(&theSection, JVX_GENWAVE_NAME_ALL_GENERATORS);

	for(i = 0; i < myGenerators.lstGenerators.size(); i++)
	{
		processor->createEmptySection(&theSubSection, (JVX_GENWAVE_NAME_ONE_GENERATOR_PREFIX + jvx_size2String(i)).c_str());
		myGenerators.selectionId = i;
		updateParams_toOutside(NULL);
		IntegrateWaveGenerators::get_configuration__properties_active_higher_one_generator(callConf, processor, theSubSection);
		processor->addSubsectionToSection(theSection, theSubSection);
		theSubSection = NULL;
	}
	processor->addSubsectionToSection(sectionWhereToAddAllSubsections, theSection);
	myGenerators.selectionId = selectionOld;
	updateParams_toOutside(NULL);
	return(JVX_NO_ERROR);
}
