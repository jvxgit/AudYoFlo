function [ H_mod, h_mod, handle, desiredImpulseResponseLength] = rtp_noAliasingFftWeighting(H, handle)
    
% Compute desired frequency response and corresponding impulse response
H_sym = [H conj(fliplr(H(2:end-1)))];
h_mod = ifft(H_sym);

if(0)

    %% Variant 1: Zero-phase filter which introduces artefacts in IND FFT Framework
    % ==============================================================================
    % Determine the number of zeros required in time domain impulse response
    numZerosH = handle.BUFFERSIZE - 1;
    numZerosH_half = floor(numZerosH/2);
    numZerosH_half = min(numZerosH_half, handle.FFTSIZE/2-1);
    
    % Transform desired frequency response into time domain IR
    
    % In IR=[ N1 X1 X2 X3 N2 X3 X2 X1 ], we need to set the values X3..X2
    % to zero to retain a real frequency response
    h_mod((handle.FFTSIZE/2 - numZerosH_half + 1):(handle.FFTSIZE/2)) = 0;
    h_mod(handle.FFTSIZE/2 + 1) = 0;
    h_mod(handle.FFTSIZE/2+2:handle.FFTSIZE/2 + numZerosH_half + 1) = 0;
    
    H_mod = real(fft(h_mod));
    H_mod = H_mod(1:handle.FFTSIZE/2+1);
    handle.additionalDelay = 0;
    
else
    
    %% Variant 2: Linear-phase filter which avoids artefacts in LINCONV mode
    % ==============================================================================
    desiredImpulseResponseLength = handle.FFTSIZE - handle.BUFFERSIZE + 1;
    desiredImpulseResponseLength = floor((desiredImpulseResponseLength - 1)/2) * 2 + 1; % <- must be odd
    desiredDelay = (desiredImpulseResponseLength - 1)/2;
    h_mod = circshift(h_mod, [0 desiredDelay]);
    h_mod(handle.FFTSIZE:-1:handle.FFTSIZE+1-(handle.FFTSIZE-desiredImpulseResponseLength)) = 0;
    %        dataProcessing.frwk.h = circshift(dataProcessing.frwk.h, [0 -dataProcessing.frwk.BUFFERSIZE/2]);
    H_mod = fft(h_mod);
    H_mod = H_mod(1:handle.FFTSIZE/2+1);
    handle.additionalDelay = desiredDelay;
end