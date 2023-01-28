function [handle] = rtp_IFFT_frame_init_ols(lFFT, lFRAME, normalization)
    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.idxRead = 0;
    handle.signals.ifftBuffer = zeros(1, handle.lFFT);
    handle.normalization = normalization;

