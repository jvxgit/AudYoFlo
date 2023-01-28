function [out, handle] = rtp_IFFT_frame_transformComplex_ols_cf(fftBuf_short_old, fftBuf_short_new, handle)
        
    % Both fft buffers have a length of FFTSIZE/2+1
    symmetricFft_old = [fftBuf_short_old conj(fftBuf_short_old(end-1:-1:2))];
    asymmetricFft_new = [(1i * fftBuf_short_new) (1i * conj(fftBuf_short_new(end-1:-1:2)))];
    
    cplxFft = symmetricFft_old + asymmetricFft_new;
    
    handle.signals.ifftBuffer = ifft(cplxFft);
   
    ifftBuffer_old = real(handle.signals.ifftBuffer);
    ifftBuffer_new = imag(handle.signals.ifftBuffer);
    
    % Spectrum to time domain buffer
    idxR = handle.idxRead;
    delta = 1/handle.lFRAME;
    
    idxInWindow_fwd = 1;
    idxInWindow_bwd = handle.lFRAME;
    
    out = zeros(1, handle.lFRAME);
    
    % Copy new samples to circular buffer first
    ll1 = min(handle.lFFT - idxR, handle.lFRAME);
    ll2 = handle.lFRAME - ll1;

    for ind = 1:ll1
%        out(ind) = ifftBuffer_new(idxR+ind);
%        out(ind) = out(ind) * handle.normalization;
        out(ind) = handle.windowHalf(idxInWindow_fwd) * ifftBuffer_old(idxR+ind)...
            + handle.windowHalf(idxInWindow_bwd) * ifftBuffer_new(idxR+ind);
        out(ind) = out(ind) * handle.normalization;
       idxInWindow_fwd = idxInWindow_fwd + 1;
       idxInWindow_bwd = idxInWindow_bwd - 1;
    end
            
    if(ll2 > 0)
        for ind = 1:ll2
%            out(ll1+ind) = ifftBuffer_new(ind);
%            out(ind) = out(ind) * handle.normalization;
            out(ll1+ind) = handle.windowHalf(idxInWindow_fwd) * ifftBuffer_old(ind)...
                + handle.windowHalf(idxInWindow_bwd) * ifftBuffer_new(ind);
            out(ind) = out(ind) * handle.normalization;
           idxInWindow_fwd = idxInWindow_fwd + 1;
           idxInWindow_bwd = idxInWindow_bwd - 1;
        end
        idxR = ll2;
    else
        idxR = idxR + handle.lFRAME;
        if(idxR >= handle.lFFT)
            idxR = idxR - handle.lFFT;
        end
    end

    handle.idxRead = idxR;
    