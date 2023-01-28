function [handle] = rtp_IFFT_frame_init_ols_cf(lFFT, lFRAME, window_second_half, normalization)
    handle.windowHalf = window_second_half;
    if(size(window_second_half, 2) ~= lFRAME)
        error('Wrong size of window half');
    end
        
    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.idxRead = 0;
    handle.signals.ifftBuffer = zeros(1, handle.lFFT);
    handle.normalization = normalization;
