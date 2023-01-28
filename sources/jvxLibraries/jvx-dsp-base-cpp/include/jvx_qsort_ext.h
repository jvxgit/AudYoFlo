#ifndef __JVX_QSORT_EXT_H__
#define __JVX_QSORT_EXT_H__

#include "jvx.h"

void jvx_qsort_ext(void* aa, size_t n, size_t es, int (*cmp)(const void*, const void*), void* bref, void* aref = NULL);

#endif