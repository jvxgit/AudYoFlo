function [suc] = jvx_insert_seq_steps(steps)
    
    suc = true;
    global jvx_host_call_global;
    for(ind=1:size(steps,2))
        step = steps{ind};
        [a b] = jvx_host_call_global('insert_step_sequence', step.seq_id, step.seq_elm_type, step.seq_queue_type, ...
             step.seq_cp_ident, step.seq_step_descr, step.seq_step_fuc_com_id, step.seq_step_timeount, ...
             step.seq_step_label, step.seq_step_label_cond1, step.seq_step_label_cond2, step.seq_step_insert);
         if(~a)
             
             suc = false;
             break;
         end
    end