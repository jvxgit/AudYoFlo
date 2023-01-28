function [ jvx_handle ] = jvxStartOffline_local( jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints)

    global inProcessing;
	        
    jvx_handle.operation_mode = 1; % 1: run all matlab; 2: run all C; 3: run matlab and C and verify
    verbose_mat = false;
    verbose_c = true;
    
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

    if(jvx_srate_in ~= jvx_srate_out)
        error('Input rate must be identical to output rate');
    end
    
    if(jvx_bsize_in ~= jvx_bsize_out)
        error('bsize rate must be identical to output rate');
    end
    
    if(jvx_format_in ~= jvx_format_out)
        error('format_in must be identical to output rate');
    end
        
    % addpath(inProcessing.jvx_start_handles.local_path);
    
    % ==============================================================

    if(bitand(jvx_handle.operation_mode, 1)) 
        
        % Full MATLAB MODE
        
        jvx_handle.nlms_handle = ayf_hr_nlms.nlms_init(20);
        jvx_handle.circ_hdl = jvx_dsp_base.circbuffer.init(1, jvx_handle.setup.in.bsize + 40);
        jvx_handle.circ_hdl = jvx_dsp_base.circbuffer.fill_update(jvx_handle.circ_hdl, 40, 0);
        
        comp_hdl = [];
        % Store handle for everything
        jvx_handle.comp_hdl = comp_hdl;
        
        
        disp('Running algorithm in Matlab');
    end
    
    if(bitand(jvx_handle.operation_mode, 2)) 

        % Start C reference version
        %global jvx_host_call_global;
        %[a,b] = jvx_host_call_global('external_call', 'CayfAuNHeadRest', 'whatever_function');
        % Store handle for everything
        jvx_handle.comp_hdl_c = comp_hdl_c;

        disp('Running algorithm in C');
    end
    
    if(jvx_handle.operation_mode == 3) 
        disp('Verification of C output');
    end
    
    % ==============================================================
    
    
        
	% Restore private data in global data struct
	inProcessing.jvx_handle = jvx_handle;
