function [hdl] = jvxProfileStart(hdl)

	global inProcessing;
	hdl.nFrames = inProcessing.in_frames;
	hdl.bsize = inProcessing.bsize;
	hdl.tp.tokenTp = hdl.cfg.tokenTp.SELECTION_BITFIELD;

	% =========================================
	runAnyway = false;
	if(hdl.hooks.inUse == true)
		[hdl] = hdl.hooks.init.start(hdl);
	else
		runAnyway = true;
	end

	if(~runAnyway)
		if(isfield(hdl.hooks, 'init') && isfield(hdl.hooks.init, 'run_anyway') && hdl.hooks.init.run_anyway)
			runAnyway = true;
		end
	end

	if(runAnyway)
	
		% Generic output here!!
		hdl.tp.numPlots = 0;
		hdl.tp.doPlot = true;
		hdl.tp.plotFrames = false;
		for(idx=0:63)
			if(jvxBitField.jvx_test(hdl.tp.tokenTp, idx))
				hdl.tp.numPlots = hdl.tp.numPlots + 1;
			end
		end
		if(hdl.tp.numPlots > 0)
			hdl.tp.fig = figure;
		end
	end
end

