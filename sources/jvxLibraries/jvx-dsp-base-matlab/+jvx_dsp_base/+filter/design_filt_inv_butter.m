function [b,a] = design_filt_inv_butter(fs, fc, N, type, rz)

% type = 'low' oder 'high'
% rz   = Radius der Nullstellen, z.B. 0.98...0.9999

    Wn = fc/(fs/2);

    % Normales Butterworth-Filter
    [b0,a] = butter(N,Wn,type);

    % Pole bleiben unverändert
    % Nullstellen des Butterworth-Filters werden ersetzt
    switch lower(type)

        case 'high'
            z = rz * ones(N,1);

            % Normierung bei Nyquist
            b = poly(z);
            K = polyval(a,-1) / polyval(b,-1);
            b = K*b;

        case 'low'
            z = -rz * ones(N,1);

            % Normierung bei DC
            b = poly(z);
            K = polyval(a,1) / polyval(b,1);
            b = K*b;

        otherwise
            error('type muss ''low'' oder ''high'' sein.');
    end

end