% Function to design a lowpass and afterwards, transform this filter into
% two allpasses. Those allpasses can be combined as 1/2 (A0(z) + A1(z)) 
% to realize this low pass and 1/2(A0(z) - A1(z)) to realize the complementary 
% high pass.
% Filer type = 1 -> Butterworth
% Filter type = 2 -> Ellip
function [A0, A1] = jvx_design_lp_allpasses(fc, fs, order, fType)

    EPS = 1e-12;
    ripple = 1;
    stopband = 50;
    %stopband = 25;
    if(mod(order,2) == 0)
        error('order of split filter must be odd!');
    end
    %disp(['cutoff freq: fc:' num2str(fc)]);
    % Optional plotting
    %if(plotme)
    %    ff = figure;
    %end

    % Normalized split frequency
    fq = fc/(fs/2);

    if(fType == 1)

        % Butterworth filter design
	% Old local version:: [b_rootszc, b_rootszs, b_poleszc, b_poleszs, H0] = butterworth_lp(order, fc, fs);
        [num, den, H0, b_rootszc, b_rootszs, b_poleszc, b_poleszs] = jvx_dsp.design.design_butterworth_lp_core(order, fc, fs);
        poles_all = [b_poleszc b_poleszs fliplr(conj(b_poleszc))];
        poles_A0 = poles_all(1:2:end);
        poles_A1 = poles_all(2:2:end);

        % From the poles, create the polynomial in z
        A0.den = real(poly(poles_A0));
        A1.den = real(poly(poles_A1));
        
        % For the allpass, create the zeros
        roots_A0 = conj(1./poles_A0);
        roots_A1 = conj(1./poles_A1);
        
        % Find the numerator term
        A0.num = real(poly(roots_A0));
        A1.num = real(poly(roots_A1));
        
        % Normalize such that we have a zero dB pass area
        A0.num = A0.num ./ A0.num(end);
        %A1.num = [A1.num ./ A1.num(end) 0];
        A1.num = A1.num ./ A1.num(end);
        
    elseif(fType == 2)
        [z, p, g] = ellip(order, ripple, stopband, fq);
        % Find the poles for the allpasses
        poles_conj = p(imag(p) > EPS);
        poles_real = p(abs(imag(p)) <= EPS);
        
        [~, idx_sort] = sort(abs(poles_conj));
        poles_conj_sorted = poles_conj(idx_sort);
        pole_real = poles_real(1);
        if(size(poles_real,1) ~= 1)
            error('Invalid poles');
        end
        
        if(fq > 0.5)
            poles_conj_sorted = fliplr(poles_conj_sorted);
            % sort poles in incrementing
        end
        
        poles_all = [conj(fliplr(poles_conj_sorted.')) pole_real poles_conj_sorted.'];
        poles_A0 = poles_all(1:2:end);
        poles_A1 = poles_all(2:2:end);
        % From the poles, create the polynomial in z
        A0.den = poly(poles_A0);
        %A1.den = [poly(poles_A1) 0];
        A1.den = poly(poles_A1);
        
        % For the allpass, create the zeros
        roots_A0 = conj(1./poles_A0);
        roots_A1 = conj(1./poles_A1);
        
        % Find the numerator term
        A0.num = poly(roots_A0);
        A1.num = poly(roots_A1);
        
        % Normalize such that we have a zero dB pass area
        A0.num = A0.num ./ A0.num(end);
        %A1.num = [A1.num ./ A1.num(end) 0];
        A1.num = A1.num ./ A1.num(end);
    else
        error('Filter type unspecified!!');
    end
end