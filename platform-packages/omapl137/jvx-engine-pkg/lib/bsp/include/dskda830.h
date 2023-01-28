/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Definitions & Register
 *
 */

#ifndef DSKDA830_
#define DSKDA830_

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Variable types                                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */

#define Uint32  unsigned int
#define Uint16  unsigned short
#define Uint8   unsigned char
#define Int32   int
#define Int16   short
#define Int8    char


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Software Breakpoint code                                                *
 *      Uses inline assembly command                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */

#define SW_BREAKPOINT       asm( " SWBP 0" );


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AEMIF Controller                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define AEMIF_BASE              0x68000000
#define AEMIF_AWCCR             *( volatile Uint32* )( AEMIF_BASE + 0x04 )
#define AEMIF_A1CR              *( volatile Uint32* )( AEMIF_BASE + 0x10 )
#define AEMIF_A2CR              *( volatile Uint32* )( AEMIF_BASE + 0x14 )
#define AEMIF_A3CR              *( volatile Uint32* )( AEMIF_BASE + 0x18 )
#define AEMIF_A4CR              *( volatile Uint32* )( AEMIF_BASE + 0x1C )
#define AEMIF_EIRR              *( volatile Uint32* )( AEMIF_BASE + 0x40 )
#define AEMIF_EIMR              *( volatile Uint32* )( AEMIF_BASE + 0x44 )
#define AEMIF_EIMSR             *( volatile Uint32* )( AEMIF_BASE + 0x48 )
#define AEMIF_EIMCR             *( volatile Uint32* )( AEMIF_BASE + 0x4C )
#define AEMIF_NANDFCR           *( volatile Uint32* )( AEMIF_BASE + 0x60 )
#define AEMIF_NANDFSR           *( volatile Uint32* )( AEMIF_BASE + 0x64 )
#define AEMIF_NANDECC2          *( volatile Uint32* )( AEMIF_BASE + 0x70 )
#define AEMIF_NANDECC3          *( volatile Uint32* )( AEMIF_BASE + 0x74 )
#define AEMIF_NANDECC4          *( volatile Uint32* )( AEMIF_BASE + 0x78 )
#define AEMIF_NANDECC5          *( volatile Uint32* )( AEMIF_BASE + 0x7C )

#define AEMIF_MAX_TIMEOUT_8BIT  0x3FFFFFFC
#define AEMIF_MAX_TIMEOUT_16BIT 0x3FFFFFFD

#define EMIF_CS2                2
#define EMIF_CS3                3
#define EMIF_CS4                4
#define EMIF_CS5                5

#define EMIF_CS0_BASE           0x02000000
#define EMIF_CS1_BASE           0x04000000

#define EMIF_NAND_MODE          1
#define EMIF_NORMAL_MODE        0

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Device System Controller                                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define SYS_BASE            0x01C14000
#define REVID               *(unsigned int*)(SYS_BASE + 0x000)  
#define DIEIDR0             *(unsigned int*)(SYS_BASE + 0x008)
#define DIEIDR1             *(unsigned int*)(SYS_BASE + 0x00C)
#define DIEIDR2             *(unsigned int*)(SYS_BASE + 0x010)
#define DIEIDR3             *(unsigned int*)(SYS_BASE + 0x014)
#define DEVIDR0             *(unsigned int*)(SYS_BASE + 0x018)
#define DEVIDR1             *(unsigned int*)(SYS_BASE + 0x01C)
#define BOOTCFG             *(unsigned int*)(SYS_BASE + 0x020)
#define CHIPREVIDR          *(unsigned int*)(SYS_BASE + 0x024)
#define KICK0R              *(unsigned int*)(SYS_BASE + 0x038)  
#define KICK1R              *(unsigned int*)(SYS_BASE + 0x03c)  
#define HOST0CFG            *(unsigned int*)(SYS_BASE + 0x040)
#define HOST1CFG            *(unsigned int*)(SYS_BASE + 0x044)
#define IRAWSTAT            *(unsigned int*)(SYS_BASE + 0x0E0)  
#define IENSTAT             *(unsigned int*)(SYS_BASE + 0x0E4)  
#define IENSET              *(unsigned int*)(SYS_BASE + 0x0E8)  
#define IENCLR              *(unsigned int*)(SYS_BASE + 0x0EC)  
#define EOI                 *(unsigned int*)(SYS_BASE + 0x0F0)  
#define FLTADDRR            *(unsigned int*)(SYS_BASE + 0x0F4)  
#define FLTSTAT             *(unsigned int*)(SYS_BASE + 0x0F8)  
#define MSTPRI0             *(unsigned int*)(SYS_BASE + 0x110)  
#define MSTPRI1             *(unsigned int*)(SYS_BASE + 0x114)  
#define MSTPRI2             *(unsigned int*)(SYS_BASE + 0x118) 
#define PINMUX0				*(unsigned int*)(SYS_BASE + 0x120)
#define PINMUX1				*(unsigned int*)(SYS_BASE + 0x124)
#define PINMUX2				*(unsigned int*)(SYS_BASE + 0x128)
#define PINMUX3				*(unsigned int*)(SYS_BASE + 0x12C)
#define PINMUX4				*(unsigned int*)(SYS_BASE + 0x130)
#define PINMUX5				*(unsigned int*)(SYS_BASE + 0x134)
#define PINMUX6				*(unsigned int*)(SYS_BASE + 0x138)
#define PINMUX7				*(unsigned int*)(SYS_BASE + 0x13C)
#define PINMUX8				*(unsigned int*)(SYS_BASE + 0x140)
#define PINMUX9				*(unsigned int*)(SYS_BASE + 0x144)
#define PINMUX10			*(unsigned int*)(SYS_BASE + 0x148)
#define PINMUX11			*(unsigned int*)(SYS_BASE + 0x14C)
#define PINMUX12			*(unsigned int*)(SYS_BASE + 0x150)
#define PINMUX13			*(unsigned int*)(SYS_BASE + 0x154)
#define PINMUX14			*(unsigned int*)(SYS_BASE + 0x158)
#define PINMUX15			*(unsigned int*)(SYS_BASE + 0x15C)
#define PINMUX16			*(unsigned int*)(SYS_BASE + 0x160)
#define PINMUX17			*(unsigned int*)(SYS_BASE + 0x164)
#define PINMUX18			*(unsigned int*)(SYS_BASE + 0x168)
#define PINMUX19			*(unsigned int*)(SYS_BASE + 0x16C)
#define SUSPSRC		 	    *(unsigned int*)(SYS_BASE + 0x170) 
#define CHIPSIG	            *(unsigned int*)(SYS_BASE + 0x174) 
#define CHIPSIG_CLR         *(unsigned int*)(SYS_BASE + 0x178) 
#define CFGCHIP0            *(unsigned int*)(SYS_BASE + 0x17C) 
#define CFGCHIP1            *(unsigned int*)(SYS_BASE + 0x180) 
#define CFGCHIP2            *(unsigned int*)(SYS_BASE + 0x184) 
#define CFGCHIP3           	*(unsigned int*)(SYS_BASE + 0x188) 
#define CFGCHIP4		 	*(unsigned int*)(SYS_BASE + 0x18C) 


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  GPIO Control                                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define GPIO_BASE               0x01E26000

#define GPIO_PCR                *( volatile Uint32* )( GPIO_BASE + 0x04 )
#define GPIO_BINTEN             *( volatile Uint32* )( GPIO_BASE + 0x08 )

// For GPIO[0:31]
#define GPIO_DIR_BASE           ( 0x10 )   // Direction Cntl
#define GPIO_OUT_DATA_BASE      ( 0x14 )   // Output data
#define GPIO_SET_DATA_BASE      ( 0x18 )   // Set data
#define GPIO_CLR_DATA_BASE      ( 0x1C )   // Clear data
#define GPIO_IN_DATA_BASE       ( 0x20 )   // Input data
#define GPIO_SET_RIS_TRIG_BASE  ( 0x24 )   // Set rising edge intr
#define GPIO_CLR_RIS_TRIG_BASE  ( 0x28 )   // Clear rising edge intr
#define GPIO_SET_FAL_TRIG_BASE  ( 0x2C )   // Set falling edge intr
#define GPIO_CLR_FAL_TRIG_BASE  ( 0x30 )   // Clear falling edge intr
#define GPIO_INSTAT_BASE        ( 0x34 )   // Intr status
#define GPIO_BASE_OFFSET        0x28

#define GPIO_01_BASE            ( GPIO_BASE + 0x10 )
#define GPIO_23_BASE            ( GPIO_01_BASE + GPIO_BASE_OFFSET )
#define GPIO_45_BASE            ( GPIO_23_BASE + GPIO_BASE_OFFSET )
#define GPIO_6_BASE             ( GPIO_45_BASE + GPIO_BASE_OFFSET )

// For GPIO[0:31]
#define GPIO_DIR01              *( volatile Uint32* )( GPIO_BASE + 0x10 )
#define GPIO_OUT_DATA01         *( volatile Uint32* )( GPIO_BASE + 0x14 )
#define GPIO_SET_DATA01         *( volatile Uint32* )( GPIO_BASE + 0x18 )
#define GPIO_CLR_DATA01         *( volatile Uint32* )( GPIO_BASE + 0x1C )
#define GPIO_IN_DATA01          *( volatile Uint32* )( GPIO_BASE + 0x20 )
#define GPIO_SET_RIS_TRIG01     *( volatile Uint32* )( GPIO_BASE + 0x24 )
#define GPIO_CLR_RIS_TRIG01     *( volatile Uint32* )( GPIO_BASE + 0x28 )
#define GPIO_SET_FAL_TRIG01     *( volatile Uint32* )( GPIO_BASE + 0x2C )
#define GPIO_CLR_FAL_TRIG01     *( volatile Uint32* )( GPIO_BASE + 0x30 )
#define GPIO_INSTAT01           *( volatile Uint32* )( GPIO_BASE + 0x34 )

// For GPIO[32:63]
#define GPIO_DIR23              *( volatile Uint32* )( GPIO_BASE + 0x38 )
#define GPIO_OUT_DATA23         *( volatile Uint32* )( GPIO_BASE + 0x3C )
#define GPIO_SET_DATA23         *( volatile Uint32* )( GPIO_BASE + 0x40 )
#define GPIO_CLR_DATA23         *( volatile Uint32* )( GPIO_BASE + 0x44 )
#define GPIO_IN_DATA23          *( volatile Uint32* )( GPIO_BASE + 0x48 )
#define GPIO_SET_RIS_TRIG23     *( volatile Uint32* )( GPIO_BASE + 0x4C )
#define GPIO_CLR_RIS_TRIG23     *( volatile Uint32* )( GPIO_BASE + 0x50 )
#define GPIO_SET_FAL_TRIG23     *( volatile Uint32* )( GPIO_BASE + 0x54 )
#define GPIO_CLR_FAL_TRIG23     *( volatile Uint32* )( GPIO_BASE + 0x58 )
#define GPIO_INSTAT23           *( volatile Uint32* )( GPIO_BASE + 0x5C )

// For GPIO[64:70]
#define GPIO_DIR45              *( volatile Uint32* )( GPIO_BASE + 0x60 )
#define GPIO_OUT_DATA45         *( volatile Uint32* )( GPIO_BASE + 0x64 )
#define GPIO_SET_DATA45         *( volatile Uint32* )( GPIO_BASE + 0x68 )
#define GPIO_CLR_DATA45         *( volatile Uint32* )( GPIO_BASE + 0x6C )
#define GPIO_IN_DATA45          *( volatile Uint32* )( GPIO_BASE + 0x70 )
#define GPIO_SET_RIS_TRIG45     *( volatile Uint32* )( GPIO_BASE + 0x74 )
#define GPIO_CLR_RIS_TRIG45     *( volatile Uint32* )( GPIO_BASE + 0x78 )
#define GPIO_SET_FAL_TRIG45     *( volatile Uint32* )( GPIO_BASE + 0x7C )
#define GPIO_CLR_FAL_TRIG45     *( volatile Uint32* )( GPIO_BASE + 0x80 )
#define GPIO_INSTAT45           *( volatile Uint32* )( GPIO_BASE + 0x84 )

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  I2C Controller                                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define I2C_BASE                0x01C22000
#define I2C_OAR                 *( volatile Uint32* )( I2C_BASE + 0x00 )
#define I2C_ICIMR               *( volatile Uint32* )( I2C_BASE + 0x04 )
#define I2C_ICSTR               *( volatile Uint32* )( I2C_BASE + 0x08 )
#define I2C_ICCLKL              *( volatile Uint32* )( I2C_BASE + 0x0C )
#define I2C_ICCLKH              *( volatile Uint32* )( I2C_BASE + 0x10 )
#define I2C_ICCNT               *( volatile Uint32* )( I2C_BASE + 0x14 )
#define I2C_ICDRR               *( volatile Uint32* )( I2C_BASE + 0x18 )
#define I2C_ICSAR               *( volatile Uint32* )( I2C_BASE + 0x1C )
#define I2C_ICDXR               *( volatile Uint32* )( I2C_BASE + 0x20 )
#define I2C_ICMDR               *( volatile Uint32* )( I2C_BASE + 0x24 )
#define I2C_ICIVR               *( volatile Uint32* )( I2C_BASE + 0x28 )
#define I2C_ICEMDR              *( volatile Uint32* )( I2C_BASE + 0x2C )
#define I2C_ICPSC               *( volatile Uint32* )( I2C_BASE + 0x30 )
#define I2C_ICPID1              *( volatile Uint32* )( I2C_BASE + 0x34 )
#define I2C_ICPID2              *( volatile Uint32* )( I2C_BASE + 0x38 )

/* I2C Field Definitions */
#define ICOAR_MASK_7                    0x007F
#define ICOAR_MASK_10                   0x03FF
#define ICSAR_MASK_7                    0x007F
#define ICSAR_MASK_10                   0x03FF
#define ICOAR_OADDR                     0x007f
#define ICSAR_SADDR                     0x0050

#define ICSTR_SDIR                      0x4000
#define ICSTR_NACKINT                   0x2000
#define ICSTR_BB                        0x1000
#define ICSTR_RSFULL                    0x0800
#define ICSTR_XSMT                      0x0400
#define ICSTR_AAS                       0x0200
#define ICSTR_AD0                       0x0100
#define ICSTR_SCD                       0x0020
#define ICSTR_ICXRDY                    0x0010
#define ICSTR_ICRRDY                    0x0008
#define ICSTR_ARDY                      0x0004
#define ICSTR_NACK                      0x0002
#define ICSTR_AL                        0x0001

#define ICMDR_NACKMOD                   0x8000
#define ICMDR_FREE                      0x4000
#define ICMDR_STT                       0x2000
#define ICMDR_IDLEEN                    0x1000
#define ICMDR_STP                       0x0800
#define ICMDR_MST                       0x0400
#define ICMDR_TRX                       0x0200
#define ICMDR_XA                        0x0100
#define ICMDR_RM                        0x0080
#define ICMDR_DLB                       0x0040
#define ICMDR_IRS                       0x0020
#define ICMDR_STB                       0x0010
#define ICMDR_FDF                       0x0008
#define ICMDR_BC_MASK                   0x0007

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  INTC controller                                                         *
 *      Controls the Interrupts                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define INTC_BASE               0xFFFE0000
#define INTC_FIQ0               *( volatile Uint32* )( INTC_BASE + 0x00 )
#define INTC_FIQ1               *( volatile Uint32* )( INTC_BASE + 0x04 )
#define INTC_IRQ0               *( volatile Uint32* )( INTC_BASE + 0x08 )
#define INTC_IRQ1               *( volatile Uint32* )( INTC_BASE + 0x0C )
#define INTC_FIQENTRY           *( volatile Uint32* )( INTC_BASE + 0x10 )
#define INTC_IRQENTRY           *( volatile Uint32* )( INTC_BASE + 0x14 )
#define INTC_EINT0              *( volatile Uint32* )( INTC_BASE + 0x18 )
#define INTC_EINT1              *( volatile Uint32* )( INTC_BASE + 0x1C )
#define INTC_INTCTL             *( volatile Uint32* )( INTC_BASE + 0x20 )
#define INTC_EABASE             *( volatile Uint32* )( INTC_BASE + 0x24 )
#define INTC_INTPRI0            *( volatile Uint32* )( INTC_BASE + 0x30 )
#define INTC_INTPRI1            *( volatile Uint32* )( INTC_BASE + 0x34 )
#define INTC_INTPRI2            *( volatile Uint32* )( INTC_BASE + 0x38 )
#define INTC_INTPRI3            *( volatile Uint32* )( INTC_BASE + 0x3C )
#define INTC_INTPRI4            *( volatile Uint32* )( INTC_BASE + 0x40 )
#define INTC_INTPRI5            *( volatile Uint32* )( INTC_BASE + 0x44 )
#define INTC_INTPRI6            *( volatile Uint32* )( INTC_BASE + 0x48 )
#define INTC_INTPRI7            *( volatile Uint32* )( INTC_BASE + 0x4C )

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Event Controller                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define EVT_BASE               0x01800000
#define EVTFLAG0               *( volatile Uint32* )( EVT_BASE + 0x00 )
#define EVTFLAG1               *( volatile Uint32* )( EVT_BASE + 0x04 )
#define EVTFLAG2               *( volatile Uint32* )( EVT_BASE + 0x08 )
#define EVTFLAG3               *( volatile Uint32* )( EVT_BASE + 0x0c )
#define EVTSET0               *( volatile Uint32* )( EVT_BASE + 0x20 )
#define EVTSET1               *( volatile Uint32* )( EVT_BASE + 0x24 )
#define EVTSET2               *( volatile Uint32* )( EVT_BASE + 0x28 )
#define EVTSET3               *( volatile Uint32* )( EVT_BASE + 0x2c )
#define EVTCLR0               *( volatile Uint32* )( EVT_BASE + 0x40 )
#define EVTCLR1               *( volatile Uint32* )( EVT_BASE + 0x44 )
#define EVTCLR2               *( volatile Uint32* )( EVT_BASE + 0x48 )
#define EVTCLR3               *( volatile Uint32* )( EVT_BASE + 0x4c )

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  PLL0 Controller                                                         *
 *      Generates DSP, ARM clocks                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */


#define PLL0_BASE   	0x01C11000   							/*SYSTEM PLL BASE ADDRESS*/

#define PLL0_PID      	*(unsigned int*) (PLL0_BASE + 0x00)		/*PID*/
#define PLL0_FUSERR   	*(unsigned int*) (PLL0_BASE + 0xE0)		/*x*FuseFarm Error Reg*/
#define PLL0_RSTYPE	    *(unsigned int*) (PLL0_BASE + 0xE4)		/*Reset Type status Reg*/
#define PLL0_PLLCTL     *(unsigned int*) (PLL0_BASE + 0x100) 	/*PLL Control Register*/
#define PLL0_OCSEL      *(unsigned int*) (PLL0_BASE + 0x104)	/*OBSCLK Select Register*/
#define PLL0_SECCTL     *(unsigned int*) (PLL0_BASE + 0x108)	/*PLL Secondary Control Register*/
#define PLL0_PLLM       *(unsigned int*) (PLL0_BASE + 0x110)    /*PLL Multiplier*/	
#define PLL0_PREDIV     *(unsigned int*) (PLL0_BASE + 0x114)    /*Pre divider*/	
#define PLL0_PLLDIV1    *(unsigned int*) (PLL0_BASE + 0x118)    /*Diveder-1*/	
#define PLL0_PLLDIV2    *(unsigned int*) (PLL0_BASE + 0x11C)    /*Diveder-2*/	
#define PLL0_PLLDIV3    *(unsigned int*) (PLL0_BASE + 0x120)    /*Diveder-3*/	
#define PLL0_OSCDIV1    *(unsigned int*) (PLL0_BASE + 0x124)    /*Oscilator Divider*/	
#define PLL0_POSTDIV    *(unsigned int*) (PLL0_BASE + 0x128)    /*Post Divider*/	
#define PLL0_BPDIV      *(unsigned int*) (PLL0_BASE + 0x12C)	/*Bypass Divider*/
#define PLL0_WAKEUP     *(unsigned int*) (PLL0_BASE + 0x130)	/*Wakeup Reg*/
#define PLL0_PLLCMD     *(unsigned int*) (PLL0_BASE + 0x138)	/*Command Reg*/
#define PLL0_PLLSTAT    *(unsigned int*) (PLL0_BASE + 0x13C)	/*Status Reg*/
#define PLL0_ALNCTL     *(unsigned int*) (PLL0_BASE + 0x140)	/*Clock Align Control Reg*/
#define PLL0_DCHANGE    *(unsigned int*) (PLL0_BASE + 0x144)	/*PLLDIV Ratio Chnage status*/
#define PLL0_CKEN       *(unsigned int*) (PLL0_BASE + 0x148)	/*Clock Enable Reg*/
#define PLL0_CKSTAT     *(unsigned int*) (PLL0_BASE + 0x14C)	/*Clock Status Reg*/
#define PLL0_SYSTAT     *(unsigned int*) (PLL0_BASE + 0x150)	/*Sysclk status reg*/
#define PLL0_PLLDIV4    *(unsigned int*) (PLL0_BASE + 0x160)	/*Divider 4*/
#define PLL0_PLLDIV5    *(unsigned int*) (PLL0_BASE + 0x164)	/*Divider 5*/
#define PLL0_PLLDIV6    *(unsigned int*) (PLL0_BASE + 0x168)	/*Divider 6*/
#define PLL0_PLLDIV7    *(unsigned int*) (PLL0_BASE + 0x16C)	/*Divider 7*/
#define PLL0_PLLDIV8    *(unsigned int*) (PLL0_BASE + 0x170)	/*Divider 8*/
#define PLL0_PLLDIV9    *(unsigned int*) (PLL0_BASE + 0x174)	/*Divider 9*/
#define PLL0_PLLDIV10   *(unsigned int*) (PLL0_BASE + 0x178)	/*Divider 10*/
#define PLL0_PLLDIV11   *(unsigned int*) (PLL0_BASE + 0x17C)	/*Divider 11*/
#define PLL0_PLLDIV12   *(unsigned int*) (PLL0_BASE + 0x180)	/*Divider 12*/
#define PLL0_PLLDIV13   *(unsigned int*) (PLL0_BASE + 0x184)	/*Divider 13*/
#define PLL0_PLLDIV14   *(unsigned int*) (PLL0_BASE + 0x188)	/*Divider 14*/
#define PLL0_PLLDIV15   *(unsigned int*) (PLL0_BASE + 0x18C)	/*Divider 15*/
#define PLL0_PLLDIV16   *(unsigned int*) (PLL0_BASE + 0x190)	/*Divider 16*/
#define PLLEN_MUX_SWITCH  4 
#define PLL_LOCK_TIME_CNT 2400


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Timer Controller                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define TIMER0_BASE         0x01C20000
#define TIMER0_EMUMGT       *( volatile Uint32* )( TIMER0_BASE + 0x04 )
#define TIMER0_TIM12        *( volatile Uint32* )( TIMER0_BASE + 0x10 )
#define TIMER0_TIM34        *( volatile Uint32* )( TIMER0_BASE + 0x14 )
#define TIMER0_PRD12        *( volatile Uint32* )( TIMER0_BASE + 0x18 )
#define TIMER0_PRD34        *( volatile Uint32* )( TIMER0_BASE + 0x1C )
#define TIMER0_TRC          *( volatile Uint32* )( TIMER0_BASE + 0x20 )
#define TIMER0_TGCR         *( volatile Uint32* )( TIMER0_BASE + 0x24 )

#define TIMER1_BASE         0x01C21000
#define TIMER1_EMUMGT       *( volatile Uint32* )( TIMER1_BASE + 0x04 )
#define TIMER1_TIM12        *( volatile Uint32* )( TIMER1_BASE + 0x10 )
#define TIMER1_TIM34        *( volatile Uint32* )( TIMER1_BASE + 0x14 )
#define TIMER1_PRD12        *( volatile Uint32* )( TIMER1_BASE + 0x18 )
#define TIMER1_PRD34        *( volatile Uint32* )( TIMER1_BASE + 0x1C )
#define TIMER1_TRC          *( volatile Uint32* )( TIMER1_BASE + 0x20 )
#define TIMER1_TGCR         *( volatile Uint32* )( TIMER1_BASE + 0x24 )

#define TIMER_EMUMGT            ( 0x04 )
#define TIMER_TIM12             ( 0x10 )
#define TIMER_TIM34             ( 0x14 )
#define TIMER_PRD12             ( 0x18 )
#define TIMER_PRD34             ( 0x1C )
#define TIMER_TRC               ( 0x20 )
#define TIMER_TGCR              ( 0x24 )

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  UART Controller                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define UART0_BASE          0x01C42000
#define UART0_RBR           *( volatile Uint32* )( UART0_BASE + 0x00 )
#define UART0_THR           *( volatile Uint32* )( UART0_BASE + 0x00 )
#define UART0_IER           *( volatile Uint32* )( UART0_BASE + 0x04 )
#define UART0_IIR           *( volatile Uint32* )( UART0_BASE + 0x08 )
#define UART0_FCR           *( volatile Uint32* )( UART0_BASE + 0x08 )
#define UART0_LCR           *( volatile Uint32* )( UART0_BASE + 0x0C )
#define UART0_MCR           *( volatile Uint32* )( UART0_BASE + 0x10 )
#define UART0_LSR           *( volatile Uint32* )( UART0_BASE + 0x14 )
#define UART0_DLL           *( volatile Uint32* )( UART0_BASE + 0x20 )
#define UART0_DLH           *( volatile Uint32* )( UART0_BASE + 0x24 )
#define UART0_PID1          *( volatile Uint32* )( UART0_BASE + 0x28 )
#define UART0_PID2          *( volatile Uint32* )( UART0_BASE + 0x2C )
#define UART0_PWREMU_MGMT   *( volatile Uint32* )( UART0_BASE + 0x30 )

#define UART1_BASE          0x01D0C000
#define UART1_RBR           *( volatile Uint32* )( UART1_BASE + 0x00 )
#define UART1_THR           *( volatile Uint32* )( UART1_BASE + 0x00 )
#define UART1_IER           *( volatile Uint32* )( UART1_BASE + 0x04 )
#define UART1_IIR           *( volatile Uint32* )( UART1_BASE + 0x08 )
#define UART1_FCR           *( volatile Uint32* )( UART1_BASE + 0x08 )
#define UART1_LCR           *( volatile Uint32* )( UART1_BASE + 0x0C )
#define UART1_MCR           *( volatile Uint32* )( UART1_BASE + 0x10 )
#define UART1_LSR           *( volatile Uint32* )( UART1_BASE + 0x14 )
#define UART1_DLL           *( volatile Uint32* )( UART1_BASE + 0x20 )
#define UART1_DLH           *( volatile Uint32* )( UART1_BASE + 0x24 )
#define UART1_PID1          *( volatile Uint32* )( UART1_BASE + 0x28 )
#define UART1_PID2          *( volatile Uint32* )( UART1_BASE + 0x2C )
#define UART1_PWREMU_MGMT   *( volatile Uint32* )( UART1_BASE + 0x30 )

#define UART2_BASE          0x01D0D000
#define UART2_THR_PTR       ( UART2_BASE + 0x00 )
#define UART2_RBR_PTR       ( UART2_BASE + 0x00 )
#define UART2_RBR           *( volatile Uint32* )( UART2_BASE + 0x00 )
#define UART2_THR           *( volatile Uint32* )( UART2_BASE + 0x00 )
#define UART2_IER           *( volatile Uint32* )( UART2_BASE + 0x04 )
#define UART2_IIR           *( volatile Uint32* )( UART2_BASE + 0x08 )
#define UART2_FCR           *( volatile Uint32* )( UART2_BASE + 0x08 )
#define UART2_LCR           *( volatile Uint32* )( UART2_BASE + 0x0C )
#define UART2_MCR           *( volatile Uint32* )( UART2_BASE + 0x10 )
#define UART2_LSR           *( volatile Uint32* )( UART2_BASE + 0x14 )
#define UART2_DLL           *( volatile Uint32* )( UART2_BASE + 0x20 )
#define UART2_DLH           *( volatile Uint32* )( UART2_BASE + 0x24 )
#define UART2_PID1          *( volatile Uint32* )( UART2_BASE + 0x28 )
#define UART2_PID2          *( volatile Uint32* )( UART2_BASE + 0x2C )
#define UART2_PWREMU_MGMT   *( volatile Uint32* )( UART2_BASE + 0x30 )

#define UART_RBR                ( 0x00 )
#define UART_THR                ( 0x00 )
#define UART_IER                ( 0x04 )
#define UART_IIR                ( 0x08 )
#define UART_FCR                ( 0x08 )
#define UART_LCR                ( 0x0C )
#define UART_MCR                ( 0x10 )
#define UART_LSR                ( 0x14 )
#define UART_DLL                ( 0x20 )
#define UART_DLH                ( 0x24 )
#define UART_PID1               ( 0x28 )
#define UART_PID2               ( 0x2C )
#define UART_PWREMU_MGMT        ( 0x30 )


/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
/* Board Initialization */
Int16 DSKDA830_init( );

/* Wait Functions */
void DSKDA830_wait( Uint32 delay );
void DSKDA830_waitusec( Uint32 usec );

#endif
