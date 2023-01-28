/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  LED header file
 *
 */

#ifndef LED_
#define LED_

#include "dskda830.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_LED_init  ( );
Int16 DSKDA830_LED_on    ( Uint16 num );
Int16 DSKDA830_LED_off   ( Uint16 num );
Int16 DSKDA830_LED_toggle( Uint16 num );
Int16 DSKDA830_LED_getall( Uint16 *pattern );
Int16 DSKDA830_LED_setall( Uint16 pattern );

#endif
