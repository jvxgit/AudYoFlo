#include "colormap_jet.h"

jvxDspBaseErrorType jvx_compute_color_map(jvxData percent, jvxUInt8* out)
{
	//blue: x=0: 133, lin bis x = 0.122: 256 bis x = 0.374, lin bis x = 0.626
	//green: 0 bis x = 0.122, lin bis x = 0.374: 256 bis x = 0.626, lin bis x = 0.878
	//red: 0 bis x = 0.374, lin bis x = 0.626: 256 bis x = 0.878, lin bis x = 1: 133
	
	//check vars
	if (!out || percent < 0.0 || percent > 1.0) return JVX_DSP_ERROR_INVALID_ARGUMENT;
	
	jvxUInt8 r, g, b, a = 255;

	if (percent < 0.122)
	{
		g = 0;
		r = 0;
		b = 133 + ((jvxUInt8)(percent / 0.122 * 122));
	}
	else if (percent < 0.374)
	{
		b = 255;
		r = 0;
		g = (jvxUInt8)((percent - 0.122) / 0.252 * 255);
	}
	else if (percent < 0.626)
	{
		g = 255;
		r = (jvxUInt8)((percent - 0.374) / 0.252 * 255);
		b = 255 - (jvxUInt8)((percent - 0.374) / 0.252 * 255);
	}
	else if (percent < 0.878)
	{
		b = 0;
		r = 255;
		g = 255 - (jvxUInt8)((percent - 0.626) / 0.252 * 255);
	}
	else
	{
		b = 0;
		g = 0;
		r = 255 - (jvxUInt8)((percent - 0.878) / 0.252 * 255);
	}

	out[0] = b;
	out[1] = g;
	out[2] = r;
	out[3] = a;

	return JVX_DSP_NO_ERROR;
}