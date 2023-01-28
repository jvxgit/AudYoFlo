/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  GPIO implementation
 *
 */

#include "dskda830_gpio.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_init( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_GPIO_init()
{
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_setDirection( number, direction )                                 *
 *                                                                          *
 *      number    <- GPIO#                                                  *
 *      direction <- 0:OUT 1:IN                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_GPIO_setDirection( Uint16 number, Uint8 direction )
{
    Uint32 bank_id = ( number >> 5 );
    Uint32 pin_id  = ( 1 << ( number & 0x1F ) );
    Uint32* gpio_dir = ( Uint32* )( GPIO_BASE + GPIO_DIR_BASE + ( bank_id * GPIO_BASE_OFFSET ) );

    if ( ( direction & 1 ) == GPIO_OUT )
        *gpio_dir &= ~( pin_id );  // Set to OUTPUT
    else
        *gpio_dir |= ( pin_id );   // Set to INPUT

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_setOutput( number, output )                                       *
 *                                                                          *
 *      number   <- GPIO#                                                   *
 *      value    <- 0:LOW 1:HIGH                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_GPIO_setOutput( Uint16 number, Uint8 output )
{
    Uint32 bank_id = ( number >> 5 );
    Uint32 pin_id  = ( 1 << ( number & 0x1F ) );
    Uint32* gpio_out = ( Uint32* )( GPIO_BASE + GPIO_OUT_DATA_BASE + ( bank_id * GPIO_BASE_OFFSET ) );

    if ( ( output & 1 )  == 0 )
        *gpio_out &= ~( pin_id );  // Set to LOW
    else
        *gpio_out |= ( pin_id );   // Set to HIGH

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_getInput( number )                                                *
 *                                                                          *
 *      number   <- GPIO#                                                   *
 *                                                                          *
 *      Returns:    0:LOW                                                   *
 *                  1:HIGH                                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_GPIO_getInput( Uint16 number )
{
    Uint32 input;
    Uint32 bank_id = ( number >> 5 );
    Uint32 pin_id  = ( number & 0x1F );
    Uint32* gpio_in = ( Uint32* )( GPIO_BASE + GPIO_IN_DATA_BASE + ( bank_id * GPIO_BASE_OFFSET ) );

    input = *gpio_in;
    input = ( input >> pin_id ) & 1;

    return input;
}
