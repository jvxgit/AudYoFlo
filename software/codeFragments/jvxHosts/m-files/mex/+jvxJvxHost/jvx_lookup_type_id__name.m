function [id] = jvx_lookup_type_id__name(lookup_key, name)
    
    global jvx_host_call_global;
    if(~ischar(lookup_key))
        error(['Wrong input for function ' mfilename ': expected the lookup key in <string> format.']);
    end
    if(~ischar(name))
        error(['Wrong input for function ' mfilename ': expected the entry in <string> format.']);
    end
    
    [suc id] = jvx_host_call_global('lookup_type_id__name', lookup_key, name);
    if(suc == false)
        error(['Failed to read entry ' name ' for lookup key ' lookup_key ...
            ', error: ' id.DESCRIPTION_STRING]);
    end
