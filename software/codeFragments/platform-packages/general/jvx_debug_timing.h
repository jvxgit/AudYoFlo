#ifndef __JVX_DEBUG_TIMING_H__
#define __JVX_DEBUG_TIMING_H__

// =============================================================
#ifdef JVX_DBG_IRQ_DMA

struct jvx_one_dbg_entry
{
	int valint;
	int timeri;
	int timero;
	int user_val[JVX_DBG_NUMBER_USER_VALUES];
};
static JVX_DBG_BUF_PREFIX jvx_one_dbg_entry theDbgBuf[JVX_DBG_CNT_MAX];
static int jvx_dbg_cnt = 0;

static inline int jvx_add_one_dbg_event(jvx_one_dbg_entry* ptradd)
{
	((jvx_one_dbg_entry* volatile)theDbgBuf)[jvx_dbg_cnt] = *ptradd;
	jvx_dbg_cnt++;
	if(jvx_dbg_cnt == JVX_DBG_CNT_MAX)
	{
		jvx_dbg_cnt = 0;
		return 1;
	}
	return 0;
}

#endif
#endif