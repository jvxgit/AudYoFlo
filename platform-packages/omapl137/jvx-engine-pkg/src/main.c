/*
 * ==============================================
 * Filename: main.c
 * ==============================================
 * Purpose: Main function for current project. Note: In dspBios, the
 *			main function is part of the initialization routine. For example,
 *			no interrupt is enabled here.
 *			In this function, init the audio part first and then the UART and comm receiver part.
 * 			COPYRIGHT HK, Javox, 2011
 */
 
#include <std.h>
#include <log.h>
#include <c62.h>

#include "projectAcfg.h"
#include "dskda830.h"
#include "dskda830_edma3.h"
#include "dskda830_mcasp.h"
#include "dskda830_led.h"
#include "systemConfig.h"

#include "nrMotor_dsp.h"


// The following extern references can be found in "audio.c"

// Three main processing functions 

#include "audio.h"
#include "uart.h"

void report_assert(const char* textCont, jvxInt32 lineno)
{
	if(textCont)
	{
		LOG_printf(&trace, "Assert error report, %s <%i>", textCont, lineno);
	}
	else
	{
		LOG_printf(&trace, "Assert error report, <%i>", lineno);
	}
}


/**
 *
 * Main function for DSKDA830 talkthrough test environment 
 * Configurations is done via defines in file "configAudioBoardSW.h"
 *///===================================================================================
void main()
{

    LOG_printf(&trace, "========================================================");
    LOG_printf(&trace, "Audio I/O based on DMA Transfers");
    LOG_printf(&trace, "========================================================");

    LOG_printf(&trace, "========================================================");
    LOG_printf(&trace, "Step I: Init the audio hardware							");
    LOG_printf(&trace, "========================================================");

	/**
	 * IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
	 ******************************************************************
	 * NOTE: AUDIO must be initialized before all others since
	 *       in the init routine, the DMA hardware is reset. All
	 *       other modules using DMA transfer should not reuse
	 * 		 previously used DMA channels
	 *///=============================================================

	// Preset all buffers with zeros
	audio_initBuffers();

	// Prepare audio processing
	audio_prepareAudio();

	// Enable transmit to McAsp over interrupt	
	C62_enableIER(C62_EINT8);

	rtp_nrMotor_initialize(40, 2, 1); // <- at first only for debugging...

    LOG_printf(&trace, "========================================================");
    LOG_printf(&trace, "Step II: Init the UART hardware							");
    LOG_printf(&trace, "========================================================");

	// Initialize the UART hardware
	uart_initialize();

	// Initialize the memory for UART data transfers
	uart_initializeMemory();

	// Enable UART2 interrupt (DSP/BIOS)	
	C62_enableIER(C62_EINT10);

    LOG_printf(&trace, "========================================================");
    LOG_printf(&trace, "Step III: Initialize the LEDs							");
    LOG_printf(&trace, "========================================================");
	DSKDA830_LED_init( );
	DSKDA830_LED_on( 0 );

    return;
} 

