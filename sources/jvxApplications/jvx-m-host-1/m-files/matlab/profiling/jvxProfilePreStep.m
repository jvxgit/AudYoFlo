function [hdl] = jvxProfilePreStep(hdl)

	% Get the frame count
	global inProcessing;
	hdl.fCnt = inProcessing.frameCnt;

	runAnyway = false;

	if(hdl.hooks.inUse == true)
		[hdl] = hdl.hooks.init.prestep(hdl);
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

