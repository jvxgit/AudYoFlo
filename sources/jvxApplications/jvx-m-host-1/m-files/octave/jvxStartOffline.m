function [  ] = jvxStartOffline( jvx_bsize, jvx_srate, jvx_num_in, jvx_num_out, jvx_format, jvx_is_offline )

    global inProcessing;
    inProcessing.processing.volume = 1.0;
    jvx_handle.setup.bsize = jvx_bsize;
    jvx_handle.setup.srate = jvx_srate;
    jvx_handle.setup.num_in = jvx_num_in;
    jvx_handle.setup.num_out = jvx_num_out;
    jvx_handle.setup.format = jvx_format;
    
    jvx_handle.processing.offline = jvx_is_offline;

    inProcessing.processing.hdlUi = jvxVolume;
	inProcessing.jvx_handle = jvx_handle;

