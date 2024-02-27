function ayf_starter_run_test(closeFig)

% Name of output wav file
exportFName = 'exported_result.wav';
runProfiling = true;

if(nargin < 1)
    closeFig = true;
end

% Start the host with a predefined config file
disp('-- Start the Matlab host for processing --');
global jvx;
eval(['jvxH = ' jvx.scriptingCommand]);

% Draw the ui to let the previous action take effect
drawnow;

% =======================================================================
% Modify properties at this position

global hdl_profile_data;
hdl_profile_data = [];

if(runProfiling)
    ayfAuNStarter.set_audio_node_develop_involveProfiling(true);
    
    hdl_profile_data.cfg = @jvxLocalConfig;
    hdl_profile_data.start = @jvxLocalStart;
    hdl_profile_data.step = @jvxLocalStep;
    hdl_profile_data.stop = @jvxLocalStop;
    hdl_profile_data.run_anyway = false;
    hdl_profile_data.args.doPlot = true;
end

% Now, run the actual simulation
[ret2] = jvxHost_off_rc('jvx_run');
if(~ret2)
    error('Running the Matlab Host failed!');
end

% Export the first output signal
[ret2] = jvxHost_off_rc('jvx_export_output_signal', 1, exportFName );
if(~ret2)
    error(['Exporting results to <' exportFName '> failed!']);
end

if(closeFig)
    % Close everything
    close(jvxH);
end

end

% ======================================================================
% =======================================================================

function [hdl] = jvxLocalConfig(hdl)
    disp([' +++++ > jvxLocalConfig < +++++']);
    hdl.hooks.data = [];
    
    bf = jvxBitField.jvx_create();
    
    % Testpoint 0 - input signal noise
    bf = jvxBitField.jvx_set(bf, 0);
    
    ayfAuNStarter.set_audio_node_develop_testpoints(bf);
    [hdl.cfg.tokenTp] = ayfAuNStarter.get_audio_node_develop_testpoints();
    
    ayfAuNStarter.set_audio_node_develop_skipInvolveCCode(0);
    hdl.cfg.skipCCode = ayfAuNStarter.get_audio_node_develop_skipInvolveCCode();
    disp(['--> Skip CCode: ' num2str(hdl.cfg.skipCCode)]); % Selection starts with 0!
end

function [hdl] = jvxLocalStart(hdl)

    disp(' +++++ > jvxLocalStart < +++++');

    hdl.hooks.data.nFrames = hdl.nFrames;
    disp(['Preparing to collect the <' num2str(hdl.hooks.data.nFrames) '> frames.']);
    
    % Get runtime data parameter
    hdl.hooks.data.M = ayfAuNStarter.get_audio_node_develop_bsizeSignal();

    hdl.hooks.data.matSignalC_cbr = jvx_dsp_base.cbr.jvxMatrixReference(hdl.nFrames, hdl.hooks.data.M);
    hdl.hooks.data.matSignalMat_cbr = jvx_dsp_base.cbr.jvxMatrixReference(hdl.nFrames, hdl.hooks.data.M);

end

function [hdl] = jvxLocalStep(hdl)
    
    if(~hdl.cfg.skipCCode)
        hdl.hooks.data.matSignalC_cbr.setRow(hdl.fCnt, hdl.tp.starter_tp0_c);
        hdl.hooks.data.matSignalMat_cbr.setRow(hdl.fCnt, hdl.tp.starter_tp0_m);
    end
end

function [] = jvxLocalStop(hdl)
    
    disp(' +++++ > jvxLocalStop < +++++');

    epsLocal = 1e-20;
    devFrames = zeros(1, hdl.fCnt);
    
    for(idx = 1:hdl.fCnt)
        sig_c = hdl.hooks.data.matSignalC_cbr.row(idx);
        sig_m = hdl.hooks.data.matSignalMat_cbr.row(idx);
        
        diff = abs(sig_c-sig_m);
        delta = diff * diff';
        ssig = sig_c * sig_c';
        val = delta/(ssig+epsLocal);
        devFrames(idx) = 10*log10(val+epsLocal);
    end
        
    figure;
    plot(devFrames);
    ylimV = ylim;
    ylimV(1) = min(0, ylimV(1));
    ylimV(2) = max(0, ylimV(2));
    ylim(ylimV);
    legend({'Per frame relative error energy, Matlab vs. C'});
    
end


    

