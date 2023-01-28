/*
 *  Copyright 2008 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Definitions & Register
 *
 */

#ifndef DSKDA830_EDMA3
#define DSKDA830_EDMA3

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  EDMA3 Controller                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
typedef struct 
{
	Uint32 opt;
	Uint32 src;
	Uint32 a_b_cnt;
	Uint32 dst;
	Uint32 src_dst_bidx;
	Uint32 link_bcntrld;
	Uint32 src_dst_cidx;
	Uint32 ccnt;
} edma_paRAM;

#define EDMA3CC              			0x01C00000
#define EDMA3CC_DRAE1 					*( volatile Uint32* )( EDMA3CC + 0x348 )

#define EDMA3CC_REGION1              	0x01C02200
#define EDMA3CC_REGION1_ER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x0 )
#define EDMA3CC_REGION1_ECR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x08 )
#define EDMA3CC_REGION1_ESR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x10 )
#define EDMA3CC_REGION1_CER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x18 )
#define EDMA3CC_REGION1_EER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x20 )
#define EDMA3CC_REGION1_EECR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x28 )
#define EDMA3CC_REGION1_EESR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x30 )
#define EDMA3CC_REGION1_SER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x38 )
#define EDMA3CC_REGION1_SECR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x40 )
#define EDMA3CC_REGION1_IER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x50 )
#define EDMA3CC_REGION1_IECR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x58 )
#define EDMA3CC_REGION1_IESR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x60 )
#define EDMA3CC_REGION1_IPR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x68 )
#define EDMA3CC_REGION1_ICR 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x70 )
#define EDMA3CC_REGION1_IEVAL 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x78 )
#define EDMA3CC_REGION1_QER 				*( volatile Uint32* )( EDMA3CC_REGION1 + 0x80 )

#define EDMA3_PARAMS              	0x01C04000

#define EDMA3_PARAMS_RAM 			  (edma_paRAM*) (EDMA3_PARAMS )
 
#endif
