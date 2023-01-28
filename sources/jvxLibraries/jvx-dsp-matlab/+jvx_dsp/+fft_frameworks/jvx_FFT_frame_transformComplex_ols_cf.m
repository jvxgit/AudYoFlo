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
% In v3 of the FFT framework, overlap-save is used. In overlap save, no
% window can be used, hence the following is mandatory:
% lFRAME = lBUF!!
%
% We must fulfill the antialiasing condition here to avoid artefacts, that
% is, the FFT must be zero padded. Given lFRAME and lFFT, the equivalent 
% impulse response in the time domain should be of maximum length
% (lFFT-lFRAME+1).
%
% Given the FFT buffer of the input as 
%
% [x x x x x x x 0 0 0 0 0 0 0]
%
% and the time domain representation of the frequenc manipulation H as
%
% [y y y y y y 0 0 0 0 0 0 0 0]
%
% the convolution shall be considered:
% Comparing the circular convolution with the linear filtering, we see that
% the last lFRAME samples of the circular convolution output are identical 
% to the last lFRAME samples of the linear convolution.
%
%
% Note: There is no delay (in addition to the buffering delay) that would
% have any impact onto the output. Antialiasing is absolutely required due
% to the fact that no window can be used..
% ========================================================================

function [fftBuf, handle] = rtp_FFT_frame_transformComplex_ols_cf(frame, handle)

    if(size(frame,2) == handle.lFRAME)
        
        idxW = handle.idxWrite;

        % Copy new samples to circular buffer first
        ll1 = min(handle.lFFT - idxW, handle.lFRAME);
        ll2 = handle.lFRAME - ll1;
        
        handle.signals.fftBuffer(idxW+1:idxW+ll1) = frame(1:ll1);
        
        if(ll2 > 0)
            handle.signals.fftBuffer(1:ll2) = frame(ll1+1:end);
            idxW = ll2;
        else
            idxW = idxW + handle.lFRAME;
            if(idxW >= handle.lFFT)
                idxW = idxW - handle.lFFT;
            end
        end
         
        % Do the FFT
        fftBuf = fft(handle.signals.fftBuffer);
        
        handle.idxWrite = idxW;
    end
    return;