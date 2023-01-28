%% ======================================================================
%--------------------
% How does it work?
%--------------------
%
% We define the FFT buffer, the BUFFER and the FRAME.
% FFT Buffer size may be 1024, BUFFER size may be 512, and FRAME size may be 40.
% In other words, the FRAME is the HOPSIZE and the BUFFER is the
% windowed signal composed of samples from the current FRAME and
% samples from the past. FFT size should to the power of two for
% high performance implementation.
%
% For reconstruction (synthesis), we obtain a SPECTRUM Buffer which is
% transformed into a time domain buffer by means of the FFT:
% Given the FFT Buffer as
%
% FFT Buffer |ffffffffffffffffffffffffffffffffffff|
%
% Let us identify from analsysis which part is related to which part of the
% input (given a talkthrough functionality)
%
% FFT Buffer |ffffffffffffffffffff ffffffffffffffff|
%            |uuuuuBUFFERuuuuuuuuu 0000000000000000|
%            |----old------++new++ 0000000000000000|
%
% If we overlap this to the previous FFT Buffer as
%
% k-2:   |----old------++new++ 0000000000000000|
% k-1:       |----old------++new++ 0000000000000000|
% k  :           |----old------++new++ 0000000000000000|
%---------------------------------------------------------
% sum:           |$$out$$%%%%%future%% 0000000000000000|
%
% we can identify that the first FRAME size samples of "sum" are the IFFT 
% output of size FRAME size to be output. As a conclusion, the signal delay
% of FFT processing is as follows:
%
% We define the delay as the shift of the output signal compared to the
% input signal (in practice, there is another FRAME size samples delay due
% to the buffering prior to processing):
% Delay = BUFFER size - FRAME size

% ========================================================================

function [out, handle] = rtp_IFFT_frame_transformComplex_sw_ola(fftBuf, handle)
    out = zeros(1, handle.lFRAME);
    
    % Spectrum to time domain buffer
    handle.signals.ifftBuffer = real(ifft(fftBuf));

    idxW = handle.control.idxBufferWrite;
    
    % Overlap output, apply window at the same time 
    ll1 = handle.lFFT - idxW;
    ll2 = handle.lFFT - ll1;
    if(ll1 > 0)
        handle.signals.buffer(idxW+1:idxW+ll1) = ...
            handle.signals.ifftBuffer(1:ll1) + handle.signals.buffer(idxW+1:idxW+ll1);
    end
    if(ll2 > 0)
        handle.signals.buffer(1:ll2) = ...
            handle.signals.ifftBuffer(1+ll1:ll1+ll2) + handle.signals.buffer(1:ll2);
    end
    
    % Outout lFRAME samples and reset the output samples to zero
    ll1 = min(handle.lFFT - idxW, handle.lFRAME);
    ll2 = handle.lFRAME - ll1;
    if(ll1 > 0)
        out(1:ll1) = handle.signals.buffer(idxW+1:idxW+ll1)* handle.normalization;
        handle.signals.buffer(idxW+1:idxW+ll1) = zeros(1,ll1);
    end
    if(ll2 > 0)
        out(ll1+1:ll1+ll2) = handle.signals.buffer(1:ll2)* handle.normalization;
        handle.signals.buffer(1:ll2) = zeros(1,ll2);
    end
    
    % Update the cricular buffer pointer
    handle.control.idxBufferWrite = handle.control.idxBufferWrite + handle.lFRAME;
    if(handle.control.idxBufferWrite >= handle.lFFT)
        handle.control.idxBufferWrite = handle.control.idxBufferWrite - handle.lFFT;
    end
    
    