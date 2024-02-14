function [passed] = jvx_compare_wavs(file1, file2, maxDeviation, plotit, ignoreDifferentSizes)
  
passed = false;
if(nargin < 4)
    plotit = true;
end

if(nargin < 5)
    ignoreDifferentSizes = true;
end

if(nargin < 3)
    maxDeviation = 1e-16;
end
[sig1, fs1] = audioread(file1);
[sig2, fs2] = audioread(file2);

sig1 = sig1';
sig2 = sig2';

if(size(sig1,1) ~= size(sig2,1))
    disp(['Size mismatch, M = ' num2str(size(sig1,1)) ' vs ' num2str(size(sig2,1))]);
    if(ignoreDifferentSizes)
        MMin = min(size(sig1,1), size(sig2,1));
        sig1 = sig1(1:MMin,:);
        sig2 = sig2(1:MMin,:);
    else
        passed = false;
        return;
    end
end

if(size(sig1,2) ~= size(sig2,2))
    disp(['Size mismatch, N = ' num2str(size(sig1,2)) ' vs ' num2str(size(sig2,2))]);
    if(ignoreDifferentSizes)
        MMin = min(size(sig1,2), size(sig2,2));
        sig1 = sig1(:,1:MMin);
        sig2 = sig2(:,1:MMin);
    else
        passed = false;
        return;
    end
end

if(fs1 ~= fs2)
    disp(['Size mismatch, fs = ' num2str(fs1) ' vs ' num2str(fs2)]);
    passed = false;
    return;
end

if(plotit)
    figure;    
end

passed = true;
for(ind = 1:size(sig1, 1))
    
    diff = sig1(ind,:) - sig2(ind,:);
    diff = abs(diff);
    idxDev = find(diff > maxDeviation);
    
    if(plotit)
        subplot(size(sig1, 1), 1, ind);
        plot(diff);
    end
    
    if(~isempty(idxDev))
        disp(['Deviation at position ' num2str(ind) ',' num2str(idxDev)]);
        passed = false;
    end
end
end
