#ifndef __JVX_COLORMAP_MATLAB_JET_H__
#define __JVX_COLORMAP_MATLAB_JET_H__

#include "jvx_dsp_base.h"

/**
 * Color map, almost identical to Matlab's "Jet" color map.
 *
 * percent: must be in range betweeen 0 and 1.0
 * out: must have 3 empty fields, rgb values are written alternatingly
*/
jvxDspBaseErrorType jvx_compute_color_map(jvxData percent, jvxUInt8* out);

//jvxDspBaseErrorType jvx_get_color_scale(unsigned char* out);

#endif