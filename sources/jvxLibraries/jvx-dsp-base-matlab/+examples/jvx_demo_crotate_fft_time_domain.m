function [] = jvx_demo_crotate_fft_time_domain(shiftDelay)

FFTL = 1024;
fs=16000;
DELAY = 0;

idx =[0:FFTL-1];
inBuf = [1 zeros(1,FFTL-1)];
shiftBuf = circshift(inBuf, DELAY);
[fftBufRe, fftBufIm] = jvxFFTWFft(shiftBuf);
fftBuf = fftBufRe + 1i*fftBufIm;

fmax = fs/2;
f = shiftDelay *[0:2*fmax/FFTL:fs/2];
arg = 2*pi*f/fs;
rotationVec = exp(1i*arg);
fftBuf1 = fftBuf .* rotationVec;

rrot = cos(arg);
irot = sin(arg);

fftBuf2Re = real(fftBuf) .* rrot - imag(fftBuf) .* irot;
fftBuf2Im = real(fftBuf) .* irot + imag(fftBuf) .* rrot;
fftBuf2 = fftBuf2Re + 1i*fftBuf2Im;

fftBuf3 = jvx_dsp_base.fft_ifft.jvx_fft_crotate_time(fftBuf, shiftDelay);

fftBuf1 = fftBuf3;
outBuf = jvxFFTWIfft(real(fftBuf1), imag(fftBuf1));

figure;
plot(outBuf);