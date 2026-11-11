function [in, out] = fDesignIir(targetDesign)

if(nargin == 0)
    targetDesign = 'JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_HIGH_QUALITY';
end

colors = {'k', 'g', 'y', 'm', 'g', 'r', 'b'};

cnt = 0;

% Parameter for design:
%
% Butterworth: order, fs, fc
% Elliptic: order, fs, fc, rippleDb, stopDbRoll - stopDbRoll: [-4..2]
% Cheby I: order, fs, fc, rippleDb
% Cheby II: order, fs, fc, stopDbRoll, stopDbRoll: 60 

% The default configuration is good for 48 -> 16 kHz resampling in high
% quality
fs = 48000;
type = 'LowPass';
Up = 1;
Down = 3;
plotSpecGram = false;

[order, design, stopDbRoll, freqFac, rip, txtLeg] = constraintDesign(targetDesign);

% Derive edge frequency
fc = freqFac * fs/2 * Up/Down;

[coeffs_sos, coeffs_g, hdl] = jvxIIRDesign('init', design, type, order, fs, fc, rip, stopDbRoll);
dsp_filter.showIir(coeffs_sos, coeffs_g, txtLeg, fs);

% Run test filter

% Noise input
%in = rand(2, 128000);

% Audio input from file
%in = audioread('O:\audyoflo\audio-win64\release\runtime\wavs\music_stereo_48000Hz.wav');
%in = in';

% A pulse input
in = randn(2, 100000);
in(1,1) = 1;
in(2,1) = 1;

[out] = jvxIIRDesign('process', hdl, in, 128);

outds3 = out(:,1:3:end);
outds3us1 = upsample(outds3(1,:), 3);
outds3us2 = upsample(outds3(2,:), 3);
outds3us = [outds3us1; outds3us2];
[outdsus] = jvxIIRDesign('process', hdl, outds3us, 128);
out = outdsus;

 if(plotSpecGram)
    figure;
    jvx_dsp_base.plot.plot_spectrogram(in(1,:), 0, 1, 32678, 0, 60, 48000);

    figure;
    jvx_dsp_base.plot.plot_spectrogram(out(1,:), 0, 1, 32678, 0, 60, 48000);

    figure;
    jvx_dsp_base.plot.plot_spectrogram(outds3us(1,:), 0, 1, 32678, 0, 60, 48000);

    figure;
    jvx_dsp_base.plot.plot_spectrogram(out(1,:), 0, 1, 32678, 0, 60, 48000);

end

jvxIIRDesign('terminate', hdl);
end

function [order, design, stopDbRoll, freqFac, rip, txtLeg] = constraintDesign(targetDesign)
    
    switch(targetDesign)

        % Butterworth: order, fs, fc
        case 'JVX_IIR_RESAMPLER_PRESET_BUTTER_LOW_QUALITY'
            order = 6;
            design = 'Butterworth';
            stopDbRoll = 0;
            freqFac = 0.6;
            rip = 0.0;
            
        case 'JVX_IIR_RESAMPLER_PRESET_BUTTER_MEDIUM_QUALITY'
            
            order = 9;
            design = 'Butterworth';
            stopDbRoll = 0;
            freqFac = 0.68;
            rip = 0.0;

        case 'JVX_IIR_RESAMPLER_PRESET_BUTTER_HIGH_QUALITY'

            order = 12;
            design = 'Butterworth';
            stopDbRoll = 0;
            freqFac = 0.75;
            rip = 0.00;

            % Elliptic: order, fs, fc, rippleDb, stopDbRoll - stopDbRoll: [-4..2]
        case 'JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_LOW_QUALITY'

            order = 6;
            design = 'Elliptic';
            stopDbRoll = -3;
            freqFac = 0.9313;
            rip = 0.05;

        case 'JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_MEDIUM_QUALITY'

            order = 8;
            design = 'Elliptic';
            stopDbRoll = -3;
            freqFac = 0.9313;
            rip = 0.05;

        case 'JVX_IIR_RESAMPLER_PRESET_ELLIPTIC_HIGH_QUALITY'

            order = 12;
            design = 'Elliptic';
            stopDbRoll = -3;
            freqFac = 0.9313;
            rip = 0.05;

            % Cheby I: order, fs, fc, rippleDb
        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYI_LOW_QUALITY'

            order = 6;
            design = 'ChebyshevI';
            stopDbRoll = 40;
            freqFac = 0.75;
            rip = 0.25;

        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYI_MEDIUM_QUALITY'

            order = 9;
            design = 'ChebyshevI';
            stopDbRoll = 40;
            freqFac = 0.83;
            rip = 0.05;

        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYI_HIGH_QUALITY'

            order = 12;
            design = 'ChebyshevI';
            stopDbRoll = 50;
            freqFac = 0.89;
            rip = 0.05;

            % Cheby II: order, fs, fc, stopDbRoll, stopDbRoll: 60 
        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYII_LOW_QUALITY'

            order = 6;
            design = 'ChebyshevII';
            stopDbRoll = 30;
            freqFac = 1;
            rip = 0.05;

        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYII_MEDIUM_QUALITY'

            order = 9;
            design = 'ChebyshevII';
            stopDbRoll = 40;
            freqFac = 1;
            rip = 0.05;

        case 'JVX_IIR_RESAMPLER_PRESET_CHEBYII_HIGH_QUALITY'

            order = 12;
            design = 'ChebyshevII';
            stopDbRoll = 50;
            freqFac = 1;
            rip = 0.05;

        otherwise
            order = 12;
            design = 'Elliptic';
            stopDbRoll = -3;
            freqFac = 0.9313;
            rip = 0.05;
    end

    txtLeg = ['Design = ' design ' -- Order = ' num2str(order) ' -- ripple = ' num2str(rip) ' -- roll = ' num2str(stopDbRoll) '.'];

if(0)

    % Confguration for more aliasing!!
    design = 'Elliptic';
    % roll = 5;
    order = 6;
    rip = 1;
    % fc = 14000;
end

if(0)
    design = 'ChebyshevII';
    roll = 60;
end

if(0)
    design = 'Butterworth';
    order = 6;
    fc = 7000;
end

if(0)
    design = 'Bessel';
end
end