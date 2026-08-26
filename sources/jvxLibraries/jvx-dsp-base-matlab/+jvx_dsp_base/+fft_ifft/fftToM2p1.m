function [buf] = fftToM2p1(buf)

    % Make the fft size even
    if(mod(length(buf),2) == 1)        
        buf = [buf 0];
    end

    LL = length(buf);
    M2P1 = LL/2+1;

    buf = fft(buf);
    buf = buf(1:M2P1);
end