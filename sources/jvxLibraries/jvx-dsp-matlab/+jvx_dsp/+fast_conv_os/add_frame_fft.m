function [hdl, out] = add_frame_fft(hdl, one_frame)
        
fshift = size(one_frame,2);
idx_fft_start = hdl.phase_start;
ll1 = min((hdl.fft_size - idx_fft_start), fshift);
ll2 = fshift - ll1;
        
hdl.sig_buffer(idx_fft_start + 1:idx_fft_start + ll1) = one_frame(1:ll1);
hdl.sig_buffer(1:ll2) = one_frame(ll1+1:end);
        
 out = jvx_dsp_base.fft_ifft.jvx_fft(hdl.sig_buffer);
