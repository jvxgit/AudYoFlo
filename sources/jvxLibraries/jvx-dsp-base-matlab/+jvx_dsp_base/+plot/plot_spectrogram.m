function [] = plot_spectrogram(sig, xstart_fac, xstop_fac, scale, clim_low, clim_up, fs, theTitle)

% =================================================================
% Define some show parameters
% =================================================================

eps = 1e-10;
lw=512;
ol=448;

% =================================================================

if(nargin < 8)
    theTitle = 'Spectrogram';
end

if(nargin < 7)
    fs = 16000;
end

if(nargin < 6)
    clim_up = 60;
end

if(nargin < 5)
    clim_low = 0;
end
    
if(nargin < 4)
    scale = 32678; % 16 bit
end

if(nargin < 3)
    xstop_fac = 1; 
end

if(nargin < 2)
    xstart_fac = 0;
end

% =================================================================
% =================================================================

% Preprocess signal
clf
sig=sig(round(xstart_fac*length(sig))+1:round(xstop_fac*length(sig)));
sig = sig / max(abs(sig));
sig = sig * scale;
S=10*log10(abs(spectrogram(sig,hanning(lw),ol, 'yaxis')+eps));

% Image limits
cl =[clim_low clim_up];

% Axes conversions
yS=0:fs/2/size(S,1):fs/2-fs/2/size(S,1);
xS=(0:length(sig)/size(S,2):length(sig)-length(sig)/size(S,2))/fs;

% Now plot
%figure(1);
%h1=gca;
imagesc(xS,yS,S,cl);
set(gca,'YDir','normal');
title(theTitle);
ylabel('Frequency [Hz]')
xlabel('Time [sec]')
colormap jet;
