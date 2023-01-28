#ifndef __HJVXCIRCBUFFERREAD_H__
#define __HJVXCIRCBUFFERREAD_H__

template <typename T> JVX_INTERFACE HjvxCircbufferRead_report
{
public:
	virtual ~HjvxCircbufferRead_report() {};
	virtual void JVX_CALLINGCONVENTION report_data(T** ptr, jvxSize numChannels, jvxSize numValues, jvxSize uId) = 0;
};

template <typename T> class HjvxCircbufferRead
{
public:
	HjvxCircbufferRead_report<T>* report;
	jvxExternalBuffer* 	theCircBufferFld;
	jvxBool theCircBufferValid;
	jvxSize num_channels;
	T** bufs;
	jvxSize uId;
	jvxDataFormat form;

	HjvxCircbufferRead()
	{
		report = NULL;
		theCircBufferFld = NULL;
		theCircBufferValid = false;
		bufs = NULL;
		uId = JVX_SIZE_UNSELECTED;
		form = template_get_type_enum<T>();
		num_channels = 0;
		
	};

	jvxErrorType start(		
		HjvxCircbufferRead_report<T>* reportArg,
		jvxSize length, jvxSize chans, 
		jvxSize uId = JVX_SIZE_UNSELECTED,
		jvxDataFormat formArg = JVX_DATAFORMAT_DATA, 
		jvxDataFormatGroup sform = JVX_DATAFORMAT_GROUP_GENERIC)
	{
		jvxSize sz = 0;
		if (theCircBufferFld == NULL)
		{
			form = formArg;
			theCircBufferFld = jvx_allocate1DCircExternalBuffer(
				length,
				chans,
				form,
				sform,
				jvx_static_lock,
				jvx_static_try_lock,
				jvx_static_unlock, &sz);
			theCircBufferValid = true;
			report = reportArg;
			num_channels = chans;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufs, T*, num_channels);
			
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	};

	jvxErrorType stop()
	{
		if (theCircBufferFld)
		{
			JVX_DSP_SAFE_DELETE_FIELD(bufs);
			jvx_deallocateExternalBuffer(theCircBufferFld);
			theCircBufferFld = NULL;
			theCircBufferValid = false;
			bufs = NULL;
			num_channels = 0;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	};

	void trigger_new_data()
	{
		jvxSize i;
		jvxSize idxRead = 0;
		jvxSize fHeight = 0;

		while (1)
		{
			theCircBufferFld->safe_access.lockf(theCircBufferFld->safe_access.priv);
			idxRead = theCircBufferFld->idx_read;
			fHeight = theCircBufferFld->fill_height;
			// std::cout << "-OUT->" << idxRead << "--" << fHeight << std::endl;
			theCircBufferFld->safe_access.unlockf(theCircBufferFld->safe_access.priv);

			if (fHeight == 0)
				break;

			T* buf = (T*)theCircBufferFld->ptrFld;
			jvxSize ll1 = JVX_MIN(fHeight, theCircBufferFld->length - idxRead);

			for (i = 0; i < num_channels; i++)
			{
				bufs[i] = buf + idxRead;
				buf += theCircBufferFld->numElmFldOneChanOneBuf;
			}

			if (report)
			{
				report->report_data(bufs, num_channels, ll1, uId);
			}

			// Update circbuffer struct
			theCircBufferFld->safe_access.lockf(theCircBufferFld->safe_access.priv);
			theCircBufferFld->fill_height -= ll1;
			theCircBufferFld->idx_read = (theCircBufferFld->idx_read + ll1) % theCircBufferFld->length;
			// std::cout << "-OUT->" << idxRead << "--" << fHeight << std::endl;
			theCircBufferFld->safe_access.unlockf(theCircBufferFld->safe_access.priv);
		}
	}
};

#endif
