#include "jvx_conv/jvx_conv.h"
#include "jvx_dsp_base.h"

jvxData
jvx_compute_energy(jvxData* inF, jvxSize ninF)
{
	jvxSize i;
	jvxData out = 0;
	for (i = 0; i < ninF; i++)
	{
		out += *inF * *inF;
		inF++;
	}
	return out;
}

jvxData 
jvx_compute_conv_energy(jvxData* inF, jvxSize ninF, jvxData* inS, jvxSize ninS)
{
	jvxData energy = 0;
	jvxData accu;
	int i0, i, j; // must be int since it might be negative check
	
	// Implementation expects that the first arrays size is greater or equal to second array size.
	// If program is called the other way around, we swap arguments quickly to coply since convoultion 
	// is commutative.
	jvxData* in1 = inF;
	jvxSize nin1 = ninF;
	jvxData* in2 = inS;
	jvxSize nin2 = ninS;
	if (nin2 > nin1)
	{
		in1 = inS;
		nin1 = ninS;
		in2 = inF;
		nin2 = ninF;
	}

	for (i0 = 0; i0 < (nin2 - 1); i0++)
	{
		accu = 0;
		jvxData* ptr1 = in1;
		jvxData* ptr2 = in2 + i0;
		for (j = 0; j <= i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		energy += accu * accu;
	}

	// This part may also be skipped if second vector is larger than first part
	for (i = 0; i <= ((int)nin1 - (int)nin2); i++)
	{
		accu = 0;
		jvxData* ptr1 = in1 + i;
		jvxData* ptr2 = in2 + (nin2-1); // Point to last element
		for (j = 0; j < nin2; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		energy += accu * accu;
	}
	
	for (i = 0; i < (nin2-1); i++, i0--)
	{
		accu = 0;
		jvxData* ptr1 = in1 + nin1 - i0; // Towards end of buffer
		jvxData* ptr2 = in2 + (nin2 - 1); // Point to last element
		for (j = 0; j < i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		energy += accu * accu;
	}
	return energy;
}

// =============================================================================================

jvxData
jvx_compute_conv_energy_add(jvxData* inF1, jvxData* inF2, jvxSize ninF, jvxData* inS1, jvxData* inS2, jvxSize ninS)
{
	jvxData energy = 0;
	jvxData accu;
	int i0, i, j; // must be int since it might be negative check

	// Implementation expects that the first arrays size is greater or equal to second array size.
	// If program is called the other way around, we swap arguments quickly to coply since convoultion 
	// is commutative.
	jvxData* in11 = inF1;
	jvxData* in12 = inF2;
	jvxSize nin1 = ninF;
	jvxData* in21 = inS1;
	jvxData* in22 = inS2;
	jvxSize nin2 = ninS;
	if (nin2 > nin1)
	{
		in11 = inS1;
		in12 = inS2;
		nin1 = ninS;
		in21 = inF1;
		in22 = inF2;
		nin2 = ninF;
	}

	for (i0 = 0; i0 < (nin2 - 1); i0++)
	{
		accu = 0;
		jvxData* ptr11 = in11;
		jvxData* ptr21 = in21 + i0;
		jvxData* ptr12 = in12;
		jvxData* ptr22 = in22 + i0;
		for (j = 0; j <= i0; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		energy += accu * accu;
	}

	// This part may also be skipped if second vector is larger than first part
	for (i = 0; i <= ((int)nin1 - (int)nin2); i++)
	{
		accu = 0;
		jvxData* ptr11 = in11 + i;
		jvxData* ptr21 = in21 + (nin2 - 1); // Point to last element
		jvxData* ptr12 = in12 + i;
		jvxData* ptr22 = in22 + (nin2 - 1); // Point to last element
		for (j = 0; j < nin2; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		energy += accu * accu;
	}

	for (i = 0; i < (nin2 - 1); i++, i0--)
	{
		accu = 0;
		jvxData* ptr11 = in11 + nin1 - i0; // Towards end of buffer
		jvxData* ptr21 = in21 + (nin2 - 1); // Point to last element
		jvxData* ptr12 = in12 + nin1 - i0; // Towards end of buffer
		jvxData* ptr22 = in22 + (nin2 - 1); // Point to last element
		for (j = 0; j < i0; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		energy += accu * accu;
	}
	return energy;
}

void
jvx_compute_conv(jvxData* inF, jvxSize ninF, jvxData* inS, jvxSize ninS, jvxData* out, jvxSize nout)
{
	// nout != nin2 - 1 + nin1 - nin2 + 1 + nin2 - 1
	//       = nin2 - 1 + nin1 

	jvxData accu;
	int i0, i, j; // must be int since it might be negative check

	// Implementation expects that the first arrays size is greater or equal to second array size.
// If program is called the other way around, we swap arguments quickly to coply since convoultion 
// is commutative.
	jvxData* in1 = inF;
	jvxSize nin1 = ninF;
	jvxData* in2 = inS;
	jvxSize nin2 = ninS;
	if (nin2 > nin1)
	{
		in1 = inS;
		nin1 = ninS;
		in2 = inF;
		nin2 = ninF;
	}

	assert(nout == nin1 + nin2 - 1);

	for (i0 = 0; i0 < (nin2 - 1); i0++)
	{
		accu = 0;
		jvxData* ptr1 = in1;
		jvxData* ptr2 = in2 + i0;
		for (j = 0; j <= i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		*out++ = accu;
	}

	// This part may also be skipped if second vector is larger than first part
	for (i = 0; i <= ((int)nin1 - (int)nin2); i++)
	{
		accu = 0;
		jvxData* ptr1 = in1 + i;
		jvxData* ptr2 = in2 + (nin2 - 1); // Point to last element
		for (j = 0; j < nin2; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		*out++ = accu;
	}

	for (i = 0; i < (nin2 - 1); i++, i0--)
	{
		accu = 0;
		jvxData* ptr1 = in1 + nin1 - i0; // Towards end of buffer
		jvxData* ptr2 = in2 + (nin2 - 1); // Point to last element
		for (j = 0; j < i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		*out++ = accu;
	}
}

void
jvx_compute_conv_constrained_size(jvxData* inF, jvxSize ninF, jvxData* inS, jvxSize ninS, jvxData* out, jvxSize nout)
{
	// nout != nin2 - 1 + nin1 - nin2 + 1 + nin2 - 1
	//       = nin2 - 1 + nin1 
	jvxSize outCnt = 0;
	jvxData accu;
	int i0, i, j; // must be int since it might be negative check

	// Implementation expects that the first arrays size is greater or equal to second array size.
// If program is called the other way around, we swap arguments quickly to coply since convoultion 
// is commutative.
	jvxData* in1 = inF;
	jvxSize nin1 = ninF;
	jvxData* in2 = inS;
	jvxSize nin2 = ninS;
	if (nin2 > nin1)
	{
		in1 = inS;
		nin1 = ninS;
		in2 = inF;
		nin2 = ninF;
	}

	for (i0 = 0; i0 < (nin2 - 1); i0++)
	{
		accu = 0;
		jvxData* ptr1 = in1;
		jvxData* ptr2 = in2 + i0;
		for (j = 0; j <= i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		if (outCnt < nout)
		{
			*out++ = accu;
			outCnt++;
		}
	}

	// This part may also be skipped if second vector is larger than first part
	for (i = 0; i <= ((int)nin1 - (int)nin2); i++)
	{
		accu = 0;
		jvxData* ptr1 = in1 + i;
		jvxData* ptr2 = in2 + (nin2 - 1); // Point to last element
		for (j = 0; j < nin2; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		if (outCnt < nout)
		{
			*out++ = accu;
			outCnt++;
		}
	}

	for (i = 0; i < (nin2 - 1); i++, i0--)
	{
		accu = 0;
		jvxData* ptr1 = in1 + nin1 - i0; // Towards end of buffer
		jvxData* ptr2 = in2 + (nin2 - 1); // Point to last element
		for (j = 0; j < i0; j++)
		{
			accu += *ptr1++ * *ptr2--;
		}
		if (outCnt < nout)
		{
			*out++ = accu;
			outCnt++;
		}
	}
}
void
jvx_compute_conv_add(jvxData* inF1, jvxData* inF2, jvxSize ninF, jvxData* inS1, jvxData* inS2, jvxSize ninS, jvxData* out, jvxSize nout)
{
	// nout != nin2 - 1 + nin1 - nin2 + 1 + nin2 - 1
	//       = nin2 - 1 + nin1 

	jvxData accu;
	int i0, i, j; // must be int since it might be negative check

	// Implementation expects that the first arrays size is greater or equal to second array size.
// If program is called the other way around, we swap arguments quickly to coply since convoultion 
// is commutative.
	jvxData* in11 = inF1;
	jvxData* in12 = inF2;
	jvxSize nin1 = ninF;
	jvxData* in21 = inS1;
	jvxData* in22 = inS2;
	jvxSize nin2 = ninS;
	if (nin2 > nin1)
	{
		in11 = inS1;
		in12 = inS2;
		nin1 = ninS;
		in21 = inF1;
		in22 = inF2;
		nin2 = ninF;
	}

	assert(nout == nin1 + nin2 - 1);

	for (i0 = 0; i0 < (nin2 - 1); i0++)
	{
		accu = 0;
		jvxData* ptr11 = in11;
		jvxData* ptr21 = in21 + i0;
		jvxData* ptr12 = in12;
		jvxData* ptr22 = in22 + i0;
		for (j = 0; j <= i0; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		*out++ = accu;
	}

	// This part may also be skipped if second vector is larger than first part
	for (i = 0; i <= ((int)nin1 - (int)nin2); i++)
	{
		accu = 0;
		jvxData* ptr11 = in11 + i;
		jvxData* ptr21 = in21 + (nin2 - 1); // Point to last element
		jvxData* ptr12 = in12 + i;
		jvxData* ptr22 = in22 + (nin2 - 1); // Point to last element
		for (j = 0; j < nin2; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		*out++ = accu;
	}

	for (i = 0; i < (nin2 - 1); i++, i0--)
	{
		accu = 0;
		jvxData* ptr11 = in11 + nin1 - i0; // Towards end of buffer
		jvxData* ptr21 = in21 + (nin2 - 1); // Point to last element
		jvxData* ptr12 = in12 + nin1 - i0; // Towards end of buffer
		jvxData* ptr22 = in22 + (nin2 - 1); // Point to last element
		for (j = 0; j < i0; j++)
		{
			accu += *ptr11++ * *ptr21-- + *ptr12++ * *ptr22--;
		}
		*out++ = accu;
	}
}