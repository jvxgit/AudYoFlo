function [handle] = rtp_IFFT_frame_init_ola_cf(lFFT, lFRAME, normalization, window_second_half)
    if(size(window_second_half, 2) ~= lFRAME)
        error('Wrong size of window half');
    end
        
    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;

    handle.windowHalf = zeros(1, handle.lFFT);
    handle.windowHalf(1:lFRAME) = window_second_half;
    handle.idxRead = 0;
    handle.signals.ifftBuffer = zeros(1, handle.lFFT);
    handle.signals.olabuffer = zeros(1, handle.lFFT);
    handle.normalization = normalization;
