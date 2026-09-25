function [in, out] = fDesignIir()

colors = {'k', 'g', 'y', 'm', 'g', 'r', 'b'};

cnt = 0;

% The default configuration is good for 48 -> 16 kHz resampling in high
% quality
order = 12;
design = 'Elliptic';
roll = -3;
type = 'LowPass';
fs = 48000;
fc = 7420;
% fc = 14000;
rip = 0.05;

if(0)

    % Confguration for more aliasing!!
    design = 'Elliptic';
    % roll = 5;
    order = 6;
    rip = 5;
    % fc = 14000;
end

if(0)
    design = 'ChebyshevII';
    roll = 60;
end

if(0)
    design = 'Butterworth';
end

if(0)
    design = 'Bessel';
end

figure;
hold on;
%for(jnx = 10)

[coeffs_sos, coeffs_g, hdl] = jvxIIRDesign('init', design, type, order, fs, fc, rip, roll);
[b, a] = sos2tf(coeffs_sos, coeffs_g);
%[h, x] = freqz(b,a, 1024, 48000);
[h, x] = freqz(coeffs_sos, 1024, 48000);

hmag = 20*log10(abs(h*coeffs_g));
clIdx = mod(cnt, size(colors,2)) + 1;
plot(x, hmag, colors{clIdx});
txt = ['Order = ' num2str(order) ' -- ripple = ' num2str(rip) ' -- roll = ' num2str(roll) '.'];
ll{cnt+1} = txt;

%cnt = cnt + 1;
%end

legend(ll);

if(1)
    % Run test filter

    % Noise input
    %in = rand(2, 128000);
    
    % Audio input from file
    %in = audioread('O:\audyoflo\audio-win64\release\runtime\wavs\music_stereo_48000Hz.wav');
    %in = in';

    % A pulse input
    in = zeros(2, 10000);
    in(1,1) = 1;
    in(2,1) = 1;

    figure;
    jvx_dsp_base.plot.plot_spectrogram(in(1,:), 0, 1, 32678, 0, 60, 48000);

    [out] = jvxIIRDesign('process', hdl, in, 128);
    
    figure;
    jvx_dsp_base.plot.plot_spectrogram(out(1,:), 0, 1, 32678, 0, 60, 48000);

    outds3 = out(:,1:3:end);
    outds3us1 = upsample(outds3(1,:), 3);
    outds3us2 = upsample(outds3(2,:), 3);
    outds3us = [outds3us1; outds3us2];

    figure;
    jvx_dsp_base.plot.plot_spectrogram(outds3us(1,:), 0, 1, 32678, 0, 60, 48000);

    [outdsus] = jvxIIRDesign('process', hdl, outds3us, 128);
    out = outdsus;

    figure;
    jvx_dsp_base.plot.plot_spectrogram(out(1,:), 0, 1, 32678, 0, 60, 48000);

end

jvxIIRDesign('terminate', hdl);