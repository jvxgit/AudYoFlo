function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle
global inProcessing;

% Default: talkthrough
jvx_out_frame = jvx_in_frame;

if(jvxBitField.jvx_test(inProcessing.hoa.cfg.general.operation_mode.jvxSelection_value, 0))
    
    % Step I: Transform the 32 mic signals into hoa outputs
    jvx_out_frame = inProcessing.hoa.the_core_cfg.encoder.mat * jvx_out_frame;

    % Step II: Filter by means of Hilbert filter
    if(inProcessing.hoa.cfg.hoa_setup_encoder.activate_hilbert)
        for(ind = 1:size(inProcessing.hoa.allHoas,2))
            in = jvx_out_frame(ind,:);
            [out, inProcessing.hoa.allHoas{ind}.hilbert.states] = ...
                jvx_dsp_base.filter.jvx_filter_cascade_1can(...
                inProcessing.hoa.allHoas{ind}.hilbert.hdl.nSection, ...
                inProcessing.hoa.allHoas{ind}.hilbert.hdl.order, ...
                inProcessing.hoa.allHoas{ind}.hilbert.states, ...
                inProcessing.hoa.allHoas{ind}.hilbert.hdl.den, ...
                inProcessing.hoa.allHoas{ind}.hilbert.hdl.num, ...
                in);
            out = out * inProcessing.hoa.allHoas{ind}.hilbert.fac;
            jvx_out_frame(ind,:) = out;
        end
    end
    
    % Step III: Filter by equalizer filter
    if(inProcessing.hoa.cfg.hoa_setup_encoder.activate_equalizer)
        for(ind = 1:size(inProcessing.hoa.allHoas,2))
            in = jvx_out_frame(ind,:);
            [inProcessing.hoa.allHoas{ind}.eq.hdl, out] = ...
                jvx_dsp_base.firfft.process(inProcessing.hoa.allHoas{ind}.eq.hdl, in);
            jvx_out_frame(ind,:) = out;
        end
    end    
end

if(jvxBitField.jvx_test(inProcessing.hoa.cfg.general.operation_mode.jvxSelection_value, 1))
    jvx_out_frame = inProcessing.hoa.the_core_cfg.decoder.mat * jvx_out_frame;
end

    
