#include <assert.h>

#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvx_dsp_base.h"

static void** jvx_allocateBuffer_setZero(jvxSize numChannels,
	jvxSize buffersize,
	jvxDataFormat format)
{
	int i;
	char** retBuffer = NULL;
	{
		assert((format > (int)JVX_DATAFORMAT_NONE) && (format < JVX_DATAFORMAT_STRING));

		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(retBuffer, char*, numChannels);
		for (i = 0; i < numChannels; i++)
		{
#ifdef __cplusplus
			retBuffer[i] = new char[(buffersize * jvxDataFormat_size[format])];
#else
			retBuffer[i] = (char*)malloc(sizeof(char)*(buffersize * jvxDataFormat_size[format]));
#endif
			memset(retBuffer[i], 0, buffersize * jvxDataFormat_size[format]);
		}
	}
	return (void**)retBuffer;
}

static void deallocateBuffer(void** buf,
	jvxSize numChannels)
{
	int i;
	char **buffer = (char**)buf;
	if (buffer)
	{
		for (i = 0; i < numChannels; i++)
		{
			JVX_DSP_SAFE_DELETE_FIELD(buffer[i]);
		}
		JVX_DSP_SAFE_DELETE_FIELD(buffer);
	}
}

jvxDspBaseErrorType
jvx_circbuffer_allocate(jvx_circbuffer** hdlOnReturn,
	jvxSize numberElements,
	jvxSize nUnits,
	jvxSize channels)
{
	jvx_circbuffer* hdl;
	int i;
	if (!hdlOnReturn)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	//hdl = (jvx_circbuffer*)malloc(sizeof(jvx_circbuffer) * nUnits);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl, jvx_circbuffer, nUnits);
	for (i = 0; i < nUnits; i++)
	{
		hdl[i].channels = channels;
		hdl[i].format = JVX_DATAFORMAT_DATA;
		hdl[i].idxRead = 0;
		hdl[i].fHeight = 0;
		hdl[i].length = numberElements;
		hdl[i].nUnits = nUnits;
		hdl[i].szElement = jvxDataFormat_size[hdl[i].format];
		hdl[i].ram.field = (jvxData**)jvx_allocateBuffer_setZero(hdl[i].channels, hdl[i].length, hdl[i].format);
	}
	*hdlOnReturn = hdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_allocate_extbuf(jvx_circbuffer** hdlOnReturn,
	jvxSize numberElements,
	jvxSize nUnits,
	jvxSize channels,
	jvxData** extBuffer)
{
	jvx_circbuffer* hdl;
	int i;
	if (!hdlOnReturn)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	//hdl = (jvx_circbuffer*)malloc(sizeof(jvx_circbuffer) * nUnits);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl, jvx_circbuffer, nUnits);
	for (i = 0; i < nUnits; i++)
	{
		hdl[i].channels = channels;
		hdl[i].format = JVX_DATAFORMAT_DATA;
		hdl[i].idxRead = 0;
		hdl[i].fHeight = 0;
		hdl[i].length = numberElements;
		hdl[i].nUnits = nUnits;
		hdl[i].szElement = jvxDataFormat_size[hdl[i].format];
		hdl[i].ram.field = extBuffer;
	}
	*hdlOnReturn = hdl;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_deallocate(jvx_circbuffer* hdl)
{
	int i;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	for (i = 0; i < hdl[0].nUnits; i++)
		deallocateBuffer((void**)hdl[i].ram.field, hdl->channels);
	JVX_DSP_SAFE_DELETE_FIELD(hdl);
	hdl = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_deallocate_extbuf(jvx_circbuffer* hdl)
{
	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	JVX_DSP_SAFE_DELETE_FIELD(hdl);
	hdl = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_reset(jvx_circbuffer* hdl)
{
	int i,j;

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	for (i = 0; i < hdl[0].nUnits; i++)
	{
		for (j = 0; j < hdl[i].channels; j++)
		{
			memset(hdl[i].ram.field[j], 0, hdl[i].length*hdl[i].szElement);
		}
		hdl[i].fHeight = 0;
		hdl[i].idxRead = 0;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_fill(jvx_circbuffer* hdl,
	jvxData toFillWith,
	jvxSize numberValuesFill)
{
	jvxSize i;
	jvxSize c;

	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* sPtr;
		if (JVX_CHECK_SIZE_UNSELECTED(numberValuesFill))
		{
			numberValuesFill = (hdl->length - hdl->fHeight);
		}
		if (hdl->length - hdl->fHeight >= numberValuesFill)
		{
			jvxSize idxWrite = (hdl->idxRead + hdl->fHeight) % hdl->length;
			jvxSize ll1 = JVX_MIN(numberValuesFill, hdl->length - idxWrite);
			jvxSize ll2 = numberValuesFill - ll1;

			for (c = 0; c < hdl->channels; c++)
			{
				sPtr = ptrField[c] + idxWrite;
				for (i = 0; i < ll1; i++)
				{
					*sPtr++ = toFillWith;
				}

				sPtr = ptrField[c];
				for (i = 0; i < ll2; i++)
				{
					*sPtr++ = toFillWith;
				}
			}
			hdl->fHeight += numberValuesFill;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_remove(jvx_circbuffer* hdl,
	jvxSize numberValuesRemove)
{
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		if (JVX_CHECK_SIZE_UNSELECTED(numberValuesRemove))
		{
			numberValuesRemove = (jvxInt32)hdl->fHeight;
		}
		if (hdl->fHeight >= numberValuesRemove)
		{
			hdl->idxRead = (hdl->idxRead + numberValuesRemove) % hdl->length;
			hdl->fHeight -= numberValuesRemove;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_write_update(jvx_circbuffer* hdl,
	const jvxData** fieldFill,
	jvxSize numberValuesFill)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite = NULL;
		const jvxData* ptrRead = NULL;

		if (hdl->length - hdl->fHeight >= numberValuesFill)
		{
			jvxSize idxWrite = (hdl->idxRead + hdl->fHeight) % hdl->length;
			jvxSize ll1 = JVX_MIN(numberValuesFill, hdl->length - idxWrite);
			jvxSize ll2 = numberValuesFill - ll1;

			for (c = 0; c < hdl->channels; c++)
			{
				ptrWrite = ptrField[c] + idxWrite;
				ptrRead = fieldFill[c];

				if (ll1 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
				}
				ptrWrite = ptrField[c];
				ptrRead = fieldFill[c] + ll1;

				if (ll2 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
				}
			}
			hdl->fHeight += numberValuesFill;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_write_update_wrap(jvx_circbuffer* hdl,
	const jvxData** fieldFill,
	jvxSize numberValuesFill)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite = NULL;
		const jvxData* ptrRead = NULL;

		jvxSize idxWrite = (hdl->idxRead + hdl->fHeight) % hdl->length;
		jvxSize ll1 = JVX_MIN(numberValuesFill, hdl->length - idxWrite);
		jvxSize ll2 = numberValuesFill - ll1;

		for (c = 0; c < hdl->channels; c++)
		{
			ptrWrite = ptrField[c] + idxWrite;
			ptrRead = fieldFill[c];

			if (ll1 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
			}
			ptrWrite = ptrField[c];
			ptrRead = fieldFill[c] + ll1;

			if (ll2 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
			}
		}
		hdl->fHeight += numberValuesFill;
		hdl->fHeight = hdl->fHeight % hdl->length;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_write_update_ignore(jvx_circbuffer* hdl,
	jvxData** fieldFill,
	jvxSize numberValuesFill)
{
	int c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite;
		jvxData* ptrRead;

		jvxSize idxWrite = (hdl->idxRead + hdl->fHeight) % hdl->length;
		jvxSize ll1 = JVX_MIN(numberValuesFill, hdl->length - idxWrite);
		jvxSize ll2 = numberValuesFill - ll1;

		for (c = 0; c < hdl->channels; c++)
		{
			ptrWrite = ptrField[c] + idxWrite;
			ptrRead = fieldFill[c];

			if (ll1 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
			}
			ptrWrite = ptrField[c];
			ptrRead = fieldFill[c] + ll1;

			if (ll2 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
			}
		}
		hdl->fHeight = (hdl->fHeight + numberValuesFill) % hdl->length;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}


jvxDspBaseErrorType
jvx_circbuffer_read_update(jvx_circbuffer* hdl,
	jvxData** fieldRead,
	jvxSize numberValuesRead)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite;
		jvxData* ptrRead;

		if (hdl->fHeight >= numberValuesRead)
		{
			jvxSize ll1 = JVX_MIN(numberValuesRead, hdl->length - hdl->idxRead);
			jvxSize ll2 = numberValuesRead - ll1;

			for (c = 0; c < hdl->channels; c++)
			{
				ptrRead = ptrField[c] + hdl->idxRead;
				ptrWrite = fieldRead[c];

				if (ll1 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
				}
				ptrRead = ptrField[c];
				ptrWrite = fieldRead[c] + ll1;

				if (ll2 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
				}
			}
			hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
			hdl->fHeight -= numberValuesRead;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_read_update_offset(jvx_circbuffer* hdl,
	jvxData** fieldRead,
	jvxSize numberValuesRead,
	jvxSize offset)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite;
		jvxData* ptrRead;
		jvxSize idxRead = hdl->idxRead;
		idxRead = (idxRead + offset) % hdl->length;

		if (hdl->fHeight >= numberValuesRead)
		{
			jvxSize ll1 = JVX_MIN(numberValuesRead, hdl->length - idxRead);
			jvxSize ll2 = numberValuesRead - ll1;

			for (c = 0; c < hdl->channels; c++)
			{
				ptrRead = ptrField[c] + idxRead;
				ptrWrite = fieldRead[c];

				if (ll1 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
				}
				ptrRead = ptrField[c];
				ptrWrite = fieldRead[c] + ll1;

				if (ll2 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
				}
			}
			hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
			hdl->fHeight -= numberValuesRead;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_read_update_conv(jvx_circbuffer* hdl,
	jvxHandle** fieldRead, jvxDataFormat form, jvxData scaleBeforeOut,
	jvxSize numberValuesRead)
{
	jvxSize c,cc;
	jvxData accu;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrRead;
		jvxInt16* ptrWritei16;
		jvxInt32* ptrWritei32;

		if (hdl->fHeight >= numberValuesRead)
		{
			jvxSize ll1 = JVX_MIN(numberValuesRead, hdl->length - hdl->idxRead);
			jvxSize ll2 = numberValuesRead - ll1;

			switch (form)
			{
			case JVX_DATAFORMAT_16BIT_LE:

				for (c = 0; c < hdl->channels; c++)
				{
					ptrRead = ptrField[c] + hdl->idxRead;
					ptrWritei16 = (jvxInt16*)fieldRead[c];

					for (cc = 0; cc < ll1; cc++)
					{
						accu = ptrRead[cc];
						accu *= scaleBeforeOut;
						*ptrWritei16 = JVX_DATA2INT16(accu);
						ptrWritei16++;
					}

					ptrRead = ptrField[c];

					for (cc = 0; cc < ll2; cc++)
					{
						accu = ptrRead[cc];
						accu *= scaleBeforeOut;
						*ptrWritei16 = JVX_DATA2INT16(accu);
						ptrWritei16++;
					}
				}
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				
				for (c = 0; c < hdl->channels; c++)
				{
					ptrRead = ptrField[c] + hdl->idxRead;
					ptrWritei32 = (jvxInt32*)fieldRead[c];

					for (cc = 0; cc < ll1; cc++)
					{
						accu = ptrRead[cc];
						accu *= scaleBeforeOut;
						*ptrWritei32 = JVX_DATA2INT32(accu);
						ptrWritei32++;
					}

					ptrRead = ptrField[c];

					for (cc = 0; cc < ll2; cc++)
					{
						accu = ptrRead[cc];
						accu *= scaleBeforeOut;
						*ptrWritei32 = JVX_DATA2INT32(accu);
						ptrWritei32++;
					}
				}
				break;
			default:
				assert(0);
			}
			hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
			hdl->fHeight -= numberValuesRead;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_copy_update_buf2buf(jvx_circbuffer* hdl_copyTo,
	jvx_circbuffer* hdl_copyFrom,
	jvxSize numberValuesFill)
{
	jvxDspBaseErrorType err = JVX_DSP_NO_ERROR;
	assert(hdl_copyTo[0].nUnits == 1);
	assert(hdl_copyFrom[0].nUnits == 1);
	if (hdl_copyTo && hdl_copyFrom)
	{
		jvxSize ll1, ll2, ll3, cc;
		jvxSize c;

		jvxData** ptrField_from = hdl_copyFrom->ram.field;
		jvxData* ptrRead_from;

		jvxData** ptrField_to = hdl_copyTo->ram.field;
		jvxData* ptrWrite_to;

		cc = JVX_MIN(hdl_copyTo->channels, hdl_copyFrom->channels);

		if (hdl_copyFrom->fHeight >= numberValuesFill)
		{
			if ((hdl_copyTo->length - hdl_copyTo->fHeight) >= numberValuesFill)
			{
				while (numberValuesFill > 0)
				{
					jvxSize idxWriteTo = (hdl_copyTo->idxRead + hdl_copyTo->fHeight) % hdl_copyTo->length;
					ll1 = JVX_MIN(numberValuesFill, hdl_copyFrom->length - (int)hdl_copyFrom->idxRead);
					ll2 = JVX_MIN(numberValuesFill, hdl_copyTo->length - (int)idxWriteTo);
					ll3 = JVX_MIN(ll1, ll2);

					for (c = 0; c < cc; c++)
					{
						ptrRead_from = ptrField_from[c] + hdl_copyFrom->idxRead;
						ptrWrite_to = ptrField_to[c] + idxWriteTo;
						memcpy(ptrWrite_to, ptrRead_from, sizeof(jvxData)*ll3);
					}

					hdl_copyTo->fHeight += ll3;

					hdl_copyFrom->idxRead = (hdl_copyFrom->idxRead + ll3) % hdl_copyFrom->length;
					hdl_copyFrom->fHeight -= ll3;

					numberValuesFill -= ll3;
				}
			}
			else
			{
				err = JVX_DSP_ERROR_BUFFER_OVERFLOW;
			}
		}
		return err;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_read_update_ignore(jvx_circbuffer* hdl,
	jvxData** fieldRead,
	jvxSize numberValuesRead)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite;
		jvxData* ptrRead;

		jvxSize ll1 = JVX_MIN(numberValuesRead, hdl->length - (int)hdl->idxRead);
		jvxSize ll2 = numberValuesRead - ll1;

		for (c = 0; c < hdl->channels; c++)
		{
			ptrRead = ptrField[c] + hdl->idxRead;
			ptrWrite = fieldRead[c];

			if (ll1 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
			}
			ptrRead = ptrField[c];
			ptrWrite = fieldRead[c] + ll1;

			if (ll2 > 0)
			{
				memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
			}
		}
		hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
		hdl->fHeight = (hdl->fHeight + hdl->length - numberValuesRead) % hdl->length;
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
jvxDspBaseErrorType
jvx_circbuffer_read_noupdate(jvx_circbuffer* hdl,
	jvxData** fieldRead,
	jvxSize numberValuesRead)
{
	jvxSize c;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData** ptrField = hdl->ram.field;
		jvxData* ptrWrite;
		jvxData* ptrRead;

		if (hdl->fHeight >= numberValuesRead)
		{
			jvxSize ll1 = JVX_MIN(numberValuesRead, hdl->length - (int)hdl->idxRead);
			jvxSize ll2 = numberValuesRead - ll1;

			for (c = 0; c < hdl->channels; c++)
			{
				ptrRead = ptrField[c] + hdl->idxRead;
				ptrWrite = fieldRead[c];

				if (ll1 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll1);
				}
				ptrRead = ptrField[c];
				ptrWrite = fieldRead[c] + ll1;

				if (ll2 > 0)
				{
					memcpy(ptrWrite, ptrRead, sizeof(jvxData)*ll2);
				}
			}

			// This is the difference to previous function
			//hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
			//hdl->fHeight -= numberValuesRead;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType 
jvx_circbuffer_advance_read_index(jvx_circbuffer * hdl,
	jvxSize numberValuesRead)
{
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		if (hdl->fHeight >= numberValuesRead)
		{
			
			hdl->idxRead = (hdl->idxRead + numberValuesRead) % hdl->length;
			hdl->fHeight -= numberValuesRead;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_BUFFER_OVERFLOW;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;

}

// =====================================================================================

jvxDspBaseErrorType
jvx_circbuffer_fir_1can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_fir_1can_2io(hdlIn, fCoeffs_fw, fieldInOut, fieldInOut, bSize);
}

jvxDspBaseErrorType jvx_circbuffer_fir_2can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_fir_2can_2io(hdlIn, fCoeffs_fw, fieldInOut, fieldInOut, bSize);
}

jvxDspBaseErrorType jvx_circbuffer_iir_1can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_iir_1can_2io(hdlIn, fCoeffs_fw, fCoeffs_bw, fieldInOut, fieldInOut, bSize);
}

jvxDspBaseErrorType jvx_circbuffer_iir_2can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_iir_2can_2io(hdlIn, fCoeffs_fw, fCoeffs_bw, fieldInOut, fieldInOut, bSize);
}

jvxDspBaseErrorType jvx_circbuffer_iir_sos1can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData* gains,
	jvxInt16 shiftFactor,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_iir_sos1can_2io(hdlIn, fCoeffs_fw, fCoeffs_bw, gains, shiftFactor, fieldInOut, fieldInOut, bSize);
}

jvxDspBaseErrorType jvx_circbuffer_iir_sos2can_1io(jvx_circbuffer* hdlIn,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData* gains,
	jvxInt16 shiftFactor,
	jvxData** fieldInOut,
	jvxSize bSize)
{
	return jvx_circbuffer_iir_sos2can_2io(hdlIn, fCoeffs_fw, fCoeffs_bw, gains, shiftFactor, fieldInOut, fieldInOut, bSize);
}

// =====================================================================================

jvxDspBaseErrorType
jvx_circbuffer_fir_1can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	int c, i, l;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData accuin = 0;
		jvxData accuout = 0;

		jvxData accu1 = 0, accu2 = 0;
		jvxData* in = NULL, *out = NULL;

		// Special condition here: length is always identical to number of filter states
		assert(hdl->idxRead == 0);

		for (c = 0; c < hdl->channels; c++)
		{
			in = fieldIn[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* statePtr = hdl->ram.field[c];
				jvxData* fCoeffsRev_fw;

				// Input
				accuin = *in++;
				accuout = (*statePtr++ + *fCoeffs_fw * accuin);

				// Store value from previous state variable (there is none at the beginning)
				accu1 = 0.0;
				fCoeffsRev_fw = fCoeffs_fw + hdl->length;

				for (l = 0; l < hdl->length - 1; l++)
				{
					accu2 = *fCoeffsRev_fw-- * accuin + accu1;
					accu1 = *statePtr;
					*statePtr++ = accu2;
				}

				// Wrap circ buffer
				statePtr = hdl->ram.field[c];

				// Last step
				*statePtr = accu1 + *fCoeffsRev_fw * accuin;

				// Output
				*out++ = accuout;
			}
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_fir_2can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	int c, i, l;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData accuin = 0;
		jvxData accuout = 0;

		jvxData accu1 = 0;
		jvxData* in = NULL;
		jvxData* out = NULL;


		jvxSize idxRead = hdl->idxRead;
		for (c = 0; c < hdl->channels; c++)
		{
			jvxData* statePtr;
			idxRead = hdl->idxRead;
			statePtr = hdl->ram.field[c] + idxRead;

			in = fieldIn[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* coeffPtr = fCoeffs_fw;
				jvxSize ll1, ll2;

				// Input
				accuin = *in++;

				// Output
				accuout = 0.0;

				// Store first fw coefficient
				accu1 = *coeffPtr++;

				ll1 = hdl->length - idxRead;
				ll2 = idxRead;

				for (l = 0; l < ll1; l++)
				{
					accuout += *coeffPtr++ * *statePtr++;
				}

				statePtr = hdl->ram.field[c];
				for (l = 0; l < ll2; l++)
				{
					accuout += *coeffPtr++ * *statePtr++;
				}

				accuout += accu1 * accuin;
				*out++ = accuout;

				// Final statePtr++ realized by index increment
				idxRead = (idxRead + hdl->length - 1) % hdl->length;
				statePtr = hdl->ram.field[c] + idxRead;

				*statePtr = accuin;

			}
		}
		hdl->idxRead = idxRead;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_iir_1can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	int c, i, l;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData accuin = 0;
		jvxData accuout = 0;

		jvxData accu1 = 0, accu2 = 0;
		jvxData* in = NULL, *out = NULL;

		// Special condition here: length is always identical to number of filter states
		assert(hdl->idxRead == 0);

		for (c = 0; c < hdl->channels; c++)
		{
			in = fieldIn[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* statePtr = hdl->ram.field[c];
				jvxData* fCoeffsRev_fw;
				jvxData* fCoeffsRev_bw;

				// Input
				accuin = *in++;
				accuout = (*statePtr++ + *fCoeffs_fw * accuin);

				// Store value from previous state variable (there is none at the beginning)
				accu1 = 0.0;
				fCoeffsRev_fw = fCoeffs_fw + hdl->length;
				fCoeffsRev_bw = fCoeffs_bw + hdl->length;

				for (l = 0; l < hdl->length - 1; l++)
				{
					accu2 = *fCoeffsRev_fw-- * accuin - *fCoeffsRev_bw-- * accuout + accu1;
					accu1 = *statePtr;
					*statePtr++ = accu2;
				}

				// Wrap circ buffer
				statePtr = hdl->ram.field[c];

				// Last step
				*statePtr = accu1 + *fCoeffsRev_fw * accuin - *fCoeffsRev_bw * accuout;

				// Output
				*out++ = accuout / (*fCoeffs_bw);
			}
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_iir_2can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	int c, i, l;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData accuin = 0;
		jvxData accuout = 0;

		jvxData accu1 = 0, accu2 = 0;
		jvxData* in = NULL;
		jvxData* out = NULL;


		jvxSize idxRead = hdl->idxRead;
		for (c = 0; c < hdl->channels; c++)
		{
			jvxData* statePtr;
			idxRead = hdl->idxRead;
			statePtr = hdl->ram.field[c] + idxRead;

			in = fieldIn[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* coeffPtr_fw = fCoeffs_fw;
				jvxData* coeffPtr_bw = fCoeffs_bw;
				jvxSize ll1, ll2;

				// Input
				accuin = *in++;

				// Output
				accuout = 0.0;

				// Store first fw and bw coefficient
				accu1 = *coeffPtr_fw++;
				accu2 = *coeffPtr_bw++;

				ll1 = hdl->length - idxRead;
				ll2 = idxRead;

				for (l = 0; l < ll1; l++)
				{
					accuin -= *coeffPtr_bw++ * *statePtr;
					accuout += *coeffPtr_fw++ * *statePtr++;
				}

				statePtr = hdl->ram.field[c];
				for (l = 0; l < ll2; l++)
				{
					accuin -= *coeffPtr_bw++ * *statePtr;
					accuout += *coeffPtr_fw++ * *statePtr++;
				}

				// Correction found 11 Dec 2014, HK
				// Before:
				//accuout += accu1 * accuin;
				//*out++ = accuout/accu2;
				accuin /= accu2;
				accuout += accu1 * accuin;
				*out++ = accuout;

				// Final statePtr++ realized by index increment
				idxRead = (idxRead + hdl->length - 1) % hdl->length;
				statePtr = hdl->ram.field[c] + idxRead;

				*statePtr = accuin;

			}
		}
		hdl->idxRead = idxRead;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_iir_sos1can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData* gains,
	jvxInt16 shiftFactor,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	/* was this implementation

		int i,j;
		 jvxData accuIn;
		 jvxData accuOut;
		 jvxData accu;
		 const jvxData* coeffsFwd_loc = coeffsFwd_in;
		 const jvxData* coeffsBwd_loc = coeffsBwd_in;
		 jvxData valShift = (1<<shiftFacInLoop);

		 for(i = 0; i < frameSize; i++)
		 {
		  // Reset filter coeffs to init position
		  coeffsFwd_loc = coeffsFwd_in;
		  coeffsBwd_loc = coeffsBwd_in;

		  accuIn = *input++ * gain;
		  accuOut = states[*circBufIdx] + *coeffsFwd_loc++ * accuIn;
		  accuOut = accuOut * valShift;

		  *output++ = accuOut;
		  coeffsBwd_loc++;
		  for(j = 0; j < order-1; j++)
		  {
			   // Decrement circular ptr
			   *circBufIdx = (*circBufIdx + (order - 1)) % order;
			   accu = states[*circBufIdx];
			   accu = accu + *coeffsFwd_loc++ * accuIn;
			   accu = accu - *coeffsBwd_loc++ * accuOut;
			   states[*circBufIdx] = accu;
		  }

		  *circBufIdx = (*circBufIdx + (order - 1)) % order;
		  accu = *coeffsFwd_loc++ * accuIn;
		  accu = accu - *coeffsBwd_loc++ * accuOut;
		  states[*circBufIdx] = accu;

		  // Shift circ pointer forward
		  *circBufIdx = (*circBufIdx + (order - 1)) % order;
		 }
	*/
	int c, i, l, s;

	jvxData accuin = 0;
	jvxData accuout = 0;

	jvxData accu1 = 0, accu2 = 0;
	jvxData* in = NULL, *out = NULL;

	jvxData *fCoeffsStage_fw;
	jvxData *fCoeffsStage_bw;

	jvxData valShift = (1 << shiftFactor);

	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;

	// Special condition here: length is always identical to number of filter states
	for (s = 0; s < hdl[0].nUnits; s++)
	{
		assert(hdl[s].idxRead == 0);
		fCoeffsStage_fw = fCoeffs_fw + s*(hdl[s].length + 1);
		fCoeffsStage_bw = fCoeffs_bw + s*(hdl[s].length + 1);
		for (c = 0; c < hdl[s].channels; c++)
		{
			if (s == 0)
				in = fieldIn[c];
			else
				in = fieldOut[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* statePtr = hdl[s].ram.field[c];
				jvxData* fCoeffsRev_fw;
				jvxData* fCoeffsRev_bw;

				// Input
				accuin = *in++ * gains[s];
				accuout = *statePtr++ + *fCoeffsStage_fw * accuin;
				accuout *= valShift;

				// Output
				*out++ = accuout / *fCoeffsStage_bw;

				// Store value from previous state variable (there is none at the beginning)
				accu1 = 0.0;
				fCoeffsRev_fw = fCoeffsStage_fw + hdl[s].length;
				fCoeffsRev_bw = fCoeffsStage_bw + hdl[s].length;

				for (l = 0; l < hdl[s].length - 1; l++)
				{
					accu2 = *fCoeffsRev_fw-- * accuin - *fCoeffsRev_bw-- * accuout + accu1;
					accu1 = *statePtr;
					*statePtr++ = accu2;
				}

				// Wrap circ buffer
				statePtr = hdl[s].ram.field[c];

				// Last step
				*statePtr = accu1 + *fCoeffsRev_fw * accuin - *fCoeffsRev_bw * accuout;

			}
		}
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType jvx_circbuffer_iir_sos2can_2io(jvx_circbuffer* hdl,
	jvxData* fCoeffs_fw,
	jvxData* fCoeffs_bw,
	jvxData* gains,
	jvxInt16 shiftFactor,
	jvxData** fieldIn,
	jvxData** fieldOut,
	jvxSize bSize)
{
	/* was this implementation
		 int i,j;
		 jvxData accuIn;
		 jvxData accuOut;
		 jvxData accu;
		 const jvxData* coeffsFwd_loc_rev;
		 const jvxData* coeffsBwd_loc_rev;
		 jvxData valShift = (1<<shiftFacInLoop);
		 for(i = 0; i < frameSize; i++)
		 {
		  coeffsFwd_loc_rev = coeffsFwd_rev_in;
		  coeffsBwd_loc_rev = coeffsBwd_rev_in;
		  accuIn = *input++ * gain;
		  accuOut = 0;
		  for(j = 0; j < order; j++)
		  {
			   // Decrement circular ptr
			   accu = states[*circBufIdx];
			   accuOut += *coeffsFwd_loc_rev++ * accu;
			   accuIn -= *coeffsBwd_loc_rev++ * accu;
			   *circBufIdx = (*circBufIdx + (order - 1)) % order;
		  }

		  accuIn = accuIn * valShift;
		  states[*circBufIdx] = accuIn;
		  accuOut += *coeffsFwd_loc_rev * accuIn;
		  *circBufIdx = (*circBufIdx + (order - 1)) % order;

		  *output++ = accuOut;
		 }
	*/
	int c, i, l, s;
	jvxData accuin = 0;
	jvxData accuout = 0;

	jvxData accu1 = 0, accu2 = 0;
	jvxData* in = NULL;
	jvxData* out = NULL;
	jvxData *fCoeffsStage_fw;
	jvxData *fCoeffsStage_bw;

	jvxData valShift = (1 << shiftFactor);

	jvxSize idxRead;
	if (!hdl)
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	for (s = 0; s < hdl[0].nUnits; s++)
	{
		fCoeffsStage_fw = fCoeffs_fw + s*(hdl[s].length + 1);
		fCoeffsStage_bw = fCoeffs_bw + s*(hdl[s].length + 1);

		for (c = 0; c < hdl[s].channels; c++)
		{
			jvxData* statePtr;
			idxRead = hdl[s].idxRead;
			statePtr = hdl[s].ram.field[c] + idxRead;

			if (s == 0)
				in = fieldIn[c];
			else
				in = fieldOut[c];
			out = fieldOut[c];
			for (i = 0; i < bSize; i++)
			{
				jvxData* coeffPtr_fw = fCoeffsStage_fw;
				jvxData* coeffPtr_bw = fCoeffsStage_bw;
				jvxSize ll1, ll2;

				// Input
				accuin = *in++ * gains[s];

				// Output
				accuout = 0.0;

				// Store first fw and bw coefficient
				accu1 = *coeffPtr_fw++;
				accu2 = *coeffPtr_bw++;

				ll1 = hdl[s].length - idxRead;
				ll2 = idxRead;

				for (l = 0; l < ll1; l++)
				{
					accuin -= *coeffPtr_bw++ * *statePtr;
					accuout += *coeffPtr_fw++ * *statePtr++;
				}

				statePtr = hdl[s].ram.field[c];
				for (l = 0; l < ll2; l++)
				{
					accuin -= *coeffPtr_bw++ * *statePtr;
					accuout += *coeffPtr_fw++ * *statePtr++;
				}

				accuin /= accu2 * valShift;
				accuout += accu1 * accuin;
				*out++ = accuout;

				// Final statePtr++ realized by index increment
				idxRead = (idxRead + hdl[s].length - 1) % hdl[s].length;
				statePtr = hdl[s].ram.field[c] + idxRead;

				*statePtr = accuin;

			}
		}
		hdl[s].idxRead = idxRead;
	}
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_allocate_1chan(jvx_circbuffer** hdlOnReturn,
	jvxSize numberElements)
{
	return jvx_circbuffer_allocate(hdlOnReturn, numberElements, 1, 1);
}

jvxDspBaseErrorType
jvx_circbuffer_write_update_1chan(jvx_circbuffer* hdl,
	const jvxData* fieldFill,
	jvxSize numberValuesFill)
{
	return jvx_circbuffer_write_update(hdl, &fieldFill, numberValuesFill);
}

jvxDspBaseErrorType
jvx_circbuffer_read_update_1chan(jvx_circbuffer* hdl,
	jvxData* fieldRead,
	jvxSize numberValuesRead)
{
	return jvx_circbuffer_read_update(hdl, &fieldRead, numberValuesRead);
}

jvxDspBaseErrorType
jvx_circbuffer_read_update_1chan_conv(jvx_circbuffer* hdl,
	jvxHandle* fieldRead, jvxDataFormat form, jvxData scaleBeforeOut,
	jvxSize numberValuesRead)
{
	return jvx_circbuffer_read_update_conv(hdl, &fieldRead, form, scaleBeforeOut, numberValuesRead);
}

jvxDspBaseErrorType
jvx_circbuffer_read_noupdate_1chan(jvx_circbuffer* hdl,
	jvxData* fieldRead,
	jvxSize numberValuesRead)
{
	return jvx_circbuffer_read_noupdate(hdl, &fieldRead, numberValuesRead);
}

jvxDspBaseErrorType jvx_circbuffer_write_convolve_update_1chan(jvx_circbuffer* hdl,
	jvxData input,
	jvxData* coeffs,
	jvxSize lengthCoeffBuffer,
	jvxData* output)
{
	int k;
	jvxSize ll1, ll2;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData* ptrWriteOrig = hdl->ram.field[0];

		jvxData* ptrWrite = ptrWriteOrig + hdl->idxRead;

		jvxData accu = 0.0;

		ll1 = hdl->length - hdl->idxRead;
		ll2 = hdl->length - ll1;


		*ptrWrite = input;
		for (k = 0; k < ll1; k++)
		{
			accu += *ptrWrite++ * *coeffs++;
		}

		for (k = 0; k < ll2; k++)
		{
			accu += *ptrWriteOrig++ * *coeffs++;
		}
		*output = accu;

		hdl->idxRead = (hdl->idxRead + (hdl->length - 1)) % hdl->length;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_write_convolve_no_update_1chan(jvx_circbuffer* hdl,
	jvxData input,
	jvxData* coeffs,
	jvxSize lengthCoeffBuffer,
	jvxData* output)
{
	int  k;
	jvxSize ll1, ll2;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData* ptrWriteOrig = hdl->ram.field[0];
		jvxData* ptrWrite = ptrWriteOrig + hdl->idxRead;

		jvxData accu = 0.0;

		ll1 = hdl->length - hdl->idxRead;
		ll2 = hdl->length - ll1;


		*ptrWrite = input;
		for (k = 0; k < ll1; k++)
		{
			accu += *ptrWrite++ * *coeffs++;
		}

		for (k = 0; k < ll2; k++)
		{
			accu += *ptrWriteOrig++ * *coeffs++;
		}
		*output = accu;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_write_convolve_only_update_1chan(jvx_circbuffer* hdl)
{
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		hdl->idxRead = (hdl->idxRead + (hdl->length - 1)) % hdl->length;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_write_norm_update_1chan(jvx_circbuffer* hdl,
	jvxData input,
	jvxData* output)
{
	int k;
	jvxSize ll1, ll2;
	jvxData tmp = 0.0;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData* ptrWriteOrig = hdl->ram.field[0];
		jvxData* ptrWrite = ptrWriteOrig + hdl->idxRead;

		jvxData accu = 0.0;

		ll1 = hdl->length - hdl->idxRead;
		ll2 = hdl->length - ll1;


		*ptrWrite = input;
		for (k = 0; k < ll1; k++)
		{
			tmp = *ptrWrite++;
			accu += tmp * tmp;
		}

		for (k = 0; k < ll2; k++)
		{
			tmp = *ptrWriteOrig++;
			accu += tmp * tmp;
		}
		*output++ = accu;

		hdl->idxRead = (hdl->idxRead + (hdl->length - 1)) % hdl->length;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_write_norm_no_update_1chan(jvx_circbuffer* hdl,
	jvxData input,
	jvxData* output)
{
	int k;
	jvxSize ll1, ll2;
	jvxData tmp = 0.0;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxData* ptrWriteOrig = hdl->ram.field[0];
		jvxData* ptrWrite = ptrWriteOrig + hdl->idxRead;
		jvxData accu = 0.0;

		ll1 = hdl->length - hdl->idxRead;
		ll2 = hdl->length - ll1;


		*ptrWrite = input;
		for (k = 0; k < ll1; k++)
		{
			tmp = *ptrWrite++;
			accu += tmp * tmp;
		}

		for (k = 0; k < ll2; k++)
		{
			tmp = *ptrWriteOrig++;
			accu += tmp * tmp;
		}
		*output++ = accu;

		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_write_norm_only_update_1chan(jvx_circbuffer* hdl)
{
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		hdl->idxRead = (hdl->idxRead + (hdl->length - 1)) % hdl->length;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_fld_mult_fac_add_circbuffer_fac(jvxData* fldInOut,
	jvxSize sz,
	jvxData fac1,
	jvxData fac2,
	jvx_circbuffer* hdl)
{
	int i;

	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxSize ll1 = hdl->length - hdl->idxRead;
		jvxSize ll2 = hdl->length - ll1;

		jvxData* ptrRead = hdl->ram.field[0] + hdl->idxRead;
		for (i = 0; i < ll1; i++)
		{
			*fldInOut = fac1 * *fldInOut + fac2 * *ptrRead++;
			fldInOut++;
		}

		ptrRead = hdl->ram.field[0];
		for (i = 0; i < ll2; i++)
		{
			*fldInOut = fac1 * *fldInOut + fac2 * *ptrRead++;
			fldInOut++;
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType jvx_circbuffer_fld_mult_fac_add_circbuffer_fac_ret_energy(jvxData* fldInOut,
	jvxSize sz,
	jvxData fac1,
	jvxData fac2,
	jvx_circbuffer* hdl,
	jvxData* energyFilter)
{
	int i;
	jvxData energy = 0;
	assert(hdl[0].nUnits == 1);
	if (hdl)
	{
		jvxSize ll1 = hdl->length - hdl->idxRead;
		jvxSize ll2 = hdl->length - ll1;

		jvxData* ptrRead = hdl->ram.field[0] + hdl->idxRead;
		for (i = 0; i < ll1; i++)
		{
			*fldInOut = fac1 * *fldInOut + fac2 * *ptrRead++;
			energy += *fldInOut * *fldInOut;
			fldInOut++;
		}

		ptrRead = hdl->ram.field[0];
		for (i = 0; i < ll2; i++)
		{
			*fldInOut = fac1 * *fldInOut + fac2 * *ptrRead++;
			energy += *fldInOut * *fldInOut;
			fldInOut++;
		}

		*energyFilter = energy;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_ccorr_1chan(jvx_circbuffer* hdl1,
	jvx_circbuffer* hdl2,
	jvxData* fieldUpdate,
	jvxData* fieldRead2,
	jvxData* fieldRead1,
	jvxSize numValues,
	jvxData smooth)
{
	// The cross assignemnt of input channels is to be according to FFT based ccorr in jvxA_beamformer_v2

	int i, j;
	jvxSize idxRead;
	jvxData tmp1, tmp2;
	jvxData* ptrHdl1;
	jvxData* ptrHdl2;

	jvxData* fieldUpdateFwd = NULL;
	jvxData* fieldUpdateBwd = NULL;

	assert(hdl1[0].nUnits == 1);
	assert(hdl2[0].nUnits == 1);

	assert(hdl1->channels == 1);
	assert(hdl2->channels == 1);
	assert(hdl1->idxRead == hdl2->idxRead);
	assert(hdl1->length == hdl2->length);

	ptrHdl1 = hdl1->ram.field[0];
	ptrHdl2 = hdl2->ram.field[0];
	idxRead = hdl1->idxRead;


	for (i = 0; i < numValues; i++)
	{
		jvxSize tt1, tt2, cnt;

		fieldUpdateFwd = fieldUpdate;
		fieldUpdateBwd = fieldUpdate + (hdl2->length - 1) * 2;

		tmp1 = *fieldRead1++;
		tmp2 = *fieldRead2++;

		ptrHdl1[idxRead] = tmp1;
		ptrHdl2[idxRead] = tmp2;

		tt1 = hdl1->length - idxRead - 1;
		tt2 = idxRead;
		cnt = idxRead + 1;

		for (j = 0; j < tt1; j++)
		{
			*fieldUpdateFwd = (1 - smooth) * tmp1 * ptrHdl2[cnt] + smooth * *fieldUpdateFwd;
			*fieldUpdateBwd = (1 - smooth) * tmp2 * ptrHdl1[cnt] + smooth * *fieldUpdateBwd;
			fieldUpdateFwd++;
			fieldUpdateBwd--;
			cnt++;
		}
		cnt = 0;

		for (j = 0; j < tt2; j++)
		{
			*fieldUpdateFwd = (1 - smooth) * tmp1 * ptrHdl2[cnt] + smooth * *fieldUpdateFwd;
			*fieldUpdateBwd = (1 - smooth) * tmp2 * ptrHdl1[cnt] + smooth * *fieldUpdateBwd;
			fieldUpdateFwd++;
			fieldUpdateBwd--;
			cnt++;
		}

		*fieldUpdateFwd = (1 - smooth) * tmp1 * tmp2 + smooth * *fieldUpdateFwd;

		idxRead = (idxRead + 1) % hdl2->length;
	}
	hdl1->idxRead = idxRead;
	hdl2->idxRead = idxRead;
	return JVX_DSP_NO_ERROR;
}

/*
 * Implementation of a filter which realizes a concatenation
 * of filters of the same order in 1st canonic form. 
 * For example, given a cascade of 2 3rd order IIR filters with
 *
 * b = [0.1301    0.3902    0.3902    0.1301] (Numerator part)
 * a = [1.0000   -0.2881    0.3553   -0.0267] (Denominator part)
 * we can filter twice the filter as
 * bb = [0.1301    0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902] 
 * (<- Numerator coeff[0] as given and all other coefficients in reversed order!)
 * aa = [ 1.0000   -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553   -0.2881]; 
 * (<- Denominator coeff[0] as given but the other coefficients in reverse order!)
 * by the following command sequence:
 *
 * states = zeros(1,6);
 * [out, states] = jvx_filter_cascade_1can(2, 3, states, aa, bb, in);
 *
 * Input 1: Number sections
 * Input 2: Order each section
 * Input 3: States, must be a field of length <Input 1> * <Input 2>
 * Input 4: Denominator coefficients
 * Input 5: Numerator coefficients
 * Input 6: Input signal (1xN)
 * The function uses a linear buffer propagation and therefore does not rely on circular buffers.
 * Example for demonstration:
 *
 * in = [1 zeros(1, 127)];
 * b = [0.1301    0.3902    0.3902    0.1301];
 * a = [1.0000   -0.2881    0.3553   -0.0267];
 * out_orig = filter(b, a, filter(b, a, in));
 *
 * bb = [0.1301    0.1301    0.3902    0.3902    0.1301    0.1301    0.3902 0.3902];
 * aa = [1.0000   -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553 -0.2881];
 * states = zeros(1,6);
 * [out_new, states] = jvx_filter_cascade_1can(2, 3, states, aa, bb, in);
 *
 * figure; plot(out_orig); hold on; plot(out_new, 'r:');
 *
 * HK, Javox/IKS, 2017
 */
 jvxDspBaseErrorType
 jvx_circbuffer_filter_cascade_1can(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize)
 {
	 jvxSize c, i, j, k;
	 assert(hdl->nUnits == 1);
	 assert(hdl->length == nSections* order);
	 assert(hdl->format == JVX_DATAFORMAT_DATA);

	 for (c = 0; c < hdl->channels; c++)
	 {
		 jvxData* bufInOut = inout[c];

		 for (i = 0; i < buffersize; i++)
		 {
			 jvxData accuin = *bufInOut, accuout = 0, st = 0, mem = 0;
			 jvxData* arun = a;
			 jvxData* brun = b;
			 jvxData* states = hdl->ram.field[c];
			 jvxData* statesHold = NULL;

			 for (j = 0; j < nSections; j++)
			 {
				 statesHold = states;
				 st = *states++;
				 accuout = *brun++ * accuin + st;
				 accuout = 1.0 / *arun++ * accuout;

				 mem = 0.0;

				 for (k = 0; k < order - 1; k++)
				 {
					 st = mem - *arun++ * accuout + *brun++ * accuin;
					 mem = *states;
					 *states++ = st;
				 }
				 st = mem - *arun++ * accuout + *brun++ * accuin;
				 *statesHold = st;

				 accuin = accuout;
			 }
			 *bufInOut = accuin;
			 bufInOut++;
		 }
	 }
	 return JVX_DSP_NO_ERROR;
 }
 
jvxDspBaseErrorType
jvx_circbuffer_filter_cascade_1can_inv(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize)
{
	jvxSize c, i, j, k;
	assert(hdl->nUnits == 1);
	assert(hdl->length == nSections* order);
	assert(hdl->format == JVX_DATAFORMAT_DATA);

	for (c = 0; c < hdl->channels; c++)
	{
		jvxData* bufInOut = inout[c];

		for (i = 0; i < buffersize; i++)
		{
			jvxData accuin = *bufInOut, accuout = 0, st = 0, mem = 0;
			jvxData* arun = a;
			jvxData* brun = b;
			jvxData* states = hdl->ram.field[c];
			jvxData* statesHold = NULL;

			for (j = 0; j < nSections; j++)
			{
				statesHold = states;
				st = *states++;
				accuout = *brun++ * accuin + st;
				accuout = 1.0 / *arun++ * accuout;

				mem = 0.0;

				for (k = 0; k < order - 1; k++)
				{
					st = mem - *arun++ * accuout + *brun++ * accuin;
					mem = *states;
					*states++ = st;
				}
				st = mem - *arun++ * accuout + *brun++ * accuin;
				*statesHold = st;

				accuin = accuout;
			}
			*bufInOut = -accuin;
			bufInOut++;
		}
	}
	return JVX_DSP_NO_ERROR;
}

/*
 * Implementation of a filter which realizes a concatenation
 * of filters of the same order in 2nd canonic form.
 * For example, given a cascade of 2 3rd order IIR filters with
 *
 * b = [0.1301    0.3902    0.3902    0.1301] (Numerator part)
 * a = [1.0000   -0.2881    0.3553   -0.0267] (Denominator part)
 * we can filter twice the filter as
 * bb = [0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902
 * 0.1301] (Numerator but in reverse order!)
 * aa = [ -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553
 * -0.2881    1.0000]; (Denominator but in reverse order!)
 * by the following command sequence:
 *
 * states = zeros(1,6);
 * [out, states, off] = jvx_filter_cascade_can2_cb(2, 3, states, aa, bb, in, 0);
 *
 * Input 1: Number Sections
 * Input 2: Order each section
 * Input 3: States, must be a field of length <Input 1> * <Input 2>
 * Input 4: Denominator coefficients
 * Input 5: Numerator coefficients
 * Input 6: Input signal (1xN)
 * Input 7: Offset in state buffer at the beginning.
 * The function uses a modulo operation in the states buffer and is
 * therefore specifically suited for architectures with support for hardware
 * supported circular buffering.
 * Example for demonstration:
 *
 * in = [1 zeros(1, 127)];
 * b = [0.1301    0.3902    0.3902    0.1301];
 * a = [1.0000   -0.2881    0.3553   -0.0267];
 * out_orig = filter(b, a, filter(b, a, in));
 *
 * bb = [0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902 0.1301];
 * aa = [ -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553 -0.2881    1.0000];
 * states = zeros(1,6);
 * [out_new, states, off] = jvx_filter_cascade_2can_cb(2, 3, states, aa, bb, in, 0);
 *
 * figure; plot(out_orig); hold on; plot(out_new, 'r:');
 *
 * HK, Javox/IKS, 2017
*/
jvxDspBaseErrorType
jvx_circbuffer_filter_cascade_2can_cb(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize)
{
	jvxSize c, i, j, k;
	assert(hdl->nUnits == 1);
	assert(hdl->format == JVX_DATAFORMAT_DATA);

	jvxSize offStart = hdl->idxRead;
	jvxSize off = 0;

	for (c = 0; c < hdl->channels; c++)
	{
		jvxData* bufInOut = inout[c];

		off = offStart;
		for (i = 0; i < buffersize; i++)
		{
			// cntLocal = offset;
			jvxData accuin = bufInOut[i], accuout = 0.0, st = 0.0;
			jvxData exc = 0.0;
			jvxData* states = hdl->ram.field[c];
			jvxData* arun = a;
			jvxData* brun = b;
			for (j = 0; j < nSections; j++)
			{
				accuout = 0;
				st = states[off];
				states[off] = exc;
				off = (off + 1) % hdl->length;

				for (k = 0; k < order - 1; k++)
				{
					accuin -= *arun++ * st;
					accuout += *brun++ * st;

					st = states[off];
					off = (off + 1) % hdl->length;
				}
				accuin -= *arun++ * st;
				accuout += *brun++ * st;

				accuin *= 1.0 / *arun++;
				exc = accuin;
				accuin = accuout + accuin * *brun++;
			}
			bufInOut[i] = accuin;
			states[off] = exc;
			off = (off + 1) % hdl->length;
		}
	}
	hdl->idxRead = off;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_filter_cascade_2can_cb_inv(jvx_circbuffer* hdl, jvxData* a, jvxData *b, jvxData** inout, jvxSize nSections, jvxSize order, jvxSize buffersize)
{
	jvxSize c, i, j, k;
	assert(hdl->nUnits == 1);
	assert(hdl->format == JVX_DATAFORMAT_DATA);

	jvxSize offStart = hdl->idxRead;
	jvxSize off = 0;

	for (c = 0; c < hdl->channels; c++)
	{
		jvxData* bufInOut = inout[c];

		off = offStart;
		for (i = 0; i < buffersize; i++)
		{
			// cntLocal = offset;
			jvxData accuin = bufInOut[i], accuout = 0.0, st = 0.0;
			jvxData exc = 0.0;
			jvxData* states = hdl->ram.field[c];
			jvxData* arun = a;
			jvxData* brun = b;
			for (j = 0; j < nSections; j++)
			{
				accuout = 0;
				st = states[off];
				states[off] = exc;
				off = (off + 1) % hdl->length;

				for (k = 0; k < order - 1; k++)
				{
					accuin -= *arun++ * st;
					accuout += *brun++ * st;

					st = states[off];
					off = (off + 1) % hdl->length;
				}
				accuin -= *arun++ * st;
				accuout += *brun++ * st;

				accuin *= 1.0 / *arun++;
				exc = accuin;
				accuin = accuout + accuin * *brun++;
			}
			bufInOut[i] = accuin;
			states[off] = exc;
			off = (off + 1) % hdl->length;
		}
	}
	hdl->idxRead = off;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_circbuffer_energy(jvx_circbuffer* hdl, jvxData* energyOut, jvxCBool update)
{
	jvxSize i, c;
	for (c = 0; c < hdl->channels; c++)
	{
		jvxData accu = 0.0;
		jvxData* ptrData = hdl->ram.field[c];
		for (i = 0; i < hdl->length; i++)
		{
			accu += ptrData[i] * ptrData[i];
		}
		energyOut[c] = accu;
	}
	if (update)
	{
		hdl->idxRead = (hdl->idxRead + hdl->fHeight) % hdl->length;
		hdl->fHeight = 0;
	}
	return JVX_NO_ERROR;
}
