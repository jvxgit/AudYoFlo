function [membytes, memwrds] = ayf_calculate_firfft_nout(Nfft, NChans )
    
    membytes = 0;
    memwrds = 0;

    if(nargin < 1)
        Nfft = 1024;
    end
    
    if(nargin < 2)
        NChans = 1;
    end

    sizefloat32 = 4;
    sizecplxfloat32 = 8;
    
    % Input fft
    membytes = membytes + Nfft * sizefloat32;
    membytes = membytes + (Nfft/2 +1) * sizecplxfloat32;

    % Output ifft
    membytes = membytes + (Nfft/2 +1) * sizecplxfloat32;
    membytes = membytes + Nfft * sizefloat32;

    % weights    
    membytes = membytes + (Nfft/2 +1) * sizecplxfloat32 * NChans;

    % DSP 32 bit words
    memwrds = membytes / 4;