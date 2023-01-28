/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  I2C header file
 *
 */

#ifndef I2C_
#define I2C_

#include "dskda830.h"


/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_I2C_init ( );
Int16 DSKDA830_I2C_close( );
Int16 DSKDA830_I2C_write( Uint16 i2c_addr, Uint8* data, Uint16 len );
Int16 DSKDA830_I2C_read ( Uint16 i2c_addr, Uint8* data, Uint16 len );

#endif
