#ifndef _HJVXCIRCBUFFERPASS_H__
#define _HJVXCIRCBUFFERPASS_H__

template  <typename T>
class HjvxCircbufferPass
{
public:

	//! Different states of operation
	typedef enum
	{
		JVX_CIRCBUFFERPASS_INACTIVE,
		JVX_CIRCBUFFERPASS_NORMAL,
		JVX_CIRCBUFFERPASS_OVERFLOW
	} bufferOperationType;

private:
	//! Operation status for hysteresis
	bufferOperationType status;

	//! Counter in case buffer is in overflow and shall be restarted
	jvxSize restart_cnt_space;

	//! Reference to external buffer reference
	jvxExternalBuffer* exBuf;

	//! Protects against removing buffer while operating. This mutex is only local for reader OR writer 
	JVX_MUTEX_HANDLE lock;

public:

	HjvxCircbufferPass()
	{
		JVX_INITIALIZE_MUTEX(lock);
		stop();		
	};

	~HjvxCircbufferPass()
	{
		JVX_TERMINATE_MUTEX(lock);
	}

	// ========================================================================

	jvxErrorType start(jvxExternalBuffer* exBufPtr, jvxSize restart_space = JVX_SIZE_UNSELECTED)
	{
		JVX_LOCK_MUTEX(lock);
		assert(exBufPtr);
		status = JVX_CIRCBUFFERPASS_NORMAL;
		restart_cnt_space = restart_space;
		if (JVX_CHECK_SIZE_UNSELECTED(restart_cnt_space))
		{
			restart_cnt_space = exBufPtr->length;
		}
		exBuf = exBufPtr;
		JVX_UNLOCK_MUTEX(lock);
		return JVX_NO_ERROR;
	};
		
	jvxErrorType stop()
	{
		JVX_LOCK_MUTEX(lock);
		exBuf = NULL;
		restart_cnt_space = JVX_SIZE_UNSELECTED;
		status = JVX_CIRCBUFFERPASS_INACTIVE;
		exBuf = NULL;
		JVX_UNLOCK_MUTEX(lock);
		return JVX_NO_ERROR;
	};

	jvxErrorType reset_status()
	{
		status = JVX_CIRCBUFFERPASS_NORMAL;
		return JVX_NO_ERROR;
	};

	// ========================================================================
	
	jvxSize pass_to_buffer(T** fldIn, jvxSize numValue, jvxSize numChannels)
	{
		jvxSize i;
		jvxSize fHeight = 0;
		jvxSize idxRead = 0;
		jvxSize toCopy = numValue;
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE resLock = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
		JVX_TRY_LOCK_MUTEX(resLock, lock);
		if (resLock == JVX_TRY_LOCK_MUTEX_SUCCESS)
		{
			exBuf->safe_access.lockf(exBuf->safe_access.priv);
			idxRead = exBuf->idx_read;
			fHeight = exBuf->fill_height;
			// std::cout << "-IN->" << idxRead << "--" << fHeight << std::endl;
			exBuf->safe_access.unlockf(exBuf->safe_access.priv);

			jvxSize space = exBuf->length - fHeight;
			jvxSize idxWrite = (idxRead + fHeight) % exBuf->length;
			jvxSize offset_read = 0;
			if (status == JVX_CIRCBUFFERPASS_OVERFLOW)
			{
				if (space >= restart_cnt_space)
				{
					status = JVX_CIRCBUFFERPASS_NORMAL;
				}				
			}

			if (status == JVX_CIRCBUFFERPASS_NORMAL)
			{
				while (toCopy)
				{
					jvxSize ll1 = JVX_MIN(toCopy, space);
					ll1 = JVX_MIN(ll1, exBuf->length - idxWrite);

					// Copy data to buffer
					T* buf = (T*)exBuf->ptrFld;
					for (i = 0; i < numChannels; i++)
					{
						T* posi_write = buf + idxWrite;
						T* posi_read = fldIn[i];
						posi_read += offset_read;
						memcpy(posi_write, posi_read, sizeof(T) * ll1);
						buf += exBuf->numElmFldOneChanOneBuf;
					}

					// Update circbuffer struct
					exBuf->safe_access.lockf(exBuf->safe_access.priv);
					exBuf->fill_height += ll1;
					idxRead = exBuf->idx_read;
					fHeight = exBuf->fill_height;
					exBuf->safe_access.unlockf(exBuf->safe_access.priv);

					// Update all local counters
					space = exBuf->length - fHeight;
					idxWrite = (idxRead + fHeight) % exBuf->length;
					toCopy -= ll1;
					offset_read += ll1;
					if (space == 0)
					{
						break;
					}
				}
				if (toCopy)
				{
					status = JVX_CIRCBUFFERPASS_OVERFLOW;
				}
			}
			JVX_UNLOCK_MUTEX(lock);
		}
		return toCopy;
	};
};

#endif
