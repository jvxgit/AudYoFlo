#include "CjvxAudioWindowsDevice.h"
#include "CjvxAudioWindowsTechnology.h"

// ====================================================================================
JVX_THREAD_ENTRY_FUNCTION(audioProcessThread, param)
{
    CjvxAudioWindowsDevice* sigProcClass = reinterpret_cast<CjvxAudioWindowsDevice*>(param);
    return sigProcClass->audioProcessThread_ic();
}

DWORD CjvxAudioWindowsDevice::audioProcessThread_ic()
{
    jvxSize i;
    bool stillPlaying = true;
    HANDLE waitArray[3] = { _ShutdownEvent, _StreamSwitchEvent, _AudioSamplesReadyEvent };
    HANDLE mmcssHandle = NULL;
    DWORD mmcssTaskIndex = 0;
    BYTE* pData = nullptr;
    UINT32 framesAvailable = 0;
    DWORD  flags = 0;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf("Unable to initialize COM in render thread: %x\n", hr);
        return hr;
    }

    //if (!DisableMMCSS)
    //{
    // This creates a task ident
    // https://matthewvaneerde.wordpress.com/2018/09/13/tracking-down-calls-to-avsetmmthreadcharacteristics/
    mmcssHandle = AvSetMmThreadCharacteristics("Audio", &mmcssTaskIndex);
    
    while (stillPlaying)
    {

        DWORD waitResult = WaitForMultipleObjects(3, waitArray, FALSE, INFINITE);
        switch (waitResult)
        {
        case WAIT_OBJECT_0 + 0:     // _ShutdownEvent
            stillPlaying = false;       // We're done, exit the loop.
            break;
        case WAIT_OBJECT_0 + 1:     // _StreamSwitchEvent
            //
            //  We need to stop the capturer, tear down the _AudioClient and _CaptureClient objects and re-create them on the new.
            //  endpoint if possible.  If this fails, abort the thread.
            //
            /*
            if (!HandleStreamSwitchEvent())
            {
                stillPlaying = false;
            }
            */
            break;
        case WAIT_OBJECT_0 + 2:     // _AudioSamplesReadyEvent
            //
            //  We need to retrieve the next buffer of samples from the audio capturer.
            //

            //
            //  Find out how much capture data is available.  We need to make sure we don't run over the length
            //  of our capture buffer.  We'll discard any samples that don't fit in the buffer.
            //
            if(isInput)
            {
                if (audioMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
                {
                    framesAvailable = 0;
                    hr = captureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
                    if (SUCCEEDED(hr))
                    {
                        // AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY
                        assert(framesAvailable == realBufferSize);
                        for (i = 0; i < num_proc_loops; i++)
                        {
                            core_buffer_process_render(&pData, procBufferSize);
                        }
                        /*
                        UINT32 framesToCopy = min(framesAvailable, static_cast<UINT32>((_CaptureBufferSize - _CurrentCaptureIndex) / _FrameSize));
                        if (framesToCopy != 0)
                        {
                         //
                         //  The flags on capture tell us information about the data.
                        //
                        //  We only really care about the silent flag since we want to put frames of silence into the buffer
                        //  when we receive silence.  We rely on the fact that a logical bit 0 is silence for both float and int formats.
                        //
                        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                        {
                            //
                            //  Fill 0s from the capture buffer to the output buffer.
                            //
                            ZeroMemory(&_CaptureBuffer[_CurrentCaptureIndex], framesToCopy * _FrameSize);
                        }
                        else
                        {
                            //
                            //  Copy data from the audio engine buffer to the output buffer.
                            //
                            CopyMemory(&_CaptureBuffer[_CurrentCaptureIndex], pData, framesToCopy * _FrameSize);
                        }
                        //
                        //  Bump the capture buffer pointer.
                        //
                        _CurrentCaptureIndex += framesToCopy * _FrameSize;
                        */
                        hr = captureClient->ReleaseBuffer(framesAvailable);
                    }
                    else
                    {
                    }
                }
                else
				{
					framesAvailable = 0;
					hr = captureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						jvxSize avail = shared_buf_length - shared_buf_fill;
						assert(avail >= framesAvailable);
						memcpy(shared_buf + (shared_buf_fill * numBytesSampleAllChans), 
                            pData, (numBytesSampleAllChans * framesAvailable));
						shared_buf_fill += framesAvailable;
						while (1)
						{
							if (shared_buf_fill >= procBufferSize)
							{
                                BYTE* pDataAlgo = (BYTE*)shared_buf;
								for (i = 0; i < num_proc_loops; i++)
								{
									core_buffer_process_render(&pDataAlgo, procBufferSize);
								}

								shared_buf_fill -= procBufferSize;
								memmove(shared_buf, shared_buf + (procBufferSize * numBytesSampleAllChans), (shared_buf_fill * numBytesSampleAllChans));
							}
							else
							{
								break;
							}
						}
                        hr = captureClient->ReleaseBuffer(framesAvailable);
					}
				}
            }
            else
            {
                pData = nullptr;
                if (audioMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
                {
                    hr = renderClient->GetBuffer(procBufferSize * num_proc_loops, &pData);
                    if (SUCCEEDED(hr))
                    {
                        for (i = 0; i < num_proc_loops; i++)
                        {
                            core_buffer_process_render(&pData, procBufferSize);
                        }
                        hr = renderClient->ReleaseBuffer(procBufferSize * num_proc_loops, 0);
                    }
					else
					{
					}
				}
                else
                {
                    while (1)
                    {
                        UINT32 numPad = 0;
                        hr = audioClient->GetCurrentPadding(&numPad);
                        if (SUCCEEDED(hr))
                        {
                            jvxSize ll = realBufferSize - numPad;
                            if (ll >= procBufferSize * num_proc_loops)
                            {
                                hr = renderClient->GetBuffer(procBufferSize * num_proc_loops, &pData);
                                if (SUCCEEDED(hr))
                                {
                                    for (i = 0; i < num_proc_loops; i++)
                                    {
                                        core_buffer_process_render(&pData, procBufferSize);
                                    }
                                    hr = renderClient->ReleaseBuffer(procBufferSize * num_proc_loops, 0);
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    if (mmcssHandle)
    {
        AvRevertMmThreadCharacteristics(mmcssHandle);
    }
    
    CoUninitialize();
    return 0;
}

jvxErrorType 
CjvxAudioWindowsDevice::core_buffer_process_render(BYTE** ptrFromTo, jvxSize numSamples)
{
    jvxSize i;
    
    // I am the master. I will start the chain here!
    if (_common_set_ocon.theData_out.con_link.connect_to)
    {
        _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
    }

    jvxSize idxToProc = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
    jvxSize idxFromProc = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;

    if(isInput)
    {
        jvxByte* ptrSrc = (jvxByte*)(*ptrFromTo);
        jvxSize chanCnt = 0;
        jvxData* ptrDestFlt = nullptr;
        jvxInt64* ptrDestI64 = nullptr;
        jvxInt32* ptrDestI32 = nullptr;
        jvxInt16* ptrDestI16 = nullptr;

        for (i = 0; i < waveFormatStrProcessing.Format.nChannels; i++)
		{
			if (jvx_bitTest(channelMaskInProc, i))
			{
				switch (_inproc.format)
				{
				case JVX_DATAFORMAT_DATA:
					ptrDestFlt = (jvxData*)_common_set_ocon.theData_out.con_data.buffers[idxToProc][chanCnt];
					switch (waveFormatStrProcessing.Format.wBitsPerSample)
					{
					case 32:
						jvx_convertSamples_from_fxp_norm_to_flp<jvxInt32, jvxData>(
							(jvxInt32*)ptrSrc, (jvxData*)ptrDestFlt, numSamples,
							JVX_MAX_INT_32_DIV, i,
							waveFormatStrProcessing.Format.nChannels,
							0, 1);
						break;

					case 24:
						jvx_convertSamples_from_bytes_shift_norm_to_flp<jvxInt32, jvxData>(
							(char*)ptrSrc, ptrDestFlt,
							numSamples, 8,
							JVX_MAX_INT_32_DIV,
							numBytesSample,
							i, waveFormatStrProcessing.Format.nChannels,
							0, 1);
						break;
					case 16:
						jvx_convertSamples_from_fxp_norm_to_flp<jvxInt16, jvxData>(
							(jvxInt16*)ptrSrc, (jvxData*)ptrDestFlt, numSamples,
							JVX_MAX_INT_16_DIV,
							i, waveFormatStrProcessing.Format.nChannels,
							0, 1);
						break;
					default:
						assert(0);
					}
					break;
				/*
                case JVX_DATAFORMAT_64BIT_LE:
					ptrSrcI64 = (jvxInt64*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
					switch (waveFormatStrProcessing.Format.wBitsPerSample)
					{
					case 32:
						jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
							ptrSrcI64, (jvxInt32*)ptrDest, numSamples, 32, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;

					case 24:
						jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt64, jvxInt32>(
							ptrSrcI64, (jvxByte*)ptrDest, numSamples, (64 - 24), 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					case 16:
						jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
							ptrSrcI64, (jvxInt16*)ptrDest, numSamples, (64 - 16), 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					default:
						assert(0);
					}
					break;

				case JVX_DATAFORMAT_32BIT_LE:
					ptrSrcI32 = (jvxInt32*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
					switch (waveFormatStrProcessing.Format.wBitsPerSample)
					{
					case 32:
						jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
							ptrSrcI32, (jvxInt32*)ptrDest, numSamples, 0, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;

					case 24:
						jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
							ptrSrcI32, (jvxByte*)ptrDest, numSamples, numBytesSample,
							(32 - 24), 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					case 16:
						jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
							ptrSrcI32, (jvxInt16*)ptrDest, numSamples, 16, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					default:
						assert(0);
					}
					break;

				case JVX_DATAFORMAT_16BIT_LE:
					ptrSrcI16 = (jvxInt16*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
					switch (waveFormatStrProcessing.Format.wBitsPerSample)
					{
					case 32:
						jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
							ptrSrcI16, (jvxInt32*)ptrDest, numSamples, 16, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;

					case 24:
						jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
							ptrSrcI16, (jvxByte*)ptrDest, numSamples, numBytesSample, 8, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					case 16:
						jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
							ptrSrcI16, (jvxInt16*)ptrDest, numSamples, 0, 0, 1,
							i, waveFormatStrProcessing.Format.nChannels);
						break;
					}
					break;
                    */
				default:

					// Everything else: unsupported
					assert(0);
				} // switch (_inproc.format)
				chanCnt++;
			} // if (jvx_bitTest(channelMaskInProc, i))
		} // for (i = 0; i < waveFormatStrProcessing.Format.nChannels; i++)       

		*ptrFromTo += numBytesSampleAllChans * numSamples;
	}

    // I am the master. I will start the chain here!
    if (_common_set_ocon.theData_out.con_link.connect_to)
    {
        _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
    }

    if (!isInput)
    {
        //static jvxData phase = 0;
        //jvxData ampl = 0;
        jvxByte* ptrDest = (jvxByte*)(*ptrFromTo);
        jvxSize chanCnt = 0;   
        jvxData* ptrSrcFlt = nullptr;
        jvxInt64* ptrSrcI64 = nullptr;
        jvxInt32* ptrSrcI32 = nullptr;
        jvxInt16* ptrSrcI16 = nullptr;

        for (i = 0; i < waveFormatStrProcessing.Format.nChannels; i++)
        {
            if (jvx_bitTest(channelMaskInProc, i))
            {
                switch (_inproc.format)
				{
				case JVX_DATAFORMAT_DATA:
					ptrSrcFlt = (jvxData*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
                    switch (waveFormatStrProcessing.Format.wBitsPerSample)
                    {
                    case 32:
                        jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt32>(
                            ptrSrcFlt, (jvxInt32*)ptrDest, numSamples, JVX_MAX_INT_32_DATA, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;

                    case 24:
                        jvx_convertSamples_from_flp_norm_to_bytes<jvxData, jvxInt32>(
                            ptrSrcFlt, ptrDest,
                            numSamples,
                            JVX_MAX_INT_24_DATA,
                            numBytesSample, 0, 1,
                            i,
                            waveFormatStrProcessing.Format.nChannels);
                        break;
                    case 16:
                        jvx_convertSamples_from_flp_norm_to_fxp<jvxData, jvxInt16>(
                            ptrSrcFlt, (jvxInt16*)ptrDest, numSamples, JVX_MAX_INT_16_DATA, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    default:
                        assert(0);
                    }
                    break;
				case JVX_DATAFORMAT_64BIT_LE:
                    ptrSrcI64 = (jvxInt64*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
                    switch (waveFormatStrProcessing.Format.wBitsPerSample)
                    {
                    case 32:
                        jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt32, jvxInt64>(
                            ptrSrcI64, (jvxInt32*)ptrDest, numSamples, 32, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;

                    case 24:
                        jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt64, jvxInt32>(
                            ptrSrcI64, (jvxByte*)ptrDest, numSamples, (64-24), 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    case 16:
                        jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt64, jvxInt16, jvxInt64>(
                            ptrSrcI64, (jvxInt16*)ptrDest, numSamples, (64-16), 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    default:
                        assert(0);
                    }
                    break;

				case JVX_DATAFORMAT_32BIT_LE:
                    ptrSrcI32 = (jvxInt32*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
                    switch (waveFormatStrProcessing.Format.wBitsPerSample)
                    {
                    case 32:
                        jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt32, jvxInt32>(
                            ptrSrcI32, (jvxInt32*)ptrDest, numSamples, 0, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;

                    case 24:
                        jvx_convertSamples_from_fxp_shiftright_to_bytes<jvxInt32, jvxInt32>(
                            ptrSrcI32, (jvxByte*)ptrDest, numSamples, numBytesSample,
                            (32 - 24), 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    case 16:
                        jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt32, jvxInt16, jvxInt32>(
                            ptrSrcI32, (jvxInt16*)ptrDest, numSamples, 16, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    default:
                        assert(0);
                    }
                    break;

				case JVX_DATAFORMAT_16BIT_LE:
                    ptrSrcI16 = (jvxInt16*)_common_set_icon.theData_in->con_data.buffers[idxFromProc][chanCnt];
                    switch (waveFormatStrProcessing.Format.wBitsPerSample)
                    {
                    case 32:
                        jvx_convertSamples_from_fxp_shiftleft_to_fxp<jvxInt16, jvxInt32, jvxInt32>(
                            ptrSrcI16, (jvxInt32*)ptrDest, numSamples, 16, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;

                    case 24:
                        jvx_convertSamples_from_fxp_shiftleft_to_bytes<jvxInt16, jvxInt32>(
                            ptrSrcI16, (jvxByte*)ptrDest, numSamples, numBytesSample, 8, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    case 16:
                        jvx_convertSamples_from_fxp_shiftright_to_fxp<jvxInt16, jvxInt16, jvxInt16>(
                            ptrSrcI16, (jvxInt16*)ptrDest, numSamples, 0, 0, 1,
                            i, waveFormatStrProcessing.Format.nChannels);
                        break;
                    }
                    break;
                default:

                    // Everything else: unsupported
                    assert(0);
				}
                chanCnt++;
            } // if (jvx_bitTest(channelMaskInProc, i))
        } // for (i = 0; i < waveFormatStrProcessing.Format.nChannels; i++)       

        *ptrFromTo += numBytesSampleAllChans * numSamples;
	} //if (!isInput)
	
    if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		_common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
	}
	return JVX_NO_ERROR;
}

// ====================================================================================

void 
CjvxAudioWindowsDevice::setHandle(
    CjvxAudioWindowsTechnology* parentTechArg, 
    IMMDevice* deviceArg,
    WAVEFORMATEXTENSIBLE* wexArg,
    jvxBool isInputArg,
    jvxBool isDefaultDevice)
{ 
	device = deviceArg; 
	parentTech = parentTechArg;
    wext_init = *wexArg;
    isInput = isInputArg;
    if (isInput)
    {
        jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_INPUT_SHIFT);
        _common_set.theName = _common_set.theName + " input";
    }
    else
    {
        jvx_bitZSet(_common_set_device.caps.capsFlags, (int)jvxDeviceCapabilityTypeShift::JVX_DEVICE_CAPABILITY_OUTPUT_SHIFT);        
        _common_set.theName = _common_set.theName + " output";
    }
    _common_set_device.report = static_cast<IjvxDevice_report*>(parentTech);
}

// ==========================================================================
template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

static int testSamplerates[] =
{
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000,
    0
};

jvxErrorType 
CjvxAudioWindowsDevice::activate_wasapi_device()
{
    jvxErrorType res = JVX_NO_ERROR;
    IAudioClient* audioClient_pre = nullptr; 
    jvxSize i;

    audioMode = AUDCLNT_SHAREMODE_SHARED;
    /*
    if (!isInput)
    {
        audioMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
    }
    */

    if (jvx_bitTest(genWindows_device::properties_selected.device_mode.value.selection(), 0))
    {
        audioMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
    }
    if (jvx_bitTest(genWindows_device::properties_selected.device_mode.value.selection(), 1))
    {
        audioMode = AUDCLNT_SHAREMODE_SHARED;
    }
    // Activate access to the device
    HRESULT hr = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&audioClient_pre));
    if (audioClient_pre == nullptr)
    {
        res = JVX_ERROR_INTERNAL;
    }

    // Find out all capabilities
    WAVEFORMATEXTENSIBLE waveFormatStr = this->wext_init;
    i = 0;
    while (1)
    {     
        waveFormatStr.Format.nSamplesPerSec = testSamplerates[i];
        if (waveFormatStr.Format.nSamplesPerSec == 0)
        {
            break;
        }
        
        HRESULT formatOk = audioClient_pre->IsFormatSupported(audioMode,
            (WAVEFORMATEX*)&waveFormatStr, nullptr);
        if (formatOk == S_OK)
        {
            supportedRates.push_back(testSamplerates[i]);
        }
        i++;
    }
    SafeRelease(&audioClient_pre);

    jvxSize defRate = this->wext_init.Format.nSamplesPerSec;
    jvxSize idDefRate = JVX_SIZE_UNSELECTED;
    auto elm = supportedRates.begin();
    elm = std::find(elm, supportedRates.end(), defRate);
    if (elm == supportedRates.end())
    {
        supportedRates.push_back(defRate);
    }

    if (isInput)
    {
        CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();
        jvx_bitFClear(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection());
        for (i = 0; i < this->wext_init.Format.nChannels; i++)
        {
            //std::string chan_name = this->_common_set_min.theDescription + " #" + jvx_size2String(i);
            std::string chan_name = "In #" + jvx_size2String(i);
            CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.push_back(chan_name);
            jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i);
        }
    }
    else
    {
        CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.clear();
        jvx_bitFClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection());
        for (i = 0; i < this->wext_init.Format.nChannels; i++)
        {
            // std::string chan_name = this->_common_set_min.theDescription + " #" + jvx_size2String(i);
            std::string chan_name = "Out #" + jvx_size2String(i);
            CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(chan_name);
            jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i);
        }
    }

    genWindows_device::properties_active.ratesselection.value.entries.clear();
    for (jvxSize entry : supportedRates)
    {
        if (entry == defRate)
        {
            idDefRate = genWindows_device::properties_active.ratesselection.value.entries.size();
        }
        genWindows_device::properties_active.ratesselection.value.entries.push_back(jvx_size2String(entry));
    }
    if (JVX_CHECK_SIZE_SELECTED(idDefRate))
    {
        jvx_bitZSet(genWindows_device::properties_active.ratesselection.value.selection(), idDefRate);
    }

    // Update all parameters
    return res;
}

jvxErrorType
CjvxAudioWindowsDevice::deactivate_wasapi_device()
{
    jvxErrorType res = JVX_NO_ERROR;
    supportedRates.clear();
    SafeRelease(&audioClient);
    audioClient = nullptr;
    return res;
}

jvxErrorType
CjvxAudioWindowsDevice::prepare_wasapi_device()
{
    jvxSize i;
    jvxErrorType res = JVX_NO_ERROR;
    jvxAudioParams params;

    assert(!startupComplete);

    currentSetupAudioParams(params);

    // Create processing events
    _ShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_ShutdownEvent == NULL)
    {
        res = JVX_ERROR_INTERNAL;
        goto exit_error_0;
    }
    _AudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_AudioSamplesReadyEvent == NULL)
    {
        res = JVX_ERROR_INTERNAL;
        goto exit_error_I;
    }

    //
    //  Create our stream switch event- we want auto reset events that start in the not-signaled state.
    //  Note that we create this event even if we're not going to stream switch - that's because the event is used
    //  in the main loop of the capturer and thus it has to be set.
    //
    _StreamSwitchEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_StreamSwitchEvent == NULL)
    {
        res = JVX_ERROR_INTERNAL;
        goto exit_error_II;
    }

    // ==============================================================================

    procBufferSize = params.buffersize;

    jvxSize eventPeriod_nsecs = (jvxSize)((jvxData)params.buffersize / (jvxData)params.samplerate * 10000000);
    waveFormatStrProcessing = this->wext_init;    
    waveFormatStrProcessing.Format.nSamplesPerSec = params.samplerate;

    /*
    waveFormatStrProcessing.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    waveFormatStrProcessing.Format.wBitsPerSample = 16;
    waveFormatStrProcessing.Format.nBlockAlign = (waveFormatStrProcessing.Format.wBitsPerSample / 8) * waveFormatStrProcessing.Format.nChannels;
    waveFormatStrProcessing.Format.nAvgBytesPerSec = waveFormatStrProcessing.Format.nSamplesPerSec * waveFormatStrProcessing.Format.nBlockAlign;
    waveFormatStrProcessing.Samples.wValidBitsPerSample = 16;
    */
    // Activate access to the device
    HRESULT hr = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&audioClient));
    if (audioClient == nullptr)
    {
        res = JVX_ERROR_INTERNAL;
        goto exit_error_III;
    }

    num_proc_loops = 1;
    jvxBool loop_run = true;
    jvxSize numBufsFlush = genWindows_device::properties_active.numBuffersFlushShared.value;

    // We implement this in a loop to allow multiple loops for a frame in case the period is too small!
	while (loop_run)
	{
		if (audioMode == AUDCLNT_SHAREMODE_EXCLUSIVE)
		{
            std::cout << __FUNCTION__ << ": Running WASAPI device in <exclusive> mode." << std::endl;
			hr = audioClient->Initialize(audioMode,
				AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
				num_proc_loops * eventPeriod_nsecs,
				num_proc_loops * eventPeriod_nsecs,
				(WAVEFORMATEX*)&waveFormatStrProcessing, NULL);
		}
		else
		{
            // In the shared mode, we need to define larger buffers to prevent artefacts.
            // Since MS does not really explain what they do I found this by testing it out!
            std::cout << __FUNCTION__ << ": Running WASAPI device in <shared> mode." << std::endl;
			hr = audioClient->Initialize(audioMode,
				AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
                numBufsFlush * num_proc_loops * eventPeriod_nsecs,
				0,
				(WAVEFORMATEX*)&waveFormatStrProcessing, NULL);
		}

        // Preset the loop flag to finish
        loop_run = false;
        res = JVX_ERROR_INTERNAL;
        switch (hr)
        {
        case S_OK:
            res = JVX_NO_ERROR;
            break;
        case AUDCLNT_E_INVALID_DEVICE_PERIOD:

            // If we are here we can increase the loop count and try again!
            loop_run = true;
            num_proc_loops++;
            break;
        case AUDCLNT_E_NOT_INITIALIZED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_NOT_INITIALIZED>" << std::endl;
            break;
        case AUDCLNT_E_ALREADY_INITIALIZED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_ALREADY_INITIALIZED>" << std::endl;
            break;
        case AUDCLNT_E_WRONG_ENDPOINT_TYPE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_WRONG_ENDPOINT_TYPE>" << std::endl;
            break;
        case AUDCLNT_E_DEVICE_INVALIDATED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_DEVICE_INVALIDATED>" << std::endl;
            break;
        case AUDCLNT_E_NOT_STOPPED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_NOT_STOPPED>" << std::endl;
            break;
        case AUDCLNT_E_BUFFER_TOO_LARGE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFFER_TOO_LARGE>" << std::endl;
            break;
        case AUDCLNT_E_OUT_OF_ORDER:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_OUT_OF_ORDER>" << std::endl;
            break;
        case AUDCLNT_E_UNSUPPORTED_FORMAT :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_UNSUPPORTED_FORMAT>" << std::endl;
            break;
        case AUDCLNT_E_INVALID_SIZE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_INVALID_SIZE>" << std::endl;
            break;
        case AUDCLNT_E_DEVICE_IN_USE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_DEVICE_IN_USE>" << std::endl;
            break;
        case  AUDCLNT_E_BUFFER_OPERATION_PENDING:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFFER_OPERATION_PENDING>" << std::endl;
            break;
        case  AUDCLNT_E_THREAD_NOT_REGISTERED :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_THREAD_NOT_REGISTERED>" << std::endl;
            break;
        case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED>" << std::endl;
            break;
        case AUDCLNT_E_ENDPOINT_CREATE_FAILED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_ENDPOINT_CREATE_FAILED>" << std::endl;
            break;
        case AUDCLNT_E_SERVICE_NOT_RUNNING  :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_SERVICE_NOT_RUNNING>" << std::endl;
            break;
        case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED>" << std::endl;
            break;
        case AUDCLNT_E_EXCLUSIVE_MODE_ONLY  :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_EXCLUSIVE_MODE_ONLY>" << std::endl;
            break;
        case  AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL>" << std::endl;
            break;
        case AUDCLNT_E_EVENTHANDLE_NOT_SET:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_EVENTHANDLE_NOT_SET>" << std::endl;
            break;
        case AUDCLNT_E_INCORRECT_BUFFER_SIZE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_INCORRECT_BUFFER_SIZE>" << std::endl;
            break;
        case AUDCLNT_E_BUFFER_SIZE_ERROR:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFFER_SIZE_ERROR>" << std::endl;
            break;
        case AUDCLNT_E_CPUUSAGE_EXCEEDED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_CPUUSAGE_EXCEEDED>" << std::endl;
            break;
        case  AUDCLNT_E_BUFFER_ERROR:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFFER_ERROR>" << std::endl;
            break;
        case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED>" << std::endl;
            break;
        case AUDCLNT_E_INVALID_STREAM_FLAG:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_INVALID_STREAM_FLAG>" << std::endl;
            break;
        case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE>" << std::endl;
            break;
        case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES>" << std::endl;
            break;
        case AUDCLNT_E_OFFLOAD_MODE_ONLY :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_OFFLOAD_MODE_ONLY>" << std::endl;
            break;
        case AUDCLNT_E_NONOFFLOAD_MODE_ONLY :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_NONOFFLOAD_MODE_ONLY>" << std::endl;
            break;
        case AUDCLNT_E_RESOURCES_INVALIDATED  :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_RESOURCES_INVALIDATED>" << std::endl;
            break;
        case AUDCLNT_E_RAW_MODE_UNSUPPORTED :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_RAW_MODE_UNSUPPORTED>" << std::endl;
            break;
        case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_ENGINE_PERIODICITY_LOCKED>" << std::endl;
            break;
        case AUDCLNT_E_ENGINE_FORMAT_LOCKED  :
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_ENGINE_FORMAT_LOCKED>" << std::endl;
            break;
        case AUDCLNT_E_HEADTRACKING_ENABLED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_HEADTRACKING_ENABLED>" << std::endl;
            break;
        case AUDCLNT_E_HEADTRACKING_UNSUPPORTED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_E_HEADTRACKING_UNSUPPORTED>" << std::endl;
            break;
        case AUDCLNT_S_BUFFER_EMPTY:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_S_BUFFER_EMPTY>" << std::endl;
            break;
        case AUDCLNT_S_THREAD_ALREADY_REGISTERED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_S_THREAD_ALREADY_REGISTERED>" << std::endl;
            break;
        case AUDCLNT_S_POSITION_STALLED:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = <AUDCLNT_S_POSITION_STALLED>" << std::endl;
            break;
        default:
            std::cout << __FUNCTION__ << ": Initialization of Audio Device failed, error code = " << hr << std::endl;
            assert(0);
            break;
        }
    }

    if (res != JVX_NO_ERROR)
    {
        goto exit_error_IV;
    }
    //
    //  Retrieve the buffer size for the audio client.
    //
    hr = audioClient->GetBufferSize(&realBufferSize);
    if (FAILED(hr))
    {
        assert(0);
    }

    hr = audioClient->SetEventHandle(_AudioSamplesReadyEvent);
    if (FAILED(hr))
    {
        assert(0);
    }

    if (isInput)
    {
        hr = audioClient->GetService(IID_PPV_ARGS(&captureClient));
    }
    else
    {
        hr = audioClient->GetService(IID_PPV_ARGS(&renderClient));
    }
 
    if (FAILED(hr))
    {
        assert(0);
    }

    numBytesSample = waveFormatStrProcessing.Format.wBitsPerSample / 8;
    numBytesSampleAllChans = waveFormatStrProcessing.Format.wBitsPerSample / 8 * (jvxSize)waveFormatStrProcessing.Format.nChannels;

    if (/* isInput &&*/ (audioMode == AUDCLNT_SHAREMODE_SHARED))
    {
        // Here, we need another buffer to provide the samples when they are requested
        shared_buf_length = numBufsFlush * (jvxSize)realBufferSize;
        shared_buf_fill = 0;
        JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(shared_buf, jvxByte, shared_buf_length * numBytesSampleAllChans);
    }

    channelMaskInProc = 0;
    for (i = 0; i < waveFormatStrProcessing.Format.nChannels; i++)
    {
        if(isInput)
        { 
            assert(i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size());
            if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
            {
                jvx_bitSet(channelMaskInProc, i);
            }
        }        
        else
        {
            assert(i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size());
            if (jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
            {
                jvx_bitSet(channelMaskInProc, i);
            }
        }
    }

    startupComplete = true;
    return res;


exit_error_IV:
    // audioClient
    SafeRelease(&audioClient);

exit_error_III:
    CloseHandle(_StreamSwitchEvent);
    _StreamSwitchEvent = JVX_INVALID_HANDLE_VALUE;

exit_error_II:
    CloseHandle(_AudioSamplesReadyEvent);
    _AudioSamplesReadyEvent = JVX_INVALID_HANDLE_VALUE;

exit_error_I:
    CloseHandle(_ShutdownEvent);
    _ShutdownEvent = JVX_INVALID_HANDLE_VALUE;

exit_error_0:
    return res;
}

jvxErrorType
CjvxAudioWindowsDevice::postprocess_wasapi_device()
{
    jvxErrorType res = JVX_NO_ERROR;

    if (!startupComplete)
    {
        return JVX_ERROR_WRONG_STATE;
    }

    if (audioMode == AUDCLNT_SHAREMODE_SHARED)
    {
        JVX_DSP_SAFE_DELETE_FIELD(shared_buf);
    }
    shared_buf = nullptr;
    shared_buf_fill = 0;
    shared_buf_length = 0;

    channelMaskInProc = 0;

    numBytesSample = 0;
    numBytesSampleAllChans = 0;

    if (isInput)
    {
        SafeRelease(&captureClient);
    }
    else
    {
        SafeRelease(&renderClient);
    }
    
    SafeRelease(&audioClient);

    CloseHandle(_ShutdownEvent);
    _ShutdownEvent = JVX_INVALID_HANDLE_VALUE;

    CloseHandle(_AudioSamplesReadyEvent);
    _AudioSamplesReadyEvent = JVX_INVALID_HANDLE_VALUE;

    CloseHandle(_StreamSwitchEvent);
    _StreamSwitchEvent = JVX_INVALID_HANDLE_VALUE;

    startupComplete = false;

    return res;
}

jvxErrorType
CjvxAudioWindowsDevice::start_wasapi_device()
{
    HRESULT hr;
    sigProcThread = CreateThread(NULL, 0, audioProcessThread, this, 0, NULL);
    if (sigProcThread == NULL)
    {
        assert(0);
    }
    JVX_SET_THREAD_PRIORITY(sigProcThread, JVX_THREAD_PRIORITY_REALTIME);

    if (!isInput)
    {
        BYTE* pData = nullptr;
        hr = renderClient->GetBuffer(realBufferSize, &pData);
        if (SUCCEEDED(hr))
        {
            jvxSize numBytesSample = waveFormatStrProcessing.Format.wBitsPerSample / 8;
            jvxSize numBytesSampleAllChans = waveFormatStrProcessing.Format.wBitsPerSample / 8 * waveFormatStrProcessing.Format.nChannels;
            memset(pData, 0, numBytesSampleAllChans * realBufferSize);
            // core_buffer_process_render(&pData, realBufferSize);

            hr = renderClient->ReleaseBuffer(realBufferSize, 0);
        }
    }

    // hr = audioClient->Reset();
    //
    //  We're ready to go, start capturing!
    //
     hr = audioClient->Start();
    if (FAILED(hr))
    {
        assert(0);
    }

    return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioWindowsDevice::stop_wasapi_device()
{
    HRESULT hr;
    if (_ShutdownEvent)
    {
        SetEvent(_ShutdownEvent);
    }

    hr = audioClient->Stop();
    if (FAILED(hr))
    {
        printf("Unable to stop audio client: %x\n", hr);
    }


    if (sigProcThread)
    {
        WaitForSingleObject(sigProcThread, INFINITE);
        CloseHandle(sigProcThread);
        sigProcThread = JVX_INVALID_HANDLE_VALUE;
    }
    return JVX_NO_ERROR;
}