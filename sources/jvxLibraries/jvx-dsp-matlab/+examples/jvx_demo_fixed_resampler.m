%% test program for resampler..
INPUT = 'test_48000Hz.wav';
OUTPUT = 'out';
METHOD = 2;
BUFFERSIZEIN = 768;
BUFFERSIZEOUT = 128;

LFILTER_FIR = 64;
LAGRANGE_TO_LEFT = 2;
LAGRANGE_TO_RIGHT = 2;

USERTPROC = false;

% =================================================
[in, fs] = wavread(INPUT);
in = in';

global hdlRtproc;
if(USERTPROC)
    hdlRtproc.ui = rtprocHost(rtproc.varsargin);
end
hdlRtproc.useRtproc = USERTPROC;


switch(METHOD)
    case 1
        [hV] = rtpResampler_fixed_fir_init(LFILTER_FIR, BUFFERSIZEIN, BUFFERSIZEOUT);
        callback = @rtpResampler_fixed_fir_process;
        
        if(hdlRtproc.useRtproc)
            [success, hdlRtproc.hdlProc] = matCall_call_host('adaptiveResampler', 'initResamplerFixedFir', ...
                int16(LAGRANGE_TO_LEFT), int16(LAGRANGE_TO_RIGHT), int16(LFILTER_FIR), int16(BUFFERSIZEIN), int16(OVERSAMPLING));
            hdlRtproc.commandProcess = 'processResamplerFixedFir';
            hdlRtproc.commandTerminate = 'terminateResamplerFixedFir';
        end
    case 2
        [hV] = rtpResampler_fixed_iir_init(LFILTER_FIR, BUFFERSIZEIN, BUFFERSIZEOUT);
        callback = @rtpResampler_fixed_iir_process;
        
        if(hdlRtproc.useRtproc)
            [success, hdlRtproc.hdlProc] = matCall_call_host('adaptiveResampler', 'initResamplerFixedIir', ...
                int16(LAGRANGE_TO_LEFT), int16(LAGRANGE_TO_RIGHT), int16(LFILTER_FIR), int16(BUFFERSIZEIN), int16(OVERSAMPLING));
            hdlRtproc.commandProcess = 'processResamplerFixedIir';
            hdlRtproc.commandTerminate = 'terminateResamplerFixedIir';
        end
        
    otherwise
        error('Wrong method selected.');
end

% Processing
numFrames = ceil(size(in,2)/BUFFERSIZEIN);
lSignalIn = BUFFERSIZEIN*numFrames;
lSignalOut = BUFFERSIZEOUT*numFrames;
inp = zeros(1, lSignalIn);
inp(1:size(in,2)) = in;
outp = [];
outpC = [];
for(lF = 1:numFrames)
    inVec = inp((lF-1)*BUFFERSIZEIN + 1: lF*BUFFERSIZEIN);
    [outVec, hV] = callback(inVec, hV);
    if(hdlRtproc.useRtproc)
        [success, outVecC] = matCall_call_host('adaptiveResampler', hdlRtproc.commandProcess, inVec(1:BUFFERSIZEIN), zeros(1,BUFFERSIZEOUT), hdlRtproc.hdlProc);

        %[success, outVecC1] = matCall_call_host('adaptiveResampler', hdlRtproc.commandProcess, inVec(1:BUFFERSIZEIN/2), zeros(1,BUFFERSIZEOUT/2), hdlRtproc.hdlProc);
        %[success, outVecC2] = matCall_call_host('adaptiveResampler', hdlRtproc.commandProcess, inVec(BUFFERSIZEIN/2+1:end), zeros(1,BUFFERSIZEOUT/2), hdlRtproc.hdlProc);
        %outVecC = [outVecC1 outVecC2];
        diff = outVec - outVecC;
        diff * diff'
    end
    
    outp = [outp outVec];
    if(hdlRtproc.useRtproc)
        outpC = [outpC outVecC];
    end
end

%figure; plot(outp-outpC);
fsout = fs * BUFFERSIZEOUT/BUFFERSIZEIN;
fName = [OUTPUT '_' num2str(BUFFERSIZEIN) '_to_' num2str(BUFFERSIZEOUT) '-resampling-' num2str(METHOD) '_fs_' num2str(fsout) 'Hz.wav'];
wavwrite(outp, fsout, fName);
disp(['Written file ' fName]);

if(hdlRtproc.useRtproc)
    [success] = matCall_call_host('adaptiveResampler', hdlRtproc.commandTerminate, hdlRtproc.hdlProc);
    delete(hdlRtproc.ui);
end

