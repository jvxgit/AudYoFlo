#include "jvx-helpers.h"

// =======================================================================================

 void jvx_samples_clear(void* to, jvxSize szElm, jvxSize numElms)
{
	memset(to, 0, szElm * numElms);
}

 void jvx_convertSamples_memcpy(void* from, void* to, jvxSize szElm, jvxSize numElms)
{
	memcpy(to, from, szElm * numElms);
}

 void jvx_mixSamples_flp(jvxData* ptrFrom, jvxData* ptrMixTo, jvxSize bsize)
 {
	 jvxSize i;
	 for (i = 0; i < bsize; i++)
	 {
		 *ptrMixTo += *ptrFrom;
		 ptrMixTo++;
		 ptrFrom++;
	 }
 }

 void jvx_mixSamples_flp_gain(jvxData* ptrFrom, jvxData* ptrMixTo, jvxSize bsize, jvxData gain)
 {
	 jvxSize i;
	 for (i = 0; i < bsize; i++)
	 {
		 *ptrMixTo += *ptrFrom * gain;
		 ptrMixTo++;
		 ptrFrom++;
	 }
 }

 void jvx_convertSamples_memcpy_offset(void* from, void* to, jvxSize szElm, jvxSize numElms, jvxInt32 offsetInput, jvxInt32 offsetOutput)
{
	memcpy(((jvxByte*)to) + (offsetOutput*szElm), ((jvxByte*)from) + (szElm * offsetInput), szElm * numElms);
}

void
jvx_fields_clear(void** to, jvxSize szElm, jvxSize numElms, jvxSize numChannels)
{
	jvxSize i;
	for (i = 0; i < numChannels; i++)
	{
		jvx_samples_clear(to[i], szElm, numElms);
	}
}

void
jvx_fields_lst_clear(void*** to, jvxSize szElm, jvxSize numElms, jvxSize numChannels, jvxSize numFlds)
{
	jvxSize i;
	for (i = 0; i < numFlds; i++)
	{
		jvx_fields_clear(to[i], szElm, numElms, numChannels);
	}
}

// =======================================================================================

jvxErrorType jvx_fieldLevelGain(jvxHandle** buf, jvxSize numC, jvxSize buffersize, jvxDataFormat format,
	jvxData* avrgPtr, jvxData* maxPtr, jvxData* gainPtr, jvxSize* channelMap, 
	jvxData smooth, jvxData dataMin)
{
	jvxSize i, j;
	jvxData* ptrDbl = NULL;
	jvxData* ptrFlt = NULL;
	jvxInt32* ptrInt32 = NULL;
	jvxInt64* ptrInt64 = NULL;
	jvxInt16* ptrInt16 = NULL;
	jvxInt8* ptrInt8 = NULL;
	jvxData tmp;

	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		for (i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];

			ptrDbl = reinterpret_cast<jvxData*>(buf[i]);
			for (j = 0; j < buffersize; j++)
			{
				tmp = *ptrDbl;
				tmp *= gainV;
				if (tmp > 0)
				{
					if (tmp < dataMin)
					{
						tmp = dataMin;
					}
				}
				else
				{
					if (tmp > -dataMin)
					{
						tmp = -dataMin;
					}
				}

				*ptrDbl++ = tmp;

				tmp *= tmp;
				if (tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth)+(1 - smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
		}
		break;

	case JVX_DATAFORMAT_8BIT:
		for (i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];

			ptrInt8 = reinterpret_cast<jvxInt8*>(buf[i]);
			for (j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT8_2_DATA(*ptrInt8);
				tmp *= gainV;
				*ptrInt8++ = JVX_DATA_2_INT8(tmp);

				tmp *= tmp;
				if (tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth)+(1 - smooth) * tmp;
				avrgV = tmp;

			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
		}
		break;

	case JVX_DATAFORMAT_16BIT_LE:
		for (i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];

			ptrInt16 = reinterpret_cast<jvxInt16*>(buf[i]);
			for (j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT16_2_DATA(*ptrInt16);
				tmp *= gainV;
				*ptrInt16++ = JVX_DATA_2_INT16(tmp);

				tmp *= tmp;
				if (tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth)+(1 - smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
		}
		break;

	case JVX_DATAFORMAT_32BIT_LE:
		for (i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];

			ptrInt32 = reinterpret_cast<jvxInt32*>(buf[i]);
			for (j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT32_2_DATA(*ptrInt32);
				tmp *= gainV;
				*ptrInt32++ = JVX_DATA_2_INT32(tmp);

				tmp *= tmp;
				if (tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth)+(1 - smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
		}
		break;

	case JVX_DATAFORMAT_64BIT_LE:
		for (i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];

			ptrInt64 = reinterpret_cast<jvxInt64*>(buf[i]);
			for (j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT64_2_DATA(*ptrInt16);
				tmp *= gainV;
				*ptrInt64++ = JVX_DATA_2_INT64(tmp);

				tmp *= tmp;
				if (tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth)+(1 - smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
		}
		break;
	}
	return(JVX_NO_ERROR);
}

// =======================================================================================

/*
 * Hint on level computation: The average and the max level are derived from x^2 !!
 */
 jvxErrorType jvx_fieldLevelGainClip(jvxHandle** buf, jvxSize numC, jvxSize buffersize, jvxDataFormat format,
								jvxData* avrgPtr, jvxData* maxPtr, jvxData* gainPtr, jvxCBool* clipPtr, jvxSize* channelMap, jvxData clipVal,
								jvxData smooth, jvxData dataMin)
{
	jvxSize i,j;
	jvxData* ptrDbl = NULL;
	jvxData* ptrFlt = NULL;
	jvxInt32* ptrInt32 = NULL;
	jvxInt64* ptrInt64 = NULL;
	jvxInt16* ptrInt16 = NULL;
	jvxInt8* ptrInt8 = NULL;
	jvxData tmp;

	switch(format)
	{
	case JVX_DATAFORMAT_DATA:
		for(i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];
			jvxBool clipV = clipPtr[mapi];

			ptrDbl = reinterpret_cast<jvxData*>(buf[i]);
			for(j = 0; j < buffersize; j++)
			{
				tmp = *ptrDbl;
				tmp *= gainV;
				if (tmp > 0)
				{
					if (tmp > clipVal)
					{
						clipV = true;
						tmp = clipVal;
					}
					if (tmp < dataMin)
					{
						tmp = dataMin;
					}
				}
				else
				{
					if (tmp < -clipVal)
					{
						clipV = true;
						tmp = clipVal;
					}
					if (tmp > -dataMin)
					{
						tmp = -dataMin;
					}
				}
				
				*ptrDbl++ = tmp;

				tmp *= tmp;
				if(tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth) + (1-smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi]= avrgV;
			clipPtr[mapi] = clipV;

		}
		break;

	case JVX_DATAFORMAT_8BIT:
		for(i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];
			jvxBool clipV = clipPtr[mapi];

			ptrInt8 = reinterpret_cast<jvxInt8*>(buf[i]);
			for(j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT8_2_DATA(*ptrInt8);
				tmp *= gainV;
				if(tmp > clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				else if(tmp < -clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				*ptrInt8++ = JVX_DATA_2_INT8(tmp);

				tmp *= tmp;
				if(tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth) + (1-smooth) * tmp;
				avrgV = tmp;
				
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
			clipPtr[mapi] = clipV;
		}
		break;

	case JVX_DATAFORMAT_16BIT_LE:
		for(i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];
			jvxBool clipV = clipPtr[mapi];

			ptrInt16 = reinterpret_cast<jvxInt16*>(buf[i]);
			for(j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT16_2_DATA(*ptrInt16);
				tmp *= gainV;
				if(tmp > clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				else if(tmp < -clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				*ptrInt16++ = JVX_DATA_2_INT16(tmp);


				tmp *= tmp;
				if(tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth) + (1-smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
			clipPtr[mapi] = clipV;
		}
		break;

	case JVX_DATAFORMAT_32BIT_LE:
		for(i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];
			jvxBool clipV = clipPtr[mapi];

			ptrInt32 = reinterpret_cast<jvxInt32*>(buf[i]);
			for(j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT32_2_DATA(*ptrInt32);
				tmp *= gainV;
				if(tmp > clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				else if(tmp < -clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				*ptrInt32++ = JVX_DATA_2_INT32(tmp);

				tmp *= tmp;
				if(tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth) + (1-smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
			clipPtr[mapi] = clipV;
		}
		break;

	case JVX_DATAFORMAT_64BIT_LE:
		for(i = 0; i < numC; i++)
		{
			jvxSize mapi = channelMap[i];
			jvxData gainV = gainPtr[mapi];
			jvxData maxV = maxPtr[mapi];
			jvxData avrgV = avrgPtr[mapi];
			jvxBool clipV = clipPtr[mapi];

			ptrInt64 = reinterpret_cast<jvxInt64*>(buf[i]);
			for(j = 0; j < buffersize; j++)
			{
				tmp = JVX_INT64_2_DATA(*ptrInt64);
				tmp *= gainV;
				if(tmp > clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				else if(tmp < -clipVal)
				{
					clipV = true;
					tmp = clipVal;
				}
				*ptrInt64++ = JVX_DATA_2_INT64(tmp);

				tmp *= tmp;
				if(tmp > maxV)
				{
					maxV = tmp;
				}
				tmp = avrgV * (smooth) + (1-smooth) * tmp;
				avrgV = tmp;
			}
			maxPtr[mapi] = maxV;
			avrgPtr[mapi] = avrgV;
			clipPtr[mapi] = clipV;
		}
		break;
	}
	return(JVX_NO_ERROR);
}

// =======================================================================================

 jvxErrorType jvx_fieldMix(jvxHandle* buf_addthis, jvxHandle* buf_tothis, jvxSize buffersize, jvxDataFormat format)
{
	jvxSize j;
	jvxData* ptrDbl1 = NULL, * ptrDbl2 = NULL;
	jvxData* ptrFlt1 = NULL, * ptrFlt2 = NULL;
	jvxInt32* ptrInt321 = NULL, * ptrInt322 = NULL;
	jvxInt64* ptrInt641 = NULL, * ptrInt642 = NULL;
	jvxInt16* ptrInt161 = NULL, * ptrInt162 = NULL;
	jvxInt8* ptrInt81 = NULL, * ptrInt82 = NULL;
	jvxData tmp1, tmp2;

	switch(format)
	{
	case JVX_DATAFORMAT_DATA:
		ptrDbl1 = reinterpret_cast<jvxData*>(buf_addthis);
		ptrDbl2 = reinterpret_cast<jvxData*>(buf_tothis);
		for(j = 0; j < buffersize; j++)
		{
			tmp1 = *ptrDbl1++;
			tmp2 = *ptrDbl2;
			tmp2 += tmp1;
			*ptrDbl2++ = tmp2;				
		}
		break;


	case JVX_DATAFORMAT_8BIT:
		ptrInt81 = reinterpret_cast<jvxInt8*>(buf_addthis);
		ptrInt82 = reinterpret_cast<jvxInt8*>(buf_tothis);
		for(j = 0; j < buffersize; j++)
		{
			tmp1 = JVX_INT8_2_DATA(*ptrInt81);
			tmp2 = JVX_INT8_2_DATA(*ptrInt82);
			tmp2 += tmp1;
			*ptrInt82 = JVX_DATA_2_INT8(tmp2);				
			ptrInt81++;
			ptrInt82++;
		}

		break;

	case JVX_DATAFORMAT_16BIT_LE:
		ptrInt161 = reinterpret_cast<jvxInt16*>(buf_addthis);
		ptrInt162 = reinterpret_cast<jvxInt16*>(buf_tothis);
		for(j = 0; j < buffersize; j++)
		{
			tmp1 = JVX_INT16_2_DATA(*ptrInt161);
			tmp2 = JVX_INT16_2_DATA(*ptrInt162);
			tmp2 += tmp1;
			*ptrInt162 = JVX_DATA_2_INT16(tmp2);				
			ptrInt161++;
			ptrInt162++;
		}

		break;

	case JVX_DATAFORMAT_32BIT_LE:
		ptrInt321 = reinterpret_cast<jvxInt32*>(buf_addthis);
		ptrInt322 = reinterpret_cast<jvxInt32*>(buf_tothis);
		for(j = 0; j < buffersize; j++)
		{
			tmp1 = JVX_INT32_2_DATA(*ptrInt321);
			tmp2 = JVX_INT32_2_DATA(*ptrInt322);
			tmp2 += tmp1;
			*ptrInt322 = JVX_DATA_2_INT32(tmp2);				
			ptrInt321++;
			ptrInt322++;
		}

		break;

	case JVX_DATAFORMAT_64BIT_LE:
		ptrInt641 = reinterpret_cast<jvxInt64*>(buf_addthis);
		ptrInt642 = reinterpret_cast<jvxInt64*>(buf_tothis);
		for(j = 0; j < buffersize; j++)
		{
			tmp1 = JVX_INT64_2_DATA(*ptrInt641);
			tmp2 = JVX_INT64_2_DATA(*ptrInt642);
			tmp2 += tmp1;
			*ptrInt642 = JVX_DATA_2_INT64(tmp2);				
			ptrInt641++;
			ptrInt642++;
		}

		break;
	}
	return(JVX_NO_ERROR);
}

 jvxDataProcessorHintDescriptor* 
jvx_hintDesriptor_push_front(jvxHandle* hint, jvxDataProcessorHintDescriptorType hintDescriptorId, jvxComponentIdentification assocHint, 
	jvxDataProcessorHintDescriptor* next, jvxSize unique_id )
{
	jvxDataProcessorHintDescriptor* newElm = new jvxDataProcessorHintDescriptor;
	newElm->hint = hint;
	newElm->hintDescriptorId = hintDescriptorId;
	newElm->uniqueId = unique_id;
	newElm->assocHint = assocHint;
	newElm->next = next;
	return(newElm);
}

jvxDataProcessorHintDescriptor*
jvx_hintDesriptor_push_front(jvxDataProcessorHintDescriptor* next_attach, jvxDataProcessorHintDescriptor* attach_to_me)
{
	 jvxDataProcessorHintDescriptor* nextElm = attach_to_me;
	 if (!nextElm)
	 {
		 return next_attach;
	 }
	 if (nextElm == attach_to_me)
	 {
		 // The same object, do not attach anything
		 return(attach_to_me);
	 }
	 while (nextElm->next)
	 {
		 nextElm = nextElm->next;
	 }
	 nextElm->next = next_attach;
	 return(attach_to_me);
}

jvxDataProcessorHintDescriptor*
jvx_hintDesriptor_pop_front(jvxDataProcessorHintDescriptor* next_detach, jvxDataProcessorHintDescriptor* detach_from_me)
{
	jvxDataProcessorHintDescriptor* nextElm = detach_from_me;

	if (!nextElm)
	{
		return NULL;
	}

	if (next_detach == detach_from_me)
	{
		return NULL;
	}

	// there is at least one element which was added at the front position on prepare
	while (nextElm)
	{
		if (nextElm->next == next_detach)
		{
			break;
		}
		nextElm = nextElm->next;		
	}

	// Detach the attached tree
	nextElm->next = NULL;
	return(detach_from_me);
}

 jvxDataProcessorHintDescriptor* 
jvx_hintDesriptor_pop_front(jvxDataProcessorHintDescriptor* popElm, jvxHandle** hint, jvxDataProcessorHintDescriptorType* hintDescriptorId, 
	jvxComponentIdentification* assocHint, jvxSize* uniqueId)
{
	jvxDataProcessorHintDescriptor* returnMe = NULL;
	if (popElm)
	{
		if (hint)
		{
			*hint = popElm->hint;
		}

		if (hintDescriptorId)
		{
			*hintDescriptorId = popElm->hintDescriptorId;
		}
		if (assocHint)
		{
			*assocHint = popElm->assocHint;
		}
		if (uniqueId)
		{
			*uniqueId = popElm->uniqueId;
		}
		returnMe = popElm->next;
		delete(popElm);
	}
	return(returnMe);
}

 jvxErrorType 
jvx_hintDesriptor_find(jvxDataProcessorHintDescriptor* entryList, 
	jvxHandle** hint, jvxDataProcessorHintDescriptorType hintDescriptorId,
	jvxComponentIdentification assocHint, jvxSize id_return)
{
	 jvxSize cnt = 0;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	while(entryList)
	{
		if(
			(entryList->hintDescriptorId == hintDescriptorId) &&
			jvx_compare_match_component_identification(entryList->assocHint, assocHint))
		{
			if (cnt == id_return)
			{
				if (hint)
				{
					*hint = entryList->hint;
					res = JVX_NO_ERROR;
					break;
				}
			}
			cnt++;
		}		
		entryList = entryList->next;
	}
	return(res);
}

