#define JVX_DEFINE_ALLOCATOR_SYMBOL
#include "jvx_allocators/jvx_allocators.h"

#include "jvx_dsp_base.h"

static jvxSize allocatedMemorySize = 0;

// ========================================================================
jvxHandle* ayf_default_allocator(jvxSize nBytes, jvxCBitField purp, jvxSize szElm)
{
	jvxHandle* retVal = NULL;
	int err = 0;
	if (szElm > 0)
		nBytes *= szElm;

	assert(nBytes > 0);

	allocatedMemorySize += nBytes;

	int heapIdx = purp & JVX_ALLOCATOR_MASK_OUT_JVX;
	int purpAlloc = purp & JVX_ALLOCATOR_MASK_IN_JVX;
	switch (purpAlloc)
	{
	case JVX_ALLOCATOR_ALLOCATE_FIELD:
	case JVX_ALLOCATOR_ALLOCATE_OBJECT:
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(retVal, jvxByte, nBytes);
		break;
	default:
		assert(0);
	}
	
	return retVal;
};

void ayf_default_deallocator(jvxHandle** ptr, jvxCBitField purp)
{
	int err = 0;

	JVX_DSP_SAFE_DELETE_FIELD(*ptr);
	*ptr = NULL;
}

/*
void ayf_default_jumpout(int id)
{

}
*/

struct jvx_allocator_references default_allocator_ref =
{
	ayf_default_allocator,
	ayf_default_deallocator,
	JVX_ALLOCATOR_OBJECT_PURPOSE_UNSPECIFIC,
	//ayf_default_jumpout
};

// ===============================================================================

struct jvx_allocator_references* init_ptr = &default_allocator_ref;
struct jvx_allocator_references* jvx_allocator = &default_allocator_ref;

struct jvx_allocator_references* jvx_get_global_allocator()
{
	return jvx_allocator;
}

int jvx_set_global_allocator(struct jvx_allocator_references* ref)
{
	if (jvx_allocator == init_ptr)
	{
		printf("Update dsp-base allocator handle pointer, so far, <" JVX_PRINTF_CAST_SIZE "> bytes have been allocated by default alloctor.", allocatedMemorySize);
		jvx_allocator = ref;
		return (int)JVX_DSP_NO_ERROR;
	}
	else
	{
		if (ref != jvx_allocator)
		{
			printf("Trying to install another allocator instance which has been installed before");
		}
	}
	return (int)JVX_DSP_ERROR_WRONG_STATE;
}

void jvx_reset_global_allocator()
{
	jvx_allocator = init_ptr;
}
