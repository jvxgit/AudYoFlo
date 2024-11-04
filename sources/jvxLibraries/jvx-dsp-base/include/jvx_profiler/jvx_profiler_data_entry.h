#ifndef __JVX_PROFILER_DATA_ENTRY_H__
#define __JVX_PROFILER_DATA_ENTRY_H__

#include "jvx_dsp_base.h"

struct jvx_profiler_data_entry
{
	jvxSize sz_elm;
	jvxHandle* fld;
	jvxCBool cplx;
	jvxCBool c_to_matlab;
	jvxCBool valid_content;
};

typedef jvxErrorType(*jvx_register_entry_profiling_data_c)(struct jvx_profiler_data_entry* dat, const char* name, jvxHandle* inst);
typedef jvxErrorType(*jvx_unregister_entry_profiling_data_c)(const char* name, jvxHandle* inst);

void jvx_profiler_allocate_single_entry(struct jvx_profiler_data_entry* entry, jvxSize szFld, jvxCBool cplxFld);
void jvx_profiler_deallocate_single_entry(struct jvx_profiler_data_entry* entry);

#if JVX_ALLOW_PROFILER_DATA_MACROS

#define JVX_DATA_OUT_DBG_TP(hdl, prv, spec, ptrTo, ptrFrom, szTo, szFrom, cplx) \
	if (hdl && (hdl->specData & ((jvxCBitField)1 << spec)) && hdl->ptrTo && prv->ptrFrom) \
	{ \
		assert(hdl->szTo == (szFrom)); \
		if(cplx) \
		{ \
			memcpy(hdl->ptrTo, prv->ptrFrom, sizeof(jvxDataCplx) * (szFrom)); \
		} \
		else \
		{ \
			memcpy(hdl->ptrTo, prv->ptrFrom, sizeof(jvxData) * (szFrom)); \
		} \
	}

#define JVX_DATA_OUT_DBG_TP_STR(hdl, prv, spec, entryTo, ptrFrom, szFrom) \
	if (hdl && (hdl->specData & ((jvxCBitField)1 << spec)) &&  hdl->entryTo.fld && ptrFrom) \
	{ \
		assert(hdl->entryTo.sz_elm == (szFrom)); \
		if(hdl->entryTo.cplx) \
		{ \
			memcpy(hdl->entryTo.fld, ptrFrom, sizeof(jvxDataCplx) * (szFrom)); \
		} \
		else \
		{ \
			memcpy(hdl->entryTo.fld, ptrFrom, sizeof(jvxData) * (szFrom)); \
		} \
	}

// Output field through Matlab profiling interface directly
#define JVX_DATA_OUT_DBG_TP_DIRECT(hdl, spec, entryTo, ptrFrom, szFrom) \
	if (hdl && (hdl->specData & ((jvxCBitField)1 << spec)) &&  hdl->entryTo.fld && ptrFrom) \
	{ \
		assert(hdl->entryTo.sz_elm == (szFrom)); \
		if(hdl->entryTo.cplx) \
		{ \
			memcpy(hdl->entryTo.fld, ptrFrom, sizeof(jvxDataCplx) * (szFrom)); \
		} \
		else \
		{ \
			memcpy(hdl->entryTo.fld, ptrFrom, sizeof(jvxData) * (szFrom)); \
		} \
	}

#define JVX_DATA_OUT_DBG_TP_STR_CHECK(var, hdl, spec, entryTo) \
	var = (hdl && (hdl->specData & ((jvxCBitField)1 << spec)) &&  hdl->entryTo.fld);

#define JVX_DATA_OUT_DBG_TP_STR_IDX(var, hdl, prv, entryTo, ptrFrom, idx) \
	if (var && hdl && hdl->entryTo.fld && prv->ptrFrom) \
	{ \
		assert(idx < hdl->entryTo.sz_elm); \
		if(hdl->entryTo.cplx) \
		{ \
			jvxDataCplx* fldOut = (jvxDataCplx*)hdl->entryTo.fld; \
			jvxDataCplx* fldIn = (jvxDataCplx*)prv->ptrFrom; \
			fldOut[idx] = fldIn[idx]; \
		} \
		else \
		{ \
			jvxData* fldOut = (jvxData*)hdl->entryTo.fld; \
			jvxData* fldIn = (jvxData*)prv->ptrFrom; \
			fldOut[idx] = fldIn[idx]; \
		} \
	}
#define JVX_DATA_OUT_DBG_TP_STR_VAL(var, hdl, entryTo, val, tp, idx) \
	if (var&& hdl && hdl->entryTo.fld) \
	{ \
		assert(idx < hdl->entryTo.sz_elm); \
		tp* fldOut = (tp*)hdl->entryTo.fld; \
		fldOut[idx] = (tp)val; \
	}
#define JVX_DATA_OUT_DBG_TP_STR_UC(hdl, prv, entryTo, ptrFrom, szFrom) \
	if (hdl && hdl->specData && hdl->entryTo.fld && prv->ptrFrom) \
	{ \
		assert(hdl->entryTo.sz_elm == (szFrom)); \
		if(hdl->entryTo.cplx) \
		{ \
			memcpy(hdl->entryTo.fld, prv->ptrFrom, sizeof(jvxDataCplx) * (szFrom)); \
		} \
		else \
		{ \
			memcpy(hdl->entryTo.fld, prv->ptrFrom, sizeof(jvxData) * (szFrom)); \
		} \
	}
#else
	#define JVX_DATA_OUT_DBG_TP(hdl, prv, spec, ptrTo, ptrFrom, szTo, szFrom, cplx)
	#define JVX_DATA_OUT_DBG_TP_STR(hdl, prv, spec, entryTo, ptrFrom, szFrom) 
	#define JVX_DATA_OUT_DBG_TP_DIRECT(hdl, spec, entryTo, ptrFrom, szFrom)
	#define JVX_DATA_OUT_DBG_TP_STR_CHECK(var, hdl, spec, entryTo) 
	#define JVX_DATA_OUT_DBG_TP_STR_IDX(var, hdl, prv, entryTo, ptrFrom, idx) 
	#define JVX_DATA_OUT_DBG_TP_STR_VAL(var, hdl, entryTo, val, tp, idx) 
	#define JVX_DATA_OUT_DBG_TP_STR_UC(hdl, prv, entryTo, ptrFrom, szFrom) 
#endif

#endif
