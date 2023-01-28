function [handle] = rtp_IFFT_frame_init_sw_ola(lFFT, lFRAME, normalization)
    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.signals.ifftBuffer = zeros(1, handle.lFFT);
    handle.signals.buffer = zeros(1, handle.lFFT);
    handle.control.idxBufferWrite = 0;
    handle.normalization = normalization;

