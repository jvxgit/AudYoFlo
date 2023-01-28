function [handle] = rtp_FFT_frame_init_ols_cf(lFFT, lFRAME, presetVals)

    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.idxWrite = 0;
    handle.signals.fftBuffer = zeros(1, handle.lFFT);    
    if(nargin == 3)
        ll = length(presetVals);
        handle.signals.fftBuffer(end-ll+1:end) = presetVals;
        handle.idxWrite = mod(handle.idxWrite + ll, handle.lFFT);
    end
    return;
    