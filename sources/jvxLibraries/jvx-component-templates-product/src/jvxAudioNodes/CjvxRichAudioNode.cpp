#include <iostream>
#include "jvxAudioNodes/CjvxRichAudioNode.h"

CjvxRichAudioNode::CjvxRichAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
     spectrumAnalyzer.active = false;
     beampatternSimulator.active = false;
     JVX_INITIALIZE_MUTEX(beampatternProcessMutex);
}
CjvxRichAudioNode::~CjvxRichAudioNode()
{
     JVX_TERMINATE_MUTEX(beampatternProcessMutex);
}

jvxErrorType
CjvxRichAudioNode::prepare()
{
     return CjvxAudioNode::prepare();
}

jvxErrorType
CjvxRichAudioNode::process_st(jvxLinkDataDescriptor* theData,
                              jvxSize idx_sender_to_receiver,
                              jvxSize idx_receiver_to_sender)
{
	jvxErrorType res = CjvxAudioNode::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
	if(res != JVX_NO_ERROR)
		return res;

	if(props.spectrumEstimation.params.syncNotify.value
           || props.beampatternSimulator.params.syncNotify.value)
		{
			updateInstrumentation(true);

			// this will call update_window in the GUI thread
			_report_property_has_changed(JVX_PROPERTY_CATEGORY_UNSPECIFIC, -1, true, 0, 1, JVX_COMPONENT_UNKNOWN);

                        props.spectrumEstimation.params.syncNotify.value = 0;
                        props.beampatternSimulator.params.syncNotify.value = 0;
		}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::postprocess()
{
     jvxErrorType res = CjvxAudioNode::postprocess();
     if(res != JVX_NO_ERROR)
          return res;

     if(spectrumAnalyzer.active)
          if(jvx_spectrumEstimation_terminate(&spectrumAnalyzer.hdl) != JVX_DSP_NO_ERROR)
               return JVX_ERROR_INTERNAL;

     if(beampatternSimulator.active)
          if(jvx_beampattern_terminate(&beampatternSimulator.hdl) != JVX_DSP_NO_ERROR)
               return JVX_ERROR_INTERNAL;

     return JVX_NO_ERROR;
}



jvxErrorType
CjvxRichAudioNode::activate()
{
     jvxErrorType res = CjvxAudioNode::activate();

     if(res != JVX_NO_ERROR)
          return res;

     // NOTE: configInit is always called no matter if the instrument is created or not

     // config init of instrumentation
     if(jvx_spectrumEstimation_configInit(&spectrumAnalyzer.hdl, 0) != JVX_DSP_NO_ERROR)
          return JVX_ERROR_INTERNAL;
     if(jvx_beampattern_configInit(&beampatternSimulator.hdl, 0) != JVX_DSP_NO_ERROR)
          return JVX_ERROR_INTERNAL;

     // spectrum analyzer parameter properties
     props.init__spectrumEstimation__params();
     props.allocate__spectrumEstimation__params();
     props.associate__spectrumEstimation__params(static_cast<CjvxProperties*>(this),
                                                 &spectrumAnalyzer.hdl.prmSync.method, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.logarithmic, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.alpha, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.fftLengthLog2, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.windowType, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.winArgA, 1,
                                                 &spectrumAnalyzer.hdl.prmSync.winArgB, 1,
                                                 &spectrumAnalyzer.hdl.readOnly.spectrumSize, 1,
                                                 &spectrumAnalyzer.hdl.samplingRate, 1,
                                                 &spectrumAnalyzer.hdl.nChannelsIn, 1);
     props.register__spectrumEstimation__params(static_cast<CjvxProperties*>(this));
     props.spectrumEstimation.params.enabled.value = 1;

     // spectrum analyzer data properties (associate later if used)
     props.init__spectrumEstimation__data();
     props.allocate__spectrumEstimation__data();
     props.register__spectrumEstimation__data(static_cast<CjvxProperties*>(this));

     // beampattern simulator parameter properties
     props.init__beampatternSimulator__params();
     props.allocate__beampatternSimulator__params();
     props.associate__beampatternSimulator__params(static_cast<CjvxProperties*>(this),
                                                   &beampatternSimulator.hdl.samplingRate, 1,
                                                   &beampatternSimulator.hdl.readOnly.spectrumSize, 1,
                                                   &beampatternSimulator.hdl.prmSync.numAzimuthAngles, 1,
                                                   &beampatternSimulator.hdl.prmSync.azimuthMin, 1,
                                                   &beampatternSimulator.hdl.prmSync.azimuthMax, 1,
                                                   &beampatternSimulator.hdl.prmSync.fftLengthLog2, 1,
                                                   &beampatternSimulator.hdl.prmSync.fracDelayFilterDelay, 1);
     props.register__beampatternSimulator__params(static_cast<CjvxProperties*>(this));
     props.beampatternSimulator.params.enabled.value = 1;

     // beampattern simulator data properties (associate later if used)
     props.init__beampatternSimulator__data();
     props.allocate__beampatternSimulator__data();
     props.register__beampatternSimulator__data(static_cast<CjvxProperties*>(this));

     return res;
}

jvxErrorType
CjvxRichAudioNode::deactivate()
{
     jvxErrorType res = CjvxAudioNode::deactivate();

     if(res != JVX_NO_ERROR)
          return res;

     // NOTE: always deinitialize instrument properties no matter if the instrument is created or not

     // spectrum analyzer parameter properties
     props.unregister__spectrumEstimation__params(static_cast<CjvxProperties*>(this));
     props.deassociate__spectrumEstimation__params(static_cast<CjvxProperties*>(this));
     props.deallocate__spectrumEstimation__params();

     // spectrum analyzer data properties
     props.unregister__spectrumEstimation__data(static_cast<CjvxProperties*>(this));
     deassociate_spectrumAnalyzerData();
     props.deallocate__spectrumEstimation__data();

     // beampattern simulator parameter properties
     props.unregister__beampatternSimulator__params(static_cast<CjvxProperties*>(this));
     props.deassociate__beampatternSimulator__params(static_cast<CjvxProperties*>(this));
     props.deallocate__beampatternSimulator__params();

     // beampattern simulator parameter properties
     props.unregister__beampatternSimulator__data(static_cast<CjvxProperties*>(this));
     deassociate_beampatternSimulatorData();
     props.deallocate__beampatternSimulator__data();

     return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
                                     jvxHandle* sectionToContainAllSubsectionsForMe, 
                                     const char* filename,
                                     jvxInt32 lineno)
{
     std::vector<std::string> warnings;
     jvxErrorType res = CjvxAudioNode::put_configuration(callConf, processor,
         sectionToContainAllSubsectionsForMe, filename, lineno);
     if (res == JVX_NO_ERROR)
     {
         if (_common_set_min.theState == JVX_STATE_ACTIVE)
         {
             props.put_configuration__spectrumEstimation__params(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
             props.put_configuration__beampatternSimulator__params(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
         }
     }
     return res;
}

jvxErrorType
CjvxRichAudioNode::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
                                     jvxHandle* sectionWhereToAddAllSubsections)
{
    jvxErrorType res = CjvxAudioNode::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
    if (res == JVX_NO_ERROR)
    {
        if (_common_set_min.theState >= JVX_STATE_ACTIVE)
        {
            props.get_configuration__spectrumEstimation__params(callConf, processor, sectionWhereToAddAllSubsections);
            props.get_configuration__beampatternSimulator__params(callConf, processor, sectionWhereToAddAllSubsections);
        }
    }
     return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::get_property(jvxCallManagerProperties& callGate,
    const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
    const jvx::propertyAddress::IjvxPropertyAddress& ident,
    const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = JVX_NO_ERROR;

    JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
	// special treatment for params which are not directly associated with the dsp lib or used only in this class
        if(beampatternSimulator.active)
             if(propId == props.beampatternSimulator.data.data.globalIdx)
                  {
                       res = processBeampatternSimulator();
                       if(res != JVX_NO_ERROR)
                            return res;
                  }

	res = CjvxAudioNode::get_property(callGate, rawPtr, ident, trans);
	return res;
}

jvxErrorType
CjvxRichAudioNode::set_property(jvxCallManagerProperties& callGate,
    const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
    const jvx::propertyAddress::IjvxPropertyAddress& ident,
    const jvx::propertyDetail::CjvxTranferDetail& trans)
{
     jvxErrorType res = CjvxAudioNode::set_property(callGate,
		 rawPtr, ident, trans);

     if(res != JVX_NO_ERROR)
          return res;

     if(_common_set_min.theState >= JVX_STATE_PREPARED)
          res = updateInstrumentation(false);

     return res;
}


jvxErrorType
CjvxRichAudioNode::addSpectrumAnalyzer(int buffersize,
                                       int nChannels,
                                       int samplerate)
{
     std::cout<<"[Instrumentation] Adding spectrum analyzer (n = "<<buffersize<<", nCh = "<<nChannels<<", fs = "<<samplerate<<")"<<std::endl;

     spectrumAnalyzer.active = true;

     if(jvx_spectrumEstimation_init(&spectrumAnalyzer.hdl,
                                    buffersize,
                                    nChannels,
                                    samplerate)
        != JVX_DSP_NO_ERROR)
          return JVX_ERROR_INTERNAL;
     associate_spectrumAnalyzerData();
     return(JVX_NO_ERROR);
}

jvxErrorType
CjvxRichAudioNode::processSpectrumAnalyzer(jvxSize channel,
                                           jvxData *input,
                                           jvxData **spectrum)
{
     if(props.spectrumEstimation.params.enabled.value)
          if(jvx_spectrumEstimation_process(&spectrumAnalyzer.hdl, input, spectrum, channel) != JVX_DSP_NO_ERROR)
               return JVX_ERROR_INTERNAL;
     return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::addBeampatternSimulator(jvxHandle *micArrayHdl,
                                            jvx_beampattern_process_cb micArrayCb,
                                            int samplerate,
                                            jvxData speedOfSound,
                                            jvxBool removePropagationToArray,
                                            jvxData radiusSimulatedCircle)
{
     jvxData *beampattern;

     std::cout<<"[Instrumentation] Adding beampattern simulator (fs = "<<samplerate<<
          ", c = "<<speedOfSound<<", removePropagationToArray = "<<removePropagationToArray<<
          ", radiusSimulatedCircle = "<<radiusSimulatedCircle<<")"<<std::endl;

     beampatternSimulator.active = true;

     // set some parameters before init call
     // FIXME/BG: should these be associated / made available in gui?
     beampatternSimulator.hdl.prmSync.speedOfSound = speedOfSound;
     beampatternSimulator.hdl.prmSync.removePropagationToArray = removePropagationToArray;
     beampatternSimulator.hdl.prmSync.radiusCircleAround = radiusSimulatedCircle;

     if(jvx_beampattern_init(&beampatternSimulator.hdl,
                             samplerate,
                             micArrayHdl,
                             micArrayCb)
        != JVX_DSP_NO_ERROR)
          return JVX_ERROR_INTERNAL;
     associate_beampatternSimulatorData();
     return(JVX_NO_ERROR);
}

jvxErrorType JVX_CALLINGCONVENTION
CjvxRichAudioNode::beampatternSimulator_addMicrophone(jvxData x,
                                                      jvxData y,
                                                      jvxData z,
                                                      jvxSize *id)
{
     if(jvx_beampattern_addMicrophone(&beampatternSimulator.hdl, x, y, z, id) != JVX_DSP_NO_ERROR)
          return JVX_ERROR_INTERNAL;
     return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::processBeampatternSimulator(jvxData **beampattern)
{
     jvxDspBaseErrorType res;

     if(props.beampatternSimulator.params.enabled.value)
          {
               // locking because of asynchronous computation of beampattern
               JVX_LOCK_MUTEX(beampatternProcessMutex);
               res = jvx_beampattern_process(&beampatternSimulator.hdl, beampattern);
               JVX_UNLOCK_MUTEX(beampatternProcessMutex);
               if(res != JVX_DSP_NO_ERROR)
                    return JVX_ERROR_INTERNAL;
          }
     return JVX_NO_ERROR;
}

jvxErrorType
CjvxRichAudioNode::updateInstrumentation(jvxBool syncUpdate)
{
     jvxDspBaseErrorType res;

     if(spectrumAnalyzer.active)
          {
               res = jvx_spectrumEstimation_update(&spectrumAnalyzer.hdl, syncUpdate);
               if(res != JVX_DSP_NO_ERROR)
                    return JVX_ERROR_INTERNAL;
               if(syncUpdate)
                    {
                         deassociate_spectrumAnalyzerData();
                         associate_spectrumAnalyzerData();
                    }
          }
     if(beampatternSimulator.active)
          {
               // locking because of asynchronous computation of beampattern
               JVX_LOCK_MUTEX(beampatternProcessMutex);
               res = jvx_beampattern_update(&beampatternSimulator.hdl, syncUpdate);
               if(syncUpdate)
                    {
                         deassociate_beampatternSimulatorData();
                         associate_beampatternSimulatorData();
                    }
               JVX_UNLOCK_MUTEX(beampatternProcessMutex);
               if(res != JVX_DSP_NO_ERROR)
                    return JVX_ERROR_INTERNAL;
          }
     return JVX_NO_ERROR;
}



////
// private methods
////

void
CjvxRichAudioNode::associate_spectrumAnalyzerData()
{
     props.associate__spectrumEstimation__data(static_cast<CjvxProperties*>(this),
                                               spectrumAnalyzer.hdl.readOnly.spectrum,
                                               spectrumAnalyzer.hdl.readOnly.spectrumSize
                                               * spectrumAnalyzer.hdl.nChannelsIn);
}

void
CjvxRichAudioNode::deassociate_spectrumAnalyzerData()
{
     props.deassociate__spectrumEstimation__data(static_cast<CjvxProperties*>(this));
}


void
CjvxRichAudioNode::associate_beampatternSimulatorData()
{
     props.associate__beampatternSimulator__data(static_cast<CjvxProperties*>(this),
                                                 beampatternSimulator.hdl.readOnly.spectrum,
                                                 beampatternSimulator.hdl.readOnly.spectrumSize
                                                 * beampatternSimulator.hdl.prmSync.numAzimuthAngles);
}

void
CjvxRichAudioNode::deassociate_beampatternSimulatorData()
{
     props.deassociate__beampatternSimulator__data(static_cast<CjvxProperties*>(this));
}
