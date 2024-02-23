function hdl = jvxProfileConfig(hdl)
   
    initHooks = false;
    if(~isempty(hdl))
        if( ...
                (isfield(hdl, 'cfg')) && ... 
                (isfield(hdl, 'start')) && ...
                (isfield(hdl, 'step')) && ...
                (isfield(hdl, 'stop')))
            initHooks = true;
        end
    end
    
    if(initHooks)
        hdl.hooks.init = hdl;
        hdl.hooks.inUse = true;
    else
        hdl.hooks.inUse = false;
    end       
    
    % Set test points to "none" by default
    hdl.cfg = [];
    hdl.cfg.tokenTp.SELECTION_BITFIELD = '0x0';
    
    if(hdl.hooks.inUse == true)
        [hdl] = hdl.hooks.init.cfg(hdl);
    else        
        warning('We need to set the profiling configuration here!');
    end       
