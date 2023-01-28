function [out, handle] = jvx_fixed_iir_process(in, handle)
    
% Create empty output buffers
out = zeros(1, handle.buffersizeOut);
    
% Step I: Produce oversampled output:
    
divC = 1/handle.den.coeffs(1);
    
% Provide the target when to pipe out the next output sample
nextIdxOut = 0;
cntOsmpl = 0;
idxOut = 0;

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
        
        if(nextIdxOut == cntOsmpl)
            out(idxOut+1) = accuOut * handle.oversamplingFactor;
            idxOut = idxOut + 1;
            nextIdxOut = nextIdxOut + handle.downsamplingFactor;    
        end
        
        % Update the output counter
        cntOsmpl = cntOsmpl + 1;
    end
end

