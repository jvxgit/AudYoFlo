function [ ] = jvxStartOffline( jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints )

    global inProcessing;
    inProcessing.processing.volume = 1.0;

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
   
    inProcessing.processing.hdlUi = jvxVolume;
	inProcessing.jvx_handle = jvx_handle;
