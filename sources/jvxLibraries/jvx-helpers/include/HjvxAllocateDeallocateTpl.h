#ifndef __JVX_ALLOCATE_DEALLOCATE_TPL_H__
#define __JVX_ALLOCATE_DEALLOCATE_TPL_H__

#include "jvx_system.h"

template<typename T> void jvx_allocateField_cpp(jvxHandle*& ptr, jvxSize buffersize)
{
	JVX_DSP_SAFE_ALLOCATE_FIELD(ptr, T, buffersize);
	memset(ptr, 0, sizeof(T) *buffersize);
}

template<typename T> void jvx_deallocateField_cpp(jvxHandle*& ptr)
{
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(ptr, T);
}

#endif