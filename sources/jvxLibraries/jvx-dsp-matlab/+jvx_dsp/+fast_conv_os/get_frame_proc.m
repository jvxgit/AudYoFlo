function [hdl, out] = get_frame_proc(hdl, complex_buffer)
    
    out_ifft = jvx_dsp_base.fft_ifft.jvx_ifft(complex_buffer);
    
    idx_ifft_start = hdl.phase_start;
    idx_ifft_start = mod(idx_ifft_start + hdl.fft_size - hdl.read_shift, hdl.fft_size);
    idx_ifft_start_olp = mod(idx_ifft_start + hdl.fft_size - hdl.read_ovlp, hdl.fft_size);
    
    num_samples = hdl.read_ovlp + hdl.fshift;
    
    ll1 = min(hdl.fft_size - idx_ifft_start_olp, num_samples);
    ll2 = num_samples - ll1;
    
    out_ovlp(1:ll1) = out_ifft(idx_ifft_start_olp+1:idx_ifft_start_olp+ll1);
    out_ovlp(ll1+1:ll1+ll2) = out_ifft(1:ll2);
    
    out(1:hdl.read_ovlp) = hdl.win_ovlp.*out_ovlp(1:hdl.read_ovlp) + (1-hdl.win_ovlp) .*hdl.ovlp_save;
    
    out(hdl.read_ovlp+1:hdl.fshift) = out_ovlp(hdl.read_ovlp+1:hdl.fshift);
    hdl.ovl_save = out_ovlp(1+hdl.fshift:end);
    
    
    