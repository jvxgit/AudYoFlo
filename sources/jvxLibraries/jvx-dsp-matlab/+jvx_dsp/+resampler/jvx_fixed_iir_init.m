function [handle, delay_samples] = jvx_fixed_iir_init(lFilter, buffersizeIn, buffersizeOut)

	writeC = false;
    % ====================================================================
    % Start config
    % ====================================================================
    % Configuration for lowpass design, FIR filter with Kaiser window
    STOPBANDATTENUATION_DB_IIR = 40;

    % Some common parameters
    tmp = gcd(buffersizeIn, buffersizeOut);
    handle.downsamplingFactor = buffersizeIn/tmp;
    handle.oversamplingFactor = buffersizeOut/tmp;
    handle.buffersizeIn = buffersizeIn;
    handle.buffersizeOut = buffersizeOut;

    % ====================================================================
    % Low pass design Low pass design Low pass design Low pass design
    % ====================================================================
    handle.stopbandattenuation_db = STOPBANDATTENUATION_DB_IIR;
    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;
    
    % IIR Rate converter: Use a different stopband attenuation
    % Compute inverse of edge frequency
    fginv = max(handle.oversamplingFactor, handle.downsamplingFactor);
    [handle.num.coeffs handle.den.coeffs] = rtpDesignIIRFilter(fginv, 1, handle.stopbandattenuation_db );
    handle.lFilter = size(handle.num.coeffs,2);
    handle.filterOrder = handle.lFilter-1;
    handle.statesIir = zeros(1, handle.filterOrder);
    
    %[handle.groupDelay, x] = grpdelay(handle.num.coeffs, handle.den.coeffs);
     
	if(writeC)
		jvx_writeConstantsForC(['rtp_resampler_iir_defines_osmpl_' num2str(handle.oversamplingFactor) '_dsmpl_' num2str(handle.downsamplingFactor) '_stpb_' num2str(handle.stopbandattenuation_db) '.h'],...
			['ars_osmpl_' num2str(handle.oversamplingFactor) '_stp_' num2str(handle.stopbandattenuation_db) 'dB'], ...
			handle.num.coeffs, 'coeffs_num', ...
			handle.den.coeffs, 'coeffs_den');
	end

    handle.buffersizeUpsampled = handle.buffersizeIn * handle.oversamplingFactor;
            
    % ====================================================================
    % Compute the delay of the resampler
    % ====================================================================

    %[handle.groupDelay, x] = grpdelay(handle.num.coeffs, handle.den.coeffs);

    delay_samples = -1;
    
end

