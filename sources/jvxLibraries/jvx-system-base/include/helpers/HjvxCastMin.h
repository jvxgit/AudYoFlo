#ifndef __HJVXCASTMIN_H__
#define __HJVXCASTMIN_H__

template <typename T> jvxDataFormat template_get_type_enum()
{
	jvxDataFormat form = JVX_DATAFORMAT_NONE;

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxData>::value)
#else
	if (std::is_same<T, jvxData>::value)
#endif
	{
		form = JVX_DATAFORMAT_DATA;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt64>::value)
#else
	if (std::is_same<T, jvxUInt64>::value)
#endif
	{
		form = JVX_DATAFORMAT_U64BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt32>::value)
#else
	if (std::is_same<T, jvxUInt32>::value)
#endif
	{
		form = JVX_DATAFORMAT_U32BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt16>::value)
#else
	if (std::is_same<T, jvxUInt16>::value)
#endif
	{
		form = JVX_DATAFORMAT_U16BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt8>::value)
#else
	if (std::is_same<T, jvxUInt8>::value)
#endif
	{
		form = JVX_DATAFORMAT_U8BIT;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt64>::value)
#else
	if (std::is_same<T, jvxInt64>::value)
#endif
	{
		form = JVX_DATAFORMAT_64BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt32>::value)
#else
	if (std::is_same<T, jvxInt32>::value)
#endif
	{
		form = JVX_DATAFORMAT_32BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt16>::value)
#else
	if (std::is_same<T, jvxInt16>::value)
#endif
	{
		form = JVX_DATAFORMAT_16BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt8>::value)
#else
	if (std::is_same<T, jvxInt8>::value)
#endif
	{
		form = JVX_DATAFORMAT_8BIT;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxByte>::value)
#else
	if (std::is_same<T, jvxByte>::value)
#endif
	{
		form = JVX_DATAFORMAT_BYTE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxSize>::value)
#else
	if (std::is_same<T, jvxSize>::value)
#endif
	{
		form = JVX_DATAFORMAT_SIZE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxApiString>::value)
#else
	if (std::is_same<T, jvxApiString>::value)
#endif
	{
		form = JVX_DATAFORMAT_STRING;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxSelectionList>::value)
#else
	if (std::is_same<T, jvxSelectionList>::value)
#endif
	{
		form = JVX_DATAFORMAT_SELECTION_LIST;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxApiStringList>::value)
#else
	if (std::is_same<T, jvxApiStringList>::value)
#endif
	{
		form = JVX_DATAFORMAT_STRING_LIST;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxValueInRange>::value)
#else
	if (std::is_same<T, jvxValueInRange>::value)
#endif
	{
		form = JVX_DATAFORMAT_VALUE_IN_RANGE;
	}

	return form;
};

jvxSize JVX_STATIC_INLINE jvx_process_icon_extract_stage(
	jvxLinkDataDescriptor* theData,
	jvxSize idx_stage)
{
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *theData->con_pipeline.idx_stage_ptr;
	}
	return idx_stage_local;
}

// Some template helper functions
template <class T> T** jvx_process_icon_extract_input_buffers(
	jvxLinkDataDescriptor* theData,
	jvxSize idx_stage)
{
	jvxDataFormat form = template_get_type_enum<T>();
	
	jvxSize idx_stage_local = jvx_process_icon_extract_stage(theData, idx_stage);
	/*
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *theData->con_pipeline.idx_stage_ptr;
	}
	*/
	// Check type if it is not a jvxHandle type
	if (!std::is_same<T, jvxHandle>::value)
	{
		assert(theData->con_params.format == form);
	}
	T** bufsIn = (T**)theData->con_data.buffers[idx_stage_local];
	return bufsIn;
};

template <class T> T** jvx_process_icon_extract_output_buffers(
	jvxLinkDataDescriptor& theData)
{
	jvxDataFormat form = template_get_type_enum<T>();

	jvxSize idx_stage_local = *theData.con_pipeline.idx_stage_ptr;
	T** bufsOut = (T**)theData.con_data.buffers[idx_stage_local];
	if (!std::is_same<T, jvxHandle>::value)
	{
		assert(theData.con_params.format == form);
	}
	return bufsOut;
}

// Function lval = accept-no-return or "convert rvalue to lvalue"
// See this unmove operator as taken from here:
// https://stackoverflow.com/questions/44677825/rvalue-to-lvalue-conversion
// There is an important hint:
// "So it is safe to use unmove() within a single full expression, but after 
// the expression has been fully evaluated, the temporaries go away."
//
template<class T> T& lval(T&& t)
{ 
	return t; 
}
/*
template<class T> T accept_noreturn()
{
	T t;
	return t;
}
*/
#endif
