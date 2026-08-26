% ========================================================================
% ========================================================================

disp('Design a high pass with an invertible Butterworth filter.');
fs = 16000;
fc = 100;
order = 3;
tp = 'high';
rinv = 0.995;

figure;
[b a] = jvx_dsp_base.filter.design_filt_inv_butter(fs, fc, order, tp, rinv);
freqz(b,a, 16384, fs);

in = zeros(1, 16384);
in(1) = 1;

% Filter notch
out1 = filter(b,a,in);

% Invert
out2 = filter(a,b,out1);

figure;
plot(in-out2);
legend({'Reconstruction Error - Filter & Inverse Filter Output'});

% ========================================================================

disp('Design a notch filter.');
f0 = 2000;
bwidth = 30;
notch_db = -35;

figure;
[b a c d] = jvx_dsp_base.filter.design_filt_inv_notch(fs, f0, bwidth, notch_db);
freqz(b,a, 16384, fs);

in = zeros(1, 16384);
in(1) = 1;

% Filter notch
out1 = filter(b,a,in);

% Invert
out2 = filter(a,b,out1);

figure;
plot(in-out2);
legend({'Reconstruction Error - Filter & Inverse Filter Output'});

% ========================================================================

disp('Design a lowpass/highpass filter.');

flow = 20;
fhigh = 100;
attenlow = -30;
attenhigh = 0;

figure;
[b a] = jvx_dsp_base.filter.design_filt_inv_lowpass(fs, flow, fhigh, attenlow, attenhigh);
freqz(b,a, 16384, fs);

in = zeros(1, 16384);
in(1) = 1;

% Filter notch
out1 = filter(b,a,in);

% Invert
out2 = filter(a,b,out1);

figure;
plot(in-out2);
legend({'Reconstruction Error - Filter & Inverse Filter Output'});

% ========================================================================
% ========================================================================
