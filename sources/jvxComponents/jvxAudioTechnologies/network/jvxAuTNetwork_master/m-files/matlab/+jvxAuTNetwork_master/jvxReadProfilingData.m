function [] = jvxReadProfilingData(startIdx, stopIdx)

mode = 2;
verbose = false;

% #define JVX_PROFILE_SEND_CHANNEL_START 1
% #define JVX_PROFILE_SEND_CHANNEL_STOP 2
% #define JVX_PROFILE_RECEIVE_CHANNEL_ENTER 3
% #define JVX_PROFILE_RECEIVE_CHANNEL_LEAVE 4
% #define JVX_PROFILE_PROCESS_BUFFER_START 5
% #define JVX_PROFILE_PROCESS_BUFFER_STOP 6
% #define JVX_PROFILE_EVENT_UNSENT_FRAME 7
% #define JVX_PROFILE_EVENT_MISSED_FRAME 8
% #define JVX_PROFILE_EVENT_NOT_READY_PROCESSING 9
sendchannel.sets = {};
sendchannel.oneSet.start = -1;
sendchannel.oneSet.stop = -1;
sendchannel.oneSet.chan = -1;

recvchannel.sets = {};
recvchannel.oneSet.start = -1;
recvchannel.oneSet.stop = -1;
recvchannel.oneSet.chan = -1;

procbuf.sets = {};
procbuf.oneSet.start = -1;
procbuf.oneSet.stop = -1;

events_unsent.sets = {};
events_missed.sets = {};
events_notready.sets = {};

enterProcess.oldVal = -1;
enterProcess.diffs.y = [];
enterProcess.diffs.x = [];

% Read content from logfile
[content] = jvxDataLogReader.jvx_read_file(''); 

sets = content{1}.data{1}.data;
times = content{1}.data{1}.time_us;

num1 = size(sets,1);
num2 = size(times,1);
num = min(num1, num2);

if(nargin < 1)
    startIdx = 1;
end
if(nargin < 2)
    stopIdx = num;
end
    
disp('-> Scanning recorded events');
progressNext = 0.0;

if(mode == 0)

    % Determine accurate timing
    for(ind = 1:num)
        progress = ind/num;
        if(progress > progressNext)
            disp([ num2str(round(progress*100)) '%']);
            progressNext = progressNext + 0.1;
        end
        
        txt1 = '-';
        txt2 = '-';
        txt3 = '-';
        if((ind > startIdx) && (ind < stopIdx))
            typeStored = sets(ind,1);
            chan = sets(ind,2);
            timeStored = double(times(ind))/1000;
            idCnt = sets(ind,4);
            
            txt2 = num2str(chan);
            txt3 = num2str(idCnt);
            txt4 = num2str(timeStored);
            
            switch(typeStored)
                case 1
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_START';
                    sendchannel.oneSet.start = timeStored;
                    sendchannel.oneSet.chan = chan;
                case 2
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_STOP';
                    sendchannel.oneSet.stop = timeStored;
                    if(sendchannel.oneSet.start > 0)
                        sendchannel.sets = [sendchannel.sets sendchannel.oneSet];
                    end
                    sendchannel.oneSet.start = -1;
                case 3
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_ENTER';
                    recvchannel.oneSet.stop = timeStored;
                    recvchannel.oneSet.chan = chan;
                    if(recvchannel.oneSet.start > 0)
                        recvchannel.sets = [recvchannel.sets recvchannel.oneSet];
                    end
                    recvchannel.oneSet.start = -1;
                case 4
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_LEAVE';
                    recvchannel.oneSet.start = timeStored;
                    
                case 5
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_START';
                    procbuf.oneSet.start = timeStored;
                    if(enterProcess.oldVal > 0)
                        enterProcess.diffs.y = [enterProcess.diffs.y (timeStored - enterProcess.oldVal)];
                        enterProcess.diffs.x = [enterProcess.diffs.x timeStored];
                    end
                    enterProcess.oldVal = timeStored;
                case 6
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_STOP';
                    procbuf.oneSet.stop = timeStored;
                    if(procbuf.oneSet.start > 0)
                        procbuf.sets = [procbuf.sets procbuf.oneSet];
                    end
                    procbuf.oneSet.start = -1;
                case 7
                    txt1 = 'JVX_PROFILE_EVENT_UNSENT_FRAME';
                    oneSet.start = timeStored;
                    oneSet.stop = timeStored;
                    events_unsent.set = [events_unsent.sets oneSet ];
                case 8
                    txt1 = 'JVX_PROFILE_EVENT_MISSED_FRAME';
                    oneSet.start = timeStored;
                    oneSet.stop = timeStored;
                    events_missed.sets = [events_missed.sets oneSet];
                case 9
                    txt1 = 'JVX_PROFILE_EVENT_NOT_READY_PROCESSING';
                    oneSet.start = timeStored;
                    oneSet.stop = timeStored;
                    events_notready.sets = [events_notready.sets oneSet];
            end
            if(verbose)
                disp([txt1 ':' txt2 ':' txt3 ':' txt4]);
            end
        end
    end
    
    disp('-> Plotting timing diagram');
    % Plot it
    numbershow = 4+2+1+3;
    ff = figure;
    hold on;
    
    ticks = [];
    tickNames = {};
    offset = 0;
    numEntries = 4;
    tickRoot = 'Wait time: recv-chan#';
    c = 'r';
    for(ind=1:size(recvchannel.sets,2))
        oneSet = recvchannel.sets{ind};
        yval_low = offset + double(oneSet.chan) + 1/3;
        yval_high = offset + double(oneSet.chan) + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    for(ind=1:numEntries)
        ticks = [ticks offset + ind - 1/2];
        tickNames = [tickNames; [tickRoot num2str(ind)]];
    end
    
    offset = offset + numEntries;
    numEntries = 2;
    tickRoot = 'Process time: send-chan#';
    c = 'g';
    for(ind=1:size(sendchannel.sets,2))
        oneSet = sendchannel.sets{ind};
        yval_low = offset + double(oneSet.chan) + 1/3;
        yval_high = offset + double(oneSet.chan) + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    for(ind=1:numEntries)
        ticks = [ticks offset + ind - 1/2];
        tickNames = [tickNames; [tickRoot num2str(ind)]];
    end
    
    offset = offset + numEntries;
    nmEntries = 1;
    tickRoot = 'Process time: bufProc';
    c = 'k';
    for(ind=1:size(procbuf.sets,2))
        oneSet = procbuf.sets{ind};
        yval_low = offset  + 1/3;
        yval_high = offset + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    ticks = [ticks offset + 1/2];
    tickNames = [tickNames; [tickRoot]];
    
    offset = offset + numEntries;
    numEntries = 1;
    tickRoot = 'Time instance: event_unsent';
    c = 'm';
    for(ind=1:size(events_unsent.sets,2))
        oneSet = events_unsent.sets{ind};
        yval_low = offset  + 1/3;
        yval_high = offset + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    ticks = [ticks offset + 1/2];
    tickNames = [tickNames; [tickRoot]];
    
    offset = offset + numEntries;
    numEntries = 1;
    tickRoot = 'Time instance: event_missed';
    c = 'm';
    for(ind=1:size(events_missed.sets,2))
        oneSet = events_missed.sets{ind};
        yval_low = offset  + 1/3;
        yval_high = offset + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    ticks = [ticks offset + 1/2];
    tickNames = [tickNames; [tickRoot]];
    
    offset = offset + numEntries;
    numEntries = 1;
    tickRoot = 'Time instance: event_notready';
    c = 'm';
    for(ind=1:size(events_notready.sets,2))
        oneSet = events_notready.sets{ind};
        yval_low = offset  + 1/3;
        yval_high = offset + 2/3;
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_low, c);
        plot([oneSet.start oneSet.stop], ones(1,2)*yval_high, c);
        plot([oneSet.start  oneSet.start ] , [yval_low yval_high], c);
        plot([oneSet.stop  oneSet.stop ] , [yval_low yval_high], c);
    end
    ticks = [ticks offset + 1/2];
    tickNames = [tickNames; [tickRoot]];
    
    set(gca,'YTick',ticks);
    set(gca,'YTickLabel',tickNames)
    title('Plot exact timing behavior');
    
    ff = figure;
    plot(enterProcess.diffs.x, enterProcess.diffs.y);
    title('Plot Delta between buffer switches');
    
elseif(mode == 1)
    
    timeStoredOld = 0;
    % Determine accurate timing
    for(ind = 1:num)
        progress = ind/num;
        if(progress > progressNext)
            disp([ num2str(round(progress*100)) '%']);
            progressNext = progressNext + 0.1;
        end
        
        txt1 = '-';
        txt2 = '-';
        txt3 = '-';
        
        if((ind > startIdx) && (ind < stopIdx))
            typeStored = sets(ind,1);
            chan = sets(ind,2);
            idCnt = sets(ind,4);
            timeStored = double(times(ind))/1000;
                 
            txt2 = num2str(chan);            
            txt3 = num2str(idCnt);
            txt4 = num2str(timeStored);
            switch(typeStored)
                case 1
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_START';
                case 2
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_STOP';
                case 3
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_ENTER';
                case 4
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_LEAVE';
                case 5
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_START';
                case 6
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_STOP';
                case 7
                    txt1 = 'JVX_PROFILE_EVENT_UNSENT_FRAME';
                case 8
                    txt1 = 'JVX_PROFILE_EVENT_MISSED_FRAME';
                case 9
                    txt1 = 'JVX_PROFILE_EVENT_NOT_READY_PROCESSING';
            end
            if(verbose)
                disp([txt1 ':' txt2 ':' txt3 ':' txt4]);
            end
            if(timeStoredOld >  timeStored)
                disp('Hier!');
            end
            timeStoredOld =  timeStored;
        end
    end
    
elseif(mode == 2)
    
    allFrameCntrs = [];
    timeStoredOld = 0;
    % Determine accurate timing
    for(ind = 1:num)
        progress = ind/num;
        if(progress > progressNext)
            disp([ num2str(round(progress*100)) '%']);
            progressNext = progressNext + 0.1;
        end
        
        txt1 = '-';
        txt2 = '-';
        txt3 = '-';
        
        if((ind > startIdx) && (ind < stopIdx))
            typeStored = sets(ind,1);
            chan = sets(ind,2);
            idCnt = sets(ind,4);
            inFrmCnt = sets(ind,5);
            
            timeStored = double(times(ind))/1000;
                 
            txt2 = num2str(chan);            
            txt3 = num2str(idCnt);
            txt4 = num2str(timeStored);
            switch(typeStored)
                case 1
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_START';
                case 2
                    txt1 = 'JVX_PROFILE_SEND_CHANNEL_STOP';
                case 3
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_ENTER';
                    if(chan == 0)
                        allFrameCntrs = [allFrameCntrs inFrmCnt];
                    end
                case 4
                    txt1 = 'JVX_PROFILE_RECEIVE_CHANNEL_LEAVE';
                case 5
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_START';
                case 6
                    txt1 = 'JVX_PROFILE_PROCESS_BUFFER_STOP';
                case 7
                    txt1 = 'JVX_PROFILE_EVENT_UNSENT_FRAME';
                case 8
                    txt1 = 'JVX_PROFILE_EVENT_MISSED_FRAME';
                case 9
                    txt1 = 'JVX_PROFILE_EVENT_NOT_READY_PROCESSING';
            end
            if(verbose)
                disp([txt1 ':' txt2 ':' txt3 ':' txt4]);
            end
            if(timeStoredOld >  timeStored)
                disp('Hier!');
            end
            timeStoredOld =  timeStored;
        end
    end
    
    ff = figure;
    plot(allFrameCntrs(2:end) - (allFrameCntrs(1:end-1)+1));
end
