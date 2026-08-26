function [bLP, aLP, bHP, aHP] = jvx_design_crossover(fs, fc)
    
    Wn = fc/(fs/2);

    % Linkwitz-Riley 8. Ordnung:
    % zwei Butterworth-Filter 4. Ordnung kaskadieren

    [bLP4,aLP4] = butter(4, Wn, 'low');
    [bHP4,aHP4] = butter(4, Wn, 'high');

    bLP = conv(bLP4,bLP4);
    aLP = conv(aLP4,aLP4);

    bHP = conv(bHP4,bHP4);
    aHP = conv(aHP4,aHP4);
end