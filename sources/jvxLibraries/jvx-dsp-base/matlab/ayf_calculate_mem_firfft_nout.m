function [membytes, memwrds] = ayf_calculate_mem_firfft_nout(Nfft, NChansIn, NChansOutPerInChannel, bsize )
    
    membytes = 0;
    memwrds = 0;

    if(nargin < 1)
        Nfft = 1024;
    end
    
    if(nargin < 2)
        NChansIn = 1;
    end

    if(nargin < 3)
        NChansOutPerInChannel = 2;
    end

    disp(['NChansIn = ' num2str(NChansIn)]);
    disp(['NChansOutPerInChannel = ' num2str(NChansIn)]);
    
    NChansOut = NChansIn * NChansOutPerInChannel;
    disp(['NChansOut = ' num2str(NChansOut)]);

    sizefloat32 = 4;
    sizecplxfloat32 = 8;
    
    % ====================================================================

    % Input fft
    % One Buffer for each frame
    nSeg = NChansIn * Nfft * sizefloat32;
    membytes = membytes + nSeg;

    % The samme complex buffers for all inputs
    nSeg = (Nfft/2 +1) * sizecplxfloat32;
    membytes = membytes + nSeg;

    % ====================================================================

    % Output ifft
    % Ifft output for one channel
    nSeg = Nfft * sizefloat32;
    membytes = membytes + nSeg;

    % Ifft input for one channel
    nSeg = (Nfft/2 +1) * sizecplxfloat32;
    membytes = membytes + nSeg;

    % ====================================================================

    % weights
    nSeg = (Nfft/2 +1) * sizecplxfloat32 * (NChansOut);
    membytes = membytes + nSeg;

    % ====================================================================
    % Interleaved to non-interleaved
    nSeg = bsize * sizefloat32 *(NChansIn + NChansIn); % The second part is the number of input channels - since we shrink down the data in number of channels!
    membytes = membytes + nSeg;

    % DSP 32 bit words
    memwrds = membytes / 4;

    disp(['Required memory <bytes> = ' num2str(membytes)]);
    disp(['Required memory <words> = ' num2str(memwrds)])
