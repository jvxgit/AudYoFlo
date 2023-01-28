%% =============================================
%% =============================================
function [out_pdm] = jvx_sigmadelta_pdm(in, fs_inout, doDither, ditherFac)
    
    ll = size(in, 2);
    out_pdm = zeros(1, ll);
    enM1 = 0;
    
    for(ind=1:ll)
        
        xn = in(ind);
        if(doDither)
            xn = xn + randn() * ditherFac;
        end
        xpn = xn - enM1;
        
        % +1 if xn >= 0
        % -1 if xn < 0
        if(xpn == 0)
            xpn = eps;
        end
        
        yn = xpn/abs(xpn);
        out_pdm(ind) = yn;
        enM1 = yn -xpn;
    end
        
