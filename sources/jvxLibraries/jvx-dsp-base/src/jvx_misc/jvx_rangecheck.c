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

#include <stdio.h>
#include <assert.h>
#include "jvx_misc/jvx_rangecheck.h"

void jvx_rangeCheck_jvxData(jvxData *x,
                            jvxData min,
                            jvxData max,
                            const char *moduleName,
                            unsigned int id,
                            const char *prmName)
{
  jvxData y = *x;
  assert(max>=min);
  if(*x<min)
    y = min;
  if(*x>max)
    y = max;
  if(*x!=y)
    {
      fprintf(stderr, "[%s/%i] WARNING: range check failed for \"%s\" (val=%g, min=%g, max=%g, newVal=%g)\n",
              moduleName, id, prmName, *x, min, max, y);
      *x = y;
    }
}

void jvx_rangeCheck_int(int *x,
                        int min,
                        int max,
                        const char *moduleName,
                        unsigned int id,
                        const char *prmName)
{
  int y = *x;
  assert(max>=min);
  if(*x<min)
    y = min;
  if(*x>max)
    y = max;
  if(*x!=y)
    {
      fprintf(stderr, "[%s/%i] WARNING: range check failed for \"%s\" (val=%i, min=%i, max=%i, newVal=%i)\n",
              moduleName, id, prmName, *x, min, max, y);
      *x = y;
    }
}

void jvx_rangeCheck_int16(jvxInt16 *x,
                          jvxInt16 min,
                          jvxInt16 max,
                          const char *moduleName,
                          unsigned int id,
                          const char *prmName)
{
  int y = *x;
  assert(max>=min);
  if(*x<min)
    y = min;
  if(*x>max)
    y = max;
  if(*x!=y)
    {
      fprintf(stderr, "[%s/%i] WARNING: range check failed for \"%si\" (val=%i, min=%i, max=%i, newVal=%i)\n",
              moduleName, id, prmName, *x, min, max, y);
      *x = y;
    }
}

void jvx_rangeCheck_jvxSize(jvxSize *x,
                            jvxSize min,
                            jvxSize max,
                            const char *moduleName,
                            unsigned int id,
                            const char *prmName)
{
  jvxSize y = *x;
  assert(max>=min);
  if(*x<min)
    y = min;
  if(*x>max)
    y = max;
  if(*x!=y)
    {
      fprintf(stderr, "[%s/%i] WARNING: range check failed for \"%si\" (val=%i, min=%i, max=%i, newVal=%i)\n",
              moduleName, id, prmName, (int)*x, (int)min, (int)max, (int)y);
      *x = y;
    }
}
