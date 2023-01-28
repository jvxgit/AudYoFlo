function [hdl, out] = fft(hdl)
    
    out = jvx_dsp_base.fft_ifft.jvx_fft(hdl.memory);
    
    % hdl->circBuffer.idxRead = (hdl->circBuffer.idxRead + hdl->circBuffer.fHeight)%hdl->circBuffer.length;
	% hdl->circBuffer.fHeight = 0;
    hdl.read = mod(hdl.read + hdl.fillheight, hdl.M);
    hdl.fillheight = 0;
end
