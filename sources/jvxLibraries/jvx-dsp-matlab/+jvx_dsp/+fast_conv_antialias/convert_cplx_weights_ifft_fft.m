function [hdl, complex_weights, ir_shift, ir_equivdelay] = convert_cplx_weights_ifft_fft(hdl, complex_weights, plotme)
if(nargin < 3)
    plotme = false;
end
ir_uc = jvx_dsp_base.fft_ifft.jvx_ifft(complex_weights);
ir = ir_uc * 0;
ir(1:ceil(hdl.l_ir/2)) = ir_uc(1:ceil(hdl.l_ir/2)) .* hdl.win_pos;
ir(hdl.fft_size - (floor(hdl.l_ir/2)-1):end) = ir_uc(hdl.fft_size - (floor(hdl.l_ir/2)-1):end) .* hdl.win_neg;
complex_weights =  jvx_dsp_base.fft_ifft.jvx_fft(ir);
if(nargout >= 3)
    ir_shift = circshift(ir, [0 floor(hdl.l_ir/2)]);
end
if(nargout >= 4)
    ir_equivdelay = floor(hdl.l_ir/2);
end
if(plotme)
    figure; plot(circshift(ir_uc, [0 ir_equivdelay]));hold on; plot(ir_shift,'r:');
end
