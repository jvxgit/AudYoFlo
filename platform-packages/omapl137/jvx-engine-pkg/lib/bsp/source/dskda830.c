/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Board Setup
 *
 */

#include "dskda830.h"
#include "dskda830_i2c.h"
#include "dskda830_gpio.h"
#include "dskda830_led.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  DSKDA830_wait( delay )                                                  *
 *                                                                          *
 *      Wait in a software loop for 'x' delay                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void DSKDA830_wait( Uint32 delay )
{
    volatile Uint32 i;
    for ( i = 0 ; i < delay ; i++ ){ };
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  DSKDA830_waitusec( usec )                                               *
 *                                                                          *
 *      Wait in a software loop for 'x' microseconds                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void DSKDA830_waitusec( Uint32 usec )
{
    DSKDA830_wait( usec * 3 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  DSKDA830_init( )                                                        *
 *                                                                          *
 *      Setup I2C, MSP430, & I2C GPIO Expander                              *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_init( )
{
    /* PSC, PLL, DDR, & AEMIF are already configured in the GEL file.  Those
	 * settings are not re-initialized here */

	DSKDA830_GPIO_init();                 // Initialize GPIO module
    DSKDA830_I2C_init( );                 // Initialize I2C module

    return 0;
}

// ==========================================================
void board_init()
{
	DSKDA830_LED_init( );
	DSKDA830_LED_on( 0 );
}

void switchOverloadLed(setFlag)
{
	if(setFlag)
	{
		DSKDA830_LED_on( 3 );
	}
	else
	{
		DSKDA830_LED_off( 3 );
	}
}
