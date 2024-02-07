#ifndef __JVX_PROFILER_DATA_ENTRY_H__
#define __JVX_PROFILER_DATA_ENTRY_H__

#include "jvx_dsp_base.h"

struct jvx_profiler_data_entry
{
	jvxSize sz;
	jvxHandle* fld;
	jvxCBool cplx;
};

typedef jvxErrorType(*jvx_register_entry_profiling_data_c)(struct jvx_profiler_data_entry* dat, const char* name, jvxHandle* inst);
typedef jvxErrorType(*jvx_unregister_entry_profiling_data_c)(const char* name, jvxHandle* inst);

void jvx_profiler_allocate_single_entry(struct jvx_profiler_data_entry* entry, jvxSize szFld, jvxCBool cplxFld);
void jvx_profiler_deallocate_single_entry(struct jvx_profiler_data_entry* entry);

#endif
