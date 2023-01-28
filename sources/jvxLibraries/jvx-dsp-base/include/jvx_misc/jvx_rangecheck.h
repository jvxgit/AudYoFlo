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
 
 #ifndef __JVX_DSP_BASE_RANGECHECK__H__
#define __JVX_DSP_BASE_RANGECHECK__H__

#include "jvx_dsp_base.h"

void jvx_rangeCheck_jvxData(jvxData *x,
                            jvxData min,
                            jvxData max,
                            const char *moduleName,
                            unsigned int id,
                            const char *prmName);

void jvx_rangeCheck_int(int *x,
                        int min,
                        int max,
                        const char *moduleName,
                        unsigned int id,
                        const char *prmName);

void jvx_rangeCheck_int16(jvxInt16 *x,
                          jvxInt16 min,
                          jvxInt16 max,
                          const char *moduleName,
                          unsigned int id,
                          const char *prmName);

void jvx_rangeCheck_jvxSize(jvxSize *x,
                            jvxSize min,
                            jvxSize max,
                            const char *moduleName,
                            unsigned int id,
                            const char *prmName);
#endif
