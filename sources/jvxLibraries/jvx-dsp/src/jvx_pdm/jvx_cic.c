#include "jvx_dsp_base.h"
#include "jvx_pdm/jvx_cic.h"

#if defined (__TMS470__) || defined(TMS_GCC)
#include "mcaspif.h" // for MCASP_RX_BUF_LEN define
#endif

void cic_decimation_filter_init(jvx_cic** hdlOnReturn, jvxSize frame_length)
{
    jvx_cic *new_cic;
    JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(new_cic, jvx_cic);
    new_cic->frame_length = frame_length;
    *hdlOnReturn = new_cic; 
}

void cic_decimation_filter_free(jvx_cic** hdlOnReturn)
{
    if (!hdlOnReturn)
        return;

    JVX_DSP_SAFE_DELETE_OBJECT(*hdlOnReturn);
}

// R = 16, N = CIC_N = 3, M = 2 (24/27/31 bits used)
// bit growth is N * log2(R*M)
// length of the out buffer is length of the in buffer / R
// NOTE input and output pointer are not allowed to overlap for optimization reasons
void cic_decimation_filter(jvx_cic* hdl, jvxInt16 * JVX_RESTRICT in_i, jvxInt16 * JVX_RESTRICT out_i)
{
     size_t m, o;
     jvxInt16 isum, tsum;
     jvxUInt16 s;

     if(!hdl)
          return;

     // integrator section
#if defined (__TMS470__)
#pragma MUST_ITERATE(MCASP_BUF_LEN, MCASP_BUF_LEN)
     for (m = 0; m < hdl->frame_length; m++)
#elif defined(TMS_GCC)
     for (m = 0; m < MCASP_BUF_LEN; m++)
#else
     for (m = 0; m < hdl->frame_length; m++)
#endif

     {
          s = in_i[m];
          for (o = 0; o < 16; o++) {
#ifdef JVX_FIR_BIT_SWAP
        	  hdl->cic_i[0] += (s & 0x80 ? 1 : -1);
               s<<=1;
#else
               hdl->cic_i[0] += (s & 0x1 ? 1 : -1);
               s>>=1;
#endif
               hdl->cic_i[1] += hdl->cic_i[0];
               hdl->cic_i[2] += hdl->cic_i[1];
          }

          // comb stages
          isum = hdl->cic_i[2] - hdl->cic_c0[0];
          hdl->cic_c0[0] = hdl->cic_c1[0];
          hdl->cic_c1[0] = hdl->cic_i[2];

          tsum = isum;
          isum -= hdl->cic_c0[1];
          hdl->cic_c0[1] = hdl->cic_c1[1];
          hdl->cic_c1[1] = tsum;

          tsum = isum;
          isum -= hdl->cic_c0[2];
          hdl->cic_c0[2] = hdl->cic_c1[2];
          hdl->cic_c1[2] = tsum;

	      out_i[m] = isum;
     }
}
