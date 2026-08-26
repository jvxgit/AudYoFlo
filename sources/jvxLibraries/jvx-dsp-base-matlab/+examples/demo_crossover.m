fs = 16000;
fc = 380;

[bLP, aLP, bHP, aHP] = jvx_design_crossover(fs, fc);

Nfft = 32768;

[Hlp,f] = freqz(bLP,aLP,Nfft,fs);
Hhp = freqz(bHP,aHP,Nfft,fs);

figure;
semilogx(f,20*log10(abs(Hlp)));
hold on;
semilogx(f,20*log10(abs(Hhp)));
grid on;
xlim([20 20000]);
ylim([-100 5]);

xlabel('Frequency [Hz]');
ylabel('Magnitude [dB]');
legend('Lowpass','Highpass');

