function [y, par] = firQMFSynthesisFilterbank(x, par)
%FIRQMFSYNTHESISFILTERBANK(x, sates, g0, g1) 
%
% filters signals in the columns of matrix "x"
% with synthesis part of two-channel FIR QMF Filterbank. First column of x
% is treated as the lowpass-subband signal and the second column as highpass
% channel output of a two channel QMF analysis filterbank.
%
% The struct "par" has to contain the following parameters:
%
% par.states.g0: filter states of synthesis lowpass
% par.states.g1: filter states of synthesis highpass
%
% par.states.Rd: state of upsampler: '0' start with sampling at firsr
%                                        sample of input
%                                    '1' start with inserting zero   
%
% par.filters.g0: coefficients of synthesis lowpass filter
% par.filters.g1: coefficients of synthesis highpass filter
%
% The reconstructed output signal is returned in y.

% created      : 19.09.2012
% author       : Andreas Friedrich
% modified     : 

% Upsampling factor
R = 2;

% input signal length
lIn = size(x,1);

% upsampling
xUp = zeros( R*lIn, 2 ); 

if par.states.Ru
    xUp(2:R:end, 1) = [0; x(2:end,1)]; 
    xUp(2:R:end, 2) = [0; x(2:end,2)];
else
    xUp(1:R:end, 1) = x(:,1); 
    xUp(1:R:end, 2) = x(:,2);
end

if mod(size(x,1), 2)
    par.states.Ru = ~par.states.Ru;
end

% synthesis filtering
[xUp(:,1), par.states.g0] = filter(par.filters.g0, 1, xUp(:,1), par.states.g0);
[xUp(:,2), par.states.g1] = filter(par.filters.g1, 1, xUp(:,2), par.states.g1);  

% output
y = sum(xUp,2);

    

end % function
