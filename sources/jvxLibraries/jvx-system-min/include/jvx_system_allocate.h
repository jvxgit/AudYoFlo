#ifndef __JVX_ALLOCATE_H__
#define __JVX_ALLOCATE_H__

#include "jvx_system.h"

JVX_SYSTEM_LIB_BEGIN

void jvx_fp_stat_allocate_z(struct jvxFloatingPointer** ptr, jvxSize sz);
void jvx_fp_stat_deallocate(struct jvxFloatingPointer* ptr, jvxHandle* priv);
void jvx_fp_stat_copy(struct jvxFloatingPointer** ptrOut, struct jvxFloatingPointer* ptr);

typedef void(*ldata_allocate)(jvxHandle** ptr, jvxSize szBytes, jvxHandle* priv);
typedef void(*ldata_deallocate)(jvxHandle** ptr, jvxHandle* priv);

// ===================================================================================
// Declare some specific allocator functions combined in a struct
// ===================================================================================
typedef enum
{
	JVX_ALLOCATOR_OBJECT_PURPOSE_UNSPECIFIC,
	JVX_ALLOCATOR_OBJECT_PURPOSE_TIME_CRITICAL,
	JVX_ALLOCATOR_OBJECT_PURPOSE_NON_TIME_CRITICAL
} jvxAllocatorObjectPurpose;

#define JVX_ALLOCATOR_ALLOCATE_FIELD 0x1000
#define JVX_ALLOCATOR_ALLOCATE_OBJECT 0x2000
#define JVX_ALLOCATOR_MASK_OUT_JVX 0x0FFF
#define JVX_ALLOCATOR_MASK_IN_JVX 0xF000

// Following memory specifications according to AWE
#ifndef JVX_MEMORY_ALLOCATE_FAST_A
	#define JVX_MEMORY_ALLOCATE_FAST_A 0
#endif

#ifndef JVX_MEMORY_ALLOCATE_FAST_B
	#define JVX_MEMORY_ALLOCATE_FAST_B 1
#endif
	
#ifndef JVX_MEMORY_ALLOCATE_SLOW	
	#define JVX_MEMORY_ALLOCATE_SLOW 2
#endif
	
#ifndef JVX_MEMORY_ALLOCATE_FAST_SLOW	
	#define JVX_MEMORY_ALLOCATE_FAST_SLOW 3
#endif
	
#ifndef JVX_MEMORY_ALLOCATE_FAST_FASTB	
	#define JVX_MEMORY_ALLOCATE_FAST_FASTB 4
#endif
	
#ifndef JVX_MEMORY_ALLOCATE_FASTB_FAST
	#define JVX_MEMORY_ALLOCATE_FASTB_FAST 5
#endif

typedef jvxHandle* (*jvx_allocator_fc)(jvxSize nElements, jvxCBitField purp, jvxSize szElm);
typedef void (*jvx_deallocator_fc)(jvxHandle** fld, jvxCBitField purp);
// typedef void (*jvx_jumpout_fc)(int id);

struct jvx_allocator_references
{
	jvx_allocator_fc alloc;
	jvx_deallocator_fc dealloc;
	jvxAllocatorObjectPurpose purpose;
	// jvx_jumpout_fc jout;
};

// ===================================================================================
// ===================================================================================

JVX_SYSTEM_LIB_END

#ifdef __cplusplus

#if defined(JVX_OS_WINDOWS) && defined(JVX_TRACK_MEMORY_LEAKS)

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define NEW_OPERATOR new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define NEW_OPERATOR new
#endif

// IMPORTANT: WE NEED TO BE CAREFUL IN SETTING THE MEMORY TO ZERO IF ALLOCATED IN C++!!
// We need to be careful to NOT set the content to zero if, e.g., we have strings in the list/object
#define JVX_SAFE_ALLOCATE_OBJECT(retPtr, dataType) { retPtr = NEW_OPERATOR dataType; }
#define JVX_SAFE_ALLOCATE_OBJECT_CPP_Z(retPtr, dataType) { retPtr = NEW_OPERATOR dataType; memset(retPtr, 0, sizeof(dataType)); }
#define JVX_SAFE_ALLOCATE_FIELD(retPtr, dataType, numElements) { retPtr = NEW_OPERATOR dataType[numElements]; }
#define JVX_SAFE_ALLOCATE_FIELD_CPP_Z(retPtr, dataType, numElements) { retPtr = NEW_OPERATOR dataType[numElements];  memset(retPtr, 0, sizeof(dataType)* (numElements)); }
#define JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(retPtr, dataType, dimY, dimX) \
{ \
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(retPtr, dataType*, dimY); \
	for(jvxSize i = 0; i < dimY; i++) \
	{ \
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(retPtr[i], dataType, dimX); \
	} \
}

#define JVX_SAFE_DELETE_OBJECT(ptr) { if(ptr) delete(ptr); ptr = nullptr; }
#define JVX_SAFE_DELETE_FIELD(ptr) { if(ptr) delete[](ptr); ptr = nullptr; }
#define JVX_SAFE_DELETE_2DFIELD(ptr, dimY) \
{ \
	if(ptr) \
	{ \
		for(jvxSize i = 0; i < dimY; i++) \
		{ \
			JVX_SAFE_DELETE_FIELD(ptr[i]); \
		} \
		JVX_SAFE_DELETE_FIELD(ptr); \
	} \
}

#define JVX_SAFE_DELETE_FIELD_TYPE(ptr, type) { if(ptr) delete[]((type*)ptr); ptr = nullptr; }
#define JVX_SAFE_DELETE_2DFIELD_TYPE(ptr, dimY, type) \
{ \
	if(ptr) \
	{ \
		for(jvxSize i = 0; i < dimY; i++) \
		{ \
			JVX_SAFE_DELETE_FIELD_TYPE(ptr[i], type*); \
		} \
		JVX_SAFE_DELETE_FIELD_TYPE(ptr, type); \
	} \
}

#else

#include <stdlib.h>
#include <string.h>

// In C, 
#define JVX_SAFE_ALLOCATE_OBJECT_Z(retPtr, dataType) { retPtr = (dataType*)malloc(sizeof(dataType)); memset(retPtr, 0, sizeof(dataType)); }
#define JVX_SAFE_ALLOCATE_FIELD_Z(retPtr, dataType, numElements) { retPtr = (dataType*)malloc(sizeof(dataType) * (numElements)); memset(retPtr, 0, sizeof(dataType)* (numElements)); }
#define JVX_SAFE_ALLOCATE_FIELD_2D_Z(retPtr, dataType, dimY, dimX) \
{ \
	JVX_SAFE_ALLOCATE_FIELD_Z(retPtr, dataType*, dimY); \
	for(jvxSize i = 0; i < dimY; i++) \
	{ \
		JVX_SAFE_ALLOCATE_FIELD_Z(retPtr[i], dataType, dimX); \
	} \
}

//#define JVX_DSP_SAFE_ALLOCATE_FIELD(retPtr, dataType, numElements) { retPtr = (dataType*)malloc(sizeof(dataType) * (numElements)); }
#define JVX_SAFE_DELETE_OBJECT(ptr) { if(ptr) free(ptr); ptr = NULL; }
#define JVX_SAFE_DELETE_FIELD(ptr) { if(ptr) free(ptr); ptr = NULL; }
#define JVX_SAFE_DELETE_FIELD_2D(retPtr, dimY) \
{ \
	for(jvxSize i = 0; i < dimY; i++) \
	{ \
		JVX_SAFE_DELETE_FIELD(retPtr[i]); \
	} \
	JVX_SAFE_DELETE_FIELD(retPtr); \
}

#define JVX_SAFE_DELETE_FIELD_TYPE(ptr, type) { if(ptr) free((type*)ptr); ptr = NULL; }

#endif

#endif
