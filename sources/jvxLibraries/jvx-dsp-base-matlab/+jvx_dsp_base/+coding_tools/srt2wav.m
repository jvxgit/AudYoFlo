%% Function to load an srt file and convert it into a wav file. The srt
% file is expected to be composed of little endian, 16 bit raw samples.
% The input is normalized to fit into -1..1 before saving as wav file.
function [] = srt2wav(fNameInPrefix, fs)
    if(nargin < 2)
        fs = 48000;
    end
    
    fID = fopen([fNameInPrefix '.srt'], 'rb');
    sig16 = fread(fID, inf, 'int16');
    fclose(fID);
    sig = sig16 / 2^15;
    chan1 = sig';
    audiowrite([fNameInPrefix '.wav'], chan1, fs);
