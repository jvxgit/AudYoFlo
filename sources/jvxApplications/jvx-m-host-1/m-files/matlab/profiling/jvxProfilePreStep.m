function [hdl] = jvxProfilePreStep(hdl)

	% Get the frame count
	global inProcessing;
	hdl.fCnt = inProcessing.frameCnt;

	runAnyway = false;

	if(hdl.hooks.inUse == true)
        if(isfield(hdl.hooks.init, 'prestep'))
            if(isa(hdl.hooks.init.prestep, 'function_handle'))
                [hdl] = hdl.hooks.init.prestep(hdl);
            end            
        end
	else
		runAnyway = true;
	end

	if(~runAnyway)
		if(isfield(hdl.hooks, 'init') && isfield(hdl.hooks.init, 'run_anyway') && hdl.hooks.init.run_anyway)
			runAnyway = true;
		end
	end

	if(runAnyway)
		% =========================================
		% Frame by frame comparison with testpoints
    end
end

