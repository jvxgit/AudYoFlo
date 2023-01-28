function [hdl] = ifft(hdl, in)
    
    hdl.memory = jvx_dsp_base.fft_ifft.jvx_ifft(in);
end
