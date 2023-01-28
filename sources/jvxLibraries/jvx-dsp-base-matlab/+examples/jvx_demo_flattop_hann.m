numWin = 100;
bsize = 512;
fshift = 123;

outBuf = zeros(1, fshift*(numWin-1) + bsize);
oneBuf = zeros(1, fshift*(numWin-1) + bsize);

win = jvx_dsp_base.windows.jvx_create_flattop_hann(bsize, fshift);
cnt = 0;

figure; 
subplot(2,1,1);
hold on;

for(ind = 1:numWin)
    cntStart = cnt+1;
    cntStop = cnt+ bsize;

    oneBuf = oneBuf * 0;
    
    oneBuf(cntStart:cntStop) =  win;
    plot(oneBuf, 'color', [rand() rand() rand()]);
    
    outBuf =  outBuf + oneBuf;
    
    cnt = cnt + fshift;
end

subplot(2,1,2);
plot(outBuf);

