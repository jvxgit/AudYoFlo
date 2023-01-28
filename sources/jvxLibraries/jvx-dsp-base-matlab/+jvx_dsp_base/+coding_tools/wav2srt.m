%% This function opens a wav file and saves the content as an srt raw audio file.
% The srt format is raw audio with 16 bit precision. The input values between -1..1
% are mapped to -2^15..2^15 in signed int16 format.
function [] = wav2srt(fNameInPrefix)
    [in, fs] = audioread([fNameInPrefix '.wav']);
    disp(['Samplerate = ' int2str(fs) ' -- num channels = ' int2str(size(in,2))]);
    chan1 = in(:,1);
    sig = chan1*2^15;
    sig16 = int16(sig);
    fID = fopen([fNameInPrefix '.srt'], 'wb');
    fwrite(fID, sig16, 'int16');
    fclose(fID);
    
