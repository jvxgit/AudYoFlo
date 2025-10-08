% Setup random genertor for seed 1
randn('seed', 1);

% input signal and ir
in = randn(1,1024);
ir = [zeros(1,10) 1 -0.4 2.1 -0.9 0.2 zeros(1,10)];

% Run c based fir filter
outFirFft = jvxFirFft(in, ir);

% Run reference filter
outFir = filter(ir, 1, in);

%======================================================
% OUTPUT
%======================================================

figure;
subplot(2,1,1);
plot(outFir); hold on; plot(outFirFft, 'r:');

subplot(2,1,2);
plot(outFir-outFirFft, 'r:');
