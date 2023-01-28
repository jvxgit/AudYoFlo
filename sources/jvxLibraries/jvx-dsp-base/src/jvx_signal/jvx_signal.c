#include "jvx_signal/jvx_signal.h"

void jvx__signal_copy_multply_gain__max_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData* maxAbs)
{
	jvxSize i,j;

	jvxData accu = 0.0;
	jvxData accuOutAbsMax = 0.0;

	jvxData* fldDataIn = NULL;
	jvxInt64* fldInt64In = NULL;
	jvxInt32* fldInt32In = NULL;
	jvxInt16* fldInt16In = NULL;
	jvxInt8* fldInt8In = NULL;

	jvxData* fldDataOut = NULL;
	jvxInt64* fldInt64Out = NULL;
	jvxInt32* fldInt32Out = NULL;
	jvxInt16* fldInt16Out = NULL;
	jvxInt8* fldInt8Out = NULL;

	switch(processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldDataIn = (jvxData*)copyFrom[i];
			fldDataOut = (jvxData*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = *fldDataIn;
				accu *= gain;
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldDataOut++ =  accu;
				fldDataIn++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt64In = (jvxInt64*)copyFrom[i];
			fldInt64Out = (jvxInt64*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT64_2_DATA(*fldInt64In);
				accu *= gain;
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt64Out++ =  JVX_DATA_2_INT64(accu);
				fldInt64In++;
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt32In = (jvxInt32*)copyFrom[i];
			fldInt32Out = (jvxInt32*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT32_2_DATA(*fldInt32In);
				accu *= gain;
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt32Out++ =  JVX_DATA_2_INT32(accu);
				fldInt32In++;
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt16In = (jvxInt16*)copyFrom[i];
			fldInt16Out = (jvxInt16*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT16_2_DATA(*fldInt16In);
				accu *= gain;
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt16Out++ =  JVX_DATA_2_INT16(accu);
				fldInt16In++;
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt8In = (jvxInt8*)copyFrom[i];
			fldInt8Out = (jvxInt8*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT8_2_DATA(*fldInt8In);
				accu *= gain;
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt8Out++ =  JVX_DATA_2_INT8(accu);
				fldInt8In++;
			}
		}
		break;
	default:
		assert(0);
	}
}

void jvx__signal_copy_multply_gain__saturate__max_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData*maxAbs)
{
	int i, j;

	jvxData accu = 0.0;
	jvxData accuOutAbsMax = 0.0;

	jvxData* fldDataIn = NULL;
	jvxInt64* fldInt64In = NULL;
	jvxInt32* fldInt32In = NULL;
	jvxInt16* fldInt16In = NULL;
	jvxInt8* fldInt8In = NULL;

	jvxData* fldDataOut = NULL;
	jvxInt64* fldInt64Out = NULL;
	jvxInt32* fldInt32Out = NULL;
	jvxInt16* fldInt16Out = NULL;
	jvxInt8* fldInt8Out = NULL;

	switch (processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		for (i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldDataIn = (jvxData*)copyFrom[i];
			fldDataOut = (jvxData*)copyTo[i];

			for (j = 0; j < buffersize; j++)
			{
				accu = *fldDataIn;
				accu *= gain;
				if (accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if (accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldDataOut++ = accu;
				fldDataIn++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		for (i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt64In = (jvxInt64*)copyFrom[i];
			fldInt64Out = (jvxInt64*)copyTo[i];

			for (j = 0; j < buffersize; j++)
			{
				accu = JVX_INT64_2_DATA(*fldInt64In);
				accu *= gain;
				if (accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if (accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt64Out++ = JVX_DATA_2_INT64(accu);
				fldInt64In++;
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		for (i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt32In = (jvxInt32*)copyFrom[i];
			fldInt32Out = (jvxInt32*)copyTo[i];

			for (j = 0; j < buffersize; j++)
			{
				accu = JVX_INT32_2_DATA(*fldInt32In);
				accu *= gain;
				if (accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if (accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt32Out++ = JVX_DATA_2_INT32(accu);
				fldInt32In++;
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for (i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt16In = (jvxInt16*)copyFrom[i];
			fldInt16Out = (jvxInt16*)copyTo[i];

			for (j = 0; j < buffersize; j++)
			{
				accu = JVX_INT16_2_DATA(*fldInt16In);
				accu *= gain;
				if (accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if (accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt16Out++ = JVX_DATA_2_INT16(accu);
				fldInt16In++;
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for (i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt8In = (jvxInt8*)copyFrom[i];
			fldInt8Out = (jvxInt8*)copyTo[i];

			for (j = 0; j < buffersize; j++)
			{
				accu = JVX_INT8_2_DATA(*fldInt8In);
				accu *= gain;
				if (accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if (accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				maxAbs[i] = JVX_MAX(fabs(accu), maxAbs[i]);
				*fldInt8Out++ = JVX_DATA_2_INT8(accu);
				fldInt8In++;
			}
		}
		break;
	default:
		assert(0);
	}
}

void jvx__signal_copy_multply_gain__saturate__smoothed_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData smooth, jvxData* repPower)
{
	int i,j;

	jvxData accu = 0.0;
	//jvxData fac1OverChansOverBsize = 1.0/((jvxData)numChans*(jvxData)buffersize);
	
	jvxData* fldDataIn = NULL;
	jvxInt64* fldInt64In = NULL;
	jvxInt32* fldInt32In = NULL;
	jvxInt16* fldInt16In = NULL;
	jvxInt8* fldInt8In = NULL;

	jvxData* fldDataOut = NULL;
	jvxInt64* fldInt64Out = NULL;
	jvxInt32* fldInt32Out = NULL;
	jvxInt16* fldInt16Out = NULL;
	jvxInt8* fldInt8Out = NULL;

	switch(processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldDataIn = (jvxData*)copyFrom[i];
			fldDataOut = (jvxData*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = *fldDataIn;
				accu *= gain;
				if(accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}

				*fldDataOut++ =  accu;

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldDataIn++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt64In = (jvxInt64*)copyFrom[i];
			fldInt64Out = (jvxInt64*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT64_2_DATA(*fldInt64In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				*fldInt64Out++ =  JVX_DATA_2_INT64(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt64In++;
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt32In = (jvxInt32*)copyFrom[i];
			fldInt32Out = (jvxInt32*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT32_2_DATA(*fldInt32In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}

				*fldInt32Out++ =  JVX_DATA_2_INT32(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt32In++;
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt16In = (jvxInt16*)copyFrom[i];
			fldInt16Out = (jvxInt16*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT16_2_DATA(*fldInt16In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}
				*fldInt16Out++ =  JVX_DATA_2_INT16(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt16In++;
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt8In = (jvxInt8*)copyFrom[i];
			fldInt8Out = (jvxInt8*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT8_2_DATA(*fldInt8In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					accu = -satValAbsMax;
				}

				*fldInt8Out++ =  JVX_DATA_2_INT8(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt8In++;
			}
		}
		break;
	default:
	  assert(0);
	}
}

void jvx__signal_copy_multply_gain__saturate_detect__smoothed_level(jvxHandle** copyTo, const jvxHandle** copyFrom, jvxSize numChans, jvxSize buffersize, jvxDataFormat processingFormat, jvxData gain, jvxData satValAbsMax, jvxData smooth, jvxData* repPower, jvxCBool* repOverload)
{
	int i,j;
	jvxData accu = 0.0;
	//jvxData fac1OverChansOverBsize = 1.0/((jvxData)numChans*(jvxData)buffersize);

	jvxData* fldDataIn = NULL;
	jvxInt64* fldInt64In = NULL;
	jvxInt32* fldInt32In = NULL;
	jvxInt16* fldInt16In = NULL;
	jvxInt8* fldInt8In = NULL;

	jvxData* fldDataOut = NULL;
	jvxInt64* fldInt64Out = NULL;
	jvxInt32* fldInt32Out = NULL;
	jvxInt16* fldInt16Out = NULL;
	jvxInt8* fldInt8Out = NULL;

	assert(repPower);
	assert(repOverload);

	switch(processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldDataIn = (jvxData*)copyFrom[i];
			fldDataOut = (jvxData*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = *fldDataIn;
				accu *= gain;
				if(accu > satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = -satValAbsMax;
				}

				*fldDataOut++ =  accu;

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldDataIn++;
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt64In = (jvxInt64*)copyFrom[i];
			fldInt64Out = (jvxInt64*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT64_2_DATA(*fldInt64In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = -satValAbsMax;
				}
				*fldInt64Out++ =  JVX_DATA_2_INT64(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt64In++;
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt32In = (jvxInt32*)copyFrom[i];
			fldInt32Out = (jvxInt32*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT32_2_DATA(*fldInt32In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = -satValAbsMax;
				}

				*fldInt32Out++ =  JVX_DATA_2_INT32(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt32In++;
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt16In = (jvxInt16*)copyFrom[i];
			fldInt16Out = (jvxInt16*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT16_2_DATA(*fldInt16In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = -satValAbsMax;
				}
				*fldInt16Out++ =  JVX_DATA_2_INT16(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt16In++;
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		for(i = 0; i < numChans; i++)
		{

			// Copy input samples to buffer, apply gain
			fldInt8In = (jvxInt8*)copyFrom[i];
			fldInt8Out = (jvxInt8*)copyTo[i];

			for(j = 0; j < buffersize; j++)
			{
				accu = JVX_INT8_2_DATA(*fldInt8In);
				accu *= gain;
				if(accu > satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = satValAbsMax;
				}
				if(accu < -satValAbsMax)
				{
					repOverload[i] = c_true;
					accu = -satValAbsMax;
				}

				*fldInt8Out++ =  JVX_DATA_2_INT8(accu);

				accu *= accu;
				repPower[i] = (1-smooth) * accu + smooth * repPower[i];

				fldInt8In++;
			}
		}
		break;
	default:
	  assert(0);
	}
}

