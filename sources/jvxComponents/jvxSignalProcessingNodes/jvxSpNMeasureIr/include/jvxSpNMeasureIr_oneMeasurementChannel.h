#ifndef __ONEMEASUREMENTCHANNEL__H__
#define __ONEMEASUREMENTCHANNEL__H__

#include "jvx.h"

#define JVX_FLAG_IR_VALID 0x1
#define JVX_FLAG_MEAS_VALID 0x2
#define JVX_FLAG_TEST_VALID 0x4

class oneMeasurementChannel
{
public:
	jvxApiString nmChan;
	jvxApiString nmMeas;
	jvxApiString fileLocate;
	jvxData* bufIr;
	jvxData* bufMeas;
	jvxData* bufTest;
	jvxSize lBuf;
	jvxCBitField validFlds;
	jvxSize rate;

	oneMeasurementChannel()
	{
		bufIr = NULL;
		bufMeas = NULL;
		bufTest = NULL;
		lBuf = 0;
		validFlds = 0;
		rate = 0;
	};
};

#endif