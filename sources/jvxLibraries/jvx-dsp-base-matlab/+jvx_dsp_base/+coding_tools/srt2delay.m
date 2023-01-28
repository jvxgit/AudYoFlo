%% This function opens two short files (raw data, little-endian, 16 bits)
% and computes the delay by deriving the xcorr function and finding the maximum
% in that buffer. 
% The input signals are read from file using the file prefix extended by the 
% ending of ".srt". Alternatively, buffers of dimension 1xL may be passed. 
% The resulting delay is the amount of samples by which the second inputz is 
% delayed compared to the first input. Negative values are allowed.
% The function tends to run out of memory. Therefore, the number of samples
% may be limited(argument sig1max). Also the xcorr buffer may be limited to the
% number of entries argument corrMax. The xcorr buffer may be plotted.
function [delay] = srt2delay(fNameInPrefix1, fNameInPrefix2, siglmax, corrMax, plotit)
     
	% Some default arguments
    if(nargin < 3)
        siglmax = inf;
    end
    
    if(nargin < 4)
        corrMax = 2048;
    end

    if(nargin < 5)
        plotit = true;
    end

	% File handling
	if(isstring(fNameInPrefix1))
		fID1 = fopen([fNameInPrefix1 '.srt'], 'rb');
		sig161 = fread(fID1, siglmax, 'int16');
		fclose(fID1);
		sig1 = sig161 / 2^15;
		chan11 = sig1';
	else
		chan11 = fNameInPrefix1;
	end
    
    if(isstring(fNameInPrefix2))
		fID2 = fopen([fNameInPrefix2 '.srt'], 'rb');
		sig162 = fread(fID2, siglmax, 'int16');
		fclose(fID2);
		sig2 = sig162 / 2^15;
		chan12 = sig2';
	else
		chan12 = fNameInPrefix2;
	end
    	    
	% Compute the xcorr buffer
    llsig = min(length(chan11), length(chan12));
    llsig = min(siglmax, llsig);
    [dd] = xcorr(chan11(1:llsig), chan12(1:llsig), corrMax);

	%plot it if desired
	if(plotit)
		figure; 
		subplot(2,1,1);
		plot(chan11, 'r');
		hold on;
		plot(chan12, 'g');
		subplot(2,1,2);
		plot(dd);
		delayidx = find(dd == max(dd));
		delay = delayidx - corrMax -1;
		delay = -delay;
    end
	