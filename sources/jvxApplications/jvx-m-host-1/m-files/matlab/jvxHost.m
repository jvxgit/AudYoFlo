% Start RTProc host, 
% parameter 'mode':
% -> 'rt-audio': Start jvxHost in realtime-audio mode
% -> 'off-audio': Start jvxHost in offline mode
% -> everything else: start fully flexible host
function [hdlRtproc] = jvxHost(mode, varargin)

hdlRtproc = [];
if(nargin == 0)
    mode = 'rt-audio';
end

varargin_plus = {mfilename() varargin{:}};

if(strcmp(mode, 'rt-audio'))
    hdlRtproc = jvxHost_rt(varargin_plus);
elseif(strcmp(mode, 'off-audio'))
    hdlRtproc = jvxHost_off(varargin_plus);
else
    error('Wrong start option, accepts rt-audio or off-audio.');
end
