/*
 * CjvxM4Device.h
 *
 *  Created on: 12.06.2017
 *      Author: hauke
 */

#include "jvx_small.h"

#ifndef CJVXM4DEVICE_H_
#define CJVXM4DEVICE_H_

#define NUM_CHAR_MSG 256
#define VERBOSE_OUT

#include "jvx_pdm/jvx_fir_lut.h"
#include "jvx_pdm/jvx_cic.h"

class CjvxM4Device
{
public:
private:
	uint16_t dbgActive;
	char bufOut[NUM_CHAR_MSG];
	float normLoad;
	jvxAudioFormat config_audio;
	float numCyclesRate;
	jvx_fir_lut myHdlFir;
	jvx_cic* myCicHdl;
	IjvxDataProcessor_core* myDataLinkObject;
public:
	CjvxM4Device();
	jvxErrorType activate();
	jvxErrorType prepare();
	jvxErrorType start();
	jvxErrorType stop();
	jvxErrorType postprocess();
	jvxErrorType deactivate();

	jvxErrorType update_main();
	void jvx_start_dbg(uint16_t* );
	void app_dmatest_start(void);
	void process(jvxSize bufferIdIn, jvxSize bufferIdOut);
};


#endif /* CJVXM4DEVICE_H_ */
