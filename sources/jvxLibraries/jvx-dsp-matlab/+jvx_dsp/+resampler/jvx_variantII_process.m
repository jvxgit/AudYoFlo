function [out handle] = jvx_variantII_process(in, bSizeOut, handle)
    % This is the memove..
    handle.states(1:handle.lStates-handle.buffersizeIn) = ...
        handle.states(end - (handle.lStates-handle.buffersizeIn) +1:end);
    handle.states((handle.lStates-handle.buffersizeIn)+1:end) = in;
    %handle.states_upsampled;

    out = zeros(1, bSizeOut);
    
    % Loop over all output samples
    for(ind=1:bSizeOut)
        offsetInFrame = (ind-1) * (handle.buffersizeIn * handle.oversamplingFactor) /bSizeOut;
        idxOffset = floor(offsetInFrame);
        alpha = offsetInFrame - idxOffset;
        
        midPositionLagrange = idxOffset - handle.lagrange_toRight;
        [fragment] = rtpResampler_fir_fragment(midPositionLagrange, handle);
        
        accuOut = 0;
        for(ind2 = 0:handle.lagrange_width-1)
            coeff = 1;
            for(ind3 = 0:(handle.lagrange_width-2))
                coeff = coeff * (alpha - handle.lagrange.rootsL(ind2+1, ind3+1));
            end
            coeff = coeff * handle.lagrange.inv_denomL(ind2+1);
            accuOut = accuOut + coeff * fragment(ind2+1);
        end
        out(ind) = accuOut;
    end
end

function [relevantSamples] = rtpResampler_fir_fragment(midPosition, handle)

    relevantSamples = zeros(1, handle.lagrange_width);
    
    for(ind = 0:handle.lagrange_width-1)
        
        % Map the linear counter 0, 1, 2,3,4 to -2, -1, 0, 1, 2
        indU = ind - handle.lagrange_toLeft;

        % Treat also the mid position
        indUM = indU + midPosition;
        
        % Shift factor for addressing within the upsampling time window
        inFrameOffset = mod(indUM, handle.oversamplingFactor);
        sampleOffset = (handle.lFilter-1) + handle.stateSpaceLagrange + floor(indUM/handle.oversamplingFactor);% -1 and +1 due to Matlab addressing
        
        %inFrameOffset
        %sampleOffset
        
        accu = 0.0;
        cntC = inFrameOffset;
        cntS = sampleOffset;
            
        for(ind2 = 1:handle.lFilter)
            op1 = handle.states(cntS + 1);
            op2 = handle.impRespZeros(cntC + 1);
            accu = accu + op1 * op2;
            cntC = cntC + handle.oversamplingFactor;
            cntS = cntS - 1;
        end
        
        relevantSamples(ind + 1) = accu * handle.oversamplingFactor;
    end
end

            
        
    