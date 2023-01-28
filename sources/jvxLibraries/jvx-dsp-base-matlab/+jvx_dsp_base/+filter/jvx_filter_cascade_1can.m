%%
% Implementation of a filter which realizes a concatenation
% of filters of the same order in 1st canonic form. 
% For example, given a cascade of 2 3rd order IIR filters with
%
% b = [0.1301    0.3902    0.3902    0.1301] (Numerator part)
% a = [1.0000   -0.2881    0.3553   -0.0267] (Denominator part)
% we can filter twice the filter as
% bb = [0.1301    0.1301    0.3902    0.3902    0.1301    0.1301    0.3902    0.3902] 
% (<- Numerator coeff[0] as given and all other coefficients in reversed order!)
% aa = [ 1.0000   -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553   -0.2881]; 
%(<- Denominator coeff[0] as given but the other coefficients in reverse order!)
% by the following command sequence:
%
% states = zeros(1,6);
% [out, states] = jvx_filter_cascade_1can(2, 3, states, aa, bb, in);
%
% Input 1: Number sections
% Input 2: Order each section
% Input 3: States, must be a field of length <Input 1> * <Input 2>
% Input 4: Denominator coefficients
% Input 5: Numerator coefficients
% Input 6: Input signal (1xN)
% The function uses a linear buffer propagation and therefore does not rely on circular buffers.
% Example for demonstration:
%
% in = [1 zeros(1, 127)];
% b = [0.1301    0.3902    0.3902    0.1301];
% a = [1.0000   -0.2881    0.3553   -0.0267];
% out_orig = filter(b, a, filter(b, a, in));
%
% bb = [0.1301    0.1301    0.3902    0.3902    0.1301    0.1301    0.3902 0.3902];
% aa = [1.0000   -0.0267    0.3553   -0.2881    1.0000   -0.0267    0.3553 -0.2881];
% states = zeros(1,6);
% [out_new, states] = jvx_filter_cascade_1can(2, 3, states, aa, bb, in);
%
% figure; plot(out_orig); hold on; plot(out_new, 'r:');
%
% HK, Javox/IKS, 2017
function [out, states] = jvx_filter_cascade_1can(nSections, order, states, a, b, in)
         
     for(jnd = 1:size(in,2))
         
         accuin = in(jnd);
         cntF = 1;         
         cntLocal = 1;
         for(ind=1:nSections)
                  
            cntL = cntLocal;
            st = states(cntLocal);
            cntLocal = cntLocal + 1;
            
            accuout = b(cntF) * accuin + st;
            accuout = 1/a(cntF) * accuout;
            cntF = cntF + 1;
         
            memory = 0;
            for(ind=1:order-1)

                st = memory - a(cntF) * accuout + b(cntF) * accuin;
                cntF = cntF + 1;
                memory = states(cntLocal);
                states(cntLocal) = st;
                cntLocal = cntLocal + 1;
            end
         
             st = memory - a(cntF) * accuout + b(cntF) * accuin;
             cntF = cntF + 1;
             
            states(cntL) = st;
            accuin = accuout;
           
         end
        out(jnd) = accuin;
     end
