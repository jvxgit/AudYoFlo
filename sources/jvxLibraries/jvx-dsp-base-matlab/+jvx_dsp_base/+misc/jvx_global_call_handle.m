function [fHdl] = jvx_global_call_handle(token)
fHdl = [];
glob_vars = whos('global');
for(ind = 1:size(glob_vars,1))
    if(strcmp(glob_vars(ind).name, token))
        if(strcmp(glob_vars(ind).class, 'function_handle'))
            com = ['global ' token '; fHdl = ' token ';'];
            eval(com);
            break;
        end
    end
end