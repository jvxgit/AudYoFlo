function [out, handle] = rtp_IFFT_frame_transformComplex_ola_cf(fftBuf_short_old, fftBuf_short_new, handle)
        
    % Both fft buffers have a length of FFTSIZE/2+1
    symmetricFft_old = [fftBuf_short_old conj(fftBuf_short_old(end-1:-1:2))];
    asymmetricFft_new = [(i * fftBuf_short_new) (i * conj(fftBuf_short_new(end-1:-1:2)))];
    
    cplxFft = symmetricFft_old + asymmetricFft_new;
    
    handle.signals.ifftBuffer = ifft(cplxFft);
   
    ifftBuffer_old = real(handle.signals.ifftBuffer);
    ifftBuffer_new = imag(handle.signals.ifftBuffer);
    
    % Spectrum to time domain buffer
    idxR = handle.idxRead;
    
    out = zeros(1, handle.lFRAME);
    
    tmp = handle.windowHalf .* ifftBuffer_old + (1-handle.windowHalf).* ifftBuffer_new;
    ll1 = handle.lFFT - idxR;
    ll2 = handle.lFFT - ll1;
    
    for(ind = 1:ll1)
        handle.signals.olabuffer(idxR+ind) = tmp(ind) + handle.signals.olabuffer(idxR+ind);
    end
    for(ind = 1:ll2)
        handle.signals.olabuffer(ind) = tmp(ll1 + ind) + handle.signals.olabuffer(ind);
    end
 
    ll1 = min(handle.lFFT - idxR, handle.lFRAME);
    ll2 = handle.lFRAME - ll1;
    if(ll1 > 0)
        out(1:ll1) = handle.signals.olabuffer(idxR+1:idxR+ll1) * handle.normalization;
        handle.signals.olabuffer(idxR+1:idxR+ll1) = zeros(1,ll1);
    end
    if(ll2 > 0)
        out(ll1+1:ll1+ll2) = handle.signals.olabuffer(1:ll2)* handle.normalization;
        handle.signals.olabuffer(1:ll2) = zeros(1,ll2);
    end
    
    % Update the cricular buffer pointer
    handle.idxRead = mod(handle.idxRead + handle.lFRAME, handle.lFFT);
    