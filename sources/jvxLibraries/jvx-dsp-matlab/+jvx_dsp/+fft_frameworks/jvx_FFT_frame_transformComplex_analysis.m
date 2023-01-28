%% ======================================================================

% ========================================================================

function [fftBuf, handle] = rtp_FFT_frame_transformComplex_analysis(frame, handle)

    if(size(frame,2) == handle.lFRAME)
        
        idxW = handle.control.idxBufferWrite;

        % Copy new samples to circular buffer first
        ll1 = min(handle.lFFT - idxW, handle.lFRAME);
        ll2 = handle.lFRAME-ll1;
        
        handle.signals.buffer(idxW+1:idxW+ll1) = frame(1:ll1);
        
        if(ll2 > 0)
            handle.signals.buffer(1:ll2) = frame(ll1+1:end);
            idxW = ll2;
        else
            idxW = mod(idxW + handle.lFRAME, handle.lFFT);
        end
        
        % Copy buffer to FFT buffer for transform
        ll1 = handle.lFFT - idxW;
        ll2 = idxW;
        for(idx = 0:ll1-1)
            handle.signals.fftBuffer(idx +1) = handle.signals.buffer(idxW + idx + 1) * handle.window(idx+1);
        end
        for(idx = 0:(ll2-1))
            handle.signals.fftBuffer(idx +1 +ll1) = handle.signals.buffer(idx + 1) * handle.window(ll1+idx+1);
        end
        
        % Do the FFT
        fftBuf = fft(handle.signals.fftBuffer);
        handle.control.idxBufferWrite = idxW;
    end
    return;
