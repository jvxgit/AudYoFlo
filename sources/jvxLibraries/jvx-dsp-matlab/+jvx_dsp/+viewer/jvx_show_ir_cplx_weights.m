function [ff] = jvx_show_ir_cplx_weights(complex_weights)
    ff = figure; 
    ir_td = jvx_dsp_base.fft_ifft.jvx_ifft(complex_weights);
    plot(ir_td);
    
