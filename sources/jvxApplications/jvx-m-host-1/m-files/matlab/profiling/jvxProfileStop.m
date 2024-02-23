function jvxProfileStop(hdl)

	runAnyway = false;
	if(hdl.hooks.inUse == true)
		hdl.hooks.init.stop(hdl);
	else
		runAnyway = true;
	end

	if(~runAnyway)
		if(isfield(hdl.hooks, 'init') && isfield(hdl.hooks.init, 'run_anyway') && hdl.hooks.init.run_anyway)
			runAnyway = true;
		end
	end
	
	if(runAnyway)
		if(hdl.tp.numPlots > 0)
			if(isvalid(hdl.tp.fig))
				close(hdl.tp.fig);
			end
		end    
	end
end