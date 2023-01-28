function [] = jvx_view_sequencer(handles)
[a b] = handles.hostcall('info_sequencer');
if(a)
    cc = struct2cell(b);
    for(ind=1:size(cc,1))
        disp(['===' cc{ind}.DESCRIPTION_STRING '===']);
        ccrun = struct2cell(cc{ind}.RUN_STEPS);
        for(jnd=1:size(ccrun,1))
            disp(['Run Step #' num2str(jnd) ': ' ccrun{jnd}.DESCRIPTION_STRING]);
        end
        ccleave = struct2cell(cc{ind}.LEAVE_STEPS);
        for(jnd=1:size(ccleave,1))
            disp(['Leave Step #' num2str(jnd) ': ' ccleave{jnd}.DESCRIPTION_STRING]);
        end
    end
end