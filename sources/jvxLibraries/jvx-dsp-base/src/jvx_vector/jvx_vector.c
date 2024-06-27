#include "jvx_dsp_base.h"
#include "jvx_vector/jvx_vector.h"

void jvx_vector_cross_product(jvxData* in1, jvxData* in2, jvxData* out, jvxSize n)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvxData tmp = 0;

		// Circular addressing
		jvxSize j0 = (i + 1) % n;
		jvxSize j1 = (i + 2) % n;
		out[i] = in1[j0] * in2[j1] - in1[j1] * in2[j0];
	}
}

// Non-transpMode
// 					| mat[0][0] mat[0,1] mat[0,2] |
//	[in in in] *	| mat[1,0] mat[1,1] mat[1,2] |
//					| mat[2,0] mat[2,1] mat[2,2] |

// TranspMode
// 					| mat[0,0] mat[0,1] mat[0,2] |^T
//	[in in in] *	| mat[1,0] mat[1,1] mat[1,2] |
//					| mat[2,0] mat[2,1] mat[2,2] |

void jvx_vector_mat_comp(jvxData* in, jvxData* out, jvxData** mat, jvxSize n, jvxCBool transpMode)
{
	for (jvxSize i = 0; i < n; i++)
	{
		jvxData tmp = 0;

		if (transpMode)
		{
			for (jvxSize j = 0; j < n; j++)
			{
				tmp += in[j] * mat[i][j];
			}
		}
		else
		{
			for (jvxSize j = 0; j < n; j++)
			{
				tmp += in[j] * mat[j][i];
			}
		}
		out[i] = tmp;
	}
}