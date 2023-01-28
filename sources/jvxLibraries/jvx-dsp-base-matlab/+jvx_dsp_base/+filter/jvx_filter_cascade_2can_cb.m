%%
% Implementation of a filter which realizes a concatenation
% of filters of the same order in 2nd canonic form. 
% For example, given a cascade of 2 3rd order IIR filters with
%
% b = [0.1301    0.3902    0.3902    0.1301] (Numerator part)
% a = [1.0000   -0.2881    0.3553   -0.0267] (Denominator part)
% we can filter twice the filter as
% bb = [0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902
% 0.1301] (Numerator but in reverse order!)
% aa = [ -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553
% -0.2881    1.0000]; (Denominator but in reverse order!)
% by the following command sequence:
%
% states = zeros(1,6);
% [out, states, off] = jvx_filter_cascade_can2_cb(2, 3, states, aa, bb, in, 0);
%
% Input 1: Number Sections
% Input 2: Order each section
% Input 3: States, must be a field of length <Input 1> * <Input 2>
% Input 4: Denominator coefficients
% Input 5: Numerator coefficients
% Input 6: Input signal (1xN)
% Input 7: Offset in state buffer at the beginning.
% The function uses a modulo operation in the states buffer and is
% therefore specifically suited for architectures with support for hardware
% supported circular buffering.
% Example for demonstration:
%
% in = [1 zeros(1, 127)];
% b = [0.1301    0.3902    0.3902    0.1301];
% a = [1.0000   -0.2881    0.3553   -0.0267];
% out_orig = filter(b, a, filter(b, a, in));
%
% bb = [0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902 0.1301];
% aa = [ -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553 -0.2881    1.0000];
% states = zeros(1,6);
% [out_new, states, off] = jvx_filter_cascade_2can_cb(2, 3, states, aa, bb, in, 0);
%
% figure; plot(out_orig); hold on; plot(out_new, 'r:');
%
% HK, Javox/IKS, 2017
function [out, states, offset] = jvx_filter_cascade_2can_cb(nSections, order, states, a, b, in, offset)
     
    lStates = size(states,2);
    if(order*nSections ~= lStates)
        error('Wrong configuration, the state buffer, number of section and filter order must match');
    end
    
    cntLocal = offset;
    for(jnd = 1:size(in,2))
        accuin = in(jnd);
        
        cntFilt = 1;
        exchange = 0;
        for(ind=1:nSections)
        
            accuout = 0;
            st = states(cntLocal+1);
            states(cntLocal+1) = exchange;
            cntLocal = mod(cntLocal + 1, lStates);
    
            for(ind=0:order-2)
                accuin = accuin - a(cntFilt) * st;
                accuout = accuout + b(cntFilt) * st;
                cntFilt = cntFilt + 1;

                st = states(cntLocal+1);
                cntLocal = mod(cntLocal + 1, lStates);
            end
            
            accuin = accuin - a(cntFilt) * st;
            accuout = accuout + b(cntFilt) * st;
            cntFilt = cntFilt + 1;
            
            accuin = accuin * 1.0/a(cntFilt);% <- !!!!
            exchange = accuin;
            accuin = accuout + accuin * b(cntFilt);
            cntFilt = cntFilt + 1;
        end
        
        out(jnd) = accuin;  
        states(cntLocal+1) = exchange;
        cntLocal = mod(cntLocal + 1, lStates);
    end
    offset = cntLocal;
    