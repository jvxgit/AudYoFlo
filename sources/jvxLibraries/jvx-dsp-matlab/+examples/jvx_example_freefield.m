FS = 16000;
FFTSIZE = 8192;
C = 340;
FSHIFT = 512;
POS_X_SIGNAL = 1.0;
POS_Y_SIGNAL = 0.0;
POS_Z_SIGNAL = 0.0;

% Define mic positions:
%sig = [1 zeros(1, 10000)];
[in, fs] = audioread('../../../../wav/test.wav');
sig = in';
sig = sig(1,:);

hdl = jvx_dsp.jvx_freefield_simu.init(C, 2);

hdl = jvx_dsp.jvx_freefield_simu.set_position(hdl, true, -0.12, 0, 0, 1);
hdl = jvx_dsp.jvx_freefield_simu.set_position(hdl, true, 0.12, 0, 0, 2);

[hdl, outs] = jvx_dsp.jvx_freefield_simu.compute_signal( hdl, true, POS_X_SIGNAL, POS_Y_SIGNAL, POS_Z_SIGNAL, fs, FFTSIZE , FSHIFT, sig);

p = audioplayer(outs', fs);
play(p);

figure; 
plot(outs(1,:));
hold on;
plot(outs(2,:), 'r:');
legend({'Left ear', 'Right ear'});

