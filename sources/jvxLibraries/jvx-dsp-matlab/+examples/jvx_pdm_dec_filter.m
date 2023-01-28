%% This script demonstrates the difference between a CIC decoder filter and linear phase FIR decoder filter

% Configure the runtime parameters

% Design fir filter such that it is symmetric around edge frequency
symmetric_fir = false;

% Filter length of FIR filter in downsampled domain
ppfilterlength = 32;

% Rate in downsampled domain
fs0 = 192000;

% Decimation factor
os = 16;

% Number of stages CIC decoder filter
N = 4;

% Some FIR filter design parameters
delta_filter = 0.4*1/2/os;

% ===================================================
% ===================================================

fs = fs0 * os;
in = [1 zeros(1,1023)];

% + + + + + + + + + + + + + + + + + + + + + + +
% Design CIC, decimation not active (for displaying)
% + + + + + + + + + + + + + + + + + + + + + + +

[hdl1] = jvx_dsp.pdm.jvx_cic_dec_init(1, os, N);
[hdl1, out1, tt1] = jvx_dsp.pdm.jvx_cic_dec_process(hdl1, in);

%[hdl2] = jvx_cic_dec_init(os, 1, N);
%[hdl2, out2, tt2] = jvx_cic_dec_process(hdl2, in);

% + + + + + + + + + + + + + + + + + + + + + + +
% Design FIR demination filter
% + + + + + + + + + + + + + + + + + + + + + + +

% Frequency band edges
if(symmetric_fir)
    f = [0 1/os - delta_filter/2 1/os+ delta_filter/2 1];         
else
    f = [0 1/os - delta_filter 1/os 1];        
end

% Desired amplitudes
a = [1  1  0 0];           

% Compue effective filter length
efffilterlength = os*ppfilterlength; 

out3 = firpm(efffilterlength,f,a);

% + + + + + + + + + + + + + + + + + + + + + + +
% + + + + + + + + + + + + + + + + + + + + + + +
% Beginn analysis
% + + + + + + + + + + + + + + + + + + + + + + +
% + + + + + + + + + + + + + + + + + + + + + + +

[Hff1 Fff1] = freqz(out1, 1, 1024, fs);
Hff1_s = 10*log10(Hff1.*conj(Hff1));
Hff1_s = Hff1_s';
Fff1_s = Fff1';
Hff1_s = [fliplr(Hff1_s(2:end)) Hff1_s];
Fff1_s = [-fliplr(Fff1_s(2:end)) Fff1_s];
Fff1_s_s = Fff1_s + fs0;

    
[Hff2 Fff2] = freqz(out3, 1, 1024, fs);
Hff2_s = 10*log10(Hff2.*conj(Hff2));
Hff2_s = Hff2_s';
Fff2_s = Fff2';
Hff2_s = [fliplr(Hff2_s(2:end)) Hff2_s];
Fff2_s = [-fliplr(Fff2_s(2:end)) Fff2_s];
Fff2_s_s = Fff2_s + fs0;

% + + + + + + + + + + + + + + + + + + + + + + +
% + + + + + + + + + + + + + + + + + + + + + + +
% Plot the results
% + + + + + + + + + + + + + + + + + + + + + + +
% + + + + + + + + + + + + + + + + + + + + + + +
figure;

subplot(1,2,1);
plot(Fff1_s, Hff1_s );
hold on;
plot(Fff1_s_s, Hff1_s, 'r:' );
xlim([0 fs0/2]);
ylim([-100 10]);
subplot(1,2,2);

plot(Fff2_s, Hff2_s );
hold on;
plot(Fff2_s_s, Hff2_s, 'r:' );
xlim([0 fs0/2]);
ylim([-100 10]);


