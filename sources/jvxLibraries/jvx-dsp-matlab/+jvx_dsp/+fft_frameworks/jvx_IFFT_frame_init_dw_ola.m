function [handle] = rtp_IFFT_frame_init_dw_ola(lFFT, lBUF, lFRAME, window, offset, normalization)
    handle.lFFT = lFFT;
    handle.lBUF = lBUF;
    handle.lFRAME = lFRAME;
    handle.window = window;
    handle.offset = offset; 
    handle.signals.ifftBuffer = zeros(1, handle.lFFT);
    handle.signals.buffer = zeros(1, handle.lBUF);
    handle.control.idxBufferWrite = 0;
    handle.normalization = normalization;

