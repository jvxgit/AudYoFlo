/*															  		*
 *  ***********************************************************		*
 *  ***********************************************************		*
 *  																*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Copyright (C) Javox Solutions GmbH - All Rights Reserved		*
 *  Unauthorized copying of this file, via any medium is			*
 *  strictly prohibited. 											*
 *  																*
 *  Proprietary and confidential									*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Written by Hauke Krüger <hk@javox-solutions.com>, 2012-2020		*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Javox Solutions GmbH, Gallierstr. 33, 52074 Aachen				*
 *  																*
 *  ***********************************************************		*
 *  																*
 *  Contact: info@javox-solutions.com, www.javox-solutions.com		*
 *  																*
 *  ********************************************************   		*/
 
#ifndef __JVX_DSP_BASE_CONVERT__H__
#define __JVX_DSP_BASE_CONVERT__H__

#include "jvx_dsp_base.h"

JVX_SYSTEM_LIB_BEGIN

jvxData jvx_uint32_float_qn_32mn(jvxUInt32 val, jvxSize N, jvxCBool twos_complement);
jvxUInt32 jvx_float_uint32_qn_32mn(jvxData val, jvxSize N, jvxCBool twos_complement);

JVX_SYSTEM_LIB_END

#endif
