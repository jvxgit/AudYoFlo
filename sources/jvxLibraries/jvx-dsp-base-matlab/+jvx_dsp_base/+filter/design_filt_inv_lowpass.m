function [b,a] = design_filt_inv_lowpass( ...
    fs, fLow, fHigh, attenuationLow_dB, attenuationHigh_dB)

% designInvertibleBandLimit
%
% Invertierbares Filter zur Begrenzung tiefer und hoher Frequenzen.
%
% fs                  Abtastrate
% fLow                untere Grenzfrequenz
% fHigh               obere Grenzfrequenz
% attenuationLow_dB   gewünschte maximale Tieffrequenz-Dämpfung, z.B. -20
% attenuationHigh_dB  gewünschte maximale Hochfrequenz-Dämpfung, z.B. -20

    % Polradien aus Grenzfrequenzen
    rpHP = exp(-2*pi*fLow/fs);

    % Für Tiefpass Abstand zu Nyquist verwenden
    rpLP = exp(-2*pi*(fs/2-fHigh)/fs);

    % gewünschte lineare Restverstärkung
    GLow  = 10^(attenuationLow_dB/20);
    GHigh = 10^(attenuationHigh_dB/20);

    % ------------------------------------------------
    % Hochpass-artiger Teil
    % ------------------------------------------------

    % Näherung für Nullstellenradius
    rzHP = 1 - GLow*(1-rpHP);

    bHP = [1 -rzHP];
    aHP = [1 -rpHP];

    % bei Nyquist auf Gain 1 normieren
    KHP = polyval(aHP,-1) / polyval(bHP,-1);
    bHP = KHP*bHP;

    % ------------------------------------------------
    % Tiefpass-artiger Teil
    % ------------------------------------------------

    rzLP = 1 - GHigh*(1-rpLP);

    bLP = [1 rzLP];
    aLP = [1 rpLP];

    % bei DC auf Gain 1 normieren
    KLP = sum(aLP)/sum(bLP);
    bLP = KLP*bLP;

    % ------------------------------------------------
    % Kombination
    % ------------------------------------------------

    b = conv(bHP,bLP);
    a = conv(aHP,aLP);

end