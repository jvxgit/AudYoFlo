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

#include "jvx_dsp_base.h"
#include "jvx_misc/jvx_convert.h"

static jvxData norm_table_div[32] =
{
	1.0 / (jvxData)((jvxUInt64)1 << (32 - 0)) /* does not make sense */,
	1.0 / (jvxData)(1 << (32 - 1)),
	1.0 / (jvxData)(1 << (32 - 2)),
	1.0 / (jvxData)(1 << (32 - 3)),
	1.0 / (jvxData)(1 << (32 - 4)),
	1.0 / (jvxData)(1 << (32 - 5)),
	1.0 / (jvxData)(1 << (32 - 6)),
	1.0 / (jvxData)(1 << (32 - 7)),
	1.0 / (jvxData)(1 << (32 - 8)),
	1.0 / (jvxData)(1 << (32 - 9)),
	1.0 / (jvxData)(1 << (32 - 10)),
	1.0 / (jvxData)(1 << (32 - 11)),
	1.0 / (jvxData)(1 << (32 - 12)),
	1.0 / (jvxData)(1 << (32 - 13)),
	1.0 / (jvxData)(1 << (32 - 14)),
	1.0 / (jvxData)(1 << (32 - 15)),
	1.0 / (jvxData)(1 << (32 - 16)),
	1.0 / (jvxData)(1 << (32 - 17)),
	1.0 / (jvxData)(1 << (32 - 18)),
	1.0 / (jvxData)(1 << (32 - 19)),
	1.0 / (jvxData)(1 << (32 - 20)),
	1.0 / (jvxData)(1 << (32 - 21)),
	1.0 / (jvxData)(1 << (32 - 22)),
	1.0 / (jvxData)(1 << (32 - 23)),
	1.0 / (jvxData)(1 << (32 - 24)),
	1.0 / (jvxData)(1 << (32 - 25)),
	1.0 / (jvxData)(1 << (32 - 26)),
	1.0 / (jvxData)(1 << (32 - 27)),
	1.0 / (jvxData)(1 << (32 - 28)),
	1.0 / (jvxData)(1 << (32 - 29)),
	1.0 / (jvxData)(1 << (32 - 30)),
	1.0 / (jvxData)(1 << (32 - 31))
};

static jvxData norm_table_mult[32] =
{
	(jvxData)((jvxUInt64)1 << (32 - 0)) /* does not make sense */,
	(jvxData)(1 << (32 - 1)),
	(jvxData)(1 << (32 - 2)),
	(jvxData)(1 << (32 - 3)),
	(jvxData)(1 << (32 - 4)),
	(jvxData)(1 << (32 - 5)),
	(jvxData)(1 << (32 - 6)),
	(jvxData)(1 << (32 - 7)),
	(jvxData)(1 << (32 - 8)),
	(jvxData)(1 << (32 - 9)),
	(jvxData)(1 << (32 - 10)),
	(jvxData)(1 << (32 - 11)),
	(jvxData)(1 << (32 - 12)),
	(jvxData)(1 << (32 - 13)),
	(jvxData)(1 << (32 - 14)),
	(jvxData)(1 << (32 - 15)),
	(jvxData)(1 << (32 - 16)),
	(jvxData)(1 << (32 - 17)),
	(jvxData)(1 << (32 - 18)),
	(jvxData)(1 << (32 - 19)),
	(jvxData)(1 << (32 - 20)),
	(jvxData)(1 << (32 - 21)),
	(jvxData)(1 << (32 - 22)),
	(jvxData)(1 << (32 - 23)),
	(jvxData)(1 << (32 - 24)),
	(jvxData)(1 << (32 - 25)),
	(jvxData)(1 << (32 - 26)),
	(jvxData)(1 << (32 - 27)),
	(jvxData)(1 << (32 - 28)),
	(jvxData)(1 << (32 - 29)),
	(jvxData)(1 << (32 - 30)),
	(jvxData)(1 << (32 - 31))
};

// ===============================================================================

jvxData 
jvx_uint32_float_qn_32mn(jvxUInt32 val, jvxSize N, jvxCBool twos_complement)
{
	jvxData tmp = 0;
	jvxUInt32 valUI32 = val;
	
	// Option for functionality control
	// valUI32 = 0xFFFFFFFF;
	jvxUInt32 neg = 0;
	if (twos_complement == c_true)
	{
		// *val = 0xFFFFFFFF;
		// This implementation uses the default format on Intel, the twos complement
		tmp = JVX_INT32_DATA(*(jvxInt32*)(&valUI32));
		tmp *= norm_table_div[N];
	}
	else
	{
		//valUI32 = 0xFFFFFFFF;
		neg = valUI32 & 0x80000000;
		valUI32 = valUI32 & 0x7FFFFFFF;
		tmp = JVX_UINT32_DATA(valUI32);
		tmp *= norm_table_div[N];
		if (neg != 0)
		{
			tmp *= -1;
		}
	}
	return tmp;
}

jvxUInt32
jvx_float_uint32_qn_32mn(jvxData val, jvxSize N, jvxCBool toos_complement)
{
	jvxData tmp;
	jvxUInt32 retVal;
	jvxInt32 valI32;
	// Option for functionality control
	// val = 32.0;

	jvxUInt32 neg = 0;
	if (toos_complement == c_true)
	{
		// *val = 0xFFFFFFFF;
		// This implementation uses the default format on Intel, the twos complement
		tmp = val * norm_table_mult[N];
		valI32 = JVX_DATA2INT32(tmp);
		retVal = *((jvxUInt32*)&valI32);
	}
	else
	{
		if (val < 0)
		{
			val *= -1.0;
			neg = 1;
		}
		tmp = val * norm_table_mult[N];
		valI32 = JVX_DATA2INT32(tmp);
		retVal = valI32;
		if (neg)
		{
			retVal |= 0x80000000;
		}
	}
	return retVal;
}