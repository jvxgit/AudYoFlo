function [out, handle] = rtp_IFFT_frame_transformComplex_ols(fftBuf, handle)
        
    % Spectrum to time domain buffer
    handle.signals.ifftBuffer = real(ifft(fftBuf));
    idxR = handle.idxRead;
    
    out = zeros(1, handle.lFRAME);
    
    % Copy new samples to circular buffer first
    ll1 = min(handle.lFFT - idxR, handle.lFRAME);
    ll2 = handle.lFRAME - ll1;
        
    out(1:ll1) = handle.signals.ifftBuffer(idxR+1:idxR+ll1)* handle.normalization;
        
    if(ll2 > 0)
        out(ll1+1:end) = handle.signals.ifftBuffer(1:ll2)* handle.normalization;
        idxR = ll2;
    else
        idxR = idxR + handle.lFRAME;
        if(idxR >= handle.lFFT)
            idxR = idxR - handle.lFFT;
        end
    end

    handle.idxRead = idxR;
    