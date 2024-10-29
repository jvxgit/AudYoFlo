#include "jvx_profiler/jvx_profiler_data_entry.h"

void jvx_profiler_allocate_single_entry(struct jvx_profiler_data_entry* entry, jvxSize szFld, jvxCBool cplxFld)
{
	entry->sz_elm = szFld;
	entry->cplx = cplxFld;
	entry->c_to_matlab = c_true;
	entry->valid_content = c_true;
	assert(entry->fld == NULL);
	if (entry->cplx)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(entry->fld, jvxDataCplx, entry->sz_elm);
	}
	else
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(entry->fld, jvxData, entry->sz_elm);
	}
}

void jvx_profiler_deallocate_single_entry(struct jvx_profiler_data_entry* entry)
{
	JVX_DSP_SAFE_DELETE_FIELD(entry->fld);
	entry->sz_elm = 0;
	entry->cplx = c_false;
	entry->c_to_matlab = c_false;
}