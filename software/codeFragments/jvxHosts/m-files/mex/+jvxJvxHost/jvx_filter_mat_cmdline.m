function [varargs, local_handles] = jvx_filter_mat_cmdline(local_handles, varargin)
    cnt = 1;
    varargs = {};
    
    in_varargs = varargin{:};
    if(size(in_varargs, 2) < 1)
        return;
    end
    
    local_handles.argv0 = in_varargs{cnt};
    varargs{1} = local_handles.argv0;
    
    local_handles.cb_init = '';
    local_handles.cb_start = '';
    local_handles.cb_stop = '';
    local_handles.cb_term = '';
    local_handles.cb_seq = '';
    local_handles.cb_conn = '';
    local_handles.start_msgq_timer = true;
    
    % Default variables rto be overwritten by command libne args
    local_handles.msgq_period = 0.1;
    local_handles.use_absolut_pathnames = false;
    
    cnt = cnt + 1;
    while(1)
        
        if(cnt > size(in_varargs,2))
            break;
        end
        
        tokenUsed = false;
        token = in_varargs{cnt};
        cnt = cnt + 1;
        if(ischar(token))
            
            if(strcmp(token, 'abs-pathnames'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                if(strcmp(in_varargs{cnt}, 'false'))
                    local_handles.use_absolut_pathnames = false;
                end
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-init-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_init = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-start-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_start = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-stop-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_stop = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-term-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_term = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-msgq-period'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.msgq_period = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-seq-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_seq = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'mat-conn-cb'))
                
                tokenUsed = true;
                if(cnt > size(in_varargs,2))
                    break;
                end
                local_handles.cb_conn = in_varargs{cnt};
                cnt = cnt + 1;
            end
            
            if(strcmp(token, 'disable-msgq-timer'))
                
                tokenUsed = true;
                local_handles.start_msgq_timer = false;
            end
            
            if(~tokenUsed)
                varargs = [varargs token];
            end
        end
    end
