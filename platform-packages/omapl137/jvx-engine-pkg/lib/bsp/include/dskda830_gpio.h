/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  GPIO header file
 *
 */

#ifndef GPIO_
#define GPIO_

#include "dskda830.h"

#define GPIO_IN                 1
#define GPIO_OUT                0

#define GPIO0                   0x00
#define GPIO1                   0x01
#define GPIO2                   0x02
#define GPIO3                   0x03
#define GPIO4                   0x04
#define GPIO5                   0x05
#define GPIO6                   0x06
#define GPIO7                   0x07
#define GPIO8                   0x08
#define GPIO9                   0x09

#define GPIO10                  0x0A
#define GPIO11                  0x0B
#define GPIO12                  0x0C
#define GPIO13                  0x0D
#define GPIO14                  0x0E
#define GPIO15                  0x0F
#define GPIO16                  0x10
#define GPIO17                  0x11
#define GPIO18                  0x12
#define GPIO19                  0x13

#define GPIO20                  0x14
#define GPIO21                  0x15
#define GPIO22                  0x16
#define GPIO23                  0x17
#define GPIO24                  0x18
#define GPIO25                  0x19
#define GPIO26                  0x1A
#define GPIO27                  0x1B
#define GPIO28                  0x1C
#define GPIO29                  0x1D

#define GPIO30                  0x1E
#define GPIO31                  0x1F
#define GPIO32                  0x20
#define GPIO33                  0x21
#define GPIO34                  0x22
#define GPIO35                  0x23
#define GPIO36                  0x24
#define GPIO37                  0x25
#define GPIO38                  0x26
#define GPIO39                  0x27

#define GPIO40                  0x28
#define GPIO41                  0x29
#define GPIO42                  0x2A
#define GPIO43                  0x2B
#define GPIO44                  0x2C
#define GPIO45                  0x2D
#define GPIO46                  0x2E
#define GPIO47                  0x2F
#define GPIO48                  0x30
#define GPIO49                  0x31

#define GPIO50                  0x32
#define GPIO51                  0x33
#define GPIO52                  0x34
#define GPIO53                  0x35

#define GPIO3V3_0               0x36
#define GPIO3V3_1               0x37
#define GPIO3V3_2               0x38
#define GPIO3V3_3               0x39
#define GPIO3V3_4               0x3A
#define GPIO3V3_5               0x3B
#define GPIO3V3_6               0x3C
#define GPIO3V3_7               0x3D
#define GPIO3V3_8               0x3E
#define GPIO3V3_9               0x3F

#define GPIO3V3_10              0x40
#define GPIO3V3_11              0x41
#define GPIO3V3_12              0x42
#define GPIO3V3_13              0x43
#define GPIO3V3_14              0x44
#define GPIO3V3_15              0x45

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_GPIO_init         ( );
Int16 DSKDA830_GPIO_setDirection ( Uint16 number, Uint8 direction );
Int16 DSKDA830_GPIO_setOutput    ( Uint16 number, Uint8 output );
Int16 DSKDA830_GPIO_getInput     ( Uint16 number );

#endif
