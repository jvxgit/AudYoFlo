/*
 * CjvxM4Device.cpp
 *
 *  Created on: 12.06.2017
 *      Author: hauke
 */
#include "board.h"
#include "i2s_defines.h"
#include "jvx_audio_misc.h"
#include "uart_printf.h"
#include <string.h>

#include <math.h>
#include "jvx_small.h"
#include "CjvxM4Device.h"
#include "jvx_audio_config.h"

#include <cr_section_macros.h>

//#include "jvx_fdesign_I_octConfig.h"
#include "jvx_fdesign_II_octConfig.h"
#include "jvx-helpers.h"

//#define JVX_SUBOPTIMAL_LUT

extern int32_t deltaTimer;
extern float maxTimer_p1;
extern int32_t deltatin;
extern int32_t deltatout;
extern uint8_t dmaChannelNum_I2S_Tx;
extern uint8_t dmaChannelNum_I2S_Rx;
extern DMA_TransferDescriptor dma_in[2];
extern DMA_TransferDescriptor dma_out[2];
extern int16_t inBuf[JVX_INTERLEAVED_BUFFERSIZE_IN*2];
extern int16_t outBuf[JVX_INTERLEAVED_BUFFERSIZE_OUT*2];
extern uint16_t* jvx_report_debug;
extern int32_t delta;
extern CjvxM4Device* this_ref;
extern int32_t delta_bSwitch;
extern jvxLinkDataDescriptor myLinkDescriptor_to;
extern jvxLinkDataDescriptor myLinkDescriptor_from;
#ifdef JVX_IO_TEMP_BUFFER
extern int16_t theCrossBuf[JVX_INTERLEAVED_BUFFERSIZE_IN];
#endif
jvxUInt16 theCircBuffer[JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16] = { 0 };


#define ALPHA_RATE 0.95

extern "C"
{
	__attribute__ ((weak)) jvxErrorType JVX_CALLINGCONVENTION jvxAuNProcess_init(IjvxDataProcessor_core** retObject)
	{
		if(retObject)
		{
			*retObject = NULL;
		}
		return JVX_NO_ERROR;
	}

	__attribute__ ((weak)) jvxErrorType JVX_CALLINGCONVENTION jvxAuNProcess_terminate(IjvxDataProcessor_core* cls)
	{
		return JVX_NO_ERROR;
	}
}

// ====================================================================
// ====================================================================
CjvxM4Device::CjvxM4Device()
{
	jvxSize i;
	for(i = 0; i < NUM_CHAR_MSG; i++)
	{
		bufOut[i] = 0;
	}
	this_ref = this;

	config_audio.bsize_in = JVX_BUFFERSIZE_IN;
	config_audio.bsize_out = JVX_BUFFERSIZE_OUT;
	config_audio.srate_in = JVX_SAMPLERATE_IN;
	config_audio.srate_out = JVX_SAMPLERATE_OUT;
	config_audio.numchans_in = JVX_NUMBER_CHANNELS_IN;
	config_audio.numchans_out = JVX_NUMBER_CHANNELS_OUT;
	config_audio.form = JVX_DATAFORMAT_PROCESSING;
	normLoad = config_audio.srate_in / config_audio.bsize_in / (float)JVX_CPU_CLOCK_I;

	cic_decimation_filter_init(&myCicHdl, JVX_INTERLEAVED_BUFFERSIZE_IN);

	// Initialize PDM decoder

	jvx_pdm_fir_lut_initConfig(&myHdlFir);
	myHdlFir.prm_init.expSz = 3;
	jvx_pdm_fir_lut_prepare(&myHdlFir);

	myHdlFir.prm_sync.ptr_lookup = JVX_FIR_PDM_DEC_TABLES_d2_ll128_fac9_os16;

	myHdlFir.prm_sync.circ_buf = theCircBuffer;
	for (i = 0; i < JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16; i++)
	{
		myHdlFir.prm_sync.circ_buf[i] = 43690; // This value is a sequence of 1010101010...10 to start with a "zero" memory in PDM
	}
	myHdlFir.prm_sync.ptr_revAddr = JVX_FIR_PDM_DEC_BITREVERSE_d2_ll128_fac9_os16;
	assert((JVX_FIR_PDM_DEC_NUM_ENTRIES_d2_ll128_fac9_os16*JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16) ==
		myHdlFir.derived.lLookup16Bit);
	assert(JVX_FIR_PDM_DEC_NUM_TABLES_d2_ll128_fac9_os16 == myHdlFir.derived.lCirc16Bit);
	//JVX_SAFE_NEW_FLD(pdm.tmp16Fir, jvxInt16, _common_set_node_params_1io.processing.buffersize);
	//memset(pdm.tmp16Fir, 0, sizeof(jvxInt16) *_common_set_node_params_1io.processing.buffersize);

	myDataLinkObject = NULL;

}

// ====================================================================
// ====================================================================
jvxErrorType
CjvxM4Device::activate()
{
	SystemCoreClockUpdate();
	Board_Init();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxM4Device::prepare()
{
	jvxErrorType res = JVX_NO_ERROR;
	dbgActive = 0;

	// Initialize the board
	Board_Audio_Init_jvx(CODEC_I2S_BUS, CODEC_INPUT_DEVICE, &config_audio);

	// Start I2S
	Chip_I2S_Init(CODEC_I2S_BUS);
	Chip_I2S_RxOnOff_jvx(CODEC_I2S_BUS, &config_audio, 1);
	Chip_I2S_TxStop(CODEC_I2S_BUS);
	Chip_I2S_DisableMute(CODEC_I2S_BUS);

	jvx_start_dbg(&dbgActive);

	// Initialize the main processing object - taken from lib
	jvxAuNProcess_init(&myDataLinkObject);

	jvx_initDataLinkDescriptor(& myLinkDescriptor_to);
#ifdef JVX_AUDIO_LINE_IO
	myLinkDescriptor_to.con_params.buffersize = config_audio.bsize_in;
	myLinkDescriptor_to.con_params.rate = config_audio.srate_in;
	myLinkDescriptor_to.con_params.number_channels = 2;
#else
	myLinkDescriptor_to.con_params.buffersize = config_audio.bsize_in * 2;
	myLinkDescriptor_to.con_params.rate = config_audio.srate_in * 2;
	myLinkDescriptor_to.con_params.number_channels = 1;
#endif
	myLinkDescriptor_to.con_params.format = JVX_DATAFORMAT_16BIT_LE;
	myLinkDescriptor_to.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED;

#ifdef JVX_IO_TEMP_BUFFER
	myLinkDescriptor_to.con_data.number_buffers = 1;
#else
	myLinkDescriptor_to.con_data.number_buffers = 2;
#endif
	jvx_allocateDataLinkDescriptor(&myLinkDescriptor_to, false);
	jvx_bitZSet(myLinkDescriptor_to.con_data.bExt.flags, JVX_BUFFER_FLAGS_TAKE_FROM_CALLER_SHIFT);

#ifdef JVX_IO_TEMP_BUFFER
	myLinkDescriptor_to.con_data.buffers[0][0] =
			theCrossBuf;
	myLinkDescriptor_to.con_data.bExt.raw[0][0] = myLinkDescriptor_to.con_data.buffers[0][0];

#else
	myLinkDescriptor_to.con_data.buffers[0][0] =
			&inBuf[0];
	myLinkDescriptor_to.con_data.buffers[1][0] =
			inBuf + JVX_INTERLEAVED_BUFFERSIZE_IN;
	myLinkDescriptor_to.con_data.bExt.raw[0][0] =
			myLinkDescriptor_to.con_data.buffers[0][0];
	myLinkDescriptor_to.con_data.bExt.raw[1][0] =
			myLinkDescriptor_to.con_data.buffers[1][0];
#endif
	myLinkDescriptor_to.con_data.bExt.sz = JVX_INTERLEAVED_BUFFERSIZE_IN;

	// ==================================================================================

	if(	myDataLinkObject)
	{
		res = myDataLinkObject->prepare_sender_to_receiver(&myLinkDescriptor_to);
		assert(res == JVX_NO_ERROR);
	}

	jvx_initDataLinkDescriptor(& myLinkDescriptor_from);
	myLinkDescriptor_from.con_params.buffersize = config_audio.bsize_out;
	myLinkDescriptor_from.con_params.rate = config_audio.srate_out;
	myLinkDescriptor_from.con_params.format = JVX_DATAFORMAT_16BIT_LE;
	myLinkDescriptor_from.con_params.number_channels = 2;
	myLinkDescriptor_from.con_data.number_buffers = 2;
	myLinkDescriptor_from.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED;
	jvx_allocateDataLinkDescriptor(&myLinkDescriptor_from, false);

	myLinkDescriptor_from.con_data.buffers[0][0] =
			outBuf;
	myLinkDescriptor_from.con_data.buffers[1][0] =
			outBuf + JVX_INTERLEAVED_BUFFERSIZE_OUT;
	myLinkDescriptor_from.con_data.bExt.raw[0][0] =
			myLinkDescriptor_from.con_data.buffers[0][0];
	myLinkDescriptor_from.con_data.bExt.raw[1][0] =
			myLinkDescriptor_from.con_data.buffers[1][0];
	myLinkDescriptor_from.con_data.bExt.sz = JVX_INTERLEAVED_BUFFERSIZE_OUT;

	// Finally prepare for one-object-processing
	jvx_ccopyDataLinkDescriptor(&myLinkDescriptor_to, &myLinkDescriptor_from);
	myLinkDescriptor_to.con_compat.from_receiver_buffer_allocated_by_sender =
			myLinkDescriptor_from.con_data.buffers;
	myLinkDescriptor_to.con_compat.bExt = myLinkDescriptor_from.con_data.bExt;
	if(	myDataLinkObject)
	{
		res = myDataLinkObject->prepare_complete_receiver_to_sender(&myLinkDescriptor_to);
		assert(res == JVX_NO_ERROR);
	}

	return JVX_NO_ERROR;
}

// ====================================================================
// ====================================================================
jvxErrorType CjvxM4Device::start()
{
	uint16_t i;
	// Start I2S clock
	Chip_I2S_TxStart(CODEC_I2S_BUS);

	Chip_I2S_TxOnOff_jvx(CODEC_I2S_BUS, &config_audio, 0);
	Chip_I2S_RxOnOff_jvx(CODEC_I2S_BUS, &config_audio, 0);

	DEBUGOUT("I2S DMA mode\r\n");

	//uint8_t continue_Flag = 1, bufferUART = 0xFF;
	Chip_I2S_DMA_TxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_1, ENABLE, 4);
	Chip_I2S_DMA_RxCmd(CODEC_I2S_BUS, I2S_DMA_REQUEST_CHANNEL_2, ENABLE, 4);
	/* Initialize GPDMA controller */
	Chip_GPDMA_Init(LPC_GPDMA);
	/* Setting GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
	NVIC_EnableIRQ(DMA_IRQn);
	//pp = NVIC_GetPriority(DMA_IRQn);

	/*
	 dmaChannelNum_I2S_Rx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, I2S_DMA_RX_CHAN);

	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Rx,
					  I2S_DMA_RX_CHAN,
					  I2S_DMA_TX_CHAN,
					  GPDMA_TRANSFERTYPE_P2P_CONTROLLER_SrcPERIPHERAL,
					  1);
	 */

	dmaChannelNum_I2S_Rx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, I2S_DMA_RX_CHAN);
	dmaChannelNum_I2S_Tx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, I2S_DMA_TX_CHAN);

	dma_in[0].dst = (uint32_t)&inBuf[0];
	dma_in[1].dst = (uint32_t)&inBuf[JVX_INTERLEAVED_BUFFERSIZE_IN];
	dma_in[0].lli = (uint32_t)&dma_in[1];
	dma_in[1].lli = (uint32_t)&dma_in[0];

	dma_out[0].src = (uint32_t)&outBuf[0];
	dma_out[1].src = (uint32_t)&outBuf[JVX_INTERLEAVED_BUFFERSIZE_OUT];
	dma_out[0].lli = (uint32_t)&dma_out[1];
	dma_out[1].lli = (uint32_t)&dma_out[0];

	Chip_GPDMA_Transfer_jvx(LPC_GPDMA, dmaChannelNum_I2S_Rx,
			I2S_DMA_RX_CHAN,
			(uint32_t)inBuf,
			GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
			JVX_INTERLEAVED_BUFFERSIZE_IN/2, &dma_in[0], &dma_in[1]);

	Chip_GPDMA_Transfer_jvx(LPC_GPDMA, dmaChannelNum_I2S_Tx,
			(uint32_t)outBuf,
			I2S_DMA_TX_CHAN,
			GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
			JVX_INTERLEAVED_BUFFERSIZE_OUT/2, &dma_out[0], &dma_out[1]);

	// Reset timer
	LPC_RITIMER->COUNTER = 0;

	//activeBufferId = 0;
	// Fill data into output FIFO
	for(i = 0; i < 8; i++)
		CODEC_I2S_BUS->TXFIFO = 0;

	// Start DMA chaining - if possible at ONE moment in time
	Chip_GPDMA_SStart_jvx(LPC_GPDMA, dmaChannelNum_I2S_Rx, dmaChannelNum_I2S_Tx);

	Chip_I2S_TxRxOnOff_jvx(CODEC_I2S_BUS, &config_audio, 1);

	return JVX_NO_ERROR;
}

// ====================================================================
// ====================================================================
jvxErrorType CjvxM4Device::postprocess()
{
	return JVX_NO_ERROR;
}

// ====================================================================
// ====================================================================
//extern uint32_t deltaTimer;
//extern float maxTimer_p1;

jvxErrorType
CjvxM4Device::update_main()
{
	jvxSize i;
	if(dbgActive)
	{
		float rel = (float)deltaTimer *normLoad * 100.0;
		//numCyclesRate = numCyclesRate * ALPHA_RATE + (1-ALPHA_RATE) * ();
		float rate = (float)JVX_CPU_CLOCK_I / (float) delta_bSwitch * JVX_BUFFERSIZE_IN;
		sprintf(bufOut, "Load %f %% (%d cycs), rate: %f", rel, (int)deltaTimer, rate);
		for(i = 0; i < NUM_CHAR_MSG; i++)
		{
			Board_UARTPutChar(bufOut[i]);
			bufOut[i] = 0;
		}
		Board_UARTPutChar('\r');
		dbgActive = 0;
	}
	return JVX_NO_ERROR;
}

// ====================================================================
// ====================================================================

void
CjvxM4Device::jvx_start_dbg(uint16_t* reportDebug)
{
	uint32_t pp = 0;
	jvx_report_debug = reportDebug;

	// Enable counter
	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_Enable(LPC_RITIMER);
	Chip_RIT_TimerDebugEnable(LPC_RITIMER);
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, JVX_CPU_CLOCK_I); // Once per second
	Chip_RIT_EnableCTRL(LPC_RITIMER, RIT_CTRL_ENCLR);
	NVIC_DisableIRQ(RITIMER_IRQn);
	NVIC_SetPriority(RITIMER_IRQn, ((0x02 << 3) | 0x01));
	pp = NVIC_GetPriority(RITIMER_IRQn);
	NVIC_EnableIRQ(RITIMER_IRQn);

}
__RAMFUNC(RAM)
void
CjvxM4Device::process(jvxSize bufferIdIn, jvxSize bufferIdOut)
{
	jvxSize i;
	int16_t* ptrFrom = NULL;
	int16_t* ptrTo = NULL;
#ifdef JVX_AUDIO_LINE_IO
	int16_t accu = 0;
	int8_t myVal;
	int8_t* myPtr;
#endif

	// Audio output buffer arrived about to be output
#ifndef JVX_AUDIO_LINE_IO

	//cic_decimation_filter(myCicHdl,
	//		(jvxInt16*)myLinkDescriptor_to.con_data.buffers[bufferIdIn][0],
	//		(jvxInt16*)myLinkDescriptor_to.con_data.buffers[bufferIdIn][0]);
#ifdef JVX_SUBOPTIMAL_LUT
	 jvx_pdm_fir_lut_process_ip(&myHdlFir,
			//(jvxInt16*)theData->con_data.buffers[idx_sender_to_receiver][2],
			(jvxInt16*)myLinkDescriptor_to.con_data.buffers[bufferIdIn][0],
		config_audio.bsize_in * 2);
#else
	jvx_pdm_fir_lut_process_ip_lwords(&myHdlFir,
			(jvxInt16*)myLinkDescriptor_to.con_data.buffers[bufferIdIn][0],
		config_audio.bsize_in * 2);
#endif

#endif

	if(myDataLinkObject)
	{
		myDataLinkObject->process_st(&myLinkDescriptor_to, bufferIdIn, bufferIdOut);
	}
	else
	{
		ptrFrom = (int16_t*)myLinkDescriptor_to.con_data.buffers[bufferIdIn][0];
		ptrTo = (int16_t*)myLinkDescriptor_to.con_compat.from_receiver_buffer_allocated_by_sender[bufferIdOut][0];


#ifdef JVX_AUDIO_LINE_IO
		// Copy input to output, works only if input and utput run at the same rate
		for(i = 0; i < JVX_INTERLEAVED_BUFFERSIZE_IN; i+=2)
		{
			myPtr = (int8_t*)(ptrFrom+i);
			myVal = *myPtr;
			myPtr++;
			myVal = *myPtr;
			myPtr++;
			myVal = *myPtr;
			myPtr++;
			myVal = *myPtr;
			accu = ptrFrom[i];
			ptrTo[i] = accu;

		}
		for(i = 1; i < JVX_INTERLEAVED_BUFFERSIZE_IN; i+= 2)
		{
			accu = ptrFrom[i];
			accu = (int16_t)(1.0 * (float)accu);
			ptrTo[i] = accu;
		}
		//memcpy(ptrTo, ptrFrom, sizeof(uint16_t)*JVX_INTERLEAVED_BUFFERSIZE_IN);
#else
		for(i = 0; i < JVX_INTERLEAVED_BUFFERSIZE_IN; i++)
		{
				ptrTo[2*i] = ptrFrom[i];
				ptrTo[2*i+1] = ptrFrom[i];
		}
#endif
	}
#if 0
	int32_t i;
	//i = 16420;// <- max at 48 kHz in debug mode
	i = 12200;// <- max at 48 kHz in debug mode
	//i = 10;
	/*
	 jvx_pdm_fir_lut_process(&myHdlFir,
			//(jvxInt16*)theData->con_data.buffers[idx_sender_to_receiver][2],
			NULL,
			NULL,
			config_audio.bsize_in * 2);
	 */

	while(i >= 0)
	{
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");

		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");

		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");

		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");

		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");

		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		i--;
	}
#endif
}
