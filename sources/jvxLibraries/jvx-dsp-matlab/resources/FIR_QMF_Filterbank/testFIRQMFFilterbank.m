% Teste FIR QMF-Filterbank mit 32 Tap FIR Filter "D" aus [1]

% created      : 19.09.2012
% authors      : Andreas Friedrich and Heiner Loellmann (IND, RWTH Aachen)
% modified     : 

% Referenzen
% [1] J. D. Johnston, “A filter family designed for use in quadrature mirror
%     filter banks,” in Proc. ICASSP-1980, May 1980, pp. 291-294.

parFIRQMF =  init_firQMF_Filterbank( ); % initialisieren der States und Parameter

% Eingangssignal
inputSignal = zeros(16000,1);
inputSignal(1) = 1; % Dirac zum ermitteln der IR
delay = 0;
inputSignal = circshift(inputSignal, delay);

% Filtern

filteredSignal = []; 

sigLen = length(inputSignal);
framesize = 129;
idxStart = 1;

while idxStart <= sigLen     
    
    idxEnd = min(idxStart + framesize-1, sigLen);
    inputFrame = inputSignal(idxStart:idxEnd);

    % Analysefilterung
    [x, parFIRQMF] = firQMFAnalysisFilterbank(inputFrame, parFIRQMF);

    % Verarbeitung
    x(:,1) = 1*x(:,1);
    x(:,2) = 1*x(:,2);     
    
    % Synthesefilterung
    [y, parFIRQMF] = firQMFSynthesisFilterbank(x, parFIRQMF); 

    filteredSignal = [filteredSignal; y];
    
    idxStart = idxStart + framesize; 
      
end   



%% Plots

nfft = 1024;
wAxis = linspace(0, 2, nfft);
lineColors = colormap(lines);

Y = fft(filteredSignal, nfft);

% Plotte Ausgangsspektrum
figure(1)
clf
set(gcf, 'Name', 'QMF Filterbank - Spektrum Ausgangssingal', 'Numbertitle', 'Off')

ax1 = subplot(2,1,1);
plot(wAxis(1:nfft/2+1), 20*log10(abs(Y(1:nfft/2+1))), 'Color', lineColors(1, :),  'LineWidth', 1);
xlabel('Normierte Frequenz \Omega/\pi (rad)')
ylabel('Betrag (dB)')
title('Betrag Ausgangssignal')
xlim([0 1])
grid on

ax2 = subplot(2,1,2);
plot(wAxis(1:nfft/2+1), 180*unwrap(angle(Y(1:nfft/2+1)))/pi, 'Color', lineColors(1, :),  'LineWidth', 1);
xlabel('Normierte Frequenz \Omega/\pi (rad)')
ylabel('Phase (Grad)')
title('Phase Ausgangssignal')
xlim([0 1])
grid on

linkaxes([ax1 ax2], 'x')

% plotte Filterübertragungsfunktionen
figure(2)
clf
set(gcf, 'Name', 'QMF Filterbank - Betragsgänge Teilbandfilter', 'Numbertitle', 'Off')

ax1 = subplot(2,1,1);
hold on
title('Analysefilter')
plot(wAxis, 20*log10(abs(fft(parFIRQMF.filters.h0, nfft))), 'Color', lineColors(1, :),  'LineWidth', 1);
plot(wAxis, 20*log10(abs(fft(parFIRQMF.filters.h1, nfft))), 'Color', lineColors(2, :),  'LineWidth', 1);
xlim([0 1])
ylim([-100 10])
grid on
xlabel('Normierte Frequenz \Omega/\pi (rad)')
ylabel('Betrag (dB)')
legend(gca, 'string', {'$H_0(e^{^j\Omega})$', '$H_1(e^{j\Omega})$'},'interpreter','latex', 'fontsize', 14);

ax2 = subplot(2,1,2);
hold on
title('Synthesefilter')
plot(wAxis, 20*log10(abs(fft(parFIRQMF.filters.g0, nfft))), 'Color', lineColors(1, :),  'LineWidth', 1);
plot(wAxis, 20*log10(abs(fft(parFIRQMF.filters.g1, nfft))), 'Color', lineColors(2, :),  'LineWidth', 1);
xlim([0 1])
ylim([-100 10])
grid on
xlabel('Normierte Frequenz \Omega/\pi (rad)')
ylabel('Betrag (dB)')
legend(gca, 'string', {'$G_0(e^{^j\Omega})$', '$G_1(e^{j\Omega})$'},'interpreter','latex', 'fontsize', 14);

linkaxes([ax1 ax2], 'x')

