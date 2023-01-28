function [hdl, out] = process(hdl, in)
	
	if(~isempty(hdl.prv))
		ll1 = min(hdl.prv.derived_cpy.szFft - hdl.prv.ram.phase, hdl.prv.init_cpy.bsize);
		ll2 = hdl.prv.init_cpy.bsize - ll1;
	
		if(ll1)
			hdl.prv.ram.in(hdl.prv.ram.phase + 1: hdl.prv.ram.phase + ll1) = in(1:ll1);
		end
		if(ll2)
			hdl.prv.ram.in(1:ll2) = in(ll1 + 1:ll1 + ll2);
		end

		tmp = jvx_dsp_base.fft_ifft.jvx_fft_c(hdl.prv.ram.in);
		outphase = mod((hdl.prv.ram.phase + hdl.prv.derived_cpy.szFft - hdl.prv.ram.outoffset), hdl.prv.derived_cpy.szFft);
	
		% Process 
        switch (hdl.prv.init_cpy.type)
            case 1
                tmp = tmp .* real(hdl.sync.firW);
                
                ll1 = min(hdl.prv.derived_cpy.szFft - outphase, hdl.prv.init_cpy.bsize);
                ll2 = hdl.prv.init_cpy.bsize - ll1;
                
            otherwise
                
                tmp = tmp .* hdl.sync.firW;
                
        end
        
		tmp = jvx_dsp_base.fft_ifft.jvx_ifft_c(tmp);

		if(ll1)
			out(1:ll1) = tmp(outphase+1:outphase+ll1) * hdl.prv.ram.normFactor;
		end
		if(ll2)
			out(1+ll1:ll1+ll2) = tmp(1:ll2) * hdl.prv.ram.normFactor;
		end

		hdl.prv.ram.phase = mod((hdl.prv.ram.phase + hdl.prv.init_cpy.bsize), hdl.prv.derived_cpy.szFft);
	else
		error('Handle not ready.');
	end
end

