function [out, delay] = process_signal(in, fft_size, fshift, complex_weights, eq_delay, ovlp, ploto)

    if(size(complex_weights, 2) ~= fft_size/2+1)
        error('Invalid complex weights');
    end
    
    l_sig = size(in,2);
    num_frames = ceil(l_sig/fshift);
    l_sige = num_frames * fshift;
    
    ine = zeros(1, l_sige);
    ine(1:l_sig) = in;
    out = zeros(1, l_sig);
    
    if(ploto.plot_ir)
        jvx_dsp.viewer.jvx_show_ir_cplx_weights(complex_weights);
    end
    
    hdl_os = jvx_dsp.fast_conv_os.init(fft_size, fshift, eq_delay, ovlp);
    delay = eq_delay;
    
    idx_start = 0;

    for(ind = 1:num_frames)
        
        start_idx = idx_start + 1;
        stop_idx = idx_start + fshift;
        stop_idx = min(stop_idx, l_sige);
        
        % Single input frame
        one_frame = zeros(1,  fshift);
        one_frame(1:stop_idx-start_idx + 1) = ine(start_idx:stop_idx);
        
        [hdl_os, FFT_SIG] = jvx_dsp.fast_conv_os.add_frame_fft(hdl_os, one_frame);
        
        % Fast convolution
        FFT_OUT = FFT_SIG .* complex_weights;
        
        [hdl_os, ifft_out] = jvx_dsp.fast_conv_os.get_frame_proc(hdl_os, FFT_OUT);
                
        out(start_idx:stop_idx) = ifft_out;
        
        idx_start = idx_start + fshift;
        
        hdl_os = jvx_dsp.fast_conv_os.update_phase(hdl_os);
        
    end
    
    if(ploto.plot_out)

        figure;
        plot(in);
        hold on;
        plot(out, 'r:');
    end    