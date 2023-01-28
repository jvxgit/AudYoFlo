function [out, handle] = jvx_variantI_process(in, bSizeOut, handle)
    
% This is the memove to copy new buffers into state buffer. A circular
% buffer would be more complex on an architecture without circular
% addressing support
handle.states(1:handle.lStates-handle.buffersizeIn) = ...
    handle.states(end - (handle.lStates-handle.buffersizeIn) +1:end);
handle.states((handle.lStates-handle.buffersizeIn)+1:end) = in;
    
handle.statesLagrangeUpsampled(1:handle.lagrange_width-1) = ...
    handle.statesLagrangeUpsampled(end-(handle.lagrange_width-1)+1:end);

% Already produce the output with a specific offset
idxOut = handle.lagrange_width -1;

% Create empty output buffers
out = zeros(1, bSizeOut);
    
% Step I: Produce oversampled output:
inFrameOffset = 0;
idxIn_offset = handle.lFilter-1;
        
for(ind = 0:(handle.buffersizeIn * handle.oversamplingFactor -1))
                        
    accu = 0.0;
    cntS = idxIn_offset;
    cntC = inFrameOffset;
            
    for(ind2 = 1:handle.lFilter)
        op1 = handle.states(cntS + 1);
        op2 = handle.impRespZeros(cntC + 1);
        accu = accu + op1 * op2;
        cntC = cntC + handle.oversamplingFactor;
        cntS = cntS - 1;
    end
    
    % Consider the power manipulationdue to oversampling
    handle.statesLagrangeUpsampled(idxOut + ind + 1) = accu * handle.oversamplingFactor;

    % Index update
    tmp = inFrameOffset + 1;
    inFrameOffset = mod(tmp, handle.oversamplingFactor);
    tmp = floor(tmp/handle.oversamplingFactor);
    idxIn_offset = idxIn_offset + tmp;
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