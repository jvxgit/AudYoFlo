function [out, handle] = jvx_fixed_fir_process(in, handle)
    
% This is the memove to copy new buffers into state buffer. A circular
% buffer would be more complex on an architecture without circular
% addressing support
handle.states(1:handle.lStates-handle.buffersizeIn) = ...
    handle.states(end - (handle.lStates-handle.buffersizeIn) +1:end);
handle.states((handle.lStates-handle.buffersizeIn)+1:end) = in;
    
% Create empty output buffers
out = zeros(1, handle.buffersizeOut);
    
% Step I: Produce oversampled output:
inFrameOffset = 0;
idxIn_offset = handle.lFilter-1;
        
for(ind = 0:(handle.buffersizeOut-1))
                        
    accu = 0.0;
    
    % Offset in state buffer
    cntS = idxIn_offset;
    
    % Offset in imp response
    cntC = inFrameOffset;
            
    for(ind2 = 1:handle.lFilter)
        op1 = handle.states(cntS + 1);
        op2 = handle.impRespZeros(cntC + 1);
        accu = accu + op1 * op2;
        cntC = cntC + handle.oversamplingFactor;
        cntS = cntS - 1;
    end
    
    % Consider the power manipulationdue to oversampling
    out(ind + 1) = accu * handle.oversamplingFactor;

    % Index update
    tmp = inFrameOffset + handle.downsamplingFactor;
    inFrameOffset = mod(tmp, handle.oversamplingFactor);
    tmp = floor(tmp/handle.oversamplingFactor);
    idxIn_offset = idxIn_offset + tmp;
end

