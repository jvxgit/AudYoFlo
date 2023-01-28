function jvxShowTimingLogFile(allEvents, idxStart, idxStop, bsize, rate) 

if(nargin < 4)
    bsize = 1024;
end

if(nargin < 5)
    rate = 48000;
end

numEvents = size(allEvents, 1);
timesArray = zeros(1, numEvents);
operArray = zeros(1, numEvents);
for(ind=1:numEvents)
    timesArray(ind) = allEvents{ind}.tstamp;
    operArray(ind) = allEvents{ind}.operation;
end
        
[notused, sortidxs] = sort(timesArray);
pri_idxs = find(operArray == 1);
sec_idxs = find(operArray == 2);

x_values = zeros(1, numEvents*2);
y_values_fh = zeros(1, numEvents*2);
y_values_av = zeros(1, numEvents*2);
y_values_st = zeros(1, numEvents*2);
y_values_op = zeros(1, numEvents*2);

for(ind=0:numEvents-1)
    
    idxEvent = sortidxs(ind+1);
    theEvent = allEvents{idxEvent};
    
    y_values_fh(ind*2 + 1) = theEvent.fHeightOnEnter;
    y_values_fh(ind*2 + 2) = theEvent.fHeightOnLeave;
    
    y_values_av(ind*2 + 1) = theEvent.average;
    y_values_av(ind*2 + 2) = theEvent.average;
   
    y_values_st(ind*2 + 1) = theEvent.stateOnEnter;
   y_values_st(ind*2 + 2) = theEvent.stateOnLeave;
   
   y_values_op(ind*2 + 1) = theEvent.operation;
   y_values_op(ind*2 + 2) = theEvent.operation;

   y_values_fa(ind*2 + 1) = theEvent.failreason;
   y_values_fa(ind*2 + 2) = theEvent.failreason;
   
   x_values(ind*2 + 1) = double(theEvent.tstamp) *1e-6;
   x_values(ind*2 + 2) = double(theEvent.tstamp) *1e-6;
   
end

num_pri_events = size(pri_idxs,2);
x_values_pri = zeros(1, num_pri_events);
for(ind=1:num_pri_events)
    idx_pri = pri_idxs(ind);
    theEvent = allEvents{idx_pri};
    x_values_pri(ind) = double(theEvent.tstamp) *1e-6;
end
tinit_pri = x_values_pri(1);
x_values_pri = x_values_pri - tinit_pri;
deltat_pri = bsize/rate;
x_values_pri_est = [0:num_pri_events-1]*deltat_pri;
x_values_pri_diff = x_values_pri-x_values_pri_est;
x_values_pri_diff_mean = sum(x_values_pri_diff) / num_pri_events;
x_values_pri_est = x_values_pri_est + x_values_pri_diff_mean;
x_values_pri_diff = x_values_pri-x_values_pri_est;

num_sec_events = size(sec_idxs,2);
x_values_sec = zeros(1, num_sec_events);
for(ind=1:num_sec_events)
    idx_sec = sec_idxs(ind);
    theEvent = allEvents{idx_sec};
    x_values_sec(ind) = double(theEvent.tstamp) *1e-6;
end
tinit_sec = x_values_sec(1);
x_values_sec = x_values_sec - tinit_sec;
deltat_sec = bsize/rate;
x_values_sec_est = [0:num_sec_events-1]*deltat_sec;
x_values_sec_diff = x_values_sec-x_values_sec_est;
x_values_sec_diff_mean = sum(x_values_sec_diff) / num_sec_events;
x_values_sec_est = x_values_sec_est + x_values_sec_diff_mean;
x_values_sec_diff = x_values_sec-x_values_sec_est;

% =================================================================0
if(nargin < 2)
    idxStart = 1;
end
if(nargin < 3)
    idxStop = numEvents*2;
end

if(idxStart < 1)
    idxStart = 1;
end
if(idxStop < 1)
    idxStop = 1;
end
if(idxStart > numEvents*2)
    idxStart = numEvents*2;
end
if(idxStop > numEvents*2)
    idxStop = numEvents*2;
end

figure;
subplot(4,1,1);
plot(x_values(idxStart:idxStop), y_values_op(idxStart:idxStop), 'r');
yticks([1 2]);
yticklabels({'Prim','Sec'});
legend({'Operations'});

subplot(4,1,2);
plot(x_values(idxStart:idxStop), y_values_st(idxStart:idxStop), 'g');
yticks([0 1 2 3 4])
yticklabels({'OFF', 'CHARGING', 'CHARGING-HELD', 'OPERATION', 'UNCHARGING'});
legend({'States'});

subplot(4,1,3);
plot(x_values(idxStart:idxStop), y_values_fh(idxStart:idxStop), 'k:');
hold on;
plot(x_values(idxStart:idxStop), y_values_av(idxStart:idxStop), 'r');
legend({'Fill Heights', 'Average Height'});

subplot(4,1,4);
plot(x_values(idxStart:idxStop), y_values_fa(idxStart:idxStop), 'k');
yticks([0 1 2 ])
yticklabels({'OK', 'ABORT', 'UNDER/OVERFLOW'});
legend({'Failresaons'});

% =================================================================0

figure;
subplot(2,1,1);
plot(x_values_pri_est, x_values_pri, 'k:');
hold on;
plot(x_values_pri_est, x_values_pri_est, 'r');

subplot(2,1,2);
plot(x_values_pri_est, x_values_pri_diff, 'r');
hold on;
plot(x_values_pri_est, ones(1, num_pri_events)* deltat_pri, 'k:');
plot(x_values_pri_est, ones(1, num_pri_events)* 0, 'k:');
plot(x_values_pri_est, ones(1, num_pri_events)* -deltat_pri, 'k:');

% =================================================================0

figure;
subplot(2,1,1);
plot(x_values_sec_est, x_values_sec, 'k:');
hold on;
plot(x_values_sec_est, x_values_sec_est, 'r');

subplot(2,1,2);
plot(x_values_sec_est, x_values_sec_diff, 'r');
hold on;
plot(x_values_sec_est, ones(1, num_sec_events)* deltat_sec, 'k:');
plot(x_values_sec_est, ones(1, num_sec_events)* 0, 'k:');
plot(x_values_sec_est, ones(1, num_sec_events)* -deltat_sec, 'k:');


