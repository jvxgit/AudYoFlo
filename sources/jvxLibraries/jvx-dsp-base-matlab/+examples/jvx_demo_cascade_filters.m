in = [1 zeros(1, 127)];

disp(['Run the implementation in 2nd canonic implementation:']);
%b = [0.1301    0.3902    0.3902    0.1301];
%a = [1.0000   -0.2881    0.3553   -0.0267];
b = 1/2*[0.1301    0.3902    0.3902    0.1301];
a = 1/2*[1.0000   -0.2881    0.3553   -0.0267];
nSections = 1;
order = size(b,2)-1;

out_orig = in;
for(ind = 1:nSections)
    out_orig = filter(b, a, out_orig);
end

% Prepare coefficients for efficient implementation
bb = [];
aa = [];
for(ind = 1:nSections)
    bb = [ bb fliplr(b)];
    aa = [ aa fliplr(a)];
end
states = zeros(1,nSections*order);
[out_new, states, off] = jvx_dsp_base.filter.jvx_filter_cascade_2can_cb(nSections, order, states, aa, bb, in, 0);

figure; 
title('2nd can implementation vs. Matlab');
subplot(2,1,1);
plot(out_orig); hold on; plot(out_new, 'r:');
legend({'Matlab filter', 'JVX implementation'});
subplot(2,1,2);
plot(out_orig-out_new);
legend({'delta'});

%% =======================================================================
%% =======================================================================
%% =======================================================================

disp(['Run the implementation in first canonic implementation:']);

bb = [];
aa = [];
for(ind = 1:nSections)
    bb = [bb b(1)    fliplr(b(2:end))];
    aa = [aa a(1) fliplr(a(2:end))];
end
states = zeros(1,nSections*order);
[out_new, states] = jvx_dsp_base.filter.jvx_filter_cascade_1can(nSections, order, states, aa, bb, in);

figure; 
title('1st can implementation vs. Matlab');
subplot(2,1,1);
plot(out_orig); hold on; plot(out_new, 'r:');
legend({'Matlab filter', 'JVX implementation'});
subplot(2,1,2);
plot(out_orig-out_new);
legend({'delta'});

