/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  DIP Switch header file
 *
 */

#ifndef DIP_
#define DIP_

#ifdef __cplusplus
extern "C" {
#endif

#include "dskda830.h"
#include "dskda830_gpio.h"


/* ------------------------------------------------------------------------ *
 *  Definitions                                                             *
 * ------------------------------------------------------------------------ */
#define DIP_0                   0
#define DIP_1                   1
#define DIP_2                   2
#define DIP_3                   3

/* ------------------------------------------------------------------------ *
 *  Switch Settings                                                         *
 * ------------------------------------------------------------------------ */
#define DIP_DOWN                0   // DIP switch is DOWN
#define DIP_UP                  1   // DIP switch is UP

/* ------------------------------------------------------------------------ *
 *  I2C Address                                                             *
 * ------------------------------------------------------------------------ */
#define DIP_I2C_ADDR            I2C_GPIO_GROUP_0

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_DIP_init( );
Int16 DSKDA830_DIP_get ( Int32 dip_num );
Int16 DSKDA830_DIP_wait( Int32 timeout );

#ifdef __cplusplus
}
#endif

#endif
