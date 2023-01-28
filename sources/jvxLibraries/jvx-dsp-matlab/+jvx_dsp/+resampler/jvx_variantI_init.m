function [handle, delay_samples] = jvx_variantI_init(lagrange_toLeft, lagrange_toRight, lFilter, buffersizeIn, oversamplingFactor)

    % ====================================================================
    % Start config
    % ====================================================================
    % Configuration for lowpass design, FIR filter with Kaiser window
    STOPBANDATTENUATION_DB_FIR = 60;
    SOCKET_FIR = 0.0;
    
    % Some common parameters
    handle.oversamplingFactor = oversamplingFactor;
    handle.buffersizeIn = buffersizeIn;
    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;
    
    % Lagrange interpolation setup
    handle.lagrange_toLeft = lagrange_toLeft;
    handle.lagrange_toRight = lagrange_toRight;
    handle.lagrange_width = handle.lagrange_toLeft + handle.lagrange_toRight + 1;

    % If we want to have a clear algorithmic delay only allow for odd
    % filter length
    if(mod(lFilter,2) == 0)
        lFilter = lFilter +1;
    end
    
    % ====================================================================
    % Low pass design Low pass design Low pass design Low pass design
    % ====================================================================
    % Setup some parameters
    handle.stopbandattenuation_db = STOPBANDATTENUATION_DB_FIR;
    handle.socket = SOCKET_FIR;
    handle.lFilter = lFilter;
    handle.lFilterUpsampled = (lFilter-1) * handle.oversamplingFactor + 1;
    handle.midPosition = (lFilter-1)/2 *handle.oversamplingFactor +1;
    handle.delay = (handle.midPosition - 1)/handle.oversamplingFactor;
    handle.alpha = 0;
    
    % From the stopband attenuation, derive the factor alpha for Kaiser
    % window
    if(handle.stopbandattenuation_db > 21)
        if(handle.stopbandattenuation_db > 50)
            handle.alpha = 0.1102 * (handle.stopbandattenuation_db - 8.7);
        else
            handle.alpha = 0.5842 * (handle.stopbandattenuation_db -21)^0.4 + 0.07886*(handle.stopbandattenuation_db -21);
        end
    end
    
    % Compute the Kaiser window
    handle.kaiserWin = jvx_kaiser(handle.lFilterUpsampled, 100, handle.alpha, 0);
    
    % Next, derive the desired frequency response parameters: Consider
    % a specific distance to the maximum allowed frequency
    handle.delta_f = 0;
    if ( handle.stopbandattenuation_db > 21.0 )
        handle.delta_f = (( handle.stopbandattenuation_db - 7.95 ) )	/ ( 14.36 * ( handle.lFilterUpsampled - 1 ) );
    else
        handle.delta_f = ( 0.9222 ) / ( handle.lFilterUpsampled - 1 );
    end
    Omega_g = ( 1/handle.oversamplingFactor - handle.delta_f );
    m_pi_omega_g = pi * Omega_g;
        
    % Compute the time domain impulse response based on the sin(x)/x
    handle.impResp = zeros(1, size(handle.kaiserWin, 2));
    for(ind = 1:size(handle.kaiserWin, 2))
        handle.impResp(ind) = si ( m_pi_omega_g * (ind - handle.midPosition))* Omega_g * handle.kaiserWin(ind);
    end
    
    % ====================================================================
    % Allocate the relevant buffers for interpolation filter
    % ====================================================================
    
    % Add some zeros to imp resp to avoid segmentation violations in high
    % efficient processing
    handle.impRespZeros = [handle.impResp zeros(1,handle.oversamplingFactor)];
    
    % State buffer for convolution
    handle.lStates = (handle.lFilter-1) + handle.buffersizeIn;
    handle.states = zeros(1,handle.lStates);
    
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
    
    % Write the constants to file
    %rtp_writeConstantsForC('rtp_resampler_fir_defines.h', 'ars', ...
    %    handle.impResp, 'h_ars', handle.groupDelay(1), 'groupdelay');

        
    
    % ====================================================================
    % Compute the delay of the resampler
    % ====================================================================

    % Compute the group delay
    %[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
    %version computed from impulse response
    handle.groupDelay = (handle.midPosition + handle.lagrange_toRight - 1) / handle.oversamplingFactor;

    delay_samples = handle.groupDelay;
    
end

