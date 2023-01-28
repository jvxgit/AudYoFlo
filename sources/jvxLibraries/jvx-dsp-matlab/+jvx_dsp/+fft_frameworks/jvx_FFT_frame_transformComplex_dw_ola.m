function [fftBuf, handle] = rtp_FFT_frame_transformComplex_dw_ola(frame, handle)

    if(size(frame,2) == handle.lFRAME)
        
        idxW = handle.control.idxBufferWrite;

        % Copy new samples to circular buffer first
        ll1 = min(handle.lBUF - idxW, handle.lFRAME);
        ll2 = handle.lFRAME-ll1;
        
        handle.signals.buffer(idxW+1:idxW+ll1) = frame(1:ll1);
        
        if(ll2 > 0)
            handle.signals.buffer(1:ll2) = frame(ll1+1:end);
            idxW = ll2;
        else
            idxW = idxW + handle.lFRAME;
            if(idxW >= handle.lBUF)
                idxW = idxW - handle.lBUF;
            end
        end
        
        % Copy buffer to FFT buffer for transform
        ll1 = handle.lBUF - idxW;
        for idx = 0:ll1-1
            handle.signals.fftBuffer(idx + handle.offset +1) = handle.signals.buffer(idxW + idx + 1) * handle.window(idx+1);
        end
        for idx = 0:(handle.lBUF-ll1-1)
            handle.signals.fftBuffer(idx + handle.offset +1 +ll1) = handle.signals.buffer(idx + 1) * handle.window(ll1+idx+1);
        end
        
        % Do the FFT
        fftBuf = fft(handle.signals.fftBuffer);
        handle.control.idxBufferWrite = idxW;
    end
    return;
