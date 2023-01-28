function [hdl] = initCfg()
	hdl.init.bsize = 128;
	hdl.init.fir = [1];
	hdl.init.type = 1; % 0: unknown, 1: symmetric FIR, 2: plain fft mult
    
    hdl.prv = [];
    
	% Compute the derived parameters
	hdl = jvx_dsp_base.firfft.update(hdl, 2, true);

	