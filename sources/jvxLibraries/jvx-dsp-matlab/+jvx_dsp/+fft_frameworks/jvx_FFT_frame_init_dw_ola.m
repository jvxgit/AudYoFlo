function [handle] = rtp_FFT_frame_init_dw_ola(lFFT, lBUF, lFRAME, window, offset, presetVals)

    handle.lFFT = lFFT;
    handle.lBUF = lBUF;
    handle.lFRAME = lFRAME;
    handle.window = window;
    handle.offset = offset;
    
    handle.signals.fftBuffer = zeros(1, handle.lFFT);    
    handle.signals.buffer = zeros(1, handle.lBUF);
    handle.control.idxBufferWrite = 0;
    if(nargin == 6)
        handle.signals.buffer(1:length(presetVals)) = presetVals;
        handle.control.idxBufferWrite = mod((handle.control.idxBufferWrite+length(presetVals)),handle.lBUF); ;
    end
    return;
    