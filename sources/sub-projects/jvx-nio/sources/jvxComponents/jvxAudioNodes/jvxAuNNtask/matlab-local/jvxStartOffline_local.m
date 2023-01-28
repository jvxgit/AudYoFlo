function [ jvx_handle ] = jvxStartOffline_local( jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints)

    global inProcessing;
	 verifyFileRead = true;   

     jvx_handle.setup.in.bsize = jvx_bsize_in;
    jvx_handle.setup.in.srate = jvx_srate_in;
    jvx_handle.setup.in.num = jvx_num_in;
    jvx_handle.setup.in.format = jvx_format_in;

    jvx_handle.setup.out.bsize = jvx_bsize_out;
    jvx_handle.setup.out.srate = jvx_srate_out;
    jvx_handle.setup.out.num = jvx_num_out;
    jvx_handle.setup.out.format = jvx_format_out;

    jvx_handle.setup.prec = precision;
    
    jvx_handle.processing.offline = jvx_is_offline;
	
    % Check in matlab is engaged or not..
    jvx_handle.processing.matlab_engaged = false;
    global jvx_host_call_global;
    [a b]= jvx_host_call_global('get_property_uniqueid', ...
        inProcessing.jvx_start_handles.parent_handle.jvx_struct.nodes.comp_type, ...
        inProcessing.jvx_start_handles.parent_handle.jvx_struct.properties.node.offline_node.cat_engange_matlab, ...
        inProcessing.jvx_start_handles.parent_handle.jvx_struct.properties.node.offline_node.id_engange_matlab);
    if(~a)
        error('Unexpected');
    end
    if(b.SUBFIELD == 1)
        jvx_handle.processing.matlab_engaged = true;
    end
    
    if(jvx_handle.processing.matlab_engaged)
        disp(' x x x x > Running algorithmus with Matlab engaged.');
    else
        disp(' x x x x > Algorithmus running in C.');
    end
    
    % Restore private data in global data struct
	inProcessing.jvx_handle = jvx_handle;

    if(jvx_srate_in ~= jvx_srate_out)
        error('Input rate must be identical to output rate');
    end
    
    if(jvx_bsize_in ~= jvx_bsize_out)
        error('Input rate must be identical to output rate');
    end
    
    if(jvx_format_in ~= jvx_format_out)
        error('Input rate must be identical to output rate');
    end
    
        
    % Here comes the specific stuff:
    inProcessing.hoa.numberMics = jvx_num_in;
    inProcessing.hoa.numberHoas = jvx_num_out;

    inProcessing.hoa.the_core_cfg = [];
    
    % Read configuration from C module
    global jvx_host_call_global;
    [suc content] = jvx_host_call_global('export_config_struct', 'audio node');
    nameFile = '';
    if(suc)
        inProcessing.hoa.cfg = jvxMisc.jvx_config_struct(content);
        inProcessing.hoa.N = inProcessing.hoa.cfg.general.order_hoa.jvxValueInRange_value;
        fCore = inProcessing.hoa.cfg.general.core_files;
        idFile = jvxBitField.jvx_selectionid(fCore.jvxSelection_value);
        if(idFile >= 0)
            if(idFile < size(fCore.jvxSelection_listEntries,1))
                nameFile = fCore.jvxSelection_listEntries{idFile+1};
            end
        end
    else
        inProcessing.hoa.N = sqrt(double(inProcessing.hoa.numberHoas)) -1;
    end
    
    if(~isempty(nameFile))
        
        disp(['Reading configuration from file ' nameFile]);
        coreStruct = [];
        coreStruct = jvxConfigRead(nameFile);
        if(~isempty(coreStruct))
            inProcessing.hoa.core_cfgs = jvxMisc.jvx_config_struct(coreStruct);
            all_cfgs = struct2cell(inProcessing.hoa.core_cfgs);
            for(ind=1:size(all_cfgs, 2))
                if(all_cfgs{ind}.bsize == jvx_bsize_in)
                  inProcessing.hoa.the_core_cfg = all_cfgs{ind};
                  break;
                end
            end
        end
    
    end
    
    if(isempty(inProcessing.hoa.the_core_cfg))
        error('Failed to read core parameters from file.');
    end
    
    inProcessing.hoa.allHoas = jvx_hoa.tools.create_all_hoa(inProcessing.hoa.N);
    
    if(verifyFileRead)
        
        % 1) Allocate HOA matrix 
        [inProcessing.hoa.theta, inProcessing.hoa.phi] = jvx_hoa.tools.positions_em32();
        inProcessing.hoa.encMat = pinv(jvx_hoa.tools.getSHMatrix(...
            inProcessing.hoa.theta, inProcessing.hoa.phi, inProcessing.hoa.N, false));
        DD = inProcessing.hoa.encMat - inProcessing.hoa.the_core_cfg.encoder.mat;
        DD = DD * DD';
        res = trace(DD);
        disp(['== > Matrix, File vs Matlab, trace(Delta) = ' num2str(res)]);
    end
    
    % Associate filters 
    for(ind = 1:size(inProcessing.hoa.allHoas,2))
        ct = mod(inProcessing.hoa.allHoas{ind}.n, 4);
        switch(ct)
            case 0
                % filter ip
                inProcessing.hoa.allHoas{ind}.hilbert.hdl = inProcessing.hoa.the_core_cfg.encoder.fs_ip_1can;
                inProcessing.hoa.allHoas{ind}.hilbert.states = zeros(1, inProcessing.hoa.allHoas{ind}.hilbert.hdl.nSection * inProcessing.hoa.allHoas{ind}.hilbert.hdl.order);
                inProcessing.hoa.allHoas{ind}.hilbert.fac = 1;
            case 1
                % filter qu
                inProcessing.hoa.allHoas{ind}.hilbert.hdl = inProcessing.hoa.the_core_cfg.encoder.fs_qu_1can;
                inProcessing.hoa.allHoas{ind}.hilbert.states = zeros(1, inProcessing.hoa.allHoas{ind}.hilbert.hdl.nSection * inProcessing.hoa.allHoas{ind}.hilbert.hdl.order);
                inProcessing.hoa.allHoas{ind}.hilbert.fac = 1;

            case 2
                % filter ip and invert
                inProcessing.hoa.allHoas{ind}.hilbert.hdl = inProcessing.hoa.the_core_cfg.encoder.fs_ip_1can;
                inProcessing.hoa.allHoas{ind}.hilbert.states = zeros(1, inProcessing.hoa.allHoas{ind}.hilbert.hdl.nSection * inProcessing.hoa.allHoas{ind}.hilbert.hdl.order);
                inProcessing.hoa.allHoas{ind}.hilbert.fac = -1;
            case 3
                inProcessing.hoa.allHoas{ind}.hilbert.hdl = inProcessing.hoa.the_core_cfg.encoder.fs_qu_1can;
                inProcessing.hoa.allHoas{ind}.hilbert.states = zeros(1, inProcessing.hoa.allHoas{ind}.hilbert.hdl.nSection * inProcessing.hoa.allHoas{ind}.hilbert.hdl.order);
                inProcessing.hoa.allHoas{ind}.hilbert.fac = -1;
        end
        
         inProcessing.hoa.allHoas{ind}.eq.hdl = jvx_dsp_base.firfft.initCfg();
         inProcessing.hoa.allHoas{ind}.eq.hdl.init.type = 2; % switch from symmetric FIR mode (1) top plain fft mult (2)
         inProcessing.hoa.allHoas{ind}.eq.hdl.init.fir = inProcessing.hoa.the_core_cfg.encoder.fir_eq(ind,:);
         inProcessing.hoa.allHoas{ind}.eq.hdl.init.bsize = double(jvx_bsize_in);
         inProcessing.hoa.allHoas{ind}.eq.hdl = jvx_dsp_base.firfft.init(inProcessing.hoa.allHoas{ind}.eq.hdl);
         
    end
