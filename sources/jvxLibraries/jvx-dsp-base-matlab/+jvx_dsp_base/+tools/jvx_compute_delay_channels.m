function [] = jvx_compute_delay_channels(fname, show_win_length_2, art_delay)

    [sig, fs] = audioread(fname);
    chan1 = sig(:,1)';
    chan2 = sig(:,2)';
    
    if(art_delay)
        chan2 = [zeros(1, art_delay) chan2(1:end-art_delay)];
    end
    
    xc = xcorr(chan1, chan2);
    lxc = length(xc);
    zero_ref_idx = (length(xc)-1)/2+1;
    
    max_xc = find(abs(xc) == max(abs(xc)));
    max_idx = max_xc(1);
    delay = max_idx - zero_ref_idx;
    
    idx_left = max_idx - show_win_length_2;
    idx_right = max_idx + show_win_length_2;
    y = xc(idx_left:idx_right);
    x = [idx_left - zero_ref_idx:idx_right - zero_ref_idx];
    
    disp(['Delay channel 2 in relation to channel 1 (neg means that channel 2 is later) => ' num2str(delay)]);
    figure;
    subplot(2,1,1);
    plot(chan1, 'g');
    hold on;
    plot(chan2, 'r:');
    legend({'Channel #1', 'Channel #2'});
    
    subplot(2,1,2);
    plot(x,y);
    legend('XCorr around delay');

