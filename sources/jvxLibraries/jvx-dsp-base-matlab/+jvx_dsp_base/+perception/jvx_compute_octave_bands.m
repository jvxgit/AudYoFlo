function [f_center, f_lower, f_upper] = jvx_compute_octave_bands(fs, max_num_bands)

    if(nargin < 1)
        fs = 48000;
    end
    if(nargin < 2)
        max_num_bands = 32; % Number bands for 48 kHz
    end
    flim = fs/2;
    
    f_center  = 10^3 * (2 .^ ((-18:-18+max_num_bands-1)'/3));
    idx_valid = find(f_center <= flim);
    f_center = f_center(idx_valid);
    
    % Half octave up
    f_upper = f_center * 2^(1/6);
    
    % Half octave down
    f_lower = f_center / 2^(1/6);
    
    % Limit frequency range
    f_lower(1) = 0;
    f_upper(end) = flim;
    
