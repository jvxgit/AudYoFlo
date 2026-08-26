function [buf] = ifftFromM2p1(buf)

    M2P1 = length(buf);
    LL =(M2P1 - 1) * 2;
    buf = resize(buf, LL);
    buf(M2P1+1:end) = fliplr(conj(buf(2:M2P1-1)));
    buf = real(ifft(buf));
end