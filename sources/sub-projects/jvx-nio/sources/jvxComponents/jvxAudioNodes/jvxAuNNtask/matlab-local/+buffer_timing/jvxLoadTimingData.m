function [allEvents] = jvxLoadTimingData(fname)
    if(nargin == 0)
        fname = '';
    end
    sig = jvxDataLogReader.jvx_read_file(fname);
    
    events = sig{1}.data{1}.data;
    numEvents = size(events, 1);
    allEvents = cell(numEvents,1);
    
    % entries_timing[0] = prof->fHeightOnEnter;
    % entries_timing[1] = prof->fHeightOnLeave;
    % entries_timing[2] = prof->state_onEnter;
    % entries_timing[3] = prof->state_onLeave;
    % entries_timing[4] = (jvxSize)prof->tStamp;
    % entries_timing[5] = JVX_ID_PRIM_AUDIO;
    % entries_timing[6] = average fillheight;
    
    for(ind=1:numEvents)
        
        % Sampes are copied from external to internal and the other direction.
        % Fillheight starts with 0 for direction external to internal
        
        % Fillheights in samples (external to internal)
        oneEvent.fHeightOnEnter = events(ind,1);
        oneEvent.fHeightOnLeave = events(ind,2);
        
        % States audio dispenser
        % JVX_AUDIO_DISPENSER_STATE_OFF = 0,
        % JVX_AUDIO_DISPENSER_STATE_CHARGING = 1,
        % JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD = 2,
        % JVX_AUDIO_DISPENSER_STATE_OPERATION = 3,
        % JVX_AUDIO_DISPENSER_STATE_UNCHARGING = 4
        oneEvent.stateOnEnter = events(ind,3);
        oneEvent.stateOnLeave = events(ind,4);
        
        % Timestamp in usec
        oneEvent.tstamp = events(ind,5);
        
        % Event types: 1: PRIM_THREAD
        % Event types: 2: SEC_THREAD
        oneEvent.operation = events(ind,6);
        
        oneEvent.average = events(ind,7);
        
        oneEvent.failreason = events(ind,8);
        
        allEvents{ind} = oneEvent;
        
    end
    