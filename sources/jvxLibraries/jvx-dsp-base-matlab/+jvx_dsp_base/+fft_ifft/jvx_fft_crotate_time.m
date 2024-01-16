%% Example Code:
% FFTL = 1024;
% fs=16000;
% DELAY = 10;
% COMBAT_DELAY = -10;

% idx =[0:FFTL-1];
% inBuf = [1 zeros(1,FFTL-1)];
% shiftBuf = circshift(inBuf, DELAY);
% [fftBufRe, fftBufIm] = jvxFFTWFft(shiftBuf);
% fftBuf = fftBufRe + 1i*fftBufIm;

% fmax = fs/2;
% f = COMBAT_DELAY *[0:2*fmax/FFTL:fs/2];
% arg = 2*pi*f/fs;
% rotationVec = exp(1i*arg);
% fftBuf1 = fftBuf .* rotationVec;

% rrot = cos(arg);
% irot = sin(arg);

% fftBuf2Re = real(fftBuf) .* rrot - imag(fftBuf) .* irot;
% fftBuf2Im = real(fftBuf) .* irot + imag(fftBuf) .* rrot;
% fftBuf2 = fftBuf2Re + 1i*fftBuf2Im;

% fftBuf3 = jvx_dsp_base.fft_ifft.jvx_fft_crotate_time(fftBuf, COMBAT_DELAY);

% fftBuf1 = fftBuf3;
% outBuf = jvxFFTWIfft(real(fftBuf1), imag(fftBuf1));

% figure;
% plot(outBuf);

% max(inBuf-outBuf)
%% End of example code

% If numValuesRotation > 0 -> rotate to left, otherwise right
function [bufOut] = jvx_fft_crotate_time(in, numValuesRotate, idxStartArg, idxStopArg)    
    
	FFTL = size(in,2);
    
    % Default still in Matlab notation
	if(nargin < 3)
		idxStartArg = 1;
	end
		
	if(nargin < 4)
		idxStopArg = FFTL;
    end

    % Switch to C notation here for phase computation
	idxStart = max(0, idxStartArg-1);
	idxStop = min(FFTL-1, idxStopArg-1);
	
	f_idx = [idxStart:idxStop];
	
%    f = numValuesRotate * [0:1/FFTL:1/2];
    f = numValuesRotate * f_idx / ((FFTL-1) * 2);

    arg = 2*pi*f;
    rrot = cos(arg);
    irot = sin(arg);
    bufIn = in(idxStart+1:idxStop+1);
    
    % Matlab array notation
    bufOutRe = real(bufIn) .* rrot - imag(bufIn) .* irot;
    bufOutIm = real(bufIn) .* irot + imag(bufIn) .* rrot;
    
    % 1) abs(bufIn) and abs(bufOut) should be identical
    % 2) angle(bufIn) + arg should be angle(bufsOut) - but modulo pi
    bufOut = bufOutRe + 1i*bufOutIm;
    