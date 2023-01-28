function [hObject, handles] = settoneutral(hObject, handles)
[a b]= jvxJvxHostRt('info');
if(a)
   
    % The host has been initialized before, re-initialize
    [a b]= jvxJvxHostRt('status_process_sequence');
    if(~a)
        jvxHost_display_error('Error code 0x8', b);       
    else
        
        % Stop sequencer here TODO
    end
    
    [a b]= jvxJvxHostRt('info_sequencer');
    if(~a)
        jvxHost_display_error('Error code 0x8', b);
    end
end