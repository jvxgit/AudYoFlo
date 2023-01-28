function [hdl] = init(hdl)
	if(isempty(hdl.prv)) 
		
        hdl.prv.init_cpy = hdl.init;
        hdl.prv.derived_cpy = hdl.derived;
        
		% Compute the derived parameters
		[hdl] = jvx_dsp_base.firfft.update(hdl, 2, true);
	
		[hdl.prv.ram.in] = zeros(1, hdl.prv.derived_cpy.szFft);
		
		if (hdl.prv.init_cpy.type == 1)
		
			hdl.prv.ram.outoffset = hdl.prv.derived_cpy.delay;

			% Copy such that we get a zero phase filter
			hdl.prv.ram.in(1) = hdl.prv.init_cpy.fir(hdl.prv.ram.outoffset+1);
            hdl.prv.ram.in(1+1:1+hdl.prv.ram.outoffset) = hdl.prv.init_cpy.fir(hdl.prv.ram.outoffset + 1 +1: hdl.prv.ram.outoffset + 1 + hdl.prv.ram.outoffset);
			hdl.prv.ram.in(end-hdl.prv.ram.outoffset+1:end) = hdl.prv.init_cpy.fir(1:hdl.prv.ram.outoffset);
		else

			hdl.prv.ram.outoffset = 0;
			hdl.prv.ram.in(1:size(hdl.prv.init_cpy.fir,2)) = hdl.prv.init_cpy.fir;
		end

		hdl.sync.firW = jvx_dsp_base.fft_ifft.jvx_fft_c(hdl.prv.ram.in);

		hdl.prv.ram.phase = 0;
        hdl.prv.ram.normFactor = 1.0 / hdl.prv.derived_cpy.szFft;
        
        % Reset in buffer
        hdl.prv.ram.in = hdl.prv.ram.in * 0;
	else
		error('Init has been called before');
	end
