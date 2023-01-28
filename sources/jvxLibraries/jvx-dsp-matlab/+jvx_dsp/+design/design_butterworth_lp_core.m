% Simplified design of butterwoth filter to be reference for c-code
% Example implementation taken from 
% https://github.com/mofr/Butterworth/
% HK, 2017, Javox/IKS
function [num, den, H0, b_rootszc, b_rootszs, b_poleszc, b_poleszs] = design_butterworth_lp_core(order, fc, fs)
    
	H0 = 0;
	b_rootszc = [];
	b_rootszs = [];
	b_poleszc = [];
	b_poleszs = [];
	
    % Frequency warp
    omegac = tan(pi*fc/fs)/pi;
    num_gen_conj = floor(order/2);
    num_gen_single = ceil(order/2) - num_gen_conj;
    
	% Poles
    % b_poles = 2*pi*omegac*exp(j*pi * (([0:order-1] + 0.5)/order+1/2));
    % Design buterworth -- if(strcmp(design, 'butter'))
        pole_abs = ones(1,order);
        pole_angles = pi*(1/2 + (0.5 + [0:order-1])/order);
    % end
    
    % Design poles in s-plane
    b_poles = pole_abs .* exp(j*pole_angles);
    
    % Do some adaptation
    b_poles = 2*pi*omegac* b_poles;
    b_roots = ones(1, order) * realmax;
    
    % Convert into z-domain
    b_polesz = (2 + b_poles)./(2 - b_poles);
    den = real(poly(b_polesz));
    
    % Select only the poles from upper 
    b_poleszc = b_polesz(1:num_gen_conj);
    b_poleszs = b_polesz(num_gen_conj+1:num_gen_conj+num_gen_single);

	% Zeros
    %b_rootsz = -ones(1, order);
    b_rootsz = (2 + b_roots) ./ (2-b_roots);
    num = real(poly(b_rootsz));
    
    b_rootszc = b_rootsz(1:num_gen_conj);
    b_rootszs = b_rootsz(num_gen_conj+1:num_gen_conj+num_gen_single);
    
	% Normalization
    H0 = sum(den)/sum(num);
	
end