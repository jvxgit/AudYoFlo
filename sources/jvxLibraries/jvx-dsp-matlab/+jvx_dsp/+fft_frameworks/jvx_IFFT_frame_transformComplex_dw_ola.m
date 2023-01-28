function [out, handle] = rtp_IFFT_frame_transformComplex_dw_ola(fftBuf, handle)
%function [out, handle, dbg] = rtp_IFFT_frame_transformComplex_dw_ola(fftBuf, handle)
out = zeros(1, handle.lFRAME);
    
    % Spectrum to time domain buffer
    handle.signals.ifftBuffer = real(ifft(fftBuf));

    idxW = handle.control.idxBufferWrite;
    
    % Overlap output, apply window at the same time 
    ll1 = handle.lBUF - idxW;
    ll2 = handle.lBUF - ll1;
    if(ll1 > 0)
        handle.signals.buffer(idxW+1:idxW+ll1) = ...
            handle.normalization * handle.signals.ifftBuffer(handle.offset+1:handle.offset+ll1) .* handle.window(1:ll1) + ...
            handle.signals.buffer(idxW+1:idxW+ll1);
    end
    if(ll2 > 0)
        handle.signals.buffer(1:ll2) = ...
            handle.normalization * handle.signals.ifftBuffer(handle.offset+1+ll1:handle.offset+ll1+ll2) .* handle.window(ll1+1:ll1+ll2) + ...
            handle.signals.buffer(1:ll2);
    end
    
    % Outout lFRAME samples and reset the output samples to zero
    ll1 = min(handle.lBUF - idxW, handle.lFRAME);
    ll2 = handle.lFRAME - ll1;
    if(ll1 > 0)
        out(1:ll1) = handle.signals.buffer(idxW+1:idxW+ll1);
        handle.signals.buffer(idxW+1:idxW+ll1) = zeros(1,ll1);
    end
    if(ll2 > 0)
        out(ll1+1:ll1+ll2) = handle.signals.buffer(1:ll2);
        handle.signals.buffer(1:ll2) = zeros(1,ll2);
    end
    
    %dbg = handle.signals.buffer;

        % Update the cricular buffer pointer
    handle.control.idxBufferWrite = handle.control.idxBufferWrite + handle.lFRAME;
    if(handle.control.idxBufferWrite >= handle.lBUF)
        handle.control.idxBufferWrite = handle.control.idxBufferWrite - handle.lBUF;
    end
    
    