% Run the butterworth lowpass design implementation to verify functiponality
% HK, 2017, Javox/IKS

fs = 48000;
fc = 1000;
order = 5;

fq = fc/(fs/2);

% Butterworth filter design - the reference
[z, p, g] = butter(order, fq);
num1 = poly(z);
den1 = poly(p);
num1 = num1 * g;

% Butterworth filter design - the implementation under test
[num2, den2] = jvx_dsp.design.design_butterworth_lp(order, fc, fs);

% Comparison
num1-num2
den1-den2