/* ************************************************** */
/* *                                                * */
/* *                      VOCODER                   * */
/* *                (c) IND 2002 David Bauer        * */
/* *                                                * */
/* ************************************************** */

#define EPS 0.00000000001
#undef min
#undef max

#include <cmath>
#include <iostream>

#include "jvxVocoder/openLoopPitchEstimation.h"

#include "jvxVocoder/vocoder.h"
#include "jvxVocoder/speech2Lsf.h"
#include "jvxVocoder/signalUtils.h"
#include "jvxVocoder/lsflpcUtils.h"
#include "jvxVocoder/lpcref.h"
#include "jvxVocoder/param.h"
#include "jvxVocoder/miscSignalUtils.h"


#define FILTER_2CAN

#ifdef FILTER_2CAN
#define LP_ANALYSE_FILTER firFilter_2can
#else
#define LP_ANALYSE_FILTER firFilter_1can
#endif

#ifdef FILTER_2CAN
#define LP_SYNTHESE_FILTER iirFilter_2can_bw
#else
#define LP_SYNTHESE_FILTER iirFilter_1can_bw
#endif

//======================================
vocoder::vocoder()
{
	memset(stat.lpc, 0, sizeof(jvxData)*(LPC_ORDER_MAX+1));
    memset(stat.lsf, 0, sizeof(jvxData)*(LPC_ORDER_MAX));
	memset(stat.ref, 0, sizeof(jvxData)*(LPC_ORDER_MAX+1));
	memset(stat.filter_states_ana, 0,  sizeof(jvxData)*(LPC_ORDER_MAX-1));
	memset(stat.filter_states_syn, 0,  sizeof(jvxData)*(LPC_ORDER_MAX-1));
    memset(stat.diameter, 0,  sizeof(jvxData)*(LPC_ORDER_MAX));
	memset(stat.excitation1, 0, sizeof(jvxData) * FRAME_SIZE);
	memset(stat.excitation2, 0, sizeof(jvxData) * FRAME_SIZE);
	memset(stat.analysisBuffer, 0, sizeof(jvxData) * FRAME_SIZE_ANALYSIS);

	stat.pulse_tail=0;
    stat.currentPitchFrequency = 200;
	stat.currentPitchFrequencyMin = 10;
	stat.currentPitchFrequencyMax = 500;
	stat.currentMixingCrit = 0;
	stat.orderLpcUsed = LPC_ORDER_MAX;
	stat.gain = 1.0;
	stat.pitchScale=1.0;
	stat.workBuffer = stat.analysisBuffer + (FRAME_SIZE_ANALYSIS - FRAME_SIZE - LOOKAHEAD);
	stat.formantWarp = 0;
	stat.old_gain = 1.0;

	autoPitch = vocoder::FUNCTIONALITY_AUTO;
	autoVUV = vocoder::FUNCTIONALITY_AUTO;
	outputRoute = vocoder::ROUTING_OUT_5;
	inputRoute = vocoder::ROUTING_IN_1;
	hardDecisionVUV = false;

	/* Init Pitch Estimation */
	pitchEstimation=new openLoopPitchEstimation(PPERIOD_MAX,PPERIOD_MIN,FRAME_SIZE,0.9,2);
}

//==================================
vocoder::~vocoder()
{
	delete pitchEstimation;
}

void
vocoder::getCurrentPitchFrequencyMin(int& val)
{
	val = stat.currentPitchFrequencyMin;
}

void
vocoder::setCurrentPitchFrequencyMin(int val)
{
	stat.currentPitchFrequencyMin = val;
	stat.currentPitchFrequencyMin = JVX_MIN(stat.currentPitchFrequencyMin, stat.currentPitchFrequencyMax);
	stat.currentPitchFrequencyMin = JVX_MAX(10, stat.currentPitchFrequencyMin);

}

void
vocoder::getCurrentPitchFrequencyMax(int& val)
{
	val = stat.currentPitchFrequencyMax;
}

void
vocoder::setCurrentPitchFrequencyMax(int val)
{
	stat.currentPitchFrequencyMax = val;
	stat.currentPitchFrequencyMax = JVX_MAX(stat.currentPitchFrequencyMin, stat.currentPitchFrequencyMax);
}
//==================================
void
vocoder::setExcitationMixingCrit(jvxData exc)
{
	stat.currentMixingCrit = exc;
}

//==================================
void
vocoder::getExcitationMixingCrit(jvxData& exc)
{
	exc = stat.currentMixingCrit;
}

//==================================
void
vocoder::setCurrentPitchFrequency(int pitch)
{
	stat.currentPitchFrequency = pitch;
	stat.currentPitchFrequency = JVX_MIN(stat.currentPitchFrequency , stat.currentPitchFrequencyMax);
	stat.currentPitchFrequency = JVX_MAX(stat.currentPitchFrequency , stat.currentPitchFrequencyMin);

}

//==================================
void
vocoder::getCurrentPitchFrequency(int& pitch)
{
	pitch = stat.currentPitchFrequency;
}

void
vocoder::getTubeDiameters(jvxData* fldDiameters, int numVals)
{
	if(numVals == LPC_ORDER_MAX)
	{
		memcpy(fldDiameters, stat.diameter, sizeof(jvxData)*numVals);
	}
}

void
vocoder::setOutputRoute(routingOut out)
{
	this->outputRoute = out;
}

void
vocoder::getOutputRoute(routingOut& out)
{
	out = this->outputRoute;
}

void
vocoder::setInputRoute(routingIn in)
{
	this->inputRoute = in;
}

void
vocoder::getInputRoute(routingIn& in)
{
	in = this->inputRoute;
}

void
vocoder::setAutoModeVUV(operationMode mode)
{
	this->autoVUV = mode;
}

void
vocoder::getAutoModeVUV(operationMode& mode)
{
	mode = this->autoVUV;
}

void
vocoder::setAutoModePitch(operationMode mode)
{
	this->autoPitch = mode;
}

void
vocoder::getAutoModePitch(operationMode& mode)
{
	mode = this->autoPitch;
}

void
vocoder::setPitchScale(jvxData scaleFactor)
{
	stat.pitchScale = scaleFactor;
}

void
vocoder::getPitchScale(jvxData& scaleFactor)
{
	scaleFactor = stat.pitchScale;
}

void
vocoder::setFormantWarp(jvxData coeff)
{
	stat.formantWarp = coeff;
}

void
vocoder::getFormantWarp(jvxData& coeff)
{
	coeff = stat.formantWarp;
}


void
vocoder::process_frame(jvxData *speech_inB1, jvxData *speech_inB2, jvxData *speech_out)
{
	int i;
	jvxData res_energy;
	jvxData c2_energy;
	jvxData localCrit = 0;
	int localPitchPeriod = 0;
	int localPitchFrequency = 0;
	int lpcOrder = stat.orderLpcUsed;
	jvxData scalePitch = stat.pitchScale;
	jvxData energyIn = 0.0;
	jvxData energyOut = 0.0;
	jvxData newGain;

	jvxBool twoInputs = false;

	if(speech_inB2)
	{
		twoInputs = true;
	}

	for(i = 0; i < FRAME_SIZE; i++)
	{
		energyIn += speech_inB1[i] *speech_inB1[i];
	}

	memset(stat.lpc, 0, sizeof(jvxData)*(LPC_ORDER_MAX+1));
    memset(stat.lsf, 0, sizeof(jvxData)*(LPC_ORDER_MAX));
	memset(stat.ref, 0, sizeof(jvxData)*(LPC_ORDER_MAX+1));

	memmove(stat.analysisBuffer, stat.analysisBuffer + FRAME_SIZE, (FRAME_SIZE_ANALYSIS-FRAME_SIZE)*sizeof(jvxData));
	memcpy(stat.analysisBuffer + (FRAME_SIZE_ANALYSIS-FRAME_SIZE), speech_inB1, FRAME_SIZE*sizeof(jvxData));

	/* First, get LSFs of current frame */
	speech2lsp_functions(stat.analysisBuffer,stat.lsf, FRAME_SIZE_ANALYSIS, lpcOrder-1, FRAME_SIZE_ANALYSIS - FRAME_SIZE/3, hammingWindow, correlationWindow);

	warp_Lsfs(stat.lsf, lpcOrder-1, stat.formantWarp);

    /* Convert LSFs to filter coefficients */
	convertLsf2Lpc(stat.lsf,stat.lpc, lpcOrder-1);

	/* Estimate Pitch Period, if necessary */
    //pitchEstimation->setInput(speech_in1);
	pitchEstimation->setInput(stat.workBuffer);
	localPitchPeriod = pitchEstimation->determineOpenLoopPitch(localCrit);
	localPitchFrequency = (int)((jvxData)SAMPLING_RATE/(jvxData)localPitchPeriod);

	if(localCrit < 0)
	{
		localCrit = 0;
	}
	if(localCrit > 1.0)
	{
		localCrit = 1.0;
	}

	if(this->autoPitch == FUNCTIONALITY_AUTO)
	{
		this->stat.currentPitchFrequency = localPitchFrequency;
		this->stat.currentPitchFrequency = (int)((jvxData)scalePitch * this->stat.currentPitchFrequency + 0.5);
	}


	if(this->autoVUV == FUNCTIONALITY_AUTO)
	{
		this->stat.currentMixingCrit = localCrit;
		if(this->hardDecisionVUV == true)
		{
			if(this->stat.currentMixingCrit > VUV_DECISION_BOUND)
			{
				this->stat.currentMixingCrit = 1.0;
			}
			else
			{
				this->stat.currentMixingCrit = 0.0;
			}
		}
	}

	/* Get Residual */
	// Inv_filt2(stat.lpc, LPC_ORDER, speech_in, stat.residual, FRAME_SIZE, stat.filter_states_ana);
	LP_ANALYSE_FILTER(stat.filter_states_ana, stat.lpc, lpcOrder, stat.workBuffer, stat.residual, FRAME_SIZE);


	 /* Get Residual Energy */
	res_energy=0;
	c2_energy = 0.0;

	if(twoInputs)
	{
		for(i=0;i < FRAME_SIZE;i++)
		{
			res_energy += stat.residual[i]*stat.residual[i];
			c2_energy += speech_inB2[i]*speech_inB2[i];
		}
	}
	else
	{
		for(i=0;i < FRAME_SIZE;i++)
		{
			res_energy += stat.residual[i]*stat.residual[i];
		}
	}

	this->stat.currentPitchFrequency = JVX_MAX(this->stat.currentPitchFrequency, this->stat.currentPitchFrequencyMin);
	this->stat.currentPitchFrequency = JVX_MIN(this->stat.currentPitchFrequency, this->stat.currentPitchFrequencyMax);

    /* Get excitation */
	vocoder::get_excitation(res_energy, this->stat.currentMixingCrit, this->stat.currentPitchFrequency);

	if(twoInputs)
	{
		for(i=0;i < FRAME_SIZE;i++)
		{
			stat.excitation2[i] = speech_inB2[i] * sqrt(res_energy/c2_energy);
		}
	}
	/*  Synthesis filtering */
	//Iir_filter_df2(stat.lpc,num,LPC_ORDER, stat.excitation,
	//	speech_out, FRAME_SIZE, stat.filter_states_syn);
	switch(this->inputRoute)
	{
	case ROUTING_IN_1:
		LP_SYNTHESE_FILTER(stat.filter_states_syn, stat.lpc, lpcOrder, stat.excitation1, stat.outBuffer, FRAME_SIZE);
		break;
	case ROUTING_IN_2:
		LP_SYNTHESE_FILTER(stat.filter_states_syn, stat.lpc, lpcOrder, stat.excitation2, stat.outBuffer, FRAME_SIZE);
		break;
	}

	energyOut = 0.0;
	for(i = 0; i < FRAME_SIZE; i++)
	{
		energyOut += stat.outBuffer[i] * stat.outBuffer[i];
	}
	newGain = sqrt(energyIn / (energyOut + EPS));

	for(i = 0; i < FRAME_SIZE; i++)
	{
		stat.outBuffer[i] *= (stat.old_gain * (1- ((jvxData)i/(jvxData)FRAME_SIZE)) + newGain * (((jvxData)i/(jvxData)FRAME_SIZE)));
	}
	stat.old_gain = newGain;

	switch(this->outputRoute)
	{
	case ROUTING_OUT_1:
		memcpy(speech_out, stat.workBuffer, sizeof(jvxData)*FRAME_SIZE);
		break;
	case ROUTING_OUT_2:
		memcpy(speech_out, stat.residual, sizeof(jvxData)*FRAME_SIZE);
		break;
	case ROUTING_OUT_3:
		if(speech_inB2)
		{
			memcpy(speech_out, speech_inB2, sizeof(jvxData)*FRAME_SIZE);
		}
		else
		{
			memset(speech_out, 0, sizeof(jvxData)*FRAME_SIZE);
		}
		break;
	case ROUTING_OUT_4:
		switch(this->inputRoute)
		{
		case ROUTING_IN_1:
			memcpy(speech_out, stat.excitation1, sizeof(jvxData)*FRAME_SIZE);
			break;
		case ROUTING_IN_2:
			memcpy(speech_out, stat.excitation2, sizeof(jvxData)*FRAME_SIZE);
			break;
		}
		break;
	case ROUTING_OUT_5:
		memcpy(speech_out, stat.outBuffer, sizeof(jvxData)*FRAME_SIZE);
		break;
	}
	// Prepare picture
	/* LPC to REF coeffs conversion */
	lpc2Ref(stat.lpc, lpcOrder, stat.ref);

	/* Now get Tube-diameters from reflection coefficients */
	compute_TubeDiameters(lpcOrder);

}

void vocoder::compute_TubeDiameters(int lpcOrder)
{
	int i;

	memset(stat.A, 0, sizeof(jvxData)*LPC_ORDER_MAX);

    stat.A[LPC_ORDER_MAX-1] = M_PI/4;  /* d_n=1 */

	for(i = lpcOrder - 2; i >= 0; i--)
	{
		stat.A[i] = stat.A[i+1]*(jvxData)(1.0+stat.ref[i+1])/(1.0-stat.ref[i+1]);
	}

    /* Get diameters from Cross section area */
    for(i = LPC_ORDER_MAX - 1; i >= 0;i--)
	{
		stat.diameter[i] = sqrt(4*stat.A[i]/M_PI);
	}
}

void vocoder::get_excitation(jvxData res_energy, jvxData crit, int pitchFreq)
{
	int i;
	jvxData noise_energy = 0;
	jvxData pulse_energy = 0;
	int pitchPeriod = SAMPLING_RATE/pitchFreq;
	int cntPulses = stat.pulse_tail;
	/* pulse exciation contribution*/


	/* Get random position in noise */
	for(i = 0; i < FRAME_SIZE; i++)
	{
		stat.excitation_noise[i] = ((rand()/(jvxData)RAND_MAX)-0.5);
		noise_energy += stat.excitation_noise[i]*stat.excitation_noise[i];
		stat.excitation_pulses[i] = 0.0;
		if(cntPulses >= pitchPeriod)
		{
			stat.excitation_pulses[i] = 1.0;
			cntPulses = 0;
		}
		else
		{
			stat.excitation_pulses[i] = 0.0;
			cntPulses++;
		}
		pulse_energy += stat.excitation_pulses[i]*stat.excitation_pulses[i];
	}

	jvxData facNoise = 1/(sqrt(noise_energy)+EPS);
	jvxData facPulses = 1/(sqrt(pulse_energy)+EPS);
	jvxData mix_energy = 0.0;

	for(i = 0; i < FRAME_SIZE; i++)
	{
		stat.excitation1[i] = facNoise * stat.excitation_noise[i] * (1-crit) + facPulses * stat.excitation_pulses[i] * crit;
		mix_energy += stat.excitation1[i]*stat.excitation1[i];
	}

	jvxData facMix = sqrt(res_energy)/(sqrt(mix_energy)+EPS);

	// Use an excitation threshold
	if(facMix < THRESHOLD_FAC)
		facMix = 0.0;

	for(i = 0; i < FRAME_SIZE; i++)
	{
		stat.excitation1[i] *= facMix;
	}

	stat.pulse_tail = cntPulses;
}

void
vocoder::setUsedLpcOrder(int orderLpcUsed)
{
	int tmp = orderLpcUsed;
	tmp = ((tmp - 1) / 2) * 2 + 1;
	stat.orderLpcUsed = tmp;
}

void
vocoder::getUsedLpcOrder(int& orderLpcUsed)
{
	orderLpcUsed = stat.orderLpcUsed;
}

void
vocoder::setVolume(jvxData val)
{
	stat.gain = val;
}

void
vocoder::getVolume(jvxData& val)
{
	val = stat.gain;
}
