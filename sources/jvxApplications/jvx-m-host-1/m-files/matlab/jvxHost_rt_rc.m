function [res, errMsg] = jvxHost_rt_rc(varargin)

errMsg = '';
if(nargin > 0)
    
    global jvxHost_rt_remote;
    if(isfield(jvxHost_rt_remote, 'hObject'))
        
        whattodo = varargin{1};
        if(ischar(whattodo))
            
            if(strcmp(whattodo, 'jvx_start_stop'))
                if(isfield(jvxHost_rt_remote, 'jvx_start_stop'))
                    try
                        res = jvxHost_rt_remote.jvx_start_stop();
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_start_stop>.'];
                end
                
            else
                res = false;
                errMsg = ['Remote call <' whattodo ' is not a valid entry point.'];                
            end
        else
            res = false;
            errMsg = ['Expects first parameter the function name of the remote call.'];
        end
    else
        res = false;
        errMsg = ['Remote function entry points not avalibale.'];
    end
else
    disp('Remote access to jvxHos_off host.');
end

                                
                    
    