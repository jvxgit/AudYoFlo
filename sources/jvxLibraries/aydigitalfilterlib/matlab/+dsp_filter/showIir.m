function [] = showIir(coeffs_sos, coeffs_g, txtLeg,fs)
    colors = {'k', 'g', 'y', 'm', 'g', 'r', 'b'};

    cnt = 1;
    [b, a] = sos2tf(coeffs_sos, coeffs_g);
    %[h, x] = freqz(b,a, 1024, 48000);
    [h, x] = freqz(coeffs_sos, 1024, fs);

    figure;
    hmag = 20*log10(abs(h*coeffs_g));
    clIdx = mod(cnt, size(colors,2)) + 1;
    plot(x, hmag, colors{clIdx});
    txt = txtLeg;
    ll{cnt+1} = txt;