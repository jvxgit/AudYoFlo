function [str] = jvx_design_fixed_resampler_fir(bsizein, bsizeout,lfilt,stopbandatten, socket, TUNING_FACTOR)
    
    FIRTP = 1; % 0: firls, 1: firpm; 2: fir2; 3: designfilt
    designlowpassbyhand = true;
    
    tmp = gcd(bsizein, bsizeout);
    dwns = bsizein / tmp;
	ovrs = bsizeout / tmp;
	bsizeos = bsizein * ovrs;

    if(mod(lfilt,2) == 0) 
        lfilt = lfilt + 1;
    end
    
    % ====================================================================
	% Low pass design Low pass design Low pass design Low pass design
	% ====================================================================
	lFilterUpsampled = (lfilt - 1) * ovrs + 1;
	midPosition = (lfilt - 1) / 2 * ovrs;
	alpha = 0;

	% From the stopband attenuation, derive the factor alpha for Kaiser
	% window
	if (stopbandatten > 21)
		if (stopbandatten > 50)
			alpha = 0.1102 * (stopbandatten - 8.7);
		else
			alpha = 0.5842 * (stopbandatten - 21.0)^ 0.4 + 0.07886*(stopbandatten - 21);
        end
    end

	% Allocate impulse response followed by some sapece for zeros
    impResp = zeros(1, (lFilterUpsampled + ovrs));
    
    % IR type 1
    fgOmega = min(1.0 / ovrs, 1.0 / dwns);
    
    ir1 = kaiser(lFilterUpsampled, alpha);
    
    % Next, derive the desired frequency response parameters: Consider
    % a specific distance to the maximum allowed frequency
    delta_f = 0;
    if (stopbandatten > 21.0)
        delta_f = ((stopbandatten - 7.95)) / (14.36 * (lFilterUpsampled - 1));
    else
        delta_f = (0.9222) / (lFilterUpsampled - 1);
    end
    
    Omega_g = (fgOmega - delta_f);
    
    % Compute the time domain impulse response based on the sin(x)/x
    for (ind=0:lFilterUpsampled-1)
        ir1(ind+1) = ir1(ind+1) * sinc(Omega_g * (ind - midPosition))* Omega_g;
    end
    ir1 = ir1';
    %ir1 = ir1./sum(ir1);
    
    % IR type 2
    %function [] = jvx_design_pdm_fir(os, ppfilterlength, FIRDESIGN_FAC, outfile)
    epsD = 10^(-stopbandatten/10);
        
    f = [0 (fgOmega - delta_f*TUNING_FACTOR) fgOmega 1];
    a = [1  1  epsD epsD];
    %dsigntoken = ['_d2_ll' num2str(efffilterlength) '_fac' num2str(FIRDESIGN_FAC) '_os' num2str(os)];
    switch(FIRTP)
        case 0
            ir2 = firls(lFilterUpsampled-1,f,a);
        case 1
            ir2 = firpm(lFilterUpsampled-1,f,a);
        case 2
            ir2 = fircls(lFilterUpsampled-1,f,a);
        %case 3
            %lpFilt = designfilt('lowpassfir','PassbandFrequency',(fgOmega - delta_f), ...
            %    'StopbandFrequency',fgOmega,...%'PassbandRipple',0.001, ...
            %    'StopbandAttenuation',stopbandatten,...%'DesignMethod','kaiserwin'... %
            %    'FilterOrder', lFilterUpsampled-1); %
            %ir2 = lpFilt.Coefficients;
    end
    
    %figure; plot(ir1); hold on; plot(ir2, 'r:');
    %figure; 
    %freqz(ir1, 1, 1024, 192000);
    %figure; 
    %freqz(ir2, 1, 1024, 192000);
    
    if(designlowpassbyhand)
        impResp(1:lFilterUpsampled) = ir1;
    else
        impResp(1:lFilterUpsampled) = ir2;
    end
    
	% ====================================================================
	% Allocate the relevant buffers for interpolation filter
	% ====================================================================
	lStates = (lfilt - 1) + bsizein;

	% ====================================================================
	% Compute the delay of the resampler
	% ====================================================================
	group_delay = (midPosition) / ovrs;

	% Here comes the new version of the resampler
	lStates = lfilt-1;
	impResp_vlength = (lFilterUpsampled - 1) / 2 + 1;
	impResp_length = (impResp_vlength + ovrs - 1);
	
	impRespN = zeros(1, impResp_length);
	impRespN(1+ovrs-1:end) = impResp(1:impResp_vlength); 
	
	str.buffersizeIn = bsizein;
	str.buffersizeOut = bsizeout;
	str.oversamplingFactor = ovrs;
	str.downsamplingFactor = dwns;
	str.lFilter = lfilt;
	str.impResp = impRespN;
	str.lStates = lStates; 
	str.designtoken = ['_' num2str(int32(bsizein)) '_' num2str(int32(bsizeout)) '_' num2str(int32(lfilt)) '_' num2str(int32(stopbandatten)) ];
	