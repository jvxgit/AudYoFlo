/*
 * CjvxM4Device_irq.cpp
 *
 *  Created on: 13.06.2017
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
//#define JVX_DBG_IRQ_DMA

// ===============================================================
// ===============================================================

// Variables for DMA transfer
uint8_t dmaChannelNum_I2S_Tx, dmaChannelNum_I2S_Rx;
 DMA_TransferDescriptor dma_in[2] = {{0}, {0}};
 DMA_TransferDescriptor dma_out[2] = {{0}, {0}};
 int activeBufferId = 0;
 int16_t inBuf[JVX_INTERLEAVED_BUFFERSIZE_IN * 2] __attribute__ ((aligned (4))) = {0};
 int16_t outBuf[JVX_INTERLEAVED_BUFFERSIZE_OUT * 2] __attribute__ ((aligned (4))) = {0};
	// ==================================================
#ifdef JVX_IO_TEMP_BUFFER
 int16_t theCrossBuf[JVX_INTERLEAVED_BUFFERSIZE_IN] __attribute__ ((aligned (4))) = {0};
#endif

// Global variables to create cross references
int32_t deltaTimer = 0;
int32_t delta_bSwitch = 0;
int32_t time_last = -1;
CjvxM4Device* this_ref = NULL;
jvxLinkDataDescriptor myLinkDescriptor_to;
jvxLinkDataDescriptor myLinkDescriptor_from;

//#define JVX_DBG_IRQ_DMA
#define JVX_DBG_CNT_MAX 2600
#define JVX_DBG_BUF_PREFIX  __DATA(RAM2)
#define JVX_DBG_NUMBER_USER_VALUES 4
#include "jvx_debug_timing.h"

// =============================================================

extern "C"
{

// Place this function into the RAM area
__RAMFUNC(RAM) void DMA_IRQHandler(void)
{
	int16_t* ptrFrom = NULL;
	int16_t* ptrTo = NULL;
	static uint32_t myTimerStart_rate = 0;
	uint32_t tmp;
#ifdef JVX_DBG_IRQ_DMA
	uint32_t myTimerStart, myTimerStop;
	jvx_one_dbg_entry oneEvent;
#endif
	uint32_t myTimerStartLoc, myTimerStopLoc;

	uint32_t val_intstat = LPC_GPDMA->INTTCSTAT;
	uint32_t val_errstat = LPC_GPDMA->INTERRSTAT;

#ifdef JVX_DBG_IRQ_DMA
	myTimerStart = LPC_RITIMER->COUNTER;
#endif

	// ==================================================
	// Buffer index management
	// ==================================================
	uint16_t bufferIdIn = 0;
	uint16_t bufferIdOut = 0;
	uint32_t hlp1 = LPC_GPDMA->CH[dmaChannelNum_I2S_Rx].LLI;
	if(hlp1 == (uint32_t)&dma_in[1])
	{
		bufferIdIn = 1;
	}
	hlp1 = LPC_GPDMA->CH[dmaChannelNum_I2S_Tx].LLI;
	if(hlp1 == (uint32_t)&dma_out[1])
	{
		bufferIdOut = 1;
	}

#ifdef JVX_DBG_IRQ_DMA
	oneEvent.valint = val_intstat;
	oneEvent.timeri = myTimerStart;
	oneEvent.user_val[0] = LPC_GPDMA->CH[dmaChannelNum_I2S_Rx].DESTADDR - dma_in[0].dst;
	oneEvent.user_val[1] = LPC_GPDMA->CH[dmaChannelNum_I2S_Tx].SRCADDR - dma_out[0].src;

#endif

	if(val_intstat & (1 << dmaChannelNum_I2S_Rx))
	{
		tmp = LPC_RITIMER->COUNTER;
		delta_bSwitch = (int32_t)tmp - (int32_t)myTimerStart_rate;
		if(delta_bSwitch < 0)
		{
			delta_bSwitch += JVX_CPU_CLOCK_I;
		}
		myTimerStart_rate = tmp;

		// Audio input buffer arrived
#ifdef JVX_IO_TEMP_BUFFER
		ptrFrom = &inBuf[bufferIdIn * JVX_INTERLEAVED_BUFFERSIZE_IN];
		ptrTo = theCrossBuf;

		// Copy to temp buffer
		memcpy(ptrTo, ptrFrom, sizeof(uint16_t)*JVX_INTERLEAVED_BUFFERSIZE_IN);
#endif
	}

	if(val_intstat & (1 << dmaChannelNum_I2S_Tx))
	{


		// ==================================================
		// Main processing area
		// ==================================================
		myTimerStartLoc = LPC_RITIMER->COUNTER;

#ifdef JVX_IO_TEMP_BUFFER

		// Buffer id is fixed zero in case of temp buffer mode
		bufferIdIn = 0;
#endif

		// Jump back to class
		this_ref->process(bufferIdIn, bufferIdOut);

		myTimerStopLoc = LPC_RITIMER->COUNTER;

		// ================================
		// Main processing area stop
		// ================================

		// Measure the temporal length of the processing
		deltaTimer = (int32_t)myTimerStopLoc - (int32_t)myTimerStartLoc;
		if(deltaTimer < 0)
		{
			deltaTimer += 204000000;
		}
	}

	/*
	if(time_last >= 0)
	{
		deltatout = myTimerStart - time_last;
		if(deltatout < 0)
		{
			deltatout += 204000000;
		}
	}
	*/

#ifdef JVX_DBG_IRQ_DMA
	myTimerStop = LPC_RITIMER->COUNTER;
	oneEvent.timero = myTimerStop;
	oneEvent.user_val[2] = LPC_GPDMA->CH[dmaChannelNum_I2S_Rx].DESTADDR - dma_in[0].dst;
	oneEvent.user_val[3] = LPC_GPDMA->CH[dmaChannelNum_I2S_Tx].SRCADDR - dma_out[0].src;

	int resD = jvx_add_one_dbg_event(&oneEvent);
	if(resD == 1)
	{
		char myTxt = 'I';
		Board_UARTPutChar(myTxt);
	}
	time_last = myTimerStart;
#endif

	LPC_GPDMA->INTTCCLEAR = val_intstat;
	LPC_GPDMA->INTERRCLR = val_errstat;
}
} // extern "C"

// ===============================================================
// ===============================================================

uint16_t* jvx_report_debug = NULL;

extern "C" {
void RIT_IRQHandler(void)
{
	if(jvx_report_debug)
		*jvx_report_debug = 1;
	Chip_RIT_ClearInt(LPC_RITIMER);
}
} // extern "C"





