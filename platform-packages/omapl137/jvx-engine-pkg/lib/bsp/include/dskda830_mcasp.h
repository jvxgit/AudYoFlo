/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  MCASP header file
 *
 */

#ifndef MCASP_
#define MCASP_

#ifdef __cplusplus
extern "C" {
#endif

#include "dskda830.h"

/* ------------------------------------------------------------------------ *
 *  MCASP IDs                                                               *
 * ------------------------------------------------------------------------ */
#define MCASP_0                 ( 0 )
#define MCASP_1                 ( 1 )
#define MCASP_2                 ( 2 )

#define SERIALIZER_0            ( 0 )
#define SERIALIZER_1            ( 1 )
#define SERIALIZER_2            ( 2 )
#define SERIALIZER_3            ( 3 )

/* ------------------------------------------------------------------------ *
 *  MCASP0 Controller                                                       *
 * ------------------------------------------------------------------------ */
#define MCASP0_BASE             0x01d00000
#define MCASP0_DATA             0x01d02000
#define MCASP0_DATA_SIZE        0x00000400
#define MCASP0_PID              *( volatile Uint32* )( MCASP0_BASE + 0x0 )
#define MCASP0_PWRDEMU          *( volatile Uint32* )( MCASP0_BASE + 0x4 )
#define MCASP0_PFUNC            *( volatile Uint32* )( MCASP0_BASE + 0x10 )
#define MCASP0_PDIR             *( volatile Uint32* )( MCASP0_BASE + 0x14 )
#define MCASP0_PDOUT            *( volatile Uint32* )( MCASP0_BASE + 0x18 )
#define MCASP0_PDIN             *( volatile Uint32* )( MCASP0_BASE + 0x1c )
#define MCASP0_PDCLR            *( volatile Uint32* )( MCASP0_BASE + 0x20 )
#define MCASP0_GBLCTL           *( volatile Uint32* )( MCASP0_BASE + 0x44 )
#define MCASP0_AMUTE            *( volatile Uint32* )( MCASP0_BASE + 0x48 )
#define MCASP0_DLBCTL           *( volatile Uint32* )( MCASP0_BASE + 0x4c )
#define MCASP0_DITCTL           *( volatile Uint32* )( MCASP0_BASE + 0x50 )
#define MCASP0_RGBLCTL          *( volatile Uint32* )( MCASP0_BASE + 0x60 )
#define MCASP0_RMASK            *( volatile Uint32* )( MCASP0_BASE + 0x64 )
#define MCASP0_RFMT             *( volatile Uint32* )( MCASP0_BASE + 0x68 )
#define MCASP0_AFSRCTL          *( volatile Uint32* )( MCASP0_BASE + 0x6c )
#define MCASP0_ACLKRCTL         *( volatile Uint32* )( MCASP0_BASE + 0x70 )
#define MCASP0_AHCLKRCTL        *( volatile Uint32* )( MCASP0_BASE + 0x74 )
#define MCASP0_RTDM             *( volatile Uint32* )( MCASP0_BASE + 0x78 )
#define MCASP0_RINTCTL          *( volatile Uint32* )( MCASP0_BASE + 0x7c )
#define MCASP0_RSTAT            *( volatile Uint32* )( MCASP0_BASE + 0x80 )
#define MCASP0_RSLOT            *( volatile Uint32* )( MCASP0_BASE + 0x84 )
#define MCASP0_RCLKCHK          *( volatile Uint32* )( MCASP0_BASE + 0x88 )
#define MCASP0_XGBLCTL          *( volatile Uint32* )( MCASP0_BASE + 0xa0 )
#define MCASP0_XMASK            *( volatile Uint32* )( MCASP0_BASE + 0xa4 )
#define MCASP0_XFMT             *( volatile Uint32* )( MCASP0_BASE + 0xa8 )
#define MCASP0_AFSXCTL          *( volatile Uint32* )( MCASP0_BASE + 0xac )
#define MCASP0_ACLKXCTL         *( volatile Uint32* )( MCASP0_BASE + 0xb0 )
#define MCASP0_AHCLKXCTL        *( volatile Uint32* )( MCASP0_BASE + 0xb4 )
#define MCASP0_XTDM             *( volatile Uint32* )( MCASP0_BASE + 0xb8 )
#define MCASP0_XINTCTL          *( volatile Uint32* )( MCASP0_BASE + 0xbc )
#define MCASP0_XSTAT            *( volatile Uint32* )( MCASP0_BASE + 0xc0 )
#define MCASP0_XSLOT            *( volatile Uint32* )( MCASP0_BASE + 0xc4 )
#define MCASP0_XCLKCHK          *( volatile Uint32* )( MCASP0_BASE + 0xc8 )
#define MCASP0_DITCSRA0         *( volatile Uint32* )( MCASP0_BASE + 0x100 )
#define MCASP0_DITCSRA1         *( volatile Uint32* )( MCASP0_BASE + 0x104 )
#define MCASP0_DITCSRA2         *( volatile Uint32* )( MCASP0_BASE + 0x108 )
#define MCASP0_DITCSRA3         *( volatile Uint32* )( MCASP0_BASE + 0x10c )
#define MCASP0_DITCSRA4         *( volatile Uint32* )( MCASP0_BASE + 0x110 )
#define MCASP0_DITCSRA5         *( volatile Uint32* )( MCASP0_BASE + 0x114 )
#define MCASP0_DITCSRB0         *( volatile Uint32* )( MCASP0_BASE + 0x118 )
#define MCASP0_DITCSRB1         *( volatile Uint32* )( MCASP0_BASE + 0x11c )
#define MCASP0_DITCSRB2         *( volatile Uint32* )( MCASP0_BASE + 0x120 )
#define MCASP0_DITCSRB3         *( volatile Uint32* )( MCASP0_BASE + 0x124 )
#define MCASP0_DITCSRB4         *( volatile Uint32* )( MCASP0_BASE + 0x128 )
#define MCASP0_DITCSRB5         *( volatile Uint32* )( MCASP0_BASE + 0x12c )
#define MCASP0_DITUDRA0         *( volatile Uint32* )( MCASP0_BASE + 0x130 )
#define MCASP0_DITUDRA1         *( volatile Uint32* )( MCASP0_BASE + 0x134 )
#define MCASP0_DITUDRA2         *( volatile Uint32* )( MCASP0_BASE + 0x138 )
#define MCASP0_DITUDRA3         *( volatile Uint32* )( MCASP0_BASE + 0x13c )
#define MCASP0_DITUDRA4         *( volatile Uint32* )( MCASP0_BASE + 0x140 )
#define MCASP0_DITUDRA5         *( volatile Uint32* )( MCASP0_BASE + 0x144 )
#define MCASP0_DITUDRB0         *( volatile Uint32* )( MCASP0_BASE + 0x148 )
#define MCASP0_DITUDRB1         *( volatile Uint32* )( MCASP0_BASE + 0x14c )
#define MCASP0_DITUDRB2         *( volatile Uint32* )( MCASP0_BASE + 0x150 )
#define MCASP0_DITUDRB3         *( volatile Uint32* )( MCASP0_BASE + 0x154 )
#define MCASP0_DITUDRB4         *( volatile Uint32* )( MCASP0_BASE + 0x158 )
#define MCASP0_DITUDRB5         *( volatile Uint32* )( MCASP0_BASE + 0x15c )
#define MCASP0_SRCTL0           *( volatile Uint32* )( MCASP0_BASE + 0x180 )
#define MCASP0_SRCTL1           *( volatile Uint32* )( MCASP0_BASE + 0x184 )
#define MCASP0_SRCTL2           *( volatile Uint32* )( MCASP0_BASE + 0x188 )
#define MCASP0_SRCTL3           *( volatile Uint32* )( MCASP0_BASE + 0x18c )
#define MCASP0_SRCTL9           *( volatile Uint32* )( MCASP0_BASE + 0x1A4 )
#define MCASP0_XBUF0            *( volatile Uint32* )( MCASP0_BASE + 0x200 )
#define MCASP0_XBUF0_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x200 )
#define MCASP0_XBUF0_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x200 )
#define MCASP0_XBUF1            *( volatile Uint32* )( MCASP0_BASE + 0x204 )
#define MCASP0_XBUF1_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x204 )
#define MCASP0_XBUF1_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x204 )
#define MCASP0_XBUF2            *( volatile Uint32* )( MCASP0_BASE + 0x208 )
#define MCASP0_XBUF2_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x208 )
#define MCASP0_XBUF2_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x208 )
#define MCASP0_XBUF3            *( volatile Uint32* )( MCASP0_BASE + 0x20c )
#define MCASP0_XBUF3_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x20c )
#define MCASP0_XBUF3_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x20c )
#define MCASP0_RBUF0            *( volatile Uint32* )( MCASP0_BASE + 0x280 )
#define MCASP0_RBUF0_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x280 )
#define MCASP0_RBUF0_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x280 )
#define MCASP0_RBUF1            *( volatile Uint32* )( MCASP0_BASE + 0x284 )
#define MCASP0_RBUF1_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x284 )
#define MCASP0_RBUF1_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x284 )
#define MCASP0_RBUF2            *( volatile Uint32* )( MCASP0_BASE + 0x288 )
#define MCASP0_RBUF2_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x288 )
#define MCASP0_RBUF2_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x288 )
#define MCASP0_RBUF3            *( volatile Uint32* )( MCASP0_BASE + 0x28c )
#define MCASP0_RBUF3_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x28c )
#define MCASP0_RBUF3_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x28c )
#define MCASP0_RBUF9_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x2A4 )

/* ------------------------------------------------------------------------ *
 *  MCASP1 Controller                                                       *
 * ------------------------------------------------------------------------ */
#define MCASP1_BASE             0x01d04000
#define MCASP1_FIFO             0x01D05000
#define MCASP1_DATA             0x01d06000
#define MCASP1_DATA_SIZE        0x00000400
#define MCASP1_PID              *( volatile Uint32* )( MCASP1_BASE + 0x0 )
#define MCASP1_PWRDEMU          *( volatile Uint32* )( MCASP1_BASE + 0x4 )
#define MCASP1_PFUNC            *( volatile Uint32* )( MCASP1_BASE + 0x10 )
#define MCASP1_PDIR             *( volatile Uint32* )( MCASP1_BASE + 0x14 )
#define MCASP1_PDOUT            *( volatile Uint32* )( MCASP1_BASE + 0x18 )
#define MCASP1_PDIN             *( volatile Uint32* )( MCASP1_BASE + 0x1c )
#define MCASP1_PDCLR            *( volatile Uint32* )( MCASP1_BASE + 0x20 )
#define MCASP1_GBLCTL           *( volatile Uint32* )( MCASP1_BASE + 0x44 )
#define MCASP1_DLBCTL           *( volatile Uint32* )( MCASP1_BASE + 0x4c )
#define MCASP1_DITCTL           *( volatile Uint32* )( MCASP1_BASE + 0x50 )
#define MCASP1_RGBLCTL          *( volatile Uint32* )( MCASP1_BASE + 0x60 )
#define MCASP1_RINTCTL          *( volatile Uint32* )( MCASP1_BASE + 0x7c )
#define MCASP1_RSTAT            *( volatile Uint32* )( MCASP1_BASE + 0x80 )
#define MCASP1_RSLOT            *( volatile Uint32* )( MCASP1_BASE + 0x84 )
#define MCASP1_RCLKCHK          *( volatile Uint32* )( MCASP1_BASE + 0x88 )
#define MCASP1_REVTCTL          *( volatile Uint32* )( MCASP1_BASE + 0x8c )
#define MCASP1_XGBLCTL          *( volatile Uint32* )( MCASP1_BASE + 0xa0 )
#define MCASP1_XMASK            *( volatile Uint32* )( MCASP1_BASE + 0xa4 )
#define MCASP1_XFMT             *( volatile Uint32* )( MCASP1_BASE + 0xa8 )
#define MCASP1_AFSXCTL          *( volatile Uint32* )( MCASP1_BASE + 0xac )
#define MCASP1_ACLKXCTL         *( volatile Uint32* )( MCASP1_BASE + 0xb0 )
#define MCASP1_AHCLKXCTL        *( volatile Uint32* )( MCASP1_BASE + 0xb4 )
#define MCASP1_XTDM             *( volatile Uint32* )( MCASP1_BASE + 0xb8 )
#define MCASP1_XINTCTL          *( volatile Uint32* )( MCASP1_BASE + 0xbc )
#define MCASP1_XSTAT            *( volatile Uint32* )( MCASP1_BASE + 0xc0 )
#define MCASP1_XSLOT            *( volatile Uint32* )( MCASP1_BASE + 0xc4 )
#define MCASP1_XCLKCHK          *( volatile Uint32* )( MCASP1_BASE + 0xc8 )
#define MCASP1_XEVTCTL          *( volatile Uint32* )( MCASP1_BASE + 0xcc )
#define MCASP1_DITCSRA0         *( volatile Uint32* )( MCASP1_BASE + 0x100 )
#define MCASP1_DITCSRA1         *( volatile Uint32* )( MCASP1_BASE + 0x104 )
#define MCASP1_DITCSRA2         *( volatile Uint32* )( MCASP1_BASE + 0x108 )
#define MCASP1_DITCSRA3         *( volatile Uint32* )( MCASP1_BASE + 0x10c )
#define MCASP1_DITCSRA4         *( volatile Uint32* )( MCASP1_BASE + 0x110 )
#define MCASP1_DITCSRA5         *( volatile Uint32* )( MCASP1_BASE + 0x114 )
#define MCASP1_DITCSRB0         *( volatile Uint32* )( MCASP1_BASE + 0x118 )
#define MCASP1_DITCSRB1         *( volatile Uint32* )( MCASP1_BASE + 0x11c )
#define MCASP1_DITCSRB2         *( volatile Uint32* )( MCASP1_BASE + 0x120 )
#define MCASP1_DITCSRB3         *( volatile Uint32* )( MCASP1_BASE + 0x124 )
#define MCASP1_DITCSRB4         *( volatile Uint32* )( MCASP1_BASE + 0x128 )
#define MCASP1_DITCSRB5         *( volatile Uint32* )( MCASP1_BASE + 0x12c )
#define MCASP1_DITUDRA0         *( volatile Uint32* )( MCASP1_BASE + 0x130 )
#define MCASP1_DITUDRA1         *( volatile Uint32* )( MCASP1_BASE + 0x134 )
#define MCASP1_DITUDRA2         *( volatile Uint32* )( MCASP1_BASE + 0x138 )
#define MCASP1_DITUDRA3         *( volatile Uint32* )( MCASP1_BASE + 0x13c )
#define MCASP1_DITUDRA4         *( volatile Uint32* )( MCASP1_BASE + 0x140 )
#define MCASP1_DITUDRA5         *( volatile Uint32* )( MCASP1_BASE + 0x144 )
#define MCASP1_DITUDRB0         *( volatile Uint32* )( MCASP1_BASE + 0x148 )
#define MCASP1_DITUDRB1         *( volatile Uint32* )( MCASP1_BASE + 0x14c )
#define MCASP1_DITUDRB2         *( volatile Uint32* )( MCASP1_BASE + 0x150 )
#define MCASP1_DITUDRB3         *( volatile Uint32* )( MCASP1_BASE + 0x154 )
#define MCASP1_DITUDRB4         *( volatile Uint32* )( MCASP1_BASE + 0x158 )
#define MCASP1_DITUDRB5         *( volatile Uint32* )( MCASP1_BASE + 0x15c )
#define MCASP1_SRCTL0           *( volatile Uint32* )( MCASP1_BASE + 0x180 )
#define MCASP1_SRCTL1           *( volatile Uint32* )( MCASP1_BASE + 0x184 )
#define MCASP1_SRCTL2           *( volatile Uint32* )( MCASP1_BASE + 0x188 )
#define MCASP1_SRCTL5           *( volatile Uint32* )( MCASP1_BASE + 0x194 )
#define MCASP1_SRCTL6           *( volatile Uint32* )( MCASP1_BASE + 0x198 )
#define MCASP1_SRCTL7           *( volatile Uint32* )( MCASP1_BASE + 0x19C )
#define MCASP1_SRCTL8           *( volatile Uint32* )( MCASP1_BASE + 0x1A0 )
#define MCASP1_SRCTL10          *( volatile Uint32* )( MCASP1_BASE + 0x1A8 )
#define MCASP1_XBUF0            *( volatile Uint32* )( MCASP1_BASE + 0x200 )
#define MCASP1_XBUF0_16BIT      *( volatile Uint16* )( MCASP1_BASE + 0x200 )
#define MCASP1_XBUF0_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x200 )
#define MCASP1_XBUF5_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x214 )
#define MCASP1_XBUF6_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x218 )
#define MCASP1_XBUF7_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x21C )
#define MCASP1_XBUF8_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x220 )
#define MCASP1_RBUF0_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x280 )
#define MCASP1_RBUF1_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x284 )
#define MCASP1_RBUF2_32BIT      *( volatile Uint32* )( MCASP1_BASE + 0x288 )
#define MCASP1_RBUF5            *( volatile Uint32* )( MCASP0_BASE + 0x294 )
#define MCASP1_RBUF5_16BIT      *( volatile Uint16* )( MCASP0_BASE + 0x294 )
#define MCASP1_RBUF5_32BIT      *( volatile Uint32* )( MCASP0_BASE + 0x294 )
#define MCASP1_RBUF10_32BIT     *( volatile Uint32* )( MCASP1_BASE + 0x2A8 )

#define MCASP1_AFIFOREV     	*( volatile Uint32* )( MCASP1_FIFO + 0x0 )
#define MCASP1_WFIFOCTL     	*( volatile Uint32* )( MCASP1_FIFO + 0x10 )
#define MCASP1_WFIFOSTS     	*( volatile Uint32* )( MCASP1_FIFO + 0x14 )
#define MCASP1_RFIFOCTL     	*( volatile Uint32* )( MCASP1_FIFO + 0x18 )
#define MCASP1_RFIFOSTS     	*( volatile Uint32* )( MCASP1_FIFO + 0x1C )

#define MCASP1_XBUF     		*( volatile Uint32* )( MCASP1_DATA + 0x0 )
#define MCASP1_RBUF     		*( volatile Uint32* )( MCASP1_DATA + 0x0 )

#define MCASP1_XBUF_PTR    		( MCASP1_DATA + 0x0 )
#define MCASP1_RBUF_PTR    		( MCASP1_DATA + 0x0 )

/* ------------------------------------------------------------------------ *
 *  MCASP2 Controller                                                       *
 * ------------------------------------------------------------------------ */
#define MCASP2_BASE             0x01d08000
#define MCASP2_DATA             0x01d0A000
#define MCASP2_DATA_SIZE        0x00000400
#define MCASP2_PID              *( volatile Uint32* )( MCASP2_BASE + 0x0 )
#define MCASP2_PWRDEMU          *( volatile Uint32* )( MCASP2_BASE + 0x4 )
#define MCASP2_PFUNC            *( volatile Uint32* )( MCASP2_BASE + 0x10 )
#define MCASP2_PDIR             *( volatile Uint32* )( MCASP2_BASE + 0x14 )
#define MCASP2_PDOUT            *( volatile Uint32* )( MCASP2_BASE + 0x18 )
#define MCASP2_PDIN             *( volatile Uint32* )( MCASP2_BASE + 0x1c )
#define MCASP2_PDCLR            *( volatile Uint32* )( MCASP2_BASE + 0x20 )
#define MCASP2_GBLCTL           *( volatile Uint32* )( MCASP2_BASE + 0x44 )
#define MCASP2_DLBCTL           *( volatile Uint32* )( MCASP2_BASE + 0x4c )
#define MCASP2_DITCTL           *( volatile Uint32* )( MCASP2_BASE + 0x50 )
#define MCASP2_RGBLCTL          *( volatile Uint32* )( MCASP2_BASE + 0x60 )
#define MCASP2_RINTCTL          *( volatile Uint32* )( MCASP2_BASE + 0x7c )
#define MCASP2_RSTAT            *( volatile Uint32* )( MCASP2_BASE + 0x80 )
#define MCASP2_XGBLCTL          *( volatile Uint32* )( MCASP2_BASE + 0xa0 )
#define MCASP2_XMASK            *( volatile Uint32* )( MCASP2_BASE + 0xa4 )
#define MCASP2_XFMT             *( volatile Uint32* )( MCASP2_BASE + 0xa8 )
#define MCASP2_AFSXCTL          *( volatile Uint32* )( MCASP2_BASE + 0xac )
#define MCASP2_ACLKXCTL         *( volatile Uint32* )( MCASP2_BASE + 0xb0 )
#define MCASP2_AHCLKXCTL        *( volatile Uint32* )( MCASP2_BASE + 0xb4 )
#define MCASP2_XTDM             *( volatile Uint32* )( MCASP2_BASE + 0xb8 )
#define MCASP2_XINTCTL          *( volatile Uint32* )( MCASP2_BASE + 0xbc )
#define MCASP2_XSTAT            *( volatile Uint32* )( MCASP2_BASE + 0xc0 )
#define MCASP2_XSLOT            *( volatile Uint32* )( MCASP2_BASE + 0xc4 )
#define MCASP2_XCLKCHK          *( volatile Uint32* )( MCASP2_BASE + 0xc8 )
#define MCASP2_DITCSRA0         *( volatile Uint32* )( MCASP2_BASE + 0x100 )
#define MCASP2_DITCSRA1         *( volatile Uint32* )( MCASP2_BASE + 0x104 )
#define MCASP2_DITCSRA2         *( volatile Uint32* )( MCASP2_BASE + 0x108 )
#define MCASP2_DITCSRA3         *( volatile Uint32* )( MCASP2_BASE + 0x10c )
#define MCASP2_DITCSRA4         *( volatile Uint32* )( MCASP2_BASE + 0x110 )
#define MCASP2_DITCSRA5         *( volatile Uint32* )( MCASP2_BASE + 0x114 )
#define MCASP2_DITCSRB0         *( volatile Uint32* )( MCASP2_BASE + 0x118 )
#define MCASP2_DITCSRB1         *( volatile Uint32* )( MCASP2_BASE + 0x11c )
#define MCASP2_DITCSRB2         *( volatile Uint32* )( MCASP2_BASE + 0x120 )
#define MCASP2_DITCSRB3         *( volatile Uint32* )( MCASP2_BASE + 0x124 )
#define MCASP2_DITCSRB4         *( volatile Uint32* )( MCASP2_BASE + 0x128 )
#define MCASP2_DITCSRB5         *( volatile Uint32* )( MCASP2_BASE + 0x12c )
#define MCASP2_DITUDRA0         *( volatile Uint32* )( MCASP2_BASE + 0x130 )
#define MCASP2_DITUDRA1         *( volatile Uint32* )( MCASP2_BASE + 0x134 )
#define MCASP2_DITUDRA2         *( volatile Uint32* )( MCASP2_BASE + 0x138 )
#define MCASP2_DITUDRA3         *( volatile Uint32* )( MCASP2_BASE + 0x13c )
#define MCASP2_DITUDRA4         *( volatile Uint32* )( MCASP2_BASE + 0x140 )
#define MCASP2_DITUDRA5         *( volatile Uint32* )( MCASP2_BASE + 0x144 )
#define MCASP2_DITUDRB0         *( volatile Uint32* )( MCASP2_BASE + 0x148 )
#define MCASP2_DITUDRB1         *( volatile Uint32* )( MCASP2_BASE + 0x14c )
#define MCASP2_DITUDRB2         *( volatile Uint32* )( MCASP2_BASE + 0x150 )
#define MCASP2_DITUDRB3         *( volatile Uint32* )( MCASP2_BASE + 0x154 )
#define MCASP2_DITUDRB4         *( volatile Uint32* )( MCASP2_BASE + 0x158 )
#define MCASP2_DITUDRB5         *( volatile Uint32* )( MCASP2_BASE + 0x15c )
#define MCASP2_SRCTL0           *( volatile Uint32* )( MCASP2_BASE + 0x180 )
#define MCASP2_XBUF0            *( volatile Uint32* )( MCASP2_BASE + 0x200 )
#define MCASP2_XBUF0_16BIT      *( volatile Uint16* )( MCASP2_BASE + 0x200 )
#define MCASP2_XBUF0_32BIT      *( volatile Uint32* )( MCASP2_BASE + 0x200 )


/* ------------------------------------------------------------------------ *
 *  GBLCTL_ Field Definitions                                               *
 * ------------------------------------------------------------------------ */
#define GBLCTL_XFRST_ON         0x1000
#define GBLCTL_XSMRST_ON        0x0800
#define GBLCTL_XSRCLR_ON        0x0400
#define GBLCTL_XHCLKRST_ON      0x0200
#define GBLCTL_XCLKRST_ON       0x0100
#define GBLCTL_RFRST_ON         0x0010
#define GBLCTL_RSMRST_ON        0x0008
#define GBLCTL_RSRCLR_ON        0x0004
#define GBLCTL_RHCLKRST_ON      0x0002
#define GBLCTL_RCLKRST_ON       0x0001

/* ------------------------------------------------------------------------ *
 *  MCASP interface                                                         *
 * ------------------------------------------------------------------------ */
typedef struct {
    volatile Uint32 PID;
    volatile Uint32 PWRDEMU;
             Uint32 rsvd_08_0c[2];
    volatile Uint32 PFUNC;
    volatile Uint32 PDIR;
    volatile Uint32 PDOUT;
    volatile Uint32 PDIN;       // aka PDSET
    volatile Uint32 PDCLR;
             Uint32 rsvd_24_40[8];
    volatile Uint32 GBLCTL;
    volatile Uint32 AMUTE;
    volatile Uint32 DLBCTL;
    volatile Uint32 DITCTL;
             Uint32 rsvd_54_5c[3];
    volatile Uint32 RGBLCTL;
    volatile Uint32 RMASK;
    volatile Uint32 RFMT;
    volatile Uint32 AFSRCTL;
    volatile Uint32 ACLKRCTL; 
    volatile Uint32 AHCLKRCTL;
    volatile Uint32 RTDM;
    volatile Uint32 RINTCTL;
    volatile Uint32 RSTAT;
    volatile Uint32 RSLOT;
    volatile Uint32 RCLKCHK;
             Uint32 rsvd_8c_9c[5];
    volatile Uint32 XGBLCTL;
    volatile Uint32 XMASK;
    volatile Uint32 XFMT;
    volatile Uint32 AFSXCTL;
    volatile Uint32 ACLKXCTL;
    volatile Uint32 AHCLKXCTL;
    volatile Uint32 XTDM;
    volatile Uint32 XINTCTL;
    volatile Uint32 XSTAT;
    volatile Uint32 XSLOT;
    volatile Uint32 XCLKCHK;
             Uint32 rsvd_cc_fc[13];
    volatile Uint32 DITCSRA0;
    volatile Uint32 DITCSRA1;
    volatile Uint32 DITCSRA2;
    volatile Uint32 DITCSRA3;
    volatile Uint32 DITCSRA4;
    volatile Uint32 DITCSRA5;
    volatile Uint32 DITCSRB0;
    volatile Uint32 DITCSRB1;
    volatile Uint32 DITCSRB2;
    volatile Uint32 DITCSRB3;
    volatile Uint32 DITCSRB4;
    volatile Uint32 DITCSRB5;
    volatile Uint32 DITUDRA0;
    volatile Uint32 DITUDRA1;
    volatile Uint32 DITUDRA2;
    volatile Uint32 DITUDRA3;
    volatile Uint32 DITUDRA4;
    volatile Uint32 DITUDRA5;
    volatile Uint32 DITUDRB0;
    volatile Uint32 DITUDRB1;
    volatile Uint32 DITUDRB2;
    volatile Uint32 DITUDRB3;
    volatile Uint32 DITUDRB4;
    volatile Uint32 DITUDRB5;
             Uint32 rsvd_160_17c[8];
    volatile Uint32 SRCTL0;
    volatile Uint32 SRCTL1; 
    volatile Uint32 SRCTL2;
    volatile Uint32 SRCTL3; 
    volatile Uint32 SRCTL4; 
    volatile Uint32 SRCTL5;
    volatile Uint32 SRCTL6;
    volatile Uint32 SRCTL7;
    volatile Uint32 SRCTL8;
    volatile Uint32 SRCTL9;
    volatile Uint32 SRCTL10;
     
             Uint32 rsvd_190_1fc[21];
    volatile Uint32 XBUF0;
    volatile Uint32 XBUF1;
    volatile Uint32 XBUF2;
    volatile Uint32 XBUF3;
    volatile Uint32 XBUF4;
    volatile Uint32 XBUF5;
	volatile Uint32 XBUF6;
	volatile Uint32 XBUF7;
	volatile Uint32 XBUF8;

             Uint32 rsvd_210_27c[23];
    volatile Uint32 RBUF0; 
    volatile Uint32 RBUF1;
    volatile Uint32 RBUF2;
    volatile Uint32 RBUF3;
    volatile Uint32 RBUF4;
    volatile Uint32 RBUF5;
} MCASP_REGS;

#define PDSET                   PDIN    // PDSET & PDIN occupy the same memory location
#define SRCTL_BASE              SRCTL0
#define XBUF_BASE               XBUF0
#define RBUF_BASE               RBUF0

typedef struct {
    Uint16 id;
    MCASP_REGS* regs;
} MCASP_OBJ;

static MCASP_OBJ MCASP_MODULE_0 = { MCASP_0, ( MCASP_REGS* )MCASP0_BASE };
static MCASP_OBJ MCASP_MODULE_1 = { MCASP_1, ( MCASP_REGS* )MCASP1_BASE };
static MCASP_OBJ MCASP_MODULE_2 = { MCASP_2, ( MCASP_REGS* )MCASP2_BASE };

typedef MCASP_OBJ* MCASP_Handle;

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 DSKDA830_MCASP_open ( Uint16 id );
Int16 DSKDA830_MCASP_close( Uint16 id );

#ifdef __cplusplus
}
#endif

#endif
