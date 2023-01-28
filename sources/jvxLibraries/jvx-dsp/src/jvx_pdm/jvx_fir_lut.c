#include "jvx_dsp_base.h"
#include "jvx_pdm/jvx_fir_lut.h"


#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8) || defined(JVX_TARGET_GCC_ARM_YOCTO)
__attribute__( ( always_inline ) ) static inline uint32_t __RBIT(uint32_t value)
#else
JVX_STATIC_INLINE uint32_t __RBIT(uint32_t value)
#endif
{
  uint32_t result = 0;

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8) || defined(JVX_TARGET_GCC_ARM_YOCTO)

   __asm volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
#else
	  jvxSize j;
	  for (j = 0; j < 32; j++)
	  {
		  result = result << 1;
		  result |= (value & 0x1);
		  value = value >> 1;
	  }
#endif

   return(result);
}


//#define JVX_FIR_BIT_SWAP
//#define JVX_FIR_LUT_WORD16_SWAP
// #define JVX_FIR_LUT_WORD32_SWAP

typedef struct
{
	jvxSize os;
	jvxSize ll0;
	jvxSize llos;
	jvxSize llfrags;
	jvxSize idxCirc;
	jvxSize maskAddr;
	jvxSize maskAddr_2;
	jvxSize incBufLookup;
} jvx_fir_lut_prv;

jvxDspBaseErrorType jvx_pdm_fir_lut_initConfig(jvx_fir_lut* hdl)
{	
	hdl->prm_init.expSz = 3;
	hdl->derived.lCirc16Bit = 0;
	hdl->derived.lLookup16Bit = 0;

	hdl->prm_sync.circ_buf = NULL;
	hdl->prm_sync.ptr_lookup = NULL;
	hdl->prm_sync.ptr_revAddr = NULL;

	hdl->prv = NULL;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_pdm_fir_lut_terminate(jvx_fir_lut* hdlOnReturn)
{
	if (!hdlOnReturn)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType jvx_pdm_fir_lut_prepare(jvx_fir_lut* hdl)
{
	if (hdl)
	{
		if (hdl->prv == NULL)
		{
			jvx_fir_lut_prv* myPrv = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(myPrv, jvx_fir_lut_prv);

			myPrv->os = 16;
			myPrv->ll0 = ((jvxSize)1 << hdl->prm_init.expSz);
			myPrv->llos = myPrv->ll0 * myPrv->os;
			myPrv->llfrags = myPrv->ll0 * 2;
			myPrv->idxCirc = 0;
			myPrv->maskAddr = myPrv->ll0 - 1;
			myPrv->maskAddr_2 = myPrv->llfrags - 1;
			myPrv->incBufLookup = 1 << 8;

			hdl->derived.lCirc16Bit = myPrv->ll0;
			hdl->derived.lLookup16Bit = myPrv->ll0 * myPrv->incBufLookup;

			hdl->prv = myPrv;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_pdm_fir_lut_postprocess(jvx_fir_lut* hdl)
{
	if (hdl)
	{
		if (hdl->prv )
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;
	
			JVX_DSP_SAFE_DELETE_OBJECT(myPrv);
			hdl->prv = NULL;

			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_pdm_fir_lut_process(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT in_i, jvxInt16 * JVX_RESTRICT out_i, jvxSize fsize)
{
	jvxInt32 accu;
	jvxSize i, j;
	jvxUInt8* bufTmp1 = NULL;
	jvxSize fwdCnt = 0;
	jvxSize bwdCnt = 0;
	jvxInt16* ptrLookup;
	jvxUInt16 idx;
	jvxSize idxCircL = 0;
	jvxUInt16 accu16;
#if defined(JVX_FIR_LUT_WORD16_SWAP) || defined(JVX_FIR_BIT_SWAP)
	jvxUInt16 accu16r;
#endif

	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;

			idxCircL = myPrv->idxCirc;

#ifdef JVX_FIR_LUT_WORD16_SWAP
			for (i = 0; i < fsize / 2; i++)
			{
				accu16 = *in_i++;
				accu16r = *in_i--;
				*in_i++ = accu16r;
				*in_i++ = accu16;
			}
#endif

			for (i = 0; i < fsize; i++)
			{
				accu16 = *in_i++;

#ifdef JVX_FIR_LUT_BYTE_SWAP
				accu16 = ((accu16 & 0xFF00) >> 8) | ((accu16 & 0x00FF) << 8);
#endif

#ifdef JVX_FIR_BIT_SWAP
				accu16r = 0;
				for(j = 0; j < 16; j++)
				{
					accu16r = accu16r << 1;
					accu16r |= (accu16 & 0x1);
					accu16 = accu16 >> 1;
				}
				hdl->prm_sync.circ_buf[idxCircL] = accu16r;
#else
				hdl->prm_sync.circ_buf[idxCircL] = accu16;
#endif
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				
				// Inner loop cnt start value
				bwdCnt = idxCircL << 1;
				fwdCnt = (bwdCnt - 1) & myPrv->maskAddr_2; // Wrap around for "bwdCNt == 0"

				bufTmp1 = (jvxUInt8*)hdl->prm_sync.circ_buf;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				for (j = 0; j < myPrv->ll0 - 1; j++)
				{
					idx = bufTmp1[fwdCnt];
					accu += ptrLookup[idx];

					idx = bufTmp1[bwdCnt];
					idx = hdl->prm_sync.ptr_revAddr[idx];
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;
					bwdCnt = (bwdCnt + 1) & myPrv->maskAddr_2;
					fwdCnt = (fwdCnt - 1) & myPrv->maskAddr_2;
				}

				idx = bufTmp1[fwdCnt];
				accu += ptrLookup[idx];

				idx = bufTmp1[bwdCnt];
				idx = hdl->prm_sync.ptr_revAddr[idx];
				accu += ptrLookup[idx];

				*out_i++ = accu & 0xFFFF;
				
			}
			myPrv->idxCirc = idxCircL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

jvxDspBaseErrorType jvx_pdm_fir_lut_process_ip(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT inout, jvxSize fsize)
{
	jvxInt32 accu;
	jvxSize i, j;
	jvxUInt8* bufTmp1 = NULL;
	jvxSize fwdCnt = 0;
	jvxSize bwdCnt = 0;
	jvxInt16* ptrLookup;
	jvxUInt16 idx;
	jvxSize idxCircL = 0;
	jvxUInt16 accu16;
#if defined(JVX_FIR_LUT_WORD16_SWAP) || defined(JVX_FIR_BIT_SWAP)
	jvxUInt16 accu16r;
#endif
#ifdef JVX_FIR_LUT_WORD16_SWAP
	jvxInt16 * myPtr = inout;
#endif
	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;

			idxCircL = myPrv->idxCirc;

#ifdef JVX_FIR_LUT_WORD16_SWAP
			for (i = 0; i < fsize / 2; i++)
			{
				accu16 = *myPtr++;
				accu16r = *myPtr--;
				*myPtr++ = accu16r;
				*myPtr++ = accu16;
			}
#endif

			for (i = 0; i < fsize; i++)
			{
				//hdl->prm_sync.circ_buf[idxCircL] = *inout;
				accu16 = *inout;
#ifdef JVX_FIR_LUT_BYTE_SWAP
				accu16 = ((accu16 & 0xFF00) >> 8) | ((accu16 & 0x00FF) << 8);
#endif
#ifdef JVX_FIR_BIT_SWAP
				accu16r = 0;
				for(j = 0; j < 16; j++)
				{
					accu16r = accu16r << 1;
					accu16r |= (accu16 & 0x1);
					accu16 = accu16 >> 1;
				}
				hdl->prm_sync.circ_buf[idxCircL] = accu16r;
#else
				hdl->prm_sync.circ_buf[idxCircL] = accu16;
#endif
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;

				// Inner loop cnt start value
				bwdCnt = idxCircL << 1;
				fwdCnt = (bwdCnt - 1) & myPrv->maskAddr_2; // Wrap around for "bwdCNt == 0"

				bufTmp1 = (jvxUInt8*)hdl->prm_sync.circ_buf;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				for (j = 0; j < myPrv->ll0 - 1; j++)
				{
					idx = bufTmp1[fwdCnt];
					accu += ptrLookup[idx];

					idx = bufTmp1[bwdCnt];
					idx = hdl->prm_sync.ptr_revAddr[idx];
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;
					bwdCnt = (bwdCnt + 1) & myPrv->maskAddr_2;
					fwdCnt = (fwdCnt - 1) & myPrv->maskAddr_2;
				}

				idx = bufTmp1[fwdCnt];
				accu += ptrLookup[idx];

				idx = bufTmp1[bwdCnt];
				idx = hdl->prm_sync.ptr_revAddr[idx];
				accu += ptrLookup[idx];

				*inout++ = accu & 0xFFFF;

			}
			myPrv->idxCirc = idxCircL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}


jvxDspBaseErrorType jvx_pdm_fir_lut_process_ip_lwords(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT inout, jvxSize fsize)
{
	jvxInt32 accu;
	jvxSize i, j;
	//jvxUInt8* bufTmp1 = NULL;
	jvxUInt16* bufTmp2 = NULL;
	//jvxSize fwdCnt = 0;
	//jvxSize bwdCnt = 0;
	jvxSize fwdCnt2 = 0;
	jvxSize bwdCnt2 = 0;
	jvxInt16* ptrLookup;
	jvxUInt16 idx;
	//jvxUInt16 idxp;
	jvxSize idxCircL = 0;
	jvxUInt16 accu16, accu16_1, accu16_2;
	jvxUInt32 accu32, accu32r;

#if defined(JVX_FIR_LUT_WORD32_SWAP)
	jvxUInt16 accu16r;
#endif

	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;

			idxCircL = myPrv->idxCirc;
			jvxSize myMask = myPrv->maskAddr_2/2;
			jvxSize lCount = myPrv->ll0 / 2 - 1;

			for (i = 0; i < fsize/2; i++)
			{
				//hdl->prm_sync.circ_buf[idxCircL] = *inout;
				
#if defined(JVX_FIR_LUT_WORD32_SWAP)

				// Here is the opportunity to optimize on Cortex M4: There is one operation which does exactly this!!
				accu32 = *((jvxUInt32*)inout);

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)

				//__asm("RBIT %0, %1"
				//		: "=r"(accu32r)
				//		: "r"(accu32));
				accu32r = __RBIT(accu32);

#else
				// Revert input 32 bit values
				accu32r = 0;
				for (j = 0; j < 32; j++)
				{
					accu32r = accu32r << 1;
					accu32r |= (accu32 & 0x1);
					accu32 = accu32 >> 1;
				}
#endif
#else
				accu32r = *((jvxUInt32*)inout);
#endif

				accu16 = (accu32r & 0xFFFF);
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;
#else
					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;
#else
					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}

				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;
#else
				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;
#else
				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
				accu += ptrLookup[idx];

				*inout++ = accu & 0xFFFF;

				// =================================================

				accu16 = (accu32r & 0xFFFF0000) >> 16;
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;

				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;
#else
					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;
#else

					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}
				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;
#else
				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

#if defined(JVX_TARGET_GCC_CORTEX_M4) || defined(JVX_TARGET_GCC_CORTEX_A8)
				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;
#else
				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];
#endif
				accu += ptrLookup[idx];

				*inout++ = accu & 0xFFFF;

			}
			myPrv->idxCirc = idxCircL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

/*
 * 16 times downsampling: 2 x 32 Bits per UInt32 entry -> output field size is 4 x fsizelr
*/
jvxDspBaseErrorType jvx_pdm_fir_lut_process_32swap_lr(jvx_fir_lut* hdl, jvxUInt32 * JVX_RESTRICT inl,
	jvxUInt32 * JVX_RESTRICT inr, jvxInt16 * JVX_RESTRICT out, jvxSize fsizelr)
{
	jvxInt32 accu;
	jvxSize i, j, ibs;
	//jvxUInt8* bufTmp1 = NULL;
	jvxUInt16* bufTmp2 = NULL;
	//jvxSize fwdCnt = 0;
	//jvxSize bwdCnt = 0;
	jvxSize fwdCnt2 = 0;
	jvxSize bwdCnt2 = 0;
	jvxInt16* ptrLookup;
	jvxUInt16 idx;
	//jvxUInt16 idxp;
	jvxSize idxCircL = 0;
	jvxUInt16 accu16, accu16_1, accu16_2;
	jvxUInt32 accu32, accu32r;

	jvxUInt16 accu16r;

	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;

			idxCircL = myPrv->idxCirc;
			jvxSize myMask = myPrv->maskAddr_2 / 2;
			jvxSize lCount = myPrv->ll0 / 2 - 1;

			for (i = 0; i < fsizelr; i++)
			{
				//hdl->prm_sync.circ_buf[idxCircL] = *inout;

				accu32 = *inl++;

				// Revert input 32 bit values
				accu32r = 0;
#if defined(_MSC_VER)
				accu32 = _byteswap_ulong(accu32);
				for (ibs = 0; ibs < 4; ibs++)
				{
					jvxUInt8 idx8 = accu32 & 0xFF;
					accu32r = accu32r >> 8;
					idx8 = hdl->prm_sync.ptr_revAddr[idx8];
					accu32r |= (jvxUInt32)idx8 << 24;
					accu32 = accu32 >> 8;
				}
#elif defined (__GNUC__)
				accu32 = (unsigned int)__builtin_bswap32(accu32);
				for (ibs = 0; ibs < 4; ibs++)
				{
					jvxUInt8 idx8 = accu32 & 0xFF;
					accu32r = accu32r >> 8;
					idx8 = hdl->prm_sync.ptr_revAddr[idx8];
					accu32r |= (jvxUInt32)idx8 << 24;
					accu32 = accu32 >> 8;
				}
#else

				for (j = 0; j < 32; j++)
				{
					accu32r = accu32r << 1;
					accu32r |= (accu32 & 0x1);
					accu32 = accu32 >> 1;
				}
#endif

				accu16 = (accu32r & 0xFFFF);
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}

				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// =================================================

				accu16 = (accu32r & 0xFFFF0000) >> 16;
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;

				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];


					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}
				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// First part - left input complete
				// =======================================================
				// =======================================================

				accu32 = *inr++;

				// Revert input 32 bit values
				accu32r = 0;
				for (j = 0; j < 32; j++)
				{
					accu32r = accu32r << 1;
					accu32r |= (accu32 & 0x1);
					accu32 = accu32 >> 1;
				}

				accu16 = (accu32r & 0xFFFF);
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}

				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// =================================================

				accu16 = (accu32r & 0xFFFF0000) >> 16;
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;

				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					idx = accu16_2 & 0xFF;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];


					idx = (accu16_2 & 0xFF00) >> 8;
					idx = hdl->prm_sync.ptr_revAddr[idx];

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}
				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				idx = accu16_2 & 0xFF;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				idx = (accu16_2 & 0xFF00) >> 8;
				idx = hdl->prm_sync.ptr_revAddr[idx];

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;


			}
			myPrv->idxCirc = idxCircL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}

/*
 * 16 times downsampling: 2 x 32 Bits per UInt32 entry -> output field size is 4 x fsizelr
*/
jvxDspBaseErrorType jvx_pdm_fir_lut_process_rbit_32swap_lr(jvx_fir_lut* hdl, jvxUInt32 * JVX_RESTRICT inl,
	jvxUInt32 * JVX_RESTRICT inr, jvxInt16 * JVX_RESTRICT out, jvxSize fsizelr)
{
	jvxInt32 accu;
	jvxSize i, j;
	//jvxUInt8* bufTmp1 = NULL;
	jvxUInt16* bufTmp2 = NULL;
	//jvxSize fwdCnt = 0;
	//jvxSize bwdCnt = 0;
	jvxSize fwdCnt2 = 0;
	jvxSize bwdCnt2 = 0;
	jvxInt16* ptrLookup;
	jvxUInt16 idx;
	//jvxUInt16 idxp;
	jvxSize idxCircL = 0;
	jvxUInt16 accu16, accu16_1, accu16_2;
	jvxUInt32 accu32, accu32r;

	jvxUInt16 accu16r;

	if (hdl)
	{
		if (hdl->prv)
		{
			jvx_fir_lut_prv* myPrv = (jvx_fir_lut_prv*)hdl->prv;

			idxCircL = myPrv->idxCirc;
			jvxSize myMask = myPrv->maskAddr_2 / 2;
			jvxSize lCount = myPrv->ll0 / 2 - 1;

			for (i = 0; i < fsizelr; i++)
			{
				//hdl->prm_sync.circ_buf[idxCircL] = *inout;

				accu32 = *inl++;

				//__asm("RBIT %0, %1"
				//		: "=r"(accu32r)
				//		: "r"(accu32));
				accu32r = __RBIT(accu32);

				accu16 = (accu32r & 0xFFFF);
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}

				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// =================================================

				accu16 = (accu32r & 0xFFFF0000) >> 16;
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;

				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}
				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// ============================================================
				// ============================================================

				accu32 = *inr++;

				//__asm("RBIT %0, %1"
				//		: "=r"(accu32r)
				//		: "r"(accu32));
				accu32r = __RBIT(accu32);

				accu16 = (accu32r & 0xFFFF);
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;
				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;
					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}

				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;

				// =================================================

				accu16 = (accu32r & 0xFFFF0000) >> 16;
				hdl->prm_sync.circ_buf[idxCircL] = accu16;

				fwdCnt2 = idxCircL;
				idxCircL = (idxCircL + 1) & myPrv->maskAddr;
				bwdCnt2 = idxCircL;

				accu = 0;
				ptrLookup = hdl->prm_sync.ptr_lookup;

				bufTmp2 = hdl->prm_sync.circ_buf;

				for (j = 0; j < lCount; j++)
				{
					accu16_1 = bufTmp2[fwdCnt2];
					accu16_2 = bufTmp2[bwdCnt2];

					idx = (accu16_1 & 0xFF00) >> 8;
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0xFF000000) >> 24;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					idx = (accu16_1 & 0xFF);
					accu += ptrLookup[idx];

					accu32 = __RBIT(accu16_2);
					idx = (accu32 & 0x00FF0000) >> 16;

					accu += ptrLookup[idx];

					ptrLookup += myPrv->incBufLookup;

					bwdCnt2 = (bwdCnt2 + 1) & myMask;
					fwdCnt2 = (fwdCnt2 - 1) & myMask;
				}
				accu16_1 = bufTmp2[fwdCnt2];
				accu16_2 = bufTmp2[bwdCnt2];

				idx = (accu16_1 & 0xFF00) >> 8;
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0xFF000000) >> 24;

				accu += ptrLookup[idx];

				ptrLookup += myPrv->incBufLookup;

				idx = (accu16_1 & 0xFF);
				accu += ptrLookup[idx];

				accu32 = __RBIT(accu16_2);
				idx = (accu32 & 0x00FF0000) >> 16;

				accu += ptrLookup[idx];

				*out++ = accu & 0xFFFF;
			}
			myPrv->idxCirc = idxCircL;
			return(JVX_DSP_NO_ERROR);
		}
		return(JVX_DSP_ERROR_WRONG_STATE);
	}
	return(JVX_DSP_ERROR_INVALID_ARGUMENT);
}




#if 0 
void jvx_pdm_dec_cpu_init(jvx_fir_lut** hdlOnReturn, jvxSize frame_length)
{
    jvx_fir_lut *new_pdm_dec;
    JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(new_pdm_dec, jvx_fir_lut);
    new_pdm_dec->frame_length = frame_length;
	new_pdm_dec->circ_buf = NULL;
    *hdlOnReturn = new_pdm_dec; 
}

void jvx_pdm_dec_cpu_free(jvx_fir_lut** hdlOnReturn)
{
    if (!hdlOnReturn)
        return;
    
    JVX_DSP_SAFE_DELETE_FIELD((*hdlOnReturn)->circ_buf);
    JVX_DSP_SAFE_DELETE_OBJECT(*hdlOnReturn);
}

// length of the out buffer is length of the in buffer / R
// NOTE input and output pointer are not allowed to overlap for optimization reasons
void jvx_pdm_dec_cpu_process_r16(jvx_fir_lut* hdl, jvxInt16 * JVX_RESTRICT in_i, jvxInt16 * JVX_RESTRICT out_i)
{
    int ind, ind2, idx, ppcnt, ppcntfwd, ppcntbwd;
    jvxUInt8* in_reint = (jvxUInt8*)in_i;
    jvxData accu;

    if(!hdl)
        return;

    ppcnt = hdl->ppcnt;

    // loop over samples
#if defined (__TMS470__)
#pragma MUST_ITERATE(MCASP_BUF_LEN, MCASP_BUF_LEN)
    for (ind = 0; ind < hdl->frame_length; ind++)

#elif defined(TMS_GCC)
    for (ind = 0; ind < MCASP_BUF_LEN; ind++)
    //__builtin_expect(hdl->frame_length, MCASP_BUF_LEN);
#else
    for (ind = 0; ind < hdl->frame_length; ind++)
#endif
    {
        // circular buffer addressing
        hdl->circ_buf[ppcnt] = in_reint[2*ind];
        ppcnt = (ppcnt - 1 + LIR_FRAGMENTS) % LIR_FRAGMENTS;
        hdl->circ_buf[ppcnt] = in_reint[2*ind+1];

        // one pointer for forward iterations
        ppcntfwd = ppcnt;

        ppcnt = (ppcnt - 1 + LIR_FRAGMENTS) % LIR_FRAGMENTS;

        // one pointer for backward iterations
        ppcntbwd = ppcnt;

        // loop over all 8 samples segments
        // exploit symmetry of impulse response and propagate forward and backward
        accu = .0;
        for (ind2 = 0; ind2 < LIR_FRAGMENTS/2; ind2++)
        {
             // forward
             idx = hdl->circ_buf[ppcntfwd];
             accu = accu + FIR_LUTS[(ind2*NUMBER_ENTRIES_TABLE)+idx];
             ppcntfwd = (ppcntfwd + 1) % LIR_FRAGMENTS;

             // backward
             idx = hdl->circ_buf[ppcntbwd];
             accu = accu + FIR_LUTS[(ind2*NUMBER_ENTRIES_TABLE)+BITREV_LUT[idx]];
             ppcntbwd = (ppcntbwd - 1 + LIR_FRAGMENTS) % LIR_FRAGMENTS;
        }

        // write output
        out_i[ind] = (jvxInt16)(accu * 32767.); // TODO use <limits.h> to get rid of hard coded number
    }
#if defined (__TMS470__)
#elif defined(TMS_GCC)
#else
#endif

    hdl->ppcnt = ppcnt;
}

#endif
