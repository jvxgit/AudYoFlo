function [out] = jvx_ifft_c(in)
    ll = size(in,2);
    num = size(in,1);
    llout = (ll - 1) * 2;
    
    out = zeros(num, llout);
    
    for(ind = 1: num)
        tmp = in(ind,:);
        tmp = [tmp conj(fliplr(tmp(2:llout/2)))];
        out(ind,:) = real(ifft(tmp)) * llout;
    end