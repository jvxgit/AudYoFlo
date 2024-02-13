function [hdl_profile_data] = jvxProfileNeleStep(hdl_profile_data)

	% Get the frame count
	global inProcessing;
	hdl.fCnt = inProcessing.frameCnt;

	runAnyway = false;

	if(hdl.hooks.inUse == true)
		[hdl] = hdl.hooks.init.step(hdl);
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
		if(isfield(hdl, 'tp'))
			nPlots = hdl.tp.numPlots;
        
			if(nPlots ~= 0)
            
				if(hdl.tp.doPlot)
					figure(hdl.tp.fig);
				end
            
				hdl.tp.plotFrames = true;
            
				cnt = 1;
            
				for(idx = 0:63)
					if(jvxBitField.jvx_test(hdl.tp.tokenTp, idx))
						nmBufPrefix = ['aec_tp' num2str(idx+1)];
						nmBufC = [nmBufPrefix '_c'];
						nmBufM = [nmBufPrefix '_m'];
						lgPrefix = ['TP' num2str(idx+1)];
						subplot(nPlots,1,cnt);
						if((isfield(hdl.tp, nmBufC)) && (isfield(hdl.tp, nmBufM)))
							sig_c = 0;
							sig_m = 0;
							cmd = ['sig_c = hdl.tp.' nmBufC ';'];
							eval(cmd);
							cmd = ['sig_m = hdl.tp.' nmBufM ';'];
							eval(cmd);
							diff = abs(sig_c-sig_m);
							delta = diff * diff';
							ssig = sig_c * sig_c';
							if(hdl.tp.doPlot)
								hold off;
								plot(abs(sig_c-sig_m), 'k', 'LineWidth', 3);
								if(hdl.tp.plotFrames)
									hold on;
									plot(abs(sig_c), 'g');
									plot(abs(sig_m), 'r:');
									legend({[lgPrefix ' comparison'], [lgPrefix ' c'], [lgPrefix ' m']});
								else
									legend({[lgPrefix ' comparison']});
								end
								txt = ['Fc = ' num2str(inProcessing.frameCnt)];
								if(delta ~= 0)
									aDiff = abs(diff);
									idxMax = find(max(aDiff) == aDiff);
									if(length(idxMax) > 0)
										idxMax = idxMax(1);
									end
									txt = [txt ' <' num2str(delta/ssig, 2) '> - ' num2str(idxMax)];
								end
								text(length(sig_c), 0, txt, 'Color','red');
							else
								if(delta ~= 0)
									display(['Error ' lgPrefix ': ' num2str(delta) ' -- Fc = ' num2str(inProcessing.frameCnt)]);
								end
							end
						end
						cnt = cnt + 1;
					end
				end
			end
		end
	end
end

