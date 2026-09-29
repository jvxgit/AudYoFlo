#ifndef __CJVXDIGITALFILTER_H__
#define __CJVXDIGITALFILTER_H__

#include "jvx.h"
#include "jvx_circbuffer/jvx_circbuffer.h"

class CjvxDigitalFilter
{
	jvxState state = JVX_STATE_NONE;
	std::vector<std::vector<jvxData> > filterCoefficientsSos;
	jvxData filterCoefficientsG = 1.0;	
	std::string errMessage;
	jvxSize N = 0;
	jvxSize M = 0;
	jvxSize M0 = 0;
	jvxSize bsizeProcessing = 0;

	jvx_circbuffer* hdlFilt;
	jvxData* coeffsFwd = nullptr;
	jvxData* coeffsBwd = nullptr;
	jvxData* gains = nullptr;

public: 

	jvxErrorType initialize(const std::string& tokenTech, const std::string& tokenDescr, jvxSize order, jvxSize fs, jvxData fc, jvxData rippleDb, jvxData stopDb, jvxData freqWidth, jvxBool withG);
	jvxErrorType terminate();

	jvxErrorType prepare(jvxSize nChans, jvxSize bsize);
	jvxErrorType postprocess();

	jvxErrorType process(const jvxData** in, jvxData** out, jvxSize nChans, jvxSize bsize);
	jvxErrorType process_ip(jvxData** inout, jvxSize nChans, jvxSize bsize);

	// ================================================================================================
	// ================================================================================================

	std::vector<std::vector<jvxData> > coefficients_sos() { return filterCoefficientsSos; };
	jvxData gain() { return filterCoefficientsG; };
};
	
#endif
