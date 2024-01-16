function [] = jvx_demo_reverse_fft()
    
    % There is a shift by one sample if doing the flip in the frequency
    % domain, http://www.claysturner.com/dsp/timereversal.pdf
    
    sig_fwd = [1:16] - 8;
    sig_bwd = fliplr(sig_fwd);
    % sig_bwd_flip = fliplr(sig_fwd);

    [fft_fwd_re, fft_fwd_im] = jvxFFTWFft(sig_fwd);
    sig_bwd_rec = jvxFFTWIfft(fft_fwd_re, -fft_fwd_im);
    
    % This shift is necessary since as shown in the lit reference
    sig_bwd_rec = circshift(sig_bwd_rec, -1);
    
    %[fft_bwd_re, fft_bwd_im] = jvxFFTWFft(sig_bwd);
    %sig_bwd_rec = jvxFFTWIfft(fft_bwd_re, -fft_bwd_im);
    
    figure; 
    plot(sig_bwd);
    hold on;
    plot(sig_bwd_rec, 'r:');
    