function [sim_events] = jvxSimulateBufferTiming()

tracksignals = true;
bsize_t1 = 1024;
bsize_t2 = 257;
rate_t12 = 48000;
jitter_t1 = 0.0;
jitter_t2 = 0.0;
num_steps = 10000;
start_t1 = 0/rate_t12;
start_t2 = bsize_t2/2/rate_t12;
startthreshold = bsize_t1 + bsize_t2;
buffersizemax = 2 * startthreshold;
% ============================================

sim_events = [];
deltat1 = bsize_t1/rate_t12;
deltat2 = bsize_t2/rate_t12;
tnext_t1_cl = start_t1;
tnext_t2_cl = start_t2;
ind = 1;

tnext_t1 = tnext_t1_cl;
tnext_t2 = tnext_t2_cl;

global jvx_host_call_global;
[suc] = jvx_host_call_global('external_call', 'CjvxAuNNtaskm', 'start_audiosample_dispenser', bsize_t1, rate_t12, buffersizemax, startthreshold);
if(~suc)
    return;
end

sim_events = cell(num_steps,1);
rng('default');
hdl.phase_sec = 0;
hdl.f_sec = 1000;
hdl.ampl_sec = 0.5;
hdl.in_sec = [];
hdl.out_sec = [];
hdl.in_cnt_sec = [];
hdl.out_cnt_sec = [];
hdl.hw_cnt_sec = 0;

hdl.phase_pri = 0;
hdl.f_pri = 250;
hdl.ampl_pri = 0.5;
hdl.in_pri = [];
hdl.out_pri = [];
hdl.in_cnt_pri = [];
hdl.out_cnt_pri = [];
hdl.hw_cnt_pri = 0;

hdl.fs = rate_t12;
hdl.tracksignals = tracksignals;

while(1)
    tnext = min(tnext_t1, tnext_t2);
    move_t1 = false;
    move_t2 = false;
    if((tnext == tnext_t1) && (tnext == tnext_t2))
        if(rand() > 0.5)
            [oneEvent, hdl] = process_t( tnext, 1, bsize_t1, hdl);
            sim_events{ind} = oneEvent;
            ind = ind +1;
            if(ind > num_steps)
                break;
            end
            [oneEvent, hdl] = process_t(tnext, 2, bsize_t2, hdl);
            sim_events{ind} = oneEvent;
            ind = ind +1;
            if(ind > num_steps)
                break;
            end
            
            move_t1 = true;
            move_t2 = true;
        else
            [oneEvent, hdl] = process_t( tnext, 2, bsize_t2, hdl);
            sim_events{ind} = oneEvent;
            ind = ind +1;
            if(ind > num_steps)
                break;
            end
            [oneEvent, hdl] = process_t(tnext, 1,bsize_t1, hdl);
            sim_events{ind} = oneEvent;
            ind = ind +1;
            if(ind > num_steps)
                break;
            end
             move_t1 = true;
            move_t2 = true;
        end
    elseif(tnext == tnext_t1)
        [oneEvent, hdl] = process_t(tnext, 1, bsize_t1, hdl);
        sim_events{ind} = oneEvent;
        ind = ind +1;
        if(ind > num_steps)
            break;
        end
         move_t1 = true;
            
    elseif(tnext == tnext_t2)
        [oneEvent, hdl] = process_t( tnext, 2, bsize_t2, hdl);
        sim_events{ind} = oneEvent;
        ind = ind +1;
        if(ind > num_steps)
            break;
        end
        move_t2 = true;
    else
        error('Haeh?');
    end
    
    if(move_t1)
        rr = (rand() * 2)-1;
        tnext_t1_cl = tnext_t1_cl + deltat1;
        tnext_t1 = tnext_t1_cl + (jitter_t1*rr) * deltat1;
    end
    if(move_t2)
        rr = (rand() * 2)-1;
        tnext_t2_cl = tnext_t2_cl + deltat2;
        tnext_t2 = tnext_t2_cl + (jitter_t2*rr) * deltat2;
    end
end
[a, b] = jvx_host_call_global('external_call', 'CjvxAuNNtaskm', 'stop_audiosample_dispenser');
if(tracksignals)
    figure;
    
    % SHow sec input and pri output
    subplot(4,1,1);
    plot(hdl.in_sec, 'k');
    hold on;
    plot(hdl.out_pri, 'r:');
    subplot(4,1,2);
    plot(hdl.in_cnt_sec, hdl.in_sec, 'k');
    hold on;
    plot(hdl.out_cnt_pri, hdl.out_pri, 'r:');
    
    subplot(4,1,3);
    plot(hdl.in_pri, 'k');
    hold on;
    plot(hdl.out_sec, 'g:');
    subplot(4,1,4);
    plot(hdl.in_cnt_pri, hdl.in_pri, 'k');
    hold on;
    plot(hdl.out_cnt_sec, hdl.out_sec, 'g:');
    
end

%% ============================================================

function [oneEvent, hdl] = process_t( tstamp, event_type, bsize, hdl)
    
    global jvx_host_call_global;
    
    tstamp_usec = tstamp * 1e6;
    switch(event_type)
        case 1
            in_cnt_pri = hdl.hw_cnt_pri + [0:bsize-1];
            hdl.hw_cnt_pri = hdl.hw_cnt_pri + bsize;
            if(hdl.tracksignals)
                phase_loc_pri = hdl.phase_pri + [0:bsize-1] * 2*pi*hdl.f_pri/hdl.fs;
                hdl.phase_pri = hdl.phase_pri + bsize * 2*pi*hdl.f_pri/hdl.fs;
                in_loc_pri = cos(phase_loc_pri) * hdl.ampl_pri;
                hdl.f_pri = hdl.f_pri -1;
            else
                in_loc_pri = zeros(1, bsize);
            end
            
            [suc, out_loc_pri, out_cnt_pri, ...
                oneEvent.fHeightOnEnter, ...
                oneEvent.fHeightOnLeave, ...
                oneEvent.stateOnEnter, ...
                oneEvent.stateOnLeave, ...
                oneEvent.average, ...
                oneEvent.failreason] = jvx_host_call_global('external_call', ...
                'CjvxAuNNtaskm', 'process_audiosample_dispenser_pri', in_loc_pri, in_cnt_pri, int64(tstamp_usec));
            if(hdl.tracksignals)
                hdl.in_pri = [hdl.in_pri in_loc_pri];
                hdl.in_cnt_pri = [hdl.in_cnt_pri in_cnt_pri];
                hdl.out_pri = [hdl.out_pri out_loc_pri];
                hdl.out_cnt_pri = [hdl.out_cnt_pri out_cnt_pri];
            end
            
        case 2
            
            in_cnt_sec = hdl.hw_cnt_sec + [0:bsize-1];
            hdl.hw_cnt_sec = hdl.hw_cnt_sec + bsize;
            if(hdl.tracksignals)
                phase_loc_sec = hdl.phase_sec + [0:bsize-1] * 2*pi*hdl.f_sec/hdl.fs;
                hdl.phase_sec = hdl.phase_sec + bsize * 2*pi*hdl.f_sec/hdl.fs;
                in_loc_sec = cos(phase_loc_sec) * hdl.ampl_sec;
                hdl.f_sec = hdl.f_sec + 1;
            else
                in_loc_sec = zeros(1, bsize);
            end
           
           [suc, out_loc_sec, out_cnt_sec, ...
               oneEvent.fHeightOnEnter, ...
                oneEvent.fHeightOnLeave, ...
                oneEvent.stateOnEnter, ...
                oneEvent.stateOnLeave, ...
                oneEvent.average, ...
                oneEvent.failreason] = jvx_host_call_global('external_call', ...
                'CjvxAuNNtaskm', 'process_audiosample_dispenser_sec', in_loc_sec, in_cnt_sec, int64(tstamp_usec));
            
            if(hdl.tracksignals)
               hdl.in_sec = [hdl.in_sec in_loc_sec];
               hdl.in_cnt_sec = [hdl.in_cnt_sec in_cnt_sec];              
               hdl.out_sec = [hdl.out_sec out_loc_sec];
               hdl.out_cnt_sec = [hdl.out_cnt_sec out_cnt_sec];
            end
        otherwise
            error('Wieder Haeh?');
    end
    
    if(oneEvent.fHeightOnLeave > 4096)
        disp('Stop');
    end

    

    % States audio dispenser 
    % oneEvent.stateOnEnter, oneEvent.stateOnLeave
    % JVX_AUDIO_DISPENSER_STATE_OFF = 0,
	% JVX_AUDIO_DISPENSER_STATE_CHARGING = 1,
	% JVX_AUDIO_DISPENSER_STATE_CHARGING_HELD = 2,
	% JVX_AUDIO_DISPENSER_STATE_OPERATION = 3,
	% JVX_AUDIO_DISPENSER_STATE_UNCHARGING = 4
    
    % Timestamp in usec
    oneEvent.tstamp = tstamp_usec;
    
    % Event types: 1: PRIM_THREAD
    % Event types: 2: SEC_THREAD
    oneEvent.operation = event_type;
  
