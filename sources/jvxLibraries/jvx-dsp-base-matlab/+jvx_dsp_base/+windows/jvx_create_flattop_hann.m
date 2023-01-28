function [win] = fthann(bsize, fshift)

win = zeros(1, bsize);

fitsin = ceil((bsize/2)/fshift);
l_plus_ft = fitsin * fshift;

for(ind = 0:bsize - l_plus_ft-1)
    tmp = 0.5*(1-cos(2*pi*ind/(2.0*(bsize - l_plus_ft)-2.0)));
    win(ind+1) = tmp;
    win(bsize-ind) = tmp;
end

for(ind = 0:2*l_plus_ft - bsize-1)
    win(ind +bsize - l_plus_ft +1 ) =1;
end

win = win / fitsin;
