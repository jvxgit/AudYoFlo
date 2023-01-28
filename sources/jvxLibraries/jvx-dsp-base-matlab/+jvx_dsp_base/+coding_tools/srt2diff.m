%% Function to load two srt files and to plot the difference between the signals.
% The purpose of this function is to visualize very small difference on a waveform 
% level.
% The filenames are specified as prefixes, the ending srt will be added. The 
% first differences may be plotted if desired. Also, the number of loaded samples from 
% file may be limited.
function [sig11L, sig12L, diff] = srt2diff(fNameInPrefix1, fNameInPrefix2, fs, plotit, ddelta, lload)
    
    if nargin < 3
        fs = 48000;
    end
    
    if nargin < 4
        plotit = false;
    end
    
    if nargin < 5
        ddelta = 0;
    end
    
    if(nargin < 6)
        lload = inf;
    end
    
    % ==============================================================
    
    fID1 = fopen([fNameInPrefix1 '.srt'], 'rb');
    sig161 = fread(fID1, lload, 'int16');
    fclose(fID1);
    sig1 = sig161 / 2^15;
    chan11 = sig1';
    
    fID2 = fopen([fNameInPrefix2 '.srt'], 'rb');
    sig162 = fread(fID2, lload, 'int16');
    fclose(fID2);
    sig2 = sig162 / 2^15;
    chan12 = sig2';
    
    ll1 = size(chan11,2);
    ll2 = size(chan12, 2);
    llmin = min(ll1, ll2);
       
    sig11L = chan11(1:llmin);
    sig12L = chan12(1:llmin);
    diff = abs(sig11L-sig12L);
    
    idxs = 1;
    llShow = llmin;
    if(ddelta > 0)
        idxs = find(diff ~= 0); 
        if(length(idxs))
            idxs = max(1, idxs(1)- 10 +1);
            llShow = min(llmin, idxs + ddelta -1);
        end
    end
    
    if(plotit)
        figure;
        hold on;
        plot(sig11L(idxs:llShow), 'r');
        plot(sig12L(idxs:llShow), 'g:');
        plot(diff(idxs:llShow), 'k');
    end
