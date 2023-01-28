#ifndef __CJVXRICHAUDIONODE_H__
#define __CJVXRICHAUDIONODE_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "jvx_spectrumEstimation/jvx_spectrumEstimation.h"
#include "jvx_beampattern/jvx_beampattern.h"
#include "pcg_CjvxRichAudioNode.h"

class CjvxRichAudioNode : public CjvxAudioNode
{
 protected:

    IjvxStateMachine* sMachine = nullptr;

	JVX_CALLINGCONVENTION CjvxRichAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxRichAudioNode();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

        virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData,
                                                              jvxSize idx_sender_to_receiver,
                                                              jvxSize idx_receiver_to_sender)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, 
								     const char* filename,
								     jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
        const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
        const jvx::propertyAddress::IjvxPropertyAddress& ident,
        const jvx::propertyDetail::CjvxTranferDetail& trans)override;

        virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
			jvxCallManagerProperties& callGate,
            const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
			const jvx::propertyAddress::IjvxPropertyAddress& ident,
			const jvx::propertyDetail::CjvxTranferDetail& trans)override;
        /////////////////////////////////////////////
        // instrumentation and measurement methods //
        /////////////////////////////////////////////

        ////
        // spectrum analyzer
        ////

        jvxErrorType JVX_CALLINGCONVENTION addSpectrumAnalyzer(int bufferszie,
                                                                int nChannels,
                                                                int samplerate);

        jvxErrorType JVX_CALLINGCONVENTION processSpectrumAnalyzer(jvxSize channel,
                                                                   jvxData *input,
                                                                   jvxData **spectrum = NULL);

        ////
        // beampattern simulator
        ////

        // FIXME/BG: only planar simulation supported so far... elevation should be kept at 0 for now
	jvxErrorType JVX_CALLINGCONVENTION addBeampatternSimulator(jvxHandle *micArrayHdl,
                                                                    jvx_beampattern_process_cb micArrayCb,
                                                                    int samplerate,
                                                                    jvxData speedOfSound,
                                                                    jvxBool removePropagationToArray,
                                                                    jvxData radiusSimulatedCircle);

        /*
         * Coordinate system:
         *
         *
         *                         | +z (90°/undefined)
         *                         |
         *                         |     (0°/0°)
         *                         |    / +y
         *                         |   /
         *                         |  /
         *                         | /
         *                         | ---------------- +x (0°/270°)
         */
        jvxErrorType JVX_CALLINGCONVENTION beampatternSimulator_addMicrophone(jvxData x,
                                                                              jvxData y,
                                                                              jvxData z,
                                                                              jvxSize *id = NULL);

        jvxErrorType JVX_CALLINGCONVENTION updateInstrumentation(jvxBool syncUpdate);

 private:

        struct {
          jvxBool active;
          jvx_spectrumEstimation hdl;
        } spectrumAnalyzer;

        struct {
          jvxBool active;
          jvx_beampattern hdl;
        } beampatternSimulator;

        CjvxRichAudioNodeProps props;

        void associate_spectrumAnalyzerData();
        void associate_beampatternSimulatorData();
        void deassociate_spectrumAnalyzerData();
        void deassociate_beampatternSimulatorData();

        JVX_MUTEX_HANDLE beampatternProcessMutex;

        // this is private because it is automatically called on demand by get_property
        jvxErrorType processBeampatternSimulator(jvxData **beampattern = NULL);
};

#endif
