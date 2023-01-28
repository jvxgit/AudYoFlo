function [window, normalizationOverlapAdd] = rtp_ComputeWindow(M, FS, windowType)

n = 0:M-1; 

if(strcmp(windowType, 'Hann') == 1)
    window = 0.5*(1-cos(2*pi*n/(M-1))); 
    normalizationOverlapAdd = (M / 2 / FS);
elseif(strcmp(windowType, 'HalfHann') == 1)
    window = 0.5*(1-cos(2*pi*(n+M-1)/(2*M-2)));  
    normalizationOverlapAdd = 1;
elseif(strcmp(windowType, 'Hamming') == 1)
    window = 0.54-0.46 * cos(2*pi*n/(M-1)); 
    normalizationOverlapAdd = (M / 2 / FS);
elseif(strcmp(windowType, 'Blackman') == 1)
    window = 0.42 - 0.5 * cos(2*pi*n/(M-1)) + 0.08 * cos(4*pi*n/(M-1)); 
    normalizationOverlapAdd = (M / 2 / FS);
elseif(strcmp(windowType, 'Flattop-Hann') == 1)
    l_plus_ft = ceil((M/2)/FS)*FS;
    l = M - l_plus_ft;
    nn = 0:l-1;
    window = 0.5*(1-cos(2*pi*nn/(2*l-2))); 
    window = [window ones(1,(l_plus_ft-l)) fliplr(window)];
    numWins = ceil(M/FS);
    normalizationOverlapAdd = floor(numWins/2) + mod(numWins,2);
else
    error(['Undefined windowType ' windowType]);
end

% Normalization is the inverse here
normalizationOverlapAdd = 1/normalizationOverlapAdd;
    
   