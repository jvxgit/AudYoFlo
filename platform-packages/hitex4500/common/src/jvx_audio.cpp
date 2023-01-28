/*
 * @brief I2S example
 * This example show how to use the I2S in 3 modes : Polling, Interrupt and DMA
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "i2s_defines.h"
#include "jvx_audio_misc.h"
#include "uart_printf.h"

#include "CjvxM4Device.h"

// config options
#define VERBOSE_OUT
//#define USE_RTC

/* Gets and shows the current time and date */
static void showTime()
{
#ifdef USE_RTC
	RTC_TIME_T FullTime;
	Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
	printf("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
			 FullTime.time[RTC_TIMETYPE_HOUR],
			 FullTime.time[RTC_TIMETYPE_MINUTE],
			 FullTime.time[RTC_TIMETYPE_SECOND],
			 FullTime.time[RTC_TIMETYPE_MONTH],
			 FullTime.time[RTC_TIMETYPE_DAYOFMONTH],
			 FullTime.time[RTC_TIMETYPE_YEAR]);
#endif
}
int main(void)
{
	CjvxM4Device myDevice;

	// Initialize the main class
	myDevice.activate();

#ifdef VERBOSE_OUT
	printf("\nExample for audio I/O\r\n");
	printf("(C) 2017 Javox Solutions GmbH\r\n");
	printf("initializing...\r\n\n");
#endif

#ifdef USE_RTC
	Chip_RTC_Init(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
#endif

#ifdef VERBOSE_OUT
	showTime();
#endif //VERBOSE_OUT


	// Prepare processing
	myDevice.prepare();

	// Start processing
	myDevice.start();

#ifdef VERBOSE_OUT
	while(1)
	{
		myDevice.update_main();
	}

#else
	printf("main thread going to sleep...\r\n");
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; /* Enable deepsleep */
	__DSB(); /* Ensure effect of last store takes effect */
	__WFI(); /* Enter sleep mode */
	printf("... woke up ... this should not happen ...\r\n");
#endif
	return 0;
}
