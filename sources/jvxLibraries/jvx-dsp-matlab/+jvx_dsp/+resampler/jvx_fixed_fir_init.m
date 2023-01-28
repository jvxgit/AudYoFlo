function [handle, delay_samples] = jvx_fixed_fir_init(lFilter, buffersizeIn, buffersizeOut, stopbandDb, socketFir)

    % ====================================================================
    % Start config
    % ====================================================================
    % Configuration for lowpass design, FIR filter with Kaiser window
    
    % Some common parameters
    handle.buffersizeIn = double(round(buffersizeIn));
    handle.buffersizeOut = double(round(buffersizeOut));
    tmp = gcd(handle.buffersizeIn, handle.buffersizeOut);
    handle.downsamplingFactor = handle.buffersizeIn/tmp;
    handle.oversamplingFactor = handle.buffersizeOut/tmp;
    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;

    % If we want to have a clear algorithmic delay only allow for odd
    % filter length
    if(mod(lFilter,2) == 0)
        lFilter = lFilter +1;
    end
    
    % ====================================================================
    % Low pass design Low pass design Low pass design Low pass design
    % ====================================================================
    % Setup some parameters
    handle.stopbandattenuation_db = stopbandDb;
    handle.socket = socketFir;
    handle.lFilter = double(round(lFilter));
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
    
    % Find the edge frequency (min of input and output frequency divided by
    % upsampled frequency
    fgOmega = min(1/handle.oversamplingFactor, 1/handle.downsamplingFactor);
    Omega_g = (fgOmega  - handle.delta_f );
    m_pi_omega_g = pi * Omega_g;
        
    % Compute the time domain impulse response based on the sin(x)/x
    handle.impResp = zeros(1, size(handle.kaiserWin, 2));
    for(ind = 1:size(handle.kaiserWin, 2))
        handle.impResp(ind) = si ( m_pi_omega_g * (ind - handle.midPosition))* Omega_g * handle.kaiserWin(ind);
    end
    
    % If desired, we may plot iresponse here
    %    freqz(handle.impResp);
    
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
    % Compute the delay of the resampler
    % ====================================================================

    % Compute the group delay
    %[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
    %version computed from impulse response
    handle.groupDelay = (handle.midPosition - 1) / handle.oversamplingFactor;

    delay_samples = handle.groupDelay;
    
end

%% Kaiser Window computation, uses i0 (Bessel function order zero)
function [out] = jvx_kaiser(N, N0, alpha, buquet)

    lSideLobe = ceil(N/2);
    sideLobe = zeros(1, lSideLobe);
    
    den = i0(alpha, N0);

    for(ind=0:lSideLobe-1)
        sideLobe(ind+1) = i0(alpha * sqrt(1- (4*ind^2)/(N-1)^2), N0)/den;
        sideLobe(ind+1) = buquet + (1-buquet) * sideLobe(ind+1);
    end    
    
    if(mod(N,2) == 1)
        out = [fliplr(sideLobe) sideLobe(2:end)];
    else
        out = [fliplr(sideLobe) sideLobe(2:end)];
    end
end

%% Bessel function order 0
function y = i0(x, N0)
    accu = 1;
    fac = 1;
    x2 = x/2;
    num = 1;
    y = 1;
    for(ind=1:N0)
        num = num * x/2; 
        fac = fac * ind;
        accu = num/fac;
        accu = accu * accu;
        y = y + accu;
    end
end
        
%% sin(x)/x
function y = si(x)
    y = 1;
    if(x ~= 0.0) 
        y = sin(x)/x;
    end
end