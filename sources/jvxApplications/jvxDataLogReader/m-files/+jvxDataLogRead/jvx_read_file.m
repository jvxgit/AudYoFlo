function [data] = jvx_read_file(fName, varargin)

% Set default return type in case there is no stored data
data = {};

runtime.printnamessessions.token = '-pss';
runtime.printnamessessions.numSearch = 0;
runtime.printnamessessions.value = false;
runtime.printnamessessions.description = '-pss: Print all sessions.';

runtime.selectonesession.token = '-ass';
runtime.selectonesession.numSearch = 1;
runtime.selectonesession.value = -1;
runtime.selectonesession.description = '-ass <session id>: Specify target session.';

runtime.printnamesdatasets.token = '-ps';
runtime.printnamesdatasets.numSearch = 0;
runtime.printnamesdatasets.value = false;
runtime.printnamesdatasets.description = '-ps: Print all datasets in specific session.';

runtime.selectonedatasetshigh.token = '-ash';
runtime.selectonedatasetshigh.numSearch = 1;
runtime.selectonedatasetshigh.value = -1;
runtime.selectonedatasetshigh.description = '-ash <min value>: Maximum limitation of datasets to be copied.';

runtime.selectonedatasetslow.token = '-asl';
runtime.selectonedatasetslow.numSearch = 1;
runtime.selectonedatasetslow.value = -1;
runtime.selectonedatasetslow.description = '-asl <max value>: Minimum limitation of datasets to be copied.';

runtime.readmax.token = '-rm';
runtime.readmax.numSearch = 1;
runtime.readmax.value = -1;
runtime.readmax.description = '-rm <max number>: Specify number of datachunks to be read.';

runtime.reset.token = '-reset';
runtime.reset.numSearch = 0;
runtime.reset.value = false;
runtime.reset.description = '-reset: Reset filereader module in case it responds error due to wrong state.';

runtime.help.token = '-help';
runtime.help.numSearch = 0;
runtime.help.value = false;
runtime.help.description = '-help: Print options.';

[runtime, suc] = jvxMisc.jvx_parse_commandline(runtime, varargin{:});
if(suc == false)
    disp('Error when parsing command line.');
    jvxMisc.jvx_print_commandline(runtime, mfilename());
    data = [];
    return;
end

if(runtime.help.value > 0)
    jvxMisc.jvx_print_commandline(runtime, mfilename());
    data = [];
    return;
end

if(runtime.reset.value)
    clear jvxDataLogReader;
end

if(nargin == 0)
    [a b] = uigetfile('*.*', 'Open Javox log file');
    fName = [b a];
else
    
    if(isempty(fName ))
        [a b] = uigetfile('*.*', 'Open Javox log file');
        fName = [b  a];
    else
    
        if(~exist(fName))
            [a b] = uigetfile('*.*', 'Open Javox log file');
            fName = [b a];
        end
    end
end

if(~ischar(fName) || isempty(fName))
    data = [];
    return;
end
   
% Start reading
disp(['Start reading out content in file ' fName '.']);

%=======================================================
% Specify input filename
%=======================================================
[result1 result2] = jvxDataLogReader('activate', fName);
if(~result1)
    disp(result2.DESCRIPTION_STRING);
    clear jvxDataLogReader;
    return;
end

%=======================================================
% Scan the input file content
%=======================================================
disp('Scanning input file..');
[result1 result2] = jvxDataLogReader('prepare');
if(~result1)
    disp(result2.DESCRIPTION_STRING);
    jvxDataLogReader('deactivate');
    clear jvxDataLogReader;
    return;
end

%=======================================================
% Determine the number of sessions
%=======================================================
[result1 numberSessions] = jvxDataLogReader('number_sessions');
if(~result1)
    disp(numberSessions.DESCRIPTION_STRING);
    jvxDataLogReader('postprocess');
    jvxDataLogReader('deactivate');
    clear jvxDataLogReader;
    return;
end

%=======================================================
% Print data sessions
%=======================================================
if(runtime.printnamessessions.value)
    for(ind=1:numberSessions)
        disp(['--> Data session ' num2str(ind)]);
    end
    jvxDataLogReader('postprocess');
    jvxDataLogReader('deactivate');
    clear jvxDataLogReader;
    return;
end

%=======================================================
% Inner loop: For each of the sessions...
%=======================================================
for(ind = 0:(double(numberSessions)-1))
       
    showthis = true;
    if(runtime.selectonesession.value >= 0)
        showthis = false;
        if(ind == runtime.selectonesession.value)
            showthis = true;
        end
    end
    
    if(showthis)
        
         [result1 tags] = jvxDataLogReader('tags_sessions', ind);
        if(~result1)
            disp(numberSessions.DESCRIPTION_STRING);
            jvxDataLogReader('postprocess');
            jvxDataLogReader('deactivate');
            clear jvxDataLogReader;
            return;
        end
        
        %=======================================================
        % Obtain the number of datasets
        %=======================================================
        [result1 numberDatasets] = jvxDataLogReader('number_datasets_session', ind);
        if(~result1)
            disp(numberSessions.DESCRIPTION_STRING);
            jvxDataLogReader('postprocess');
            jvxDataLogReader('deactivate');
            clear jvxDataLogReader;
            return;
        end
    
        dataSession = [];
        cntInd2 = 0;
        %=======================================================
        % Now aqcuire the data for each dataset channel...
        %=======================================================
        for(ind2 = 0:double(numberDatasets)-1)
            
            showthisaswell = true;
            
            if( (runtime.selectonedatasetslow.value >= 0) || (runtime.selectonedatasetshigh.value >= 0))
                showthisaswell = false;
                
                if(runtime.selectonedatasetslow.value < 0)
                    runtime.selectonedatasetslow.value = 0;
                end
                if(runtime.selectonedatasetshigh.value < 0)
                    runtime.selectonedatasetshigh.value = numberDatasets-1;
                end
                if((ind2 >= runtime.selectonedatasetslow.value) && (ind2 <= runtime.selectonedatasetshigh.value))
                    showthisaswell = true;
                end
            end
            
            if(showthisaswell)
                %=======================================================
                % Dataset with given id..
                %=======================================================
                [result1, datasetStruct] = jvxDataLogReader('dataset_session', ind, ind2);
                if(~result1)
                    disp(datasetStruct.DESCRIPTION_STRING);
                    jvxDataLogReader('postprocess');
                    jvxDataLogReader('deactivate');
                    clear jvxDataLogReader;
                    return;
                end
                
                dataSession{cntInd2+1}.object = datasetStruct;                
                
                if(runtime.printnamesdatasets.value == false)
                    
                    disp(['Reading dataset ' num2str(ind2) ': ' datasetStruct.DESCRIPTION])
                    %=======================================================
                    % Copy the complete data with one function call for speed up
                    %=======================================================
                    [result1, dt] = jvxDataLogReader('next_datachunk_dataset_session_complete', ind, ind2, -1, 'copy_data');
                    if(~result1)
                        disp(dt.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    dataSession{cntInd2+1}.data = dt;
                    
                    %=======================================================
                    % Rewind file
                    %=======================================================
                    [result1, result2] = jvxDataLogReader('rewind_dataset_session', ind, ind2);
                    if(~result1)
                        disp(result2.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    %=======================================================
                    % Copy the complete user ids with one function call for speed up
                    %=======================================================
                    [result1, dt] = jvxDataLogReader('next_datachunk_dataset_session_complete', ind, ind2, -1, 'copy_id_user');
                    if(~result1)
                        disp(dt.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    dataSession{cntInd2+1}.idUser = dt;
                    
                    %=======================================================
                    % Rewind file
                    %=======================================================
                    [result1, result2] = jvxDataLogReader('rewind_dataset_session', ind, ind2);
                    if(~result1)
                        disp(result2.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    %=======================================================
                    % Copy the complete user sub ids with one function call for speed up
                    %=======================================================
                    [result1, dt] = jvxDataLogReader('next_datachunk_dataset_session_complete', ind, ind2, -1, 'copy_sub_id_user');
                    if(~result1)
                        disp(dt.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    dataSession{cntInd2+1}.subIdUser = dt;

                    %=======================================================
                    % Rewind file
                    %=======================================================
                    [result1, result2] = jvxDataLogReader('rewind_dataset_session', ind, ind2);
                    if(~result1)
                        disp(result2.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    
                    %=======================================================
                    % Copy the complete entries of timestamps with one function call for speed up
                    %=======================================================
                    [result1, tt] = jvxDataLogReader('next_datachunk_dataset_session_complete', ind, ind2, -1, 'copy_timing');
                    if(~result1)
                        disp(tt.DESCRIPTION_STRING);
                        jvxDataLogReader('postprocess');
                        jvxDataLogReader('deactivate');
                        clear jvxDataLogReader;
                        return;
                    end
                    dataSession{cntInd2+1}.time_us = tt;
                else
                    
                    disp(['--> Dataset ' num2str(cntInd2) ': ' datasetStruct.DESCRIPTION]);
                end
                cntInd2 = cntInd2 + 1;
            end
        end
        data{ind+1}.DESCRIPTION_STRING = ['Session #' num2str(ind)];
        data{ind+1}.data = dataSession;
        data{ind+1}.tags = tags;
    end
end

%==============================================================
 jvxDataLogReader('postprocess');
 jvxDataLogReader('deactivate');
clear jvxDataLogReader;
