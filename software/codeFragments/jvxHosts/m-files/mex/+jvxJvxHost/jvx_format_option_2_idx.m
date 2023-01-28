function [c_id] = jvx_format_option_2_idx(oneOption, compiledForDouble)

global jvx_host_call_global;
[a, c_id] = jvx_host_call_global('lookup_type_id__name', 'jvxDataFormat', oneOption{1});
if(~a) error('Failed to run function <jvx_host_call_global(''lookup_type_id__name'', ''jvxDataFormat'', oneOption)>'); end
