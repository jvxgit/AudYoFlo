%% test program for resampler..
INPUT = 'test_8000Hz.wav';
OUTPUT = 'out';
METHOD = 1;
OVERSAMPLING = 8;
BUFFERSIZEIN = 128;
BUFFERSIZEOUT = 130;

LFILTER_FIR = 64;
LAGRANGE_TO_LEFT = 2;
LAGRANGE_TO_RIGHT = 2;

USERTPROC = true;

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
        [hV] = rtpResampler_variantI_init(LAGRANGE_TO_LEFT, LAGRANGE_TO_RIGHT, LFILTER_FIR, BUFFERSIZEIN, OVERSAMPLING);
        callback = @rtpResampler_variantI_process;
        
        if(hdlRtproc.useRtproc)
            [success, hdlRtproc.hdlProc] = matCall_call_host('adaptiveResampler', 'initResamplerV1', ...
                int16(LAGRANGE_TO_LEFT), int16(LAGRANGE_TO_RIGHT), int16(LFILTER_FIR), int16(BUFFERSIZEIN), int16(OVERSAMPLING));
            hdlRtproc.commandProcess = 'processResamplerV1';
            hdlRtproc.commandTerminate = 'terminateResamplerV1';
        end
        
    case 2
        [hV] = rtpResampler_variantII_init(LAGRANGE_TO_LEFT, LAGRANGE_TO_RIGHT, LFILTER_FIR, BUFFERSIZEIN, OVERSAMPLING);
        callback = @rtpResampler_variantII_process;

        if(hdlRtproc.useRtproc)
            [success, hdlRtproc.hdlProc] = matCall_call_host('adaptiveResampler', 'initResamplerV2', ...
                int16(LAGRANGE_TO_LEFT), int16(LAGRANGE_TO_RIGHT), int16(LFILTER_FIR), int16(BUFFERSIZEIN), int16(OVERSAMPLING));
            hdlRtproc.commandProcess = 'processResamplerV2';
            hdlRtproc.commandTerminate = 'terminateResamplerV2';
        end
    case 3
        [hV] = rtpResampler_variantIII_init(LAGRANGE_TO_LEFT, LAGRANGE_TO_RIGHT, LFILTER_FIR, BUFFERSIZEIN, OVERSAMPLING);
        callback = @rtpResampler_variantIII_process;

        if(hdlRtproc.useRtproc)
            [success, hdlRtproc.hdlProc] = matCall_call_host('adaptiveResampler', 'initResamplerV3', ...
                 int16(LAGRANGE_TO_LEFT), int16(LAGRANGE_TO_RIGHT), int16(LFILTER_FIR), int16(BUFFERSIZEIN), int16(OVERSAMPLING));
            hdlRtproc.commandProcess = 'processResamplerV3';
            hdlRtproc.commandTerminate = 'terminateResamplerV3';
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
    [outVec, hV] = callback(inVec, BUFFERSIZEOUT, hV);
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

if(hdlRtproc.useRtproc)
    figure; plot(outp-outpC);
end
fName = [OUTPUT '_' num2str(fs*BUFFERSIZEOUT/BUFFERSIZEIN) 'Hz-Lagrange-oversmpl'  num2str(OVERSAMPLING) '-variant' num2str(METHOD) '.wav'];
wavwrite(outp, fs, fName);

[success] = matCall_call_host('adaptiveResampler', hdlRtproc.commandTerminate, hdlRtproc.hdlProc);

delete(hdlRtproc.ui);