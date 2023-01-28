#ifndef __JVX_ALLOCATE_H__
#define __JVX_ALLOCATE_H__

#include "jvx_system.h"

JVX_SYSTEM_LIB_BEGIN

void jvx_fp_stat_allocate_z(struct jvxFloatingPointer** ptr, jvxSize sz);
void jvx_fp_stat_deallocate(struct jvxFloatingPointer* ptr, jvxHandle* priv);
void jvx_fp_stat_copy(struct jvxFloatingPointer** ptrOut, struct jvxFloatingPointer* ptr);

typedef void(*ldata_allocate)(jvxHandle** ptr, jvxSize szBytes, jvxHandle* priv);
typedef void(*ldata_deallocate)(jvxHandle** ptr, jvxHandle* priv);

JVX_SYSTEM_LIB_END

#ifdef __cplusplus

// IMPORTANT: WE NEED TO BE CAREFUL IN SETTING THE MEMORY TO ZERO IF ALLOCATED IN C++!!
// We need to be careful to NOT set the content to zero if, e.g., we have strings in the list/object
#define JVX_SAFE_ALLOCATE_OBJECT(retPtr, dataType) { retPtr = new dataType; }
#define JVX_SAFE_ALLOCATE_OBJECT_CPP_Z(retPtr, dataType) { retPtr = new dataType; memset(retPtr, 0, sizeof(dataType)); }
#define JVX_SAFE_ALLOCATE_FIELD(retPtr, dataType, numElements) { retPtr = new dataType[numElements]; }
#define JVX_SAFE_ALLOCATE_FIELD_CPP_Z(retPtr, dataType, numElements) { retPtr = new dataType[numElements];  memset(retPtr, 0, sizeof(dataType)* (numElements)); }
#define JVX_SAFE_DELETE_OBJECT(ptr) { if(ptr) delete(ptr); ptr = nullptr; }
#define JVX_SAFE_DELETE_FIELD(ptr) { if(ptr) delete[](ptr); ptr = nullptr; }
#define JVX_SAFE_DELETE_FIELD_TYPE(ptr, type) { if(ptr) delete[]((type*)ptr); ptr = nullptr; }

#else

#include <stdlib.h>
#include <string.h>

// In C, 
#define JVX_SAFE_ALLOCATE_OBJECT_Z(retPtr, dataType) { retPtr = (dataType*)malloc(sizeof(dataType)); memset(retPtr, 0, sizeof(dataType)); }
#define JVX_SAFE_ALLOCATE_FIELD_Z(retPtr, dataType, numElements) { retPtr = (dataType*)malloc(sizeof(dataType) * (numElements)); memset(retPtr, 0, sizeof(dataType)* (numElements)); }
//#define JVX_DSP_SAFE_ALLOCATE_FIELD(retPtr, dataType, numElements) { retPtr = (dataType*)malloc(sizeof(dataType) * (numElements)); }
#define JVX_SAFE_DELETE_OBJECT(ptr) { if(ptr) free(ptr); ptr = NULL; }
#define JVX_SAFE_DELETE_FIELD(ptr) { if(ptr) free(ptr); ptr = NULL; }
#define JVX_SAFE_DELETE_FIELD_TYPE(ptr, type) { if(ptr) free((type*)ptr); ptr = NULL; }

#endif

#endif
