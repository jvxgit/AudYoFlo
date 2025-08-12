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
#include "jvx_misc/jvx_allocate_fields.h"

void jvx_allocateField_c_a(jvxHandle** ptr, jvxSize buffersize, jvxSize elemSize, jvxSize addSpace, jvxSize* szBytes, 
	ldata_allocate falloc, jvxHandle* fpriv)
{
	jvxSize sz = elemSize * buffersize;
	sz += addSpace;
	if (falloc)
	{
		falloc(ptr, sz, fpriv);
	}
	else
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(*ptr, jvxByte, sz);
	}
	if (szBytes)
		*szBytes = sz;
}

void jvx_deallocateField_c_a(jvxHandle** ptr, ldata_deallocate fdealloc, jvxHandle* fpriv)
{
	if (fdealloc)
	{
		fdealloc(ptr, fpriv);
	}
	else
	{
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(*ptr, jvxByte);
	}
	*ptr = NULL;
}

void jvx_fp_stat_allocate_z(struct jvxFloatingPointer** ptr, jvxSize sz)
{
	jvxByte* ptrNew = NULL;
	if (ptr)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(ptrNew, jvxByte, sz);
		memset(ptrNew, 0, sz);
		*ptr = (struct jvxFloatingPointer*)ptrNew;
		(*ptr)->sz = sz;
		(*ptr)->tagid = JVX_SIZE_UNSELECTED;
	}
}

void jvx_fp_stat_deallocate(struct jvxFloatingPointer* ptr, jvxHandle* priv)
{
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(ptr, jvxByte);
}

void jvx_fp_stat_copy(struct jvxFloatingPointer** ptrOut, struct jvxFloatingPointer* ptr)
{
	jvxByte* newPtr = NULL;
	if (ptr && ptrOut)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(newPtr, jvxByte, ptr->sz);
		memcpy(newPtr, ptr, ptr->sz);
		*ptrOut = (struct jvxFloatingPointer*)newPtr;
	}
}
