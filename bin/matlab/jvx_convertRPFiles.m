function [] = jvx_convertRPFiles(folder)
    
    disp(['Entering directory <' folder '>']);
    
    allEntries = dir(folder);
    for(ind=1:size(allEntries,1))
        oneEntry = allEntries(ind).name;
        if( (strcmp(oneEntry, '.') == 0) && (strcmp(oneEntry, '..') == 0))
            rfolder = [folder '/' oneEntry];
            if(isdir(rfolder))
                jvx_convertRPFiles(rfolder);
            end
        end
    end
    
    tfolder = [folder];
    allEntries = dir(tfolder);
    for(ind=1:size(allEntries,1))
        oneEntry = allEntries(ind).name;
        if(~isdir(oneEntry))
            if(size(oneEntry,2) >= 2)
                ending = oneEntry(end-1:end);
                if(strcmp(ending, '.m'))
                    filen = [ tfolder '/' oneEntry];
                    disp(['Pcode file ' filen]);
					pcode(filen, '-inplace');
                    delete(filen);
                end
            end
        end
    end
    
    
    