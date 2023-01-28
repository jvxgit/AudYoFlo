#ifndef __HJVXDATAPROCESSOR_H__
#define __HJVXDATAPROCESSOR_H__

template<typename Tto> void jvx_convertSamples_from_data_to_float(jvxData* source, Tto* dest, jvxSize numSamples,
												   jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Tto accu; 	
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accu = (Tto) *source;
		source += hopFrom; 
		*dest = accu;
		dest += hopTo;
	}
}

template<typename Tfrom>  void jvx_convertSamples_from_float_to_data(Tfrom* source, jvxData* dest, jvxSize numSamples,
												   jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	jvxData accu; 	
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accu = (jvxData) *source;
		source += hopFrom; 
		*dest = accu;
		dest += hopTo;
	}
}

template<typename T>  void jvx_convertSamples_from_to(T* source, T* dest, jvxSize numSamples,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	T accu;
	source += offsetFrom;
	dest += offsetTo;
	for (i = 0; i < numSamples; i++)
	{
		accu = *source;
		source += hopFrom;
		*dest = accu;
		dest += hopTo;
	}
}

// =======================================================================================

template<typename Tfrom, typename Tto>  void jvx_convertSamples_from_fxp_norm_to_flp(Tfrom* source, Tto* dest, jvxSize numSamples, Tto norm, 
										jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Tto accu; 	
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accu = (Tto) *source;
		source += hopFrom; 
		accu *= norm;
		*dest = accu;
		dest += hopTo;
	}
}

// =======================================================================================
/* Hint: If we really see 24 bit values, we need to shift to the left bound of the accu to allow negative values!! */
template<typename Taccu, typename Tto>  void jvx_convertSamples_from_bytes_shift_norm_to_flp(char* source, Tto* dest, jvxSize numSamples, jvxSize shiftB4Norm, Tto norm, jvxSize numBytes,
												jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Tto accuTo; 	
	Taccu accuFrom; 
	source += (offsetFrom*numBytes);
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = 0;
		memcpy(&accuFrom, source, numBytes);
		accuFrom = accuFrom << shiftB4Norm;
		accuTo = (Tto)accuFrom ;
		accuTo *= norm;
		*dest = accuTo;
		source += numBytes * hopFrom;
		dest += hopTo;
	}
}

// =============================================================================================================

template<typename Tfrom, typename Tto>  void jvx_convertSamples_from_flp_norm_to_fxp(Tfrom* source, Tto* dest, jvxSize numSamples, Tfrom norm,
													jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Tfrom accu; 	
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accu = *source;
		accu *= norm;
		*dest = (Tto)accu;
		source += hopFrom;
		dest += hopTo;
	}
}

// =======================================================================================

template<typename Tfrom, typename Taccu>  void jvx_convertSamples_from_flp_norm_to_bytes(Tfrom* source, char* dest, jvxSize numSamples, Tfrom norm, jvxSize numBytes,
											jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Tfrom accuFrom; 	
	Taccu accuTo;
	source += offsetFrom;
	dest += (numBytes * offsetTo);
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = *source * norm;
		accuTo = (Taccu)accuFrom;
		memcpy(dest, &accuTo, numBytes);
		source += hopFrom;
		dest += (numBytes * hopTo);
	}
}

// =============================================================================================================

template<typename Tto, typename Taccu>  void jvx_convertSamples_from_bytes_shiftleft_to_fxp(
	char* source, Tto* dest, jvxSize numSamples, jvxSize numBytes, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Taccu accuFrom;
	source += numBytes * offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = 0;
		memcpy(&accuFrom, source, numBytes);
		accuFrom = accuFrom << numShift;
		*dest = (Tto)accuFrom;
		source += numBytes * hopFrom;
		dest += hopTo;
	}
}

template<typename Tto, typename Taccu>  void jvx_convertSamples_from_bytes_shiftright_to_fxp(
	char* source, Tto* dest, jvxSize numSamples, jvxSize numBytes, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Taccu accuFrom; 
	source += numBytes * offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = 0;
		memcpy(&accuFrom, source, numBytes);
		accuFrom = accuFrom >> numShift;
		*dest = (Tto)accuFrom;
		source += numBytes * hopFrom;
		dest += hopTo;
	}
}

// =======================================================================================

template<typename Tfrom, typename Tto, typename Taccu>  void jvx_convertSamples_from_fxp_noshift_to_fxp(
	Tfrom* source, Tto* dest, jvxSize numSamples, 
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;	
	Taccu accuFrom;
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = (Taccu) *source;
		//accuFrom = accuFrom;
		*dest = (Tto)accuFrom;
		source += hopFrom;
		dest += hopTo;
	}
}
// =======================================================================================

template<typename Tfrom, typename Tto, typename Taccu>  void jvx_convertSamples_from_fxp_shiftleft_to_fxp(
	Tfrom* source, Tto* dest, jvxSize numSamples, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;	
	Taccu accuFrom;
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = (Taccu) *source;
		accuFrom = accuFrom << numShift;
		*dest = (Tto)accuFrom;
		source += hopFrom;
		dest += hopTo;
	}
}

template<typename Tfrom, typename Tto, typename Taccu>  void jvx_convertSamples_from_fxp_shiftright_to_fxp(
	Tfrom* source, Tto* dest, jvxSize numSamples, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;	
	Taccu accuFrom;
	source += offsetFrom;
	dest += offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = (Taccu) *source;
		accuFrom = accuFrom >> numShift;
		*dest = (Tto)accuFrom;
		source += hopFrom;
		dest += hopTo;
	}
}

// =======================================================================================

template<typename Tfrom, typename Taccu>  void jvx_convertSamples_from_fxp_shiftleft_to_bytes(
	Tfrom* source, char* dest, jvxSize numSamples, jvxSize numBytes, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Taccu accuFrom;
	source += offsetFrom;
	dest += offsetTo * numBytes;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = (Taccu)*source;
		accuFrom = accuFrom << numShift;
		memcpy(dest, &accuFrom, numBytes);
		source += hopFrom;
		dest += numBytes * hopTo;
	}
}

template<typename Tfrom, typename Taccu>  void jvx_convertSamples_from_fxp_shiftright_to_bytes(
	Tfrom* source, char* dest, jvxSize numSamples, jvxSize numBytes, jvxSize numShift,
	jvxSize offsetFrom = 0, jvxSize hopFrom = 1, jvxSize offsetTo = 0, jvxSize hopTo = 1)
{
	jvxSize i;
	Taccu accuFrom;
	source += offsetFrom;
	dest += numBytes * offsetTo;
	for(i = 0; i < numSamples; i++)
	{
		accuFrom = (Taccu)*source;
		accuFrom = accuFrom >> numShift;
		memcpy(dest, &accuFrom, numBytes);
		source += hopFrom;
		dest += numBytes * hopTo;
	}
}

// =======================================================================================

void jvx_samples_clear(void* to, jvxSize szElm, jvxSize numElms);

void jvx_fields_clear(void** to, jvxSize szElm, jvxSize numElms, jvxSize numChannels);

void jvx_fields_lst_clear(void*** to, jvxSize szElm, jvxSize numElms, jvxSize numChannels, jvxSize numFlds);

void jvx_convertSamples_memcpy(void* from, void* to, jvxSize szElm, jvxSize numElms);

void jvx_mixSamples_flp(jvxData* ptrFrom, jvxData* ptrMixTo, jvxSize bsize);
void jvx_mixSamples_flp_gain(jvxData* ptrFrom, jvxData* ptrMixTo, jvxSize bsize, jvxData gain);

void jvx_convertSamples_memcpy_offset(void* from, void* to, jvxSize szElm, jvxSize numElms, jvxInt32 offsetInput, jvxInt32 offsetOutput);

jvxErrorType jvx_fieldLevelGain(jvxHandle** buf, jvxSize numC, jvxSize buffersize, jvxDataFormat format,
	jvxData* avrgPtr, jvxData* maxPtr, jvxData* gainPtr, jvxSize* channelMap,
	jvxData smooth, jvxData dataMin);

jvxErrorType jvx_fieldLevelGainClip(jvxHandle** buf, jvxSize numC, jvxSize buffersize, jvxDataFormat format,
	jvxData* avrgVal, jvxData* maxVal, jvxData* gainVal, jvxCBool* clipPtr,
	jvxSize* channelMap, jvxData clipVal,
	jvxData smooth, jvxData = JVX_DATA_MIN_UNDERFLOW_PREVENT);


// =======================================================================================

jvxErrorType jvx_fieldMix(jvxHandle* buf_addthis, jvxHandle* buf_tothis, jvxSize buffersize, jvxDataFormat format);

jvxDataProcessorHintDescriptor* jvx_hintDesriptor_push_front(jvxHandle* hint, 
	jvxDataProcessorHintDescriptorType hintDescriptorId, 
	jvxComponentIdentification assocHint, 
	jvxDataProcessorHintDescriptor* next, 
	jvxSize unId = JVX_SIZE_UNSELECTED);

jvxDataProcessorHintDescriptor*
jvx_hintDesriptor_push_front(jvxDataProcessorHintDescriptor* next_attach, jvxDataProcessorHintDescriptor* attach_to_me);

jvxDataProcessorHintDescriptor* 
jvx_hintDesriptor_pop_front(jvxDataProcessorHintDescriptor* popElm, 
	jvxHandle** hint, 
	jvxDataProcessorHintDescriptorType* hintDescriptorId, 
	jvxComponentIdentification* assocHint, 
	jvxSize* uniqueId = NULL);

jvxDataProcessorHintDescriptor*
jvx_hintDesriptor_pop_front(jvxDataProcessorHintDescriptor* next_detach, jvxDataProcessorHintDescriptor* detach_from_me);

jvxErrorType 
jvx_hintDesriptor_find(jvxDataProcessorHintDescriptor* entryList, jvxHandle** hint, 
	jvxDataProcessorHintDescriptorType hintDescriptorId, jvxComponentIdentification assocHint,
	jvxSize id_return = 0);

#endif
