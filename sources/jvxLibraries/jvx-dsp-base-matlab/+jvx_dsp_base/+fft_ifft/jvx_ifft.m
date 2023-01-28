function [out] = jvx_ifft(in)
    
    useFftw = false;
    global system_cfg;
    if(isstruct(system_cfg))
        if(isfield(system_cfg, 'fftw'))
            if(system_cfg.fftw)
                useFftw = true;
            end
        end
    end
    
    ll = size(in,2);
    num = size(in,1);
    llout = (ll - 1) * 2;
    
    out = zeros(num, llout);
    
    for(ind = 1: num)
        tmp = in(ind,:);
        
        if(useFftw)
            tmp = jvxFFTWIfft_m(real(tmp), imag(tmp));
        else
            tmp = [tmp conj(fliplr(tmp(2:llout/2)))];
            tmp = real(ifft(tmp));
        end
        out(ind,:) = tmp;
    end
    