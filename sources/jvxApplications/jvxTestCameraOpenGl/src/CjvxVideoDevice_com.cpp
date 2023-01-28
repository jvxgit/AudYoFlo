#include "CjvxVideoDevice.h"

#include <shlwapi.h> // QITAB and friends
#include <objbase.h> // IID_PPV_ARGS and friends
//#include <dshow.h> // IAMVideoProcAmp and friends

// ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##

// IUnknown methods
STDMETHODIMP 
CjvxVideoDevice::QueryInterface(REFIID aRiid, void** aPpv)
{
	static const QITAB qit[] =
	{
		QITABENT(CjvxVideoDevice, IMFSourceReaderCallback),
		{ 0 },
	};
	return QISearch(this, qit, aRiid, aPpv);
}

STDMETHODIMP_(ULONG) 
CjvxVideoDevice::AddRef()
{
	return InterlockedIncrement(&mRefCount);
}

STDMETHODIMP_(ULONG) 
CjvxVideoDevice::Release()
{
	ULONG count = InterlockedDecrement(&mRefCount);
	if (count == 0)
	{
		delete this;
	}
	// For thread safety, return a temporary variable.
	return count;
}

// IMFSourceReaderCallback methods
STDMETHODIMP 
CjvxVideoDevice::OnReadSample(
	HRESULT aStatus,
	DWORD aStreamIndex,
	DWORD aStreamFlags,
	LONGLONG aTimestamp,
	IMFSample *aSample
)
{
	HRESULT hr = S_OK;
	IMFMediaBuffer *mediabuffer = NULL;

	if (FAILED(aStatus))
	{
		// Bug workaround: some resolutions may just return error.
		// http://stackoverflow.com/questions/22788043/imfsourcereader-giving-error-0x80070491-for-some-resolutions
		// we fix by marking the resolution bad and retrying, which should use the next best match.
		std::cout << "Buffer switch callback failed!" << std::endl;
		/*
		mRedoFromStart = 1;
		if (mBadIndices == mMaxBadIndices)
		{
			unsigned int *t = new unsigned int[mMaxBadIndices * 2];
			memcpy(t, mBadIndex, mMaxBadIndices * sizeof(unsigned int));
			delete[] mBadIndex;
			mBadIndex = t;
			mMaxBadIndices *= 2;
		}
		mBadIndex[mBadIndices] = mUsedIndex;
		mBadIndices++;*/
		return aStatus;
	}
	else
	{
		//EnterCriticalSection(&mCritsec);
		if (aSample)
		{
			jvxTick startt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);

			if (runtime.lastTime > 0)
			{
				runtime.avrgT = runtime.avrgT * (ALPHA)+(1 - ALPHA) * (startt - runtime.lastTime);
			}
			runtime.lastTime = startt;

			IMF2DBuffer     *m2DBuffer;
			// Get the video frame buffer from the sample.

			hr = aSample->GetBufferByIndex(0, &mediabuffer);
			
			// Access buffer

			// Query for the 2-D buffer interface. 
			mediabuffer->QueryInterface(IID_PPV_ARGS(&m2DBuffer));
			if (m2DBuffer)
			{
				runtime.numFrames++;
				JVX_LOCK_MUTEX(safeAccess);
				jvxSize bufIdx = (runtime.bufIdx + runtime.bufFHeight) % runtime.numBufs;
				jvxState stat = runtime.fldBufStat[bufIdx];
				JVX_UNLOCK_MUTEX(safeAccess);

				if (stat == JVX_STATE_INIT)
				{
					jvxSize i;
					BYTE* src = NULL;
					LONG stride = 0;
					jvxByte* dest = runtime.fldRaw[bufIdx];
					m2DBuffer->Lock2D(&src, &stride);

					// If source is browsed from toe to the head, we do the same for the
					// texture target buffer
					if (stride < 0)
					{
						dest += (runtime.height - 1)* runtime.szLine;
					}

					// This copies the lines in target buffer: from 
					// - .. higher lines to lower lines in target buffer
					// - .. from end to beginning in src buffer
					for (i = 0; i < runtime.height; i++)
					{
						memcpy(dest, src, runtime.szLine);
						src += stride;
						dest -= runtime.szLine;
					}
					m2DBuffer->Unlock2D();
				}
				else
				{
					runtime.inputOverrun++;
				}

				JVX_LOCK_MUTEX(safeAccess);
				runtime.fldBufStat[bufIdx] = JVX_STATE_ACTIVE;
				runtime.bufFHeight++;
				JVX_UNLOCK_MUTEX(safeAccess);
			}
			else
			{
				assert(0);
			}

			m2DBuffer->Release();

			mediabuffer->Unlock();
			mediabuffer->Release();
		}
	}

	if (runtime.status = JVX_STATE_PROCESSING)
	{
		// Request the next frame.
		hr = readPtr->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			NULL,   // actual
			NULL,   // flags
			NULL,   // timestamp
			NULL    // sample
		);
	}
	else
	{
		if (runtime.status = JVX_STATE_COMPLETE)
		{
			// We need to stop here
			runtime.status = JVX_STATE_PREPARED;
		}
	}

	return hr;
}

STDMETHODIMP 
CjvxVideoDevice::OnEvent(DWORD, IMFMediaEvent *)
{
	return S_OK;
}

STDMETHODIMP 
CjvxVideoDevice::OnFlush(DWORD)
{
	return S_OK;
}