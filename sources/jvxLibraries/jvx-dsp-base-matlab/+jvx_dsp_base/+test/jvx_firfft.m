function [] = jvx_firfft(nChannels)

if(nargin < 1)
    nChannels = 1;
end
if(nChannels < 1)
    nChannels = 1;
end

% Setup random genertor for seed 1
randn('seed', 1);

% input signal and ir
in = randn(1,1024);

ir = [zeros(1,10) 1 -0.4 2.1 -0.9 0.2 zeros(1,10)];
szFilt = size(ir,1);

for(idx = 2:nChannels)

    irx = [zeros(1,10) randn(1,5) zeros(1,10)];
    ir(idx,:) = irx;
end

% Run c based fir filter
outFirFft = jvxFirFft(in, ir);

% Run reference filter
outFirFilt = zeros(nChannels, size(in,2));
for(idx=1:nChannels)
    outFirFilt(idx,:) = filter(ir(idx,:), 1, in);
end

%======================================================
% OUTPUT
%======================================================

N = ceil(sqrt(nChannels));
M = ceil(nChannels/N);

figure;
for(idx=1:nChannels)
    subplot(N,M,idx);
    plot(outFirFft(idx,:)); hold on; plot(outFirFilt(idx,:), 'r:');
    delta = outFirFft(idx,:)-outFirFilt(idx,:);
    deltaMax = max(abs(delta));
    plot(delta, 'g.-');
    legend({['Output FFT - IR(' num2str(idx)], ['Output FIR - IR(' num2str(idx)], ['DeltaMax = ' num2str(deltaMax)]});

end

