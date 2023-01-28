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
 
 #ifndef _JVX_ALLOCATE_FIELDS_H__
#define _JVX_ALLOCATE_FIELDS_H__

#include "jvx_system.h"

JVX_SYSTEM_LIB_BEGIN

void jvx_allocateField_c_a(jvxHandle** ptr, jvxSize buffersize, jvxSize elemSize, jvxSize addSpace, jvxSize* szBytes, 
	ldata_allocate falloc, jvxHandle* fpriv);
void jvx_deallocateField_c_a(jvxHandle** ptr,
	ldata_deallocate fdealloc, jvxHandle* fpriv);

JVX_SYSTEM_LIB_END

#endif