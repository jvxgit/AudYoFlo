% jvxInitCallback.m

% Load test file into global variable in
global in;
global dbg;

LLSIG = 480000;

% Produce x(k)
sigx = randn(1, LLSIG);

% Produce y(k)
filt = [zeros(1,45) 1 zeros(1, 92) -0.5 zeros(1, 56) 0.25];

dbg.filt_orig = filt;

sigy = filter(filt, 1, sigx);

minSz = min(size(sigx,2), size(sigy,2));
in = [sigx(1:minSz); sigy(1:minSz)];

