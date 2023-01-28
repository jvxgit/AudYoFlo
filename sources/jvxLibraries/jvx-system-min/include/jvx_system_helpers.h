#ifndef __JVX_DSP_BASE_HELPERS_H__
#define __JVX_DSP_BASE_HELPERS_H__

// pointer constant
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


// Some simple math
#define JVX_SIGN(a) ((a) > 0 ? 1:-1)
#define JVX_ABS(a) ((a) > 0 ? (a):-(a))
#define JVX_MAX(a, b) ((a)>(b) ? (a):(b))
#define JVX_MIN(a, b) ((a)<(b) ? (a):(b))
#define JVX_CMPLX_ASQR(z) (z.re * z.re + z.im * z.im)
/*
#define JVX_SNR_APOST(y, n) ((jvxData)y / n)
#define JVX_SNR_APRIO(y, n) ((jvxData)y / n - 1.)
*/
#endif
