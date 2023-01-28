function [y, par] = firQMFAnalysisFilterbank(x, par)
%FIRQMFANALYSISFILTERBANK(x, sates, h0, h1) 
%
% filters input Signal "x" with analysis-part of
% two-channel FIR QMF Filterbank and returns the resulting subband signals,
% downsampled by factor 2, in the columns of matrix "y". The first column
% corresponds to the lowpass channel output, second column contains the 
% output of the highpass channel respectively.
%
% The struct "par" has to contain the following parameters:
%
% par.states.h0: filter states of analysis lowpass
% par.states.h1: filter states of analysis highpass
% par.states.Rd: state of downsampler: '0' start with sampling at first
%                                          sample of x
%                                      '1' start sampling at second sample
%                                          of x
%                                  
%
% par.filters.h0: coefficients of analysis lowpass filter 
% par.filters.h1: coefficients of analysis highpass filter


% created      : 19.09.2012
% author       : Andreas Friedrich
% modified     : 


% Downsampling factor
R = 2;

% initialize output signals
y = zeros(ceil(length(x)/R),2);

% Analysis filtering
[x0, par.states.h0] = filter(par.filters.h0, 1, x, par.states.h0);
[x1, par.states.h1] = filter(par.filters.h1, 1, x, par.states.h1); 

x0 = x0(:);
x1 = x1(:);

% Downsampling
if par.states.Rd    
    y(:,1) = [0; x0(2:R:end)];
    y(:,2) = [0; x1(2:R:end)];
else
    y(:,1) = x0(1:R:end);
    y(:,2) = x1(1:R:end);
end

if mod(length(x), 2)
    par.states.Rd = ~par.states.Rd;
else

end % function

