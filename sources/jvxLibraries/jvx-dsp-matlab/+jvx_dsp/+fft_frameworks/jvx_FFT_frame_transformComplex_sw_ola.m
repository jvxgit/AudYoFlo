%% ======================================================================
%--------------------
% How does it work?
%--------------------
%
% We define the FFT buffer, the BUFFER and the FRAME.
% FFT Buffer size may be 1024, BUFFER size may be 512, and FRAME size may be 40.
% In other words, the FRAME is the HOPSIZE and the BUFFER is the
% windowed signal composed of samples from the current FRAME and
% samples from the past. FFT size should to the power of two for
% high performance implementation.
%
% Given the BUFFER as follows:
%
% FRAME:                           |++new++|
%                      <copy to>
% BUFFER: |----------old------------++new++|
%
% At first, FRAME size new samples are copied to the last part of the
% BUFFER. This is realized based on a circular buffer to achieve maximum
% performance.
% Next, the BUFFER is copied into the FFT Buffer. While doing so, the
% window is applied,
%
% BUFFER       |---------old----------++new++|
%                    <multiply with window>
% FFT BUFFER   |---------old----------++new++000000000000000000000000|
%
% Note: There are FFTBuffer size - BUFFER size zeros in the FFT Buffer in
% order to do zero padding
% ========================================================================

function [fftBuf, handle] = rtp_FFT_frame_transformComplex_v2(frame, handle)

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
        for(idx = 0:ll1-1)
            handle.signals.fftBuffer(idx +1) = handle.signals.buffer(idxW + idx + 1) * handle.window(idx+1);
        end
        for(idx = 0:(handle.lBUF-ll1-1))
            handle.signals.fftBuffer(idx +1 +ll1) = handle.signals.buffer(idx + 1) * handle.window(ll1+idx+1);
        end
        
        % Do the FFT
        fftBuf = fft(handle.signals.fftBuffer);
        handle.control.idxBufferWrite = idxW;
    end
    return;
