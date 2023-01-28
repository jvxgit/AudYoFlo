function [out, handle] = jvx_variantIII_process(in, bSizeOut, handle)
    
handle.statesLagrangeUpsampled(1:handle.lagrange_width-1) = ...
    handle.statesLagrangeUpsampled(end-(handle.lagrange_width-1)+1:end);

% Already produce the output with a specific offset
idxOut = handle.lagrange_width -1;

% Create empty output buffers
out = zeros(1, bSizeOut);
    
% Step I: Produce oversampled output:
    
divC = 1/handle.den.coeffs(1);
    
% Loop over input samples
for(ind = 0:handle.buffersizeIn-1)
        
    % Set input and output accu
    accuIn = in(ind + 1);
    accuOut = 0;
        
    % Loop over one non-zero and handle.oversampling-1 zero samples
    for(ind2 = 0:handle.oversamplingFactor-1)
            
        % First write output of filter
        accuOut = (handle.statesIir(1) + handle.num.coeffs(1) * accuIn)*divC;
        
        % Consider the states
        for(ind3 = 0:handle.filterOrder-2)
            handle.statesIir(ind3+1) = handle.statesIir(ind3+2) + ...
                accuIn * handle.num.coeffs(ind3+2) - ...
                accuOut * handle.den.coeffs(ind3+2);
        end
        
        % The last state is fed only from input and output sample
        handle.statesIir((handle.filterOrder-1)+1) = ...
            accuIn * handle.num.coeffs(handle.filterOrder+1) - ...
            accuOut * handle.den.coeffs(handle.filterOrder+1);
        
        % For all except the first sample, set input to zero
        accuIn = 0;
        
        handle.statesLagrangeUpsampled(idxOut + 1) = accuOut * handle.oversamplingFactor;
        
        % Update the output counter
        idxOut = idxOut + 1;
    end
end

% If we are here, we have computed the upsampled version of the input
% Next, procede with the Lagrange interpolation

% Loop over all output samples
for(ind=0:bSizeOut-1)
    offsetInFrame = (ind) * (handle.buffersizeIn * handle.oversamplingFactor) /bSizeOut;
    idxOffset = floor(offsetInFrame);
    alpha = offsetInFrame - idxOffset;
   
    % Reset accu
    accuOut = 0;
    for(ind2 = 0:handle.lagrange_width-1)
        % Compute and apply Lagrange coefficients
        coeff = 1;
        for(ind3 = 0:(handle.lagrange_width-2))
            coeff = coeff * (alpha - handle.lagrange.rootsL(ind2+1, ind3+1));
        end
        coeff = coeff * handle.lagrange.inv_denomL(ind2+1);
        accuOut = accuOut + coeff * handle.statesLagrangeUpsampled(idxOffset + ind2 + 1);
    end
    out(ind+1) = accuOut;
end

