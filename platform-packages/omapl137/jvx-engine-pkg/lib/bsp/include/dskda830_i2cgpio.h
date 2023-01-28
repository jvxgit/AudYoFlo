/*
 *  Copyright 2007 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  I2C GPIO I/O expander header file
 *
 */

#ifndef I2C_GPIO_
#define I2C_GPIO_

#ifdef __cplusplus
extern "C" {
#endif

#include "dskda830_i2c.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_I2C_GPIO_init( );
Int16 DSKDA830_I2C_GPIO_getall( Uint16 group_id, Uint8 *pattern );
Int16 DSKDA830_I2C_GPIO_setall( Uint16 group_id, Uint8 pattern );

Int16 DSKDA830_I2C_GPIO_setOutput( Uint16 group_id, Uint8 number, Uint8 value );
Int16 DSKDA830_I2C_GPIO_getInput ( Uint16 group_id, Uint8 number );

#ifdef __cplusplus
}
#endif

#endif
