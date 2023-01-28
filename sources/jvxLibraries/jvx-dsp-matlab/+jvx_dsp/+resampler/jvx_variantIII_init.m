function [handle, delay_samples] = jvx_variantIII_init(lagrange_toLeft, lagrange_toRight, lFilter, buffersizeIn, oversamplingFactor)

    % ====================================================================
    % Start config
    % ====================================================================
    % Configuration for lowpass design, FIR filter with Kaiser window
    STOPBANDATTENUATION_DB_IIR = 40;

    % ====================================================================
    % Low pass design Low pass design Low pass design Low pass design
    % ====================================================================
    handle.stopbandattenuation_db = STOPBANDATTENUATION_DB_IIR;
    handle.oversamplingFactor = oversamplingFactor;
    handle.buffersizeIn = buffersizeIn;
    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;
    
    % Lagrange interpolation setup
    handle.lagrange_toLeft = lagrange_toLeft;
    handle.lagrange_toRight = lagrange_toRight;
    handle.lagrange_width = handle.lagrange_toLeft + handle.lagrange_toRight + 1;

    % IIR Rate converter: Use a different stopband attenuation
    [handle.num.coeffs handle.den.coeffs] = rtpDesignIIRFilter(handle.oversamplingFactor, 1, handle.stopbandattenuation_db );
    handle.lFilter = size(handle.num.coeffs,2);
    handle.filterOrder = handle.lFilter-1;
    handle.statesIir = zeros(1, handle.filterOrder);
    
    %[handle.groupDelay, x] = grpdelay(handle.num.coeffs, handle.den.coeffs);
        
    rtp_writeConstantsForC(['rtp_resampler_iir_defines_osmpl_' num2str(handle.oversamplingFactor) '_stpb_' num2str(handle.stopbandattenuation_db) '.h'],...
        ['ars_osmpl_' num2str(handle.oversamplingFactor) '_stp_' num2str(handle.stopbandattenuation_db) 'dB'], ...
        handle.num.coeffs, 'coeffs_num', ...
        handle.den.coeffs, 'coeffs_den');

    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;
    
    % Lagrange interpolation setup
    handle.lagrange_toLeft = lagrange_toLeft;
    handle.lagrange_toRight = lagrange_toRight;
    handle.lagrange_width = handle.lagrange_toLeft + handle.lagrange_toRight + 1;

    % ====================================================================
    % Compute parameters for Lagrange interpolation
    % ====================================================================
    handle.lagrange.inv_denomL = zeros(1, handle.lagrange_width);
    handle.lagrange.rootsL = zeros(handle.lagrange_width, handle.lagrange_width-1);
    
    cntO = 1;
    for(ind = -handle.lagrange_toLeft:handle.lagrange_toRight)
        cntI = 1;
        denT = 1;
        for(ind2 = -handle.lagrange_toLeft:handle.lagrange_toRight)
            if(ind2 ~= ind)
                denT = denT * (ind - ind2);
                handle.lagrange.rootsL(cntO, cntI) = ind2;
                cntI = cntI + 1;
            end
        end
        handle.lagrange.inv_denomL(cntO) = 1/denT;
        cntO = cntO + 1;
    end
    
    % ====================================================================
    % Allocate space to hold upsampled audio samples plus the Lagrange
    % "states"
    % ====================================================================
    handle.lStatesLagrangeUpsampled = handle.lagrange_width - 1 + handle.buffersizeIn * handle.oversamplingFactor;
    handle.statesLagrangeUpsampled = zeros(1, handle.lStatesLagrangeUpsampled);
    
    % ====================================================================
    % Compute the delay of the resampler
    % ====================================================================

    %[handle.groupDelay, x] = grpdelay(handle.num.coeffs, handle.den.coeffs);

    delay_samples = -1;
    
end

