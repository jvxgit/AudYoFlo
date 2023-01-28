function [out] = jvx_fft(in)
    
    useFftw = false;
    global system_cfg;
    if(isstruct(system_cfg))
        if(isfield(system_cfg, 'fftw'))
            if(system_cfg.fftw)
                useFftw = true;
            end
        end
    end
    
    ll = size(in, 2);
    num = size(in, 1);
    out = zeros(num, ll/2+1);
    
    for(ind = 1:num)
        inV = in(ind,:);
        if(useFftw)
            [tre, tim] = jvxFFTWFft_m(in(ind,:));
            tmp = tre+1i*tim;
        else
            tmp = fft(inV);
            tmp = tmp(1:ll/2+1);
        end
        % delta = sum(abs(tmp-tmp2).^2)
        out(ind,:) = tmp;
    end