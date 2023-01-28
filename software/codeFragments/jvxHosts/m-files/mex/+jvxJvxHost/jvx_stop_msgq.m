function [] = jvx_stop_msgq()

foundRunningMsgq = false;
was_started = true;

global jvx_global_control;

    if(isfield(jvx_global_control, 'tasks'))
        if(isfield(jvx_global_control.tasks, 'msgq'))
            if(isfield(jvx_global_control.tasks.msgq, 'started'))
                was_started = jvx_global_control.tasks.msgq.started;
            end
            if(was_started)
                if(isfield(jvx_global_control.tasks.msgq, 'hdl'))
                    if(isvalid(jvx_global_control.tasks.msgq.hdl))
                        foundRunningMsgq = true;
                        stop(jvx_global_control.tasks.msgq.hdl);
                        delete(jvx_global_control.tasks.msgq.hdl);
                        jvx_global_control.tasks.msgq.userdata = [];
                    end
                end
            end
        end
    end


if(foundRunningMsgq)
    disp('Global message queue stopped.');
else
    disp('No global message queue running.');
end

