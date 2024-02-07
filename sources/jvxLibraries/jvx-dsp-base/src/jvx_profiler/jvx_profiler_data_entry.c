#include "jvx_profiler/jvx_profiler_data_entry.h"

void jvx_profiler_allocate_single_entry(struct jvx_profiler_data_entry* entry, jvxSize szFld, jvxCBool cplxFld)
{
	entry->sz = szFld;
	entry->cplx = cplxFld;
	assert(entry->fld == NULL);
	if (entry->cplx)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(entry->fld, jvxDataCplx, entry->sz);
	}
	else
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(entry->fld, jvxData, entry->sz);
	}
}

void jvx_profiler_deallocate_single_entry(struct jvx_profiler_data_entry* entry)
{
	JVX_DSP_SAFE_DELETE_FIELD(entry->fld);
	entry->sz = 0;
	entry->cplx = false;
}