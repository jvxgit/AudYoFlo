function [handle] = rtp_FFT_frame_init_ola_cf(lFFT, lFRAME)

    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.signals.fftBuffer = zeros(1, handle.lFFT);    
    return;
    