function [str] = convert_tf_sos(num, den)
    
    [SOS, G] = tf2sos(num, den);
    nSections = size(SOS, 1);
    str.fOrder = 2;
    str.shiftFac = 0;
    % str.gains = [G ones(1, nSections-1)];
	str.gains = repmat(G^(1/nSections),1, nSections); % Distribute gain over multiple sections
    str.sosA = zeros(nSections, 3);
    str.sosB = zeros(nSections, 3);
    for i = 1:nSections
        [b, a] = sos2tf(SOS(i, :));
        str.sosB(i,1:length(b)) = b;
        str.sosA(i,1:length(a)) = a;
    end