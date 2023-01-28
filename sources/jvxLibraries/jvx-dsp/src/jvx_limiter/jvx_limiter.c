#include "jvx_dsp_base.h"
#include "jvx_limiter/jvx_limiter.h"
#include <assert.h>
#include <math.h>

#ifndef NAN
	#define NAN sqrt(-1)
#endif

//////////////////////
// internal data
//////////////////////

static jvxData limiter_x3[7][7] =
  {
    { 0.86, 21.86588921283092900000, -64.57725947522689600000,
      63.55685131196099700000, -19.94548104956499300000, 0.9, 1}, // delta = 1;x0 = 0.86;x1 = 0.9;
    { 0.71, 4.51023002173125230000, -13.29287793677513800000,
      13.05506580835655100000, -3.47241789331264710000, 0.8, 1}, // delta = 1; x0 = 0.71;x1 = 0.8;
    { 0.55, 1.64609053497942570000, -4.93827160493827750000,
      4.93827160493827310000, -0.94609053497942441000, 0.7, 1}, // delta = 1;x0 = 0.55;x1 = 0.7;
    { 0.4, 0.92592592592592737000, -2.77777777777777950000,
      2.77777777777777990000, -0.32592592592592606000, 0.6, 1}, // delta = 1; x0 = 0.4;x1 = 0.6;
    { 0.25, 0.59259259259259323000, -1.77777777777777860000,
      1.77777777777777810000, -0.09259259259259261500, 0.5, 1}, // delta = 1; x0 = 0.25;sx1 = 0.5;
    { 0.1, 0.41152263374485598000, -1.23456790123456810000,
      1.23456790123456780000, -0.01152263374485595900, 0.4, 1}, // x0 = 0.1; x1 = 0.4; delta= 1
    { 0.005, 0.20709085692597762000,  -0.71419951706716789000,
      0.80712646335640237000,-0.00001780321521244882, 0.3, 0.8} // delta = 0.8; x0 = 0.005; x1 = 0.3;
  };

typedef struct
{
  jvx_limiter_async async;
  //jvx_limiter_sync sync;
  //jvx_limiter_config config;

  jvxData a;
  jvxData b;
  jvxData c;

} jvx_limiter_intern;


//////////////////////
// internal functions
//////////////////////

// TODO: this function seems to be incomplete!
// compute internal parameters
// only write unsynced internal data fields here
static void
derive_params(jvx_limiter *hdl)
{
  jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;
  jvxData x0 = intern->async.x0;

  if(x0 >= 1.0)
    x0 = 0.99;

  switch(intern->async.type)
    {
    case JVX_LIMITER_TYPE_NONE:
      intern->async.limVal = 1.0;
      intern->async.x0 = NAN;
      intern->async.delta = NAN;
      break;
    case JVX_LIMITER_TYPE_SEGMENTS:
      intern->async.limVal = x0 + (1.0 - x0) * intern->async.delta;
      break;
    case JVX_LIMITER_TYPE_SOFT_X2:
      intern->a = intern->async.delta / (1.0 - x0) / 2.0;
      intern->b = 2.0 * intern->a;
      intern->c = intern->async.delta * x0 + intern->a * x0 * x0 - intern->b * x0;
      intern->async.limVal = -intern->a + intern->b + intern->c;
      break;
    case JVX_LIMITER_TYPE_SOFT_X3_03:
    case JVX_LIMITER_TYPE_SOFT_X3_04:
    case JVX_LIMITER_TYPE_SOFT_X3_05:
    case JVX_LIMITER_TYPE_SOFT_X3_06:
    case JVX_LIMITER_TYPE_SOFT_X3_07:
    case JVX_LIMITER_TYPE_SOFT_X3_08:
    case JVX_LIMITER_TYPE_SOFT_X3_09:
      intern->async.limVal = 1 - 0 - 0.1 * (jvxData)(intern->async.type - JVX_LIMITER_TYPE_SOFT_X2);
      intern->async.x0 = limiter_x3[intern->async.type - JVX_LIMITER_TYPE_SOFT_X3_03][0];
      intern->async.delta = NAN;
      break;
    default:
      assert(0);
      break;
    }

  // sync back to extern
  jvx_limiter_update_async(hdl, false);
  // jvx_limiter_update_sync(hdl, false);
}


//////////////////////
// API functions
//////////////////////

jvxDspBaseErrorType jvx_limiter_init(jvx_limiter *hdl)
{
  jvx_limiter_intern *intern;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  jvxSize sz = sizeof(jvx_limiter_async);

  // allocate and init private part
  JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(intern, jvx_limiter_intern);

  // copy config
  // be careful: memcpy not possible anymore as soon as config struct contains pointers
  //intern->config = hdl->config;

  // hook private part into struct & perform update
  hdl->intern = intern;
  jvx_limiter_update_async(hdl, true);
  //jvx_limiter_update_sync(hdl, true);

  // fill internal data
  derive_params(hdl);

  return JVX_DSP_NO_ERROR;
}


jvxDspBaseErrorType jvx_limiter_terminate(jvx_limiter *hdl)
{
  jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_DELETE_OBJECT(intern);
	hdl->intern = NULL;

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_limiter_process(jvx_limiter *hdl,
                                               jvxData* input,
                                               jvxData* output,
                                               jvxSize framesize)
{

  jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;
  int i;
  int sgn;
  jvxData valabs;
  jvxData tmp_lim;
  jvxData *ptr_lim;

   if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch(intern->async.type)
    {
    case JVX_LIMITER_TYPE_NONE:
      memcpy(output, input, sizeof(jvxData) * framesize);
      break;

    case JVX_LIMITER_TYPE_SEGMENTS:
      for(i = 0; i < framesize; i++)
        {
          sgn = JVX_SIGN(input[i]);
          valabs = JVX_ABS(input[i]);
          if(valabs > intern->async.x0)
              valabs = (valabs - intern->async.x0) * intern->async.delta + intern->async.x0;
          output[i] = valabs * sgn;
        }
      break;

    case JVX_LIMITER_TYPE_SOFT_X2:
      for(i = 0; i < framesize; i++)
        {
          sgn = JVX_SIGN(input[i]);
          valabs = JVX_ABS(input[i]);
          if(valabs > intern->async.x0)
            valabs = -intern->a * valabs * valabs + intern->b * valabs + intern->c;
          output[i] = valabs * sgn;
        }
      break;

    case JVX_LIMITER_TYPE_SOFT_X3_03:
    case JVX_LIMITER_TYPE_SOFT_X3_04:
    case JVX_LIMITER_TYPE_SOFT_X3_05:
    case JVX_LIMITER_TYPE_SOFT_X3_06:
    case JVX_LIMITER_TYPE_SOFT_X3_07:
    case JVX_LIMITER_TYPE_SOFT_X3_08:
    case JVX_LIMITER_TYPE_SOFT_X3_09:
      i = intern->async.type - JVX_LIMITER_TYPE_SOFT_X3_03;
      assert(i < 7);
      ptr_lim = limiter_x3[i];

      for(i = 0; i < framesize; i++)
        {
          sgn = JVX_SIGN(input[i]);
          valabs = JVX_ABS(input[i]);
          if(valabs > ptr_lim[0])
            {
              tmp_lim = valabs;
              valabs = ptr_lim[4] + tmp_lim * ptr_lim[3];
              tmp_lim *= tmp_lim;
              valabs += tmp_lim * ptr_lim[2];
              tmp_lim *= tmp_lim;
              valabs += tmp_lim * ptr_lim[1];
            }
          else
            {
              valabs *= ptr_lim[6];
            }
          output[i] = valabs * sgn;
        }
    default:
      assert(0);
    }

  return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_limiter_process_inplace(jvx_limiter *hdl,
	jvxData* inout,
	jvxSize framesize)
{

	jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;
	int i;
	int sgn;
	jvxData valabs;
	jvxData tmp_lim;
	jvxData *ptr_lim;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	switch (intern->async.type)
	{
	case JVX_LIMITER_TYPE_NONE:
		break;

	case JVX_LIMITER_TYPE_SEGMENTS:
		for (i = 0; i < framesize; i++)
		{
			sgn = JVX_SIGN(inout[i]);
			valabs = JVX_ABS(inout[i]);
			if (valabs > intern->async.x0)
				valabs = (valabs - intern->async.x0) * intern->async.delta + intern->async.x0;
			inout[i] = valabs * sgn;
		}
		break;

	case JVX_LIMITER_TYPE_SOFT_X2:
		for (i = 0; i < framesize; i++)
		{
			sgn = JVX_SIGN(inout[i]);
			valabs = JVX_ABS(inout[i]);
			if (valabs > intern->async.x0)
				valabs = -intern->a * valabs * valabs + intern->b * valabs + intern->c;
			inout[i] = valabs * sgn;
		}
		break;

	case JVX_LIMITER_TYPE_SOFT_X3_03:
	case JVX_LIMITER_TYPE_SOFT_X3_04:
	case JVX_LIMITER_TYPE_SOFT_X3_05:
	case JVX_LIMITER_TYPE_SOFT_X3_06:
	case JVX_LIMITER_TYPE_SOFT_X3_07:
	case JVX_LIMITER_TYPE_SOFT_X3_08:
	case JVX_LIMITER_TYPE_SOFT_X3_09:
		i = intern->async.type - JVX_LIMITER_TYPE_SOFT_X3_03;
		assert(i < 7);
		ptr_lim = limiter_x3[i];

		for (i = 0; i < framesize; i++)
		{
			sgn = JVX_SIGN(inout[i]);
			valabs = JVX_ABS(inout[i]);
			if (valabs > ptr_lim[0])
			{
				tmp_lim = valabs;
				valabs = ptr_lim[4] + tmp_lim * ptr_lim[3];
				tmp_lim *= tmp_lim;
				valabs += tmp_lim * ptr_lim[2];
				tmp_lim *= tmp_lim;
				valabs += tmp_lim * ptr_lim[1];
			}
			else
			{
				valabs *= ptr_lim[6];
			}
			inout[i] = valabs * sgn;
		}
	default:
		assert(0);
	}

	return JVX_DSP_NO_ERROR;
}

/*
jvxDspBaseErrorType jvx_limiter_update_sync(jvx_limiter *hdl, jvxCBool do_set)
{
  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

  jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;

  if(intern)
    {
      if(do_set)
        {
          // be careful: memcpy not possible anymore as soon as sync struct contains pointers
          //intern->sync = hdl->sync;
          derive_params(hdl);
        }
      else
        {
          // be careful: memcpy not possible anymore as soon as sync struct contains pointers
          //hdl->sync = intern->sync;
        }
    }

  return JVX_DSP_NO_ERROR;
}
*/

jvxDspBaseErrorType jvx_limiter_update_async(jvx_limiter *hdl, jvxCBool do_set)
{

  jvx_limiter_intern *intern = (jvx_limiter_intern*)hdl->intern;

  if(!hdl)
    return JVX_DSP_ERROR_INVALID_ARGUMENT;

	if(intern)
    {
      if(do_set)
        {
          // be careful: memcpy not possible anymore as soon as async struct contains pointers
          intern->async = hdl->async;
          derive_params(hdl);
        }
      else
        {
          // be careful: memcpy not possible anymore as soon as async struct contains pointers
          //hdl->async = intern->async;
        }
    }

  return JVX_DSP_NO_ERROR;
}
