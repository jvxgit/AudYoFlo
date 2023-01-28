function [ ] = jvxStartOffline( jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints )

    global inProcessing;

	[ jvx_handle ] = jvxStartOffline_local(jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints);

	inProcessing.jvx_handle = jvx_handle;
