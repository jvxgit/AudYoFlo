% Allocate two signals
% Signal 1
sig1 = zeros(1, 1024);
sig1(1:7) = [3 2 1 0 -1 -2 -1];
%sig1 = randn(1, 1024);

% Signal 2
sig2 = zeros(1, 1024);
sig2(1:7) = [-3 2 -1 1 0 2 -1];
% sig2 = randn(1, 1024);

% Combine in a single complex signal
sig12 = sig1 + j*sig2;

% Run fft
fsig = fft(sig12);

% Separate result
fsig1r = real((fsig(2:512) + fliplr(fsig(514:end)))/2);
fsig1i = imag((fsig(2:512) - fliplr(fsig(514:end)))/2);
fsig1f = [real(fsig(1)) (fsig1r + 1j*fsig1i) real(fsig(513)) fliplr(conj(fsig1r + 1j*fsig1i))];

fsig2i = real((fsig(2:512) - fliplr(fsig(514:end)))/2);
fsig2r = imag((fsig(2:512) + fliplr(fsig(514:end)))/2);
fsig2f = [imag(fsig(1)) (fsig2r - 1j*fsig2i) imag(fsig(513)) fliplr(conj(fsig2r - 1j*fsig2i))];

% Compare the output
figure; 

fsig1o = fft(sig1);
dd1 = fsig1f - fsig1o;
fsig2o = fft(sig2);
dd2 = fsig2f - fsig2o;
subplot(2,1,1);
plot(abs(dd1)); 
subplot(2,1,2);
plot(abs(dd2));

