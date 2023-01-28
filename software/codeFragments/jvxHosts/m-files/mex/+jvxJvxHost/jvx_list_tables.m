function [] = jvx_list_tables()

global jvx_host_call_global;
[a b] = jvx_host_call_global('lookup_type_id__name');
if(~a) error(['Function call faild: ' 'lookup_type_id__name: ' b.DESCRIPTION_STRING]); end

for(ind= 1:size(b,1))
    disp(['========================' b{ind} '=============================']);
    [a c] = jvx_host_call_global('lookup_type_id__name', b{ind});
    if(~a) error(['Function call faild: ' 'lookup_type_id__name, ' b{ind} ': ' c.DESCRIPTION_STRING]); end
    for(jnd= 1:size(c,1))
        [a d] = jvx_host_call_global('lookup_type_id__name', b{ind}, c{jnd});
        if(~a) error(['Function call faild: ' 'lookup_type_id__name, ' b{ind} ', ' c{jnd}  ': ' c.DESCRIPTION_STRING]); end
        if(isnumeric(d))
            disp(['--> <' b{ind} '>::<' c{jnd} '>=' num2str(d)]);
        else
            disp(['--> <' b{ind} '>::<' c{jnd} '>=' d]);
        end
    end
end

    
    

    