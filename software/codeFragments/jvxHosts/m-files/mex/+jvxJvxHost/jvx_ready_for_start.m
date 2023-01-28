function [ready] = jvx_ready_for_start(handles)

[a b] = handles.hostcall('status_process_sequence');
 if(~a)
     jvxJvxHost.jvx_display_error(12, mfilename, b.DESCRIPTION_STRING); 
 end
 
ready = false;
state_init_sequencer =  jvxJvxHost.jvx_lookup_type_id__name('jvxSequencerStatus', 'JVX_SEQUENCER_STATUS_NONE');
 
 % Start only if seqeuncer is in neutral init state
 if(b.STATUS_INT32 == state_init_sequencer)
     ready = true;
 end