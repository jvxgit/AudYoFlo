function [handle] = rtp_FFT_frame_init_analysis(lFFT, lFRAME, window)

    handle.lFFT = lFFT;
    handle.lFRAME = lFRAME;
    handle.window = window;
    
    handle.signals.fftBuffer = zeros(1, handle.lFFT);    
    handle.signals.buffer = zeros(1, handle.lFFT);
    handle.control.idxBufferWrite = 0;
    return;
    