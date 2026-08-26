function [b, a, rp, rz] = design_filt_inv_notch(fs, f0, BW, notchDepth_dB)

% design_filt_inv_notch
% Entwirft einen stabilen, invertierbaren IIR-Notch zweiter Ordnung.
%
% Eingaben:
%   fs               Abtastrate [Hz]
%   f0               Notch-Frequenz [Hz]
%   BW               ungefähre 3-dB-Bandbreite [Hz]
%   notchDepth_dB    gewünschte Dämpfung bei f0, z.B. -20
%
% Ausgaben:
%   b, a             Filterkoeffizienten
%   rp               Polradius
%   rz               Nullstellenradius
%
% Das Filter wird auf DC-Gain = 1 normiert.
% Definition of the coefficients:
% H(z) = B(z)/A(z) = (sum_0_N bi*z-i)/(sum_0_N ai*z-i)
% b: forward coefficients, a: backward coefficients
% freqz requires use with freqz(b,a...)

    arguments
        fs (1,1) double {mustBePositive}
        f0 (1,1) double {mustBePositive}
        BW (1,1) double {mustBePositive}
        notchDepth_dB (1,1) double {mustBeNegative}
    end

    if f0 >= fs/2
        error('f0 muss kleiner als fs/2 sein.');
    end

    omega0 = 2*pi*f0/fs;

    % Polradius aus gewünschter Bandbreite
    rp = exp(-pi*BW/fs);

    % Zielgain am Notch
    Gtarget = 10^(notchDepth_dB/20);

    % Funktion, die für gegebenes rz die tatsächliche
    % Verstärkung bei f0 berechnet.
    gainAtNotch = @(rz) localGainAtNotch(rz, rp, omega0);

    % rz muss für einen Notch zwischen rp und 1 liegen.
    % rz = 1 würde einen perfekten, aber nicht invertierbaren Notch geben.
    rzMin = rp;
    rzMax = 1 - 1e-12;

    gMin = gainAtNotch(rzMin);
    gMax = gainAtNotch(rzMax);

    if Gtarget > gMin
        error(['Die gewünschte Notch-Tiefe ist mit dieser Parametrisierung ', ...
               'nicht erreichbar. Versuche eine kleinere Bandbreite oder ', ...
               'eine stärkere Dämpfung.']);
    end

    if Gtarget < gMax
        error(['Die gewünschte Dämpfung ist numerisch zu tief. ', ...
               'Die Nullstellen müssten praktisch auf dem Einheitskreis liegen.']);
    end

    % rz numerisch exakt bestimmen
    fun = @(rz) gainAtNotch(rz) - Gtarget;
    rz = fzero(fun, [rzMin, rzMax]);

    % Filterkoeffizienten
    b = [1, -2*rz*cos(omega0), rz^2];
    a = [1, -2*rp*cos(omega0), rp^2];

    % Auf DC-Gain = 1 normieren
    K = sum(a)/sum(b);
    b = K*b;

end


function G = localGainAtNotch(rz, rp, omega0)

    b = [1, -2*rz*cos(omega0), rz^2];
    a = [1, -2*rp*cos(omega0), rp^2];

    % gleiche DC-Normierung wie später im echten Filter
    K = sum(a)/sum(b);
    b = K*b;

    z = exp(1j*omega0);

    num = b(1) + b(2)/z + b(3)/z^2;
    den = a(1) + a(2)/z + a(3)/z^2;

    G = abs(num/den);

end