/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Audio codec setup functions
 */

#include "dskda830.h"
#include "dskda830_gpio.h"

/* ------------------------------------------------------------------------ *
 *  SPI Controller                                                          *
 * ------------------------------------------------------------------------ */
#define SPI_BASE                0x01E12000
#define SPI_SPIGCR0             *( volatile Uint32* )( SPI_BASE + 0x0 )
#define SPI_SPIGCR1             *( volatile Uint32* )( SPI_BASE + 0x4 )
#define SPI_SPIINT              *( volatile Uint32* )( SPI_BASE + 0x8 )
#define SPI_SPILVL              *( volatile Uint32* )( SPI_BASE + 0xc )
#define SPI_SPIFLG              *( volatile Uint32* )( SPI_BASE + 0x10 )
#define SPI_SPIPC0              *( volatile Uint32* )( SPI_BASE + 0x14 )
#define SPI_SPIPC2              *( volatile Uint32* )( SPI_BASE + 0x1c )
#define SPI_SPIDAT1_TOP         *( volatile Uint16* )( SPI_BASE + 0x3c )
#define SPI_SPIDAT1             *( volatile Uint32* )( SPI_BASE + 0x3c )
#define SPI_SPIDAT1_PTR16       *( volatile Uint16* )( SPI_BASE + 0x3e )
#define SPI_SPIDAT1_PTR8        *( volatile Uint8* ) ( SPI_BASE + 0x3f )
#define SPI_SPIBUF              *( volatile Uint32* )( SPI_BASE + 0x40 )
#define SPI_SPIBUF_PTR16        *( volatile Uint16* )( SPI_BASE + 0x42 )
#define SPI_SPIBUF_PTR8         *( volatile Uint8* ) ( SPI_BASE + 0x43 )
#define SPI_SPIEMU              *( volatile Uint32* )( SPI_BASE + 0x44 )
#define SPI_SPIDELAY            *( volatile Uint32* )( SPI_BASE + 0x48 )
#define SPI_SPIDEF              *( volatile Uint32* )( SPI_BASE + 0x4c )
#define SPI_SPIFMT0             *( volatile Uint32* )( SPI_BASE + 0x50 )
#define SPI_SPIFMT1             *( volatile Uint32* )( SPI_BASE + 0x54 )
#define SPI_SPIFMT2             *( volatile Uint32* )( SPI_BASE + 0x58 )
#define SPI_SPIFMT3             *( volatile Uint32* )( SPI_BASE + 0x5c )
#define SPI_INTVEC0             *( volatile Uint32* )( SPI_BASE + 0x60 )
#define SPI_INTVEC1             *( volatile Uint32* )( SPI_BASE + 0x64 )

static Uint32 spidat1;
static Uint8 cmd[8];

/* ------------------------------------------------------------------------ *
 *  spi_init( )                                                        *
 * ------------------------------------------------------------------------ */
void ak4588_init( )
{
    /* Configure pin muxing and chip select control */
    KICK0R = 0x83e70b13;  // Kick0 register + data (unlock)
    KICK1R = 0x95a4f1e0;  // Kick1 register + data (unlock)

	// Want SPI_CLK, SPI_SIMO and SPI_SOMI as SPI pins, GPIO58 as chip select
//    PINMUX11 = (PINMUX11 & 0x0000f000) | 0x00008000;  // AXR0[10] as GPIO 3[10] = GPIO58;
    PINMUX11 = 0x00008000;  // AXR0[10] as GPIO 3[10] = GPIO58; 
    PINMUX8 = PINMUX8 | 0x00000111;

	DSKDA830_GPIO_setOutput(58, 1);              // Drive AK4588 chip select high
    DSKDA830_GPIO_setDirection( 58, GPIO_OUT );  // Enable AK4588 chip select as output

    /* Reset SPI */
    SPI_SPIGCR0 = 0;
    DSKDA830_wait( 1000 );

    /* Release SPI */
    SPI_SPIGCR0 = 1;

    /* SPI 4-Pin Mode setup */
    SPI_SPIGCR1 = 0
        | ( 0 << 24 )
        | ( 0 << 16 )
        | ( 1 << 1 )
        | ( 1 << 0 );

    SPI_SPIPC0 = 0
        | ( 1 << 11 )   // DI
        | ( 1 << 10 )   // DO
        | ( 1 << 9 )    // CLK
        | ( 1 << 1 )    // EN1
        | ( 1 << 0 );   // EN0

    SPI_SPIFMT0 = 0
        | ( 0 << 20 )   // SHIFTDIR
        | ( 0 << 17 )   // Polarity
        | ( 1 << 16 )   // Phase
        | ( 19<< 8 )   // Prescale to 4MHz
        | ( 8 << 0 );   // Char Len

    spidat1 = 0
        | ( 1 << 28 )   // CSHOLD 
        | ( 0 << 24 )   // Format [0]
        | ( 2 << 16 )   // CSNR   [only CS0 enbled]
        | ( 0 << 0 );   //

    SPI_SPIDAT1 = spidat1;

    SPI_SPIDELAY = 0
        | ( 8 << 24 )   // C2TDELAY
        | ( 8 << 16 );  // T2CDELAY

    SPI_SPIDEF = 0
        | ( 1 << 1 )    // EN1 inactive high
        | ( 1 << 0 );   // EN0 inactive high

    SPI_SPIINT = 0
        | ( 0 << 16 )   //
        | ( 0 << 8 )    //
        | ( 0 << 6 )    //
        | ( 1 << 4 );   //

    SPI_SPILVL = 0
        | ( 0 << 8 )    // EN0
        | ( 0 << 6 )    // EN0
        | ( 0 << 4 );   // EN0


    /* Enable SPI */
    SPI_SPIGCR1 |= ( 1 << 24 );
}

/* ------------------------------------------------------------------------ *
 *  spicycle(buf, len)                                                      *
 *                                                                          *
 *  Execute a SPI spiflash data transfer cycle.  Each byte in buf is shifted*
 *  out and replaced with data coming back from the spiflash.               *
 * ------------------------------------------------------------------------ */

static void spicycle(Uint8 *buf, Uint16 len)
{
    Uint16 i;

    /* Chip select low */
    DSKDA830_GPIO_setOutput(58, 0);

	/* Clear any old data */
	SPI_SPIBUF;

	/* spiflash access cycle */
	for (i = 0; i <= len; i++)
	{
	    // Wait for transmit ready
		while( SPI_SPIBUF & 0x10000000 );
		if (i == len )
            SPI_SPIDAT1 = (spidat1 & 0x0ffcffff) | buf[i] ;
		else
            SPI_SPIDAT1 = spidat1 | buf[i];

		// Wait for receive data ready
        while ( SPI_SPIBUF & ( 0x80000000 ) );

        /* Read 1 byte */
        buf[i] = SPI_SPIBUF;
	}

    /* Chip select high */
    DSKDA830_GPIO_setOutput(58, 1);
}

Int16 ak4588_rset(Uint16 id, Uint16 regnum, Uint16 regval)
{
    cmd[0] = ((id & 0x3) << 6) | (1 << 5) | (regnum & 0x1f);
	cmd[1] = regval;

	spicycle(cmd, 1);

	return 0;
}

Int16 ak4588_rget(Uint16 id, Uint16 regnum, Uint16 *regval)
{
    cmd[0] = ((id & 0x3) << 6) | (0 << 5) | (regnum & 0x1f);

	spicycle(cmd, 1);

	*regval = cmd[1];

	return 0;
}
