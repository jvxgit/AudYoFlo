#ifndef __JVXVSTP__H__
#define __JVXVSTP__H__

#include "public.sdk/source/vst/vstaudioeffect.h" 
#include "vstgui/lib/cvstguitimer.h"

// JVXRT STUFF
#include "CjvxPluginProcessor.h" 
#include "CjvxVstPluginExtender.h"

namespace Steinberg {
	namespace Vst {

		class CjvxVstProcessor :public CjvxPluginProcessor, public CjvxVstPluginExtender, public IjvxPluginMessageControl
		{
		public:

		};

		//------------------------------------------------------------------------
		// loudness: directly derived from the helper class AudioEffect
		//------------------------------------------------------------------------
		class jvxvstp : public AudioEffect, public CjvxVstProcessor
		{
		public:
			jvxvstp();
			virtual ~jvxvstp(); // do not forget virtual here

			//--- ---------------------------------------------------------------------
			// AudioEffect overrides:
			//--- ---------------------------------------------------------------------
			/** Called at first after constructor */
			tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

			/** Called at the end before destructor */
			tresult PLUGIN_API terminate() SMTG_OVERRIDE;

			/** Switch the plug-in on/off */
			tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;

			/** Start processing */
			tresult PLUGIN_API setProcessing(TBool state) SMTG_OVERRIDE;

			/** Here we go...the process call */
			tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;

			/** Test of a communication channel between controller and component */
			tresult receiveText(const char* text) SMTG_OVERRIDE;

			/** For persistence */
			tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
			tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;

			/** Will be called before any process call */
			tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) SMTG_OVERRIDE;

			/** Bus arrangement managing: in this example the 'loudness' will be mono for mono input/output and
			 * stereo for other arrangements. */
			tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
				SpeakerArrangement* outputs,
				int32 numOuts) SMTG_OVERRIDE;

			/** Asks if a given sample size is supported see \ref SymbolicSampleSizes. */
			tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;

			/** We want to receive message. */
			tresult PLUGIN_API notify(IMessage* message) SMTG_OVERRIDE;

			//------------------------------------------------------------------------

				/* ======================================================================= */
				/* Interface functions from jvxVst */
				/* =======================================================================*/
			virtual jvxErrorType plugin_send_message(jvxAudioPluginBinaryHeader* fld) override;

			virtual void startup() = 0;
			virtual void shutdown() = 0;
			virtual void start_processing() = 0;
			virtual void stop_processing() = 0;

		protected:

			// Current processing mode, realtime, offline..
			Steinberg::int32 currentProcessMode;
		};

		//------------------------------------------------------------------------
	} // namespace Vst
} // namespace Steinberg

#endif
