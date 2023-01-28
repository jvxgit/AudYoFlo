% Simulate the buffering in async mode

%% Processing Parameters:
bSize_sw = 480;
bSize_hw = 396;
outputSafety = 100; % in Prozent
inputSafety = 0;

%avrgProcessingTime = 0;
muProcessingTime = 0.0;

avrgProcessingDelayStart = 0;
muProcessingDelayStart = 0.0;

load = 0.7;
loadEstimate = 0.0;

avrgProcessingTime = floor(load*bSize_sw)
%avrgProcessingTime = 7;

granularityWait = 1000;
minProcTime = 0;
minStartDelayTime = 0;
simulationLoops = 1;

idxStartShow = 1;
idxStopShow = -1;

%%=======================================================

simulationTime = 2*bSize_hw*bSize_sw * simulationLoops;

tBuf = 1:simulationTime;

% Statistic process to model soft processing parameters
bufProcessingTime = round(avrgProcessingTime + randn(1,simulationTime)*muProcessingTime);
bufProcessingTime = max(bufProcessingTime, minProcTime);

bufProcessingDelayStart = round(avrgProcessingDelayStart + randn(1,simulationTime)*muProcessingDelayStart);
bufProcessingDelayStart = max(bufProcessingDelayStart, minStartDelayTime);

figure; hist(bufProcessingTime);
figure; hist(bufProcessingDelayStart);

% Special mode: hw buffersize fits into sw buffersize
useZeroInputDelay = false;
%if(mod(bSize_sw, bSize_hw) == 0)
%    useZeroInputDelay = true;
%end

% Buffering
minBuf = min(bSize_sw, bSize_hw);
maxBuf = max(bSize_sw, bSize_hw);

inBuf.size = bSize_sw+bSize_hw;% + bSize_sw;
inBuf.buffer = zeros(1, inBuf.size);
inBuf.delay = 0;%min(bSize_hw, bSize_sw);
%inBuf.delay = bSize_hw;
inBuf.idxRead = 1;
inBuf.numSamples = inBuf.delay;

outBuf.size = bSize_sw+bSize_hw + bSize_sw;
outBuf.buffer = zeros(1, outBuf.size);
outBuf.delay = bSize_sw+bSize_hw +ceil(loadEstimate*bSize_sw);% max(bSize_hw, bSize_sw)
% outBuf.delay = round(outputSafety * 0.01* bSize_hw)+ bSize_sw;
outBuf.idxRead = 1;
outBuf.numSamples = outBuf.delay;




computedOverallDelay = outBuf.delay + inBuf.delay;
disp(['Input-output delay:' num2str(computedOverallDelay)]);

hw_buf_in = zeros(1, bSize_hw);
hw_buf_out = zeros(1, bSize_hw);
sw_buf_in = zeros(1, bSize_sw);
sw_buf_out = zeros(1, bSize_sw);


% State machine SW: 
%   0: Waiting for samples 
%   1: Start Processing
%   2: In Processing
stateSW = 0;

samplesIn = [1:simulationTime];
samplesOut = zeros(1, simulationTime);

counters.startBuffer = 1;
counters.samplesCntHW = bSize_hw;

counters.waitUntilProcessing = -1;
counters.waitUntilProcessingReady = -1;

% Prepare debug output
dbg.atEntrance.inBuf.numSamples = zeros(1,simulationTime);
dbg.atEntrance.inBuf.space = zeros(1,simulationTime);
dbg.atEntrance.outBuf.numSamples = zeros(1,simulationTime);
dbg.atEntrance.outBuf.space = zeros(1,simulationTime);

dbg.signalGiven = zeros(1,simulationTime);;
dbg.hwBufferswitch = zeros(1, simulationTime);
dbg.hwUnderOverflow = zeros(1, simulationTime);

dbg.signalReceived = zeros(1,simulationTime);;
dbg.swBufferswitchStart = zeros(1, simulationTime);
dbg.swBufferswitchStop = zeros(1, simulationTime);
dbg.swUnderOverflow = zeros(1, simulationTime);

dbg.atExit.inBuf.numSamples = zeros(1,simulationTime);
dbg.atExit.inBuf.space = zeros(1,simulationTime);
dbg.atExit.outBuf.numSamples = zeros(1,simulationTime);
dbg.atExit.outBuf.space = zeros(1,simulationTime);

dbg.instantaneousProcessingStartDelay = zeros(1,simulationTime);
dbg.instantaneousProcessingTime = zeros(1,simulationTime);

wb = waitbar(0.0, 'progress');


for(t = 1: simulationTime)
    
    if(mod(t,granularityWait) == 0)
        waitbar(t/simulationTime, wb);
    end
    dbg.atEntrance.inBuf.numSamples(t) = inBuf.numSamples;
    dbg.atEntrance.inBuf.space(t) = inBuf.size - inBuf.numSamples;
    dbg.atEntrance.outBuf.numSamples(t) = outBuf.numSamples;
    dbg.atEntrance.outBuf.space(t) = outBuf.size - outBuf.numSamples;

    % HW side
    if(counters.samplesCntHW == 1)
        
        dbg.hwBufferswitch(t) = 1;

        % New buffer is available
        hw_buf_in = samplesIn(counters.startBuffer:t);

        canDo = false;
        if((inBuf.size-inBuf.numSamples >= bSize_hw)&&(outBuf.numSamples >= bSize_hw))
            canDo = true;
        end

        % Copy input and output buffers
        if(canDo)
            
            % Write to buffer (should not fail)
            idxWrite = inBuf.idxRead+inBuf.numSamples;
            if(idxWrite > inBuf.size)
                idxWrite = idxWrite - inBuf.size;
            end
            
            l1 = min(bSize_hw, (inBuf.size-idxWrite+1));
            l2 = bSize_hw - l1;
            
            if(l1+l2 ~= bSize_hw)
                disp('Hier');
            end
            if(l1)
                inBuf.buffer(idxWrite:idxWrite+l1-1) = hw_buf_in(1:l1);
            end
            if(l2)
                inBuf.buffer(1:l2) = hw_buf_in(l1+1:l1+l2);
            end
            inBuf.numSamples = inBuf.numSamples + bSize_hw;
            
 
            l1 = min(bSize_hw, (outBuf.size-outBuf.idxRead+1));
            l2 = bSize_hw - l1;
            
            if(l1)
                hw_buf_out(1:l1) = outBuf.buffer(outBuf.idxRead:outBuf.idxRead+l1-1);
            end
            if(l2)
                hw_buf_out(l1+1:l1+l2) = outBuf.buffer(1:l2);
            end
            outBuf.idxRead = outBuf.idxRead + bSize_hw;
            if(outBuf.idxRead > outBuf.size)
                outBuf.idxRead = outBuf.idxRead - outBuf.size;
            end
            outBuf.numSamples = outBuf.numSamples - bSize_hw;
        else
            
            if(inBuf.size-inBuf.numSamples >= bSize_hw)                    
                disp('  XXXXX  HW Output buffer underflow');
                dbg.hwUnderOverflow(t) = 1;
            else
                disp('  XXXXX  HW Input buffer overflow');
                dbg.hwUnderOverflow(t) = -1;
            end
            hw_buf_out = zeros(1,bSize_hw);
        end
            
        % Write data to hardware
        samplesOut(counters.startBuffer:t) = hw_buf_out;

        % Restart counter for audio samples
        counters.samplesCntHW = bSize_hw+1;

        dbg.signalGiven(t) = 1;
        % Trigger processing in software
        counters.waitUntilProcessing = bufProcessingDelayStart(t);
        
        dbg.instantaneousProcessingStartDelay(t) = counters.waitUntilProcessing;

        % Update sample counter
        counters.startBuffer = t+1;

    end
    
    doLoop = true;
    
    while(doLoop)
        
        % SW side
        if(stateSW == 0)
            if(counters.waitUntilProcessing == 0)

                dbg.signalReceived(t) = 1;
                stateSW = 1;
            else
                doLoop = false;
            end
        end
    
        if(stateSW == 1)
        
            % Check buffer for whether processing is possible
            canProcess = false;
            if((outBuf.size-outBuf.numSamples >= bSize_sw) && (inBuf.numSamples >= bSize_sw))
                canProcess = true;
            end
        
            if(canProcess)
                dbg.swBufferswitchStart(t) = 1;
                l1 = min(bSize_sw, (inBuf.size-inBuf.idxRead+1));
                l2 = bSize_sw - l1;
                
                if(l1)
                    sw_buf_in(1:l1) = inBuf.buffer(inBuf.idxRead:inBuf.idxRead+l1-1);
                end
                if(l2)
                    sw_buf_in(l1+1:l1+l2) = inBuf.buffer(1:l2);
                end
                inBuf.idxRead = inBuf.idxRead + bSize_sw;
                if(inBuf.idxRead > inBuf.size)
                    inBuf.idxRead = inBuf.idxRead - inBuf.size;
                end
                inBuf.numSamples = inBuf.numSamples - bSize_sw;
                
                stateSW = 2;
                counters.waitUntilProcessingReady = bufProcessingTime(t);
                dbg.instantaneousProcessingTime(t) = counters.waitUntilProcessingReady;
            else
                
                if((inBuf.numSamples >= bSize_sw)&&(outBuf.size-outBuf.numSamples < bSize_sw))                     
                    disp('We need to avoid this!');
                end

                dbg.swUnderOverflow(t) = -2;
                if(outBuf.size-outBuf.numSamples >= bSize_sw) 
                    % Input buffer not filled
                    dbg.swUnderOverflow(t) = 1;
                end
                if(inBuf.numSamples >= bSize_sw)
                    % Output buffer too small
                   dbg.swUnderOverflow(t) = -1;
                end
                doLoop = false;
                stateSW = 0;
                
            end
        end
        if(stateSW == 2)
        
            if(counters.waitUntilProcessingReady == 0)
                
                dbg.swBufferswitchStop(t) = 1;
                
                % Complete processing Copy data to output: Talkthrough
                sw_buf_out = sw_buf_in; 
                
                % Is there enough space to copy?
                if(outBuf.size-outBuf.numSamples >= bSize_sw)
                
                    % Write to buffer (should not fail)
                    idxWrite = outBuf.idxRead+outBuf.numSamples;
                    if(idxWrite > outBuf.size)
                        idxWrite = idxWrite - outBuf.size;
                    end
                    
                    l1 = min(bSize_sw, (outBuf.size-idxWrite+1));
                    l2 = bSize_sw - l1;
                    
                    if(l1)
                        outBuf.buffer(idxWrite:idxWrite+l1-1) = sw_buf_out(1:l1);
                    end
                    if(l2)
                        outBuf.buffer(1:l2) = sw_buf_out(l1+1:l1+l2);
                    end
                    outBuf.numSamples = outBuf.numSamples + bSize_sw;
                    
                else
                    error('Unexpected');
                end
                
                stateSW = 1;
            else
                doLoop = false;
            end
        end        
    end
    
    counters.waitUntilProcessing = counters.waitUntilProcessing -1;
    counters.waitUntilProcessingReady = counters.waitUntilProcessingReady -1;
    counters.samplesCntHW = counters.samplesCntHW -1;

    dbg.atExit.inBuf.numSamples(t) = inBuf.numSamples;
    dbg.atExit.inBuf.space(t) = inBuf.size - inBuf.numSamples;
    dbg.atExit.outBuf.numSamples(t) = outBuf.numSamples;
    dbg.atExit.outBuf.space(t) = outBuf.size - outBuf.numSamples;

end
 
if(idxStartShow <= 0)
    idxStartShow = 1;
end

if(idxStopShow <= 0)
    idxStopShow = simulationTime;
end

if(idxStartShow > idxStopShow)
    idxStopShow = idxStartShow + 1;
end
    
% Next: The evaluation:
figure;
subplot(3,1,1);
hold on;
plot(dbg.atEntrance.inBuf.numSamples(idxStartShow:idxStopShow));
plot(inBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
plot(bSize_sw*ones(1, (idxStopShow-idxStartShow+1)), 'g');
legend({'IN BUF ENTER::NUM SAMPLES', 'Max buffsersize', 'Buffersize software'});

%subplot(5,1,2);
%hold on;plot(dbg.atEntrance.inBuf.space(idxStartShow:idxStopShow));
%plot(inBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'IN BUF ENTER::SPACE'});

subplot(3,1,2);
hold on;plot(dbg.atEntrance.outBuf.numSamples(idxStartShow:idxStopShow));
plot(outBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
plot(bSize_sw+bSize_hw*ones(1, (idxStopShow-idxStartShow+1)), 'g');
plot(bSize_sw*ones(1, (idxStopShow-idxStartShow+1)), 'g');
legend({'OUT BUF ENTER::NUM SAMPLES', 'Buffsersize SW + buffersize HW', 'Buffersize software'});

%subplot(5,1,4);
%hold on;plot(dbg.atEntrance.outBuf.space(idxStartShow:idxStopShow));
%%plot(outBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'OUT BUF ENTER::SPACE'});

%subplot(12,1,5);
%plot(dbg.signalGiven(idxStartShow:idxStopShow));
%legend({'HW::SIGNAL GIVEN'});

%subplot(12,1,6);
%plot(dbg.hwBufferswitch(idxStartShow:idxStopShow));
%legend({'HW BUFFERSWITCH'});

subplot(3,1,3);
plot(dbg.hwUnderOverflow(idxStartShow:idxStopShow));
legend({'HW OVERUNDERFLOW'});

%subplot(12,1,8);
%plot(dbg.signalReceived(idxStartShow:idxStopShow));
%legend({'SIGNAL RECEIVED'});

%subplot(12,1,9);
%plot(dbg.swBufferswitchStart(idxStartShow:idxStopShow));
%legend({'SW BUFFERSWITCH START'});

%subplot(12,1,10);
%plot(dbg.swBufferswitchStop(idxStartShow:idxStopShow));
%legend({'SW BUFFERSWITCH STOP'});

%subplot(12,1,11);
%plot(dbg.swUnderOverflow(idxStartShow:idxStopShow));
%legend({'SW OVERUNDERFLOW'});

%subplot(12,1,12);
%hold on;
%plot(dbg.atExit.inBuf.numSamples(idxStartShow:idxStopShow));
%plot(inBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'IN BUF EXIT::NUM SAMPLES'});

%subplot(17,1,13);
%hold on;
%plot(dbg.atExit.inBuf.space(idxStartShow:idxStopShow));
%plot(inBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'IN BUF EXIT::SPACE'});

%subplot(17,1,14);
%hold on;
%plot(dbg.atExit.outBuf.numSamples(idxStartShow:idxStopShow));
%plot(outBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'OUT BUF EXIT::NUM SAMPLES'});

%subplot(17,1,15);
%hold on;
%plot(dbg.atExit.outBuf.space(idxStartShow:idxStopShow));
%plot(outBuf.size*ones(1, (idxStopShow-idxStartShow+1)), 'r');
%legend({'IN BUF EXIT::SPACE'});

%subplot(17,1,16);
%plot(dbg.instantaneousProcessingTime(idxStartShow:idxStopShow));
%legend({'INST PROCESSING TIME'});

%subplot(17,1,17);
%plot(dbg.instantaneousProcessingStartDelay(idxStartShow:idxStopShow));
%legend({'INST START DELAY'});
                
%figure;
%plot(samplesIn); hold on; plot(samplesOut, 'r'); plot(samplesOut(1+computedOverallDelay:end), 'g:');
%legend({'Input', 'Output', 'Output Delay Compensation'});
        
%figure;
%dif = samplesIn(1:end-computedOverallDelay) - samplesOut(1+computedOverallDelay:end);
%plot(dif, 'r');

close(wb);
        