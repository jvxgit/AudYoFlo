%% ======================================================================
%--------------------
% How does it work?
%--------------------
%
% ========================================================================

function [fftBuf, handle] = rtp_FFT_frame_transformComplex_ola_cf(frame, handle)

    if(size(frame,2) == handle.lFRAME)
        
        handle.signals.fftBuffer(1:handle.lFFT) = zeros(1, handle.lFFT);
        handle.signals.fftBuffer(1:handle.lFRAME) = frame;
        
        % Do the FFT
        fftBuf = fft(handle.signals.fftBuffer);
    end
    return;
