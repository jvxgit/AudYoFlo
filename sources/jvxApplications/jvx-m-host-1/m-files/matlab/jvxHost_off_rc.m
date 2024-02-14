function [res, errMsg] = jvxHost_off_rc(varargin)

errMsg = '';
if(nargin > 0)
    
    global jvxHost_off_remote;
    if(isfield(jvxHost_off_remote, 'hObject'))
        
        whattodo = varargin{1};
        if(ischar(whattodo))
            
            if(strcmp(whattodo, 'jvx_run'))
                if(isfield(jvxHost_off_remote, 'jvx_run'))
                    try
                        res = jvxHost_off_remote.jvx_run();
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_run>.'];
                end
                
            elseif(strcmp(whattodo, 'jvx_force_stop'))
                if(isfield(jvxHost_off_remote, 'jvx_force_stop'))
                    try
                        res = jvxHost_off_remote.jvx_force_stop();
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_force_stop>.'];
                end    
                
            elseif(strcmp(whattodo, 'jvx_set_samplerate'))
                value = varargin{2};
                if(isfield(jvxHost_off_remote, 'jvx_set_samplerate'))
                    try
                        res = jvxHost_off_remote.jvx_set_samplerate(value);
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_set_samplerate>.'];
                end
                
            elseif(strcmp(whattodo, 'jvx_set_buffersize'))
                value = varargin{2};
                if(isfield(jvxHost_off_remote, 'jvx_set_buffersize'))
                    try
                        res = jvxHost_off_remote.jvx_set_buffersize(value);
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_set_buffersize>.'];
                end
              
            elseif(strcmp(whattodo, 'jvx_mesgq_immediate'))
                
                if(isfield(jvxHost_off_remote, 'jvx_mesgq_immediate'))
                    try
                        res = jvxHost_off_remote.jvx_mesgq_immediate();
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_msgq_immediate>.'];
                end
      
            elseif(strcmp(whattodo, 'jvx_input_var'))
                
                name_var = varargin{2};

                if(isfield(jvxHost_off_remote, 'jvx_input_var'))
                    try
                        if(size(varargin,2) >= 3)
                            rate = varargin{3};
                            res = jvxHost_off_remote.jvx_input_var(name_var,rate);
                        else
                            res = jvxHost_off_remote.jvx_input_var(name_var);
                        end
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_input_var>.'];
                end
                
             elseif(strcmp(whattodo, 'jvx_input_var_direct'))
                
                name_var = varargin{2};

                if(isfield(jvxHost_off_remote, 'jvx_input_var_direct'))
                    try
                        if(size(varargin,2) >= 3)
                            rate = varargin{3};
                            res = jvxHost_off_remote.jvx_input_var_direct(name_var,rate);
                        else
                            res = jvxHost_off_remote.jvx_input_var_direct(name_var);
                        end
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_input_var_direct>.'];
                end   
                
            elseif(strcmp(whattodo, 'jvx_set_engage_matlab'))
                                
                if(isfield(jvxHost_off_remote, 'jvx_set_engage_matlab'))
                    try
                        if(size(varargin,2) >= 1)
                            to_set = varargin{2};
                            res = jvxHost_off_remote.jvx_set_engage_matlab(to_set);
                        else
                            res = jvxHost_off_remote.jvx_set_engage_matlab();
                        end
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_input_var>.'];
                end
                
            elseif(strcmp(whattodo, 'jvx_export_output_signal'))
                                
                if(isfield(jvxHost_off_remote, 'jvx_export_signal'))
                    try
                        exportId = 1;
                        fname = 'data_exported.wav';
                        if(size(varargin,2) > 1)
                            exportId = varargin{2};
                        end
                        if(size(varargin,2) > 2)
                            fname = varargin{3};
                        end
                        res = jvxHost_off_remote.jvx_export_signal(true, exportId, fname);
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_export_signal>.'];
                end
                
                elseif(strcmp(whattodo, 'jvx_export_input_signal'))
                                
                if(isfield(jvxHost_off_remote, 'jvx_export_signal'))
                    try
                        exportId = 1;
                        fname = 'data_exported.wav';
                        if(size(varargin,2) > 1)
                            exportId = varargin{2};
                        end
                        if(size(varargin,2) > 2)
                            fname = varargin{3};
                        end
                        res = jvxHost_off_remote.jvx_export_signal(false, exportId, fname);
                    catch ME
                        res = false;
                        errMsg = ME.message;
                    end
                else
                    res = false;
                    errMsg = ['No entry point <jvx_export_signal>.'];
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

                                
                    
    