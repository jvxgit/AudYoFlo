function hdl_profile_data = jvxProfileNeleConfig()
    hdl_profile_data = [];
	
	% Allocate Debug handle
    hdl = [];
    hdl.hooks.inUse = false;
    
    global hdl_profile_data;
    if(~isempty(hdl_profile_data))
        if( ...
                (isfield(hdl_profile_data, 'cfg')) && ... 
                (isfield(hdl_profile_data, 'start')) && ...
                (isfield(hdl_profile_data, 'step')) && ...
                (isfield(hdl_profile_data, 'stop')))
            hdl.hooks.init = hdl_profile_data;
            hdl.hooks.inUse = true;
        end
    end
    
    if(hdl.hooks.inUse == true)
        [hdl] = hdl.hooks.init.cfg(hdl);
    else
        warning('We need to set the profiling configuration here!');
    end
        
   warning('Populate <hdl.cfg.tokenTp> at this point to control the degree of debug output!');
