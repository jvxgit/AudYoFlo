#include "jvxvstp.h"
#include "CjvxVstPluginCommon.h"

#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "public.sdk/source/vst/vsthelpers.h"
#include "pluginterfaces/base/ibstream.h"
#include "base/source/fstreamer.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// loudness Implementation
//------------------------------------------------------------------------
jvxvstp::jvxvstp(): currentProcessMode (-1) // -1 means not initialized
{
	
}

//------------------------------------------------------------------------
jvxvstp::~jvxvstp()
{
	
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	// We need to set this reference first
	this->set_runtime_reference(static_cast<IjvxAudioPluginRuntime*>(this));
	
	// Then startup
	startup();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::terminate ()
{
	// Shutdown at first
	shutdown();

	// Then reset reference
	this->unset_runtime_reference();

	// nothing to do here yet...except calling our parent terminate
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::setActive (TBool state)
{
	if (state)
	{
		// Start and stop must be in lock to allow on the fly start / stop while processing is still active
		this->lock_processing();
		start_processing();
		this->unlock_processing();
	}
	else
	{
		// Start and stop must be in lock to allow on the fly start / stop while processing is still active
		this->lock_processing();
		stop_processing();
		this->unlock_processing();
	}
	
	// call our parent setActive
	return AudioEffect::setActive (state);
}

tresult PLUGIN_API jvxvstp::setProcessing(TBool state)
{
	if (state)
	{
		// Inform about start of processing
	}
	else
	{
		// Inform about stop of processing
	}

	return AudioEffect::setProcessing(state);
}
//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::process (ProcessData& data)
{
	jvxSize i;

	// finally the process function
	// In this example there are 4 steps:
	// 1) Read inputs parameters coming from host (in order to adapt our model values)
	// 2) Read inputs events coming from host (we apply a gain reduction depending of the velocity of pressed key)
	// 3) Process the gain of the input buffer to the output buffer
	// 4) Write the new VUmeter value to the output Parameters queue
	if(this->try_lock_processing() == JVX_NO_ERROR)
	{
		//-------------------------------------
		// ---0) Associate the data struct to be available in subclass
		//-------------------------------------
		this->associate_processing_data(&data);

		//-------------------------------------
		// ---1) Input parameter processing
		//-------------------------------------
		this->process_input_parameters();

		//-------------------------------------
		// ---2) Read input events-------------
		//-------------------------------------
		// Still to be considered
		IEventList* eventList = data.inputEvents;
		if (eventList)
		{
			int32 numEvent = eventList->getEventCount();
			for (int32 i = 0; i < numEvent; i++)
			{
				Event event;
				if (eventList->getEvent(i, event) == kResultOk)
				{

				}
			}
		}

		//-------------------------------------
		//---3) Process Audio---------------------
		//-------------------------------------
		if (data.numInputs == 0 || data.numOutputs == 0)
		{
			// nothing to do
			//goto output_params;
		}
		else
		{
			// (simplification) we suppose in this example that we have the same input channel count than
			// the output
			// int32 numChannels = data.inputs[0].numChannels;

			//---get audio buffers----------------
			// uint32 sampleFramesSize = Steinberg::Vst::getSampleFramesSizeInBytes(processSetup, data.numSamples);
			// void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
			// void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

			jvxLinkDataDescriptor* datIO = NULL;
			jvxErrorType resL = theMicroConnection->prepare_process_connection(&datIO);
			if (resL == JVX_NO_ERROR)
			{
				jvxSize i;
				jvxSize numChannelsInMin = JVX_MIN(data.inputs[0].numChannels, (int)datIO->con_params.number_channels);
				jvxData** bufsOutJvxRt = jvx_process_icon_extract_output_buffers<jvxData>(datIO);
				if (processSetup.symbolicSampleSize == kSample32)
				{
					float** bufsInVst = data.inputs[0].channelBuffers32;
					for (i = 0; i < numChannelsInMin; i++)
					{
						jvx_convertSamples_from_float_to_data<float>(bufsInVst[i], bufsOutJvxRt[i], descr_conn_out.con_params.buffersize);
					}
				}
				else
				{
					double** bufsInVst = data.inputs[0].channelBuffers64;
					for (i = 0; i < numChannelsInMin; i++)
					{
						jvx_convertSamples_from_float_to_data<double>(bufsInVst[i], bufsOutJvxRt[i], descr_conn_out.con_params.buffersize);
					}
				}

				resL = theMicroConnection->process_connection(&datIO);
				jvxData** bufsInJvxRt = jvx_process_icon_extract_output_buffers<jvxData>(datIO);
				jvxSize numChannelsOutMin = JVX_MIN(data.outputs[0].numChannels, (int)datIO->con_params.number_channels);
				if (processSetup.symbolicSampleSize == kSample32)
				{
					float** bufsOutVst = data.outputs[0].channelBuffers32;
					for (i = 0; i < numChannelsOutMin; i++)
					{
						jvx_convertSamples_from_data_to_float<float>(bufsInJvxRt[i], bufsOutVst[i], descr_conn_in.con_params.buffersize);
					}
				}
				else
				{
					double** bufsOutVst = data.inputs[0].channelBuffers64;
					for (i = 0; i < numChannelsOutMin; i++)
					{
						jvx_convertSamples_from_data_to_float<double>(bufsInJvxRt[i], bufsOutVst[i], descr_conn_in.con_params.buffersize);
					}
				}

				resL = theMicroConnection->postprocess_process_connection();
			}
		}
		// output_params:
		//-------------------------------------
		// --- 4) Request output parameters		
		//-------------------------------------
		this->process_output_parameters();

		//-------------------------------------
		// --- 5) Remove link to struct as processing is done
		//-------------------------------------
		this->deassociate_processing_data(&data);

		this->unlock_processing();
	}
	else
	{
		// If processing is not available, we output 0 values
		uint32 sampleFramesSize = Steinberg::Vst::getSampleFramesSizeInBytes(processSetup, data.numSamples);
		//void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
		void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);
		for (i = 0; i < (jvxSize)data.outputs[0].numChannels; i++)
		{
			memset(out[i], 0, sampleFramesSize);
		}
		processSetup, data.outputs[0].silenceFlags = 1;
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult jvxvstp::receiveText (const char* text)
{
	// received from Controller
	fprintf (stderr, "[jvxvstp] received: ");
	fprintf (stderr, "%s", text);
	fprintf (stderr, "\n");
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::setState (IBStream* state)
{
	jvxBool err = false;
	Steinberg::IBStreamer streamer(state, kLittleEndian);

	// called when we load a preset, the model has to be reloaded

	std::string txtIn = CjvxVstPluginCommon::readConfiguration(streamer, err);	
	if (!err)
	{
		this->putConfiguration(txtIn);
	}

	/*
	float savedGain = 0.f;
	if (streamer.readFloat (savedGain) == false)
		return kResultFalse;

	float savedGainReduction = 0.f;
	if (streamer.readFloat (savedGainReduction) == false)
		return kResultFalse;

	int32 savedBypass = 0;
	if (streamer.readInt32 (savedBypass) == false)
		return kResultFalse;

	fInGain = savedGain;
	fGainReduction = savedGainReduction;
	bBypass = savedBypass > 0;

	if (Helpers::isProjectState (state) == kResultTrue)
	{
		// we are in project loading context...

		// Example of using the IStreamAttributes interface
		FUnknownPtr<IStreamAttributes> stream (state);
		if (stream)
		{
			if (IAttributeList* list = stream->getAttributes ())
			{
				// get the full file path of this state
				TChar fullPath[1024];
				memset (fullPath, 0, 1024 * sizeof (TChar));
				if (list->getString (PresetAttributes::kFilePathStringType, fullPath,
				                     1024 * sizeof (TChar)) == kResultTrue)
				{
					// here we have the full path ...
				}
			}
		}
	}
	*/
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::getState (IBStream* state)
{
	std::string txtConfig;
	// here we need to save the model
	this->getConfiguration(txtConfig);

	IBStreamer streamer (state, kLittleEndian);

	CjvxVstPluginCommon::writeConfiguration(streamer, txtConfig);
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::setupProcessing (ProcessSetup& newSetup)
{
	// called before the process call, always in a disable state (not active)

	// here we keep a trace of the processing mode (offline,...) for example.
	currentProcessMode = newSetup.processMode;
	
	update_plugin_parameters(newSetup.maxSamplesPerBlock, newSetup.sampleRate);
	
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
                                              SpeakerArrangement* outputs, int32 numOuts)
{
	if (numIns == 1 && numOuts == 1)
	{
		// the host wants Mono => Mono (or 1 channel -> 1 channel)
		if (SpeakerArr::getChannelCount (inputs[0]) == 1 &&
		    SpeakerArr::getChannelCount (outputs[0]) == 1)
		{
			auto* bus = FCast<AudioBus> (audioInputs.at (0));
			if (bus)
			{
				// check if we are Mono => Mono, if not we need to recreate the busses
				if (bus->getArrangement () != inputs[0])
				{
					getAudioInput (0)->setArrangement (inputs[0]);
					getAudioInput (0)->setName (STR16 ("Mono In"));
					getAudioOutput (0)->setArrangement (inputs[0]);
					getAudioOutput (0)->setName (STR16 ("Mono Out"));
				}
				return kResultOk;
			}
		}
		// the host wants something else than Mono => Mono, 
		// in this case we are always Stereo => Stereo
		else
		{
			auto* bus = FCast<AudioBus> (audioInputs.at (0));
			if (bus)
			{
				tresult result = kResultFalse;

				// the host wants 2->2 (could be LsRs -> LsRs)
				if (SpeakerArr::getChannelCount (inputs[0]) == 2 &&
				    SpeakerArr::getChannelCount (outputs[0]) == 2)
				{
					getAudioInput (0)->setArrangement (inputs[0]);
					getAudioInput (0)->setName (STR16 ("Stereo In"));
					getAudioOutput (0)->setArrangement (outputs[0]);
					getAudioOutput (0)->setName (STR16 ("Stereo Out"));
					result = kResultTrue;
				}
				// the host want something different than 1->1 or 2->2 : in this case we want stereo
				else if (bus->getArrangement () != SpeakerArr::kStereo)
				{
					getAudioInput (0)->setArrangement (SpeakerArr::kStereo);
					getAudioInput (0)->setName (STR16 ("Stereo In"));
					getAudioOutput (0)->setArrangement (SpeakerArr::kStereo);
					getAudioOutput (0)->setName (STR16 ("Stereo Out"));
					result = kResultFalse;
				}

				return result;
			}
		}
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::canProcessSampleSize (int32 symbolicSampleSize)
{
	
	if (symbolicSampleSize == kSample32)
		return kResultTrue;

	// we support double processing
	if (symbolicSampleSize == kSample64)
		return kResultTrue;

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API jvxvstp::notify (IMessage* message)
{
	if (!message)
		return kInvalidArgument;

	const char* ptrC = message->getMessageID();
	if (!strcmp (ptrC, "BinaryMessage"))
	{
		const void* data;
		uint32 size;
		if (message->getAttributes ()->getBinary ("jvxrt-audioplugin", data, size) == kResultOk)
		{
			jvxAudioPluginBinaryParamHeader* hdr = (jvxAudioPluginBinaryParamHeader*)data;
			assert(size == hdr->hdr.sz);
			jvxErrorType res = handle_message_from_controller(static_cast<IjvxPluginMessageControl*>(this), (jvxAudioPluginBinaryHeader*) hdr);
			if(res == JVX_NO_ERROR)
				return kResultOk;
			return kResultFalse;
		}
	}

	return AudioEffect::notify (message);
}

jvxErrorType 
jvxvstp::plugin_send_message(jvxAudioPluginBinaryHeader* fld)
{
	if (IPtr<IMessage> message = owned(allocateMessage()))
	{
		message->setMessageID("BinaryMessage");
		message->getAttributes()->setBinary("jvxrt-audioplugin", fld, fld->sz);
		sendMessage(message);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INTERNAL;
}

// ================================================================
// ================================================================
//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
