#ifndef _VOC_H_
#define _VOC_H_

#include "jvx_dsp_base.h"
#include "jvx_platform.h"
#include "configVocoder.h"

class openLoopPitchEstimation;

class vocoder
{
public:

	// Operation mode, auto: analyse input signal; fixed: use fixed value
	typedef enum
	{
		FUNCTIONALITY_FIXED = 0,
		FUNCTIONALITY_AUTO = 1
	} operationMode;

	typedef enum
	{
		ROUTING_OUT_1 = 0,
		ROUTING_OUT_2 = 1,
		ROUTING_OUT_3 = 2,
		ROUTING_OUT_4 = 3,
		ROUTING_OUT_5 = 4
	} routingOut;

	typedef enum
	{
		ROUTING_IN_1 = 0,
		ROUTING_IN_2 = 1
	} routingIn;
private:

	operationMode autoPitch;
	operationMode autoVUV;
	routingOut outputRoute;
	routingIn inputRoute;
	bool hardDecisionVUV;

	// Hold some runtime parameters
	struct
	{
		jvxData gain;
		int orderLpcUsed;
		int currentPitchFrequency;
		int currentPitchFrequencyMin;
		int currentPitchFrequencyMax;
		jvxData currentMixingCrit;
		jvxData lpc[LPC_ORDER_MAX+1];
		jvxData lsf[LPC_ORDER_MAX];
		jvxData ref[LPC_ORDER_MAX+1];
		jvxData diameter[LPC_ORDER_MAX];
		jvxData residual[FRAME_SIZE];
		jvxData analysisBuffer[FRAME_SIZE_ANALYSIS];
		jvxData* workBuffer;
		jvxData outBuffer[FRAME_SIZE];
		jvxData excitation_noise[FRAME_SIZE];
		jvxData excitation_pulses[FRAME_SIZE];
		jvxData excitation1[FRAME_SIZE];
		jvxData excitation2[FRAME_SIZE];
		jvxData filter_states_ana[LPC_ORDER_MAX-1];
		jvxData filter_states_syn[LPC_ORDER_MAX-1];
		jvxData A[LPC_ORDER_MAX];
		int pulse_tail;
		jvxData pitchScale;
		jvxData formantWarp;
		jvxData old_gain;
	} stat;

	openLoopPitchEstimation *pitchEstimation;

public :
	vocoder();
	~vocoder();

	void process_frame(jvxData *speech_in1, jvxData *speech_in2, jvxData *speech_out);

	void getExcitationMixingCrit(jvxData& val);
	void setExcitationMixingCrit(jvxData val);
	void getCurrentPitchFrequency(int& val);
	void setCurrentPitchFrequency(int val);

	void setCurrentPitchFrequencyMin(int val);
	void getCurrentPitchFrequencyMin(int& val);

	void setCurrentPitchFrequencyMax(int val);
	void getCurrentPitchFrequencyMax(int& val);

	void setOutputRoute(routingOut out);
	void getOutputRoute(routingOut& out);

	void setInputRoute(routingIn out);
	void getInputRoute(routingIn& out);

	void setVUVHardDecisision(bool val){hardDecisionVUV = val;};
	void getVUVHardDecisision(bool& val){val = hardDecisionVUV;};

	void setAutoModeVUV(operationMode mode);
	void getAutoModeVUV(operationMode& mode);

	void setAutoModePitch(operationMode mode);
	void getAutoModePitch(operationMode& mode);

	void setUsedLpcOrder(int orderLpcUsed);
	void getUsedLpcOrder(int& orderLpcUsed);

	void setVolume(jvxData val);
	void getVolume(jvxData& val);

	void setPitchScale(jvxData scaleFactor);
	void getPitchScale(jvxData& scaleFactor);

	void setFormantWarp(jvxData coeff);
	void getFormantWarp(jvxData& coeff);

	void getTubeDiameters(jvxData* fldDiameters, int numVals);

private:

	void get_excitation(jvxData energy, jvxData crit, int pitch);
	void compute_TubeDiameters(int lpcOrder);


};


#endif
