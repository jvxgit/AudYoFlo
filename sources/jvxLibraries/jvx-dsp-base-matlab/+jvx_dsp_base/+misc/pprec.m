function [] = pprec(vec, varNameArg, numColsArg, idxStartArg, idxStopArg)
      
    varName = 'var';
    if(nargin > 1)
        varName = varNameArg;
    end
    
    idxStart = 1;
    if(nargin > 3)
        idxStart = idxStartArg;
    end     
        
    vecsz = size(vec,2);
    
    idxStop = vecsz;
    if(nargin > 4)
        idxStop = min(idxStop, idxStopArg);
    end

    numCols = vecsz;    
    if(nargin > 2)
        numCols = numColsArg;
    end
    numRows = floor(vecsz / numCols);
    if(numCols * numRows < vecsz)
        numRows = numRows + 1;
    end
    
   disp('');
   disp([varName ' = [ ...']);
    cnt = 1;
    for(jnd=1:numRows)
        out = '   ';
        for(knd = 1:numCols)
            if(cnt <= idxStop && cnt >= idxStart)
                if(cnt == idxStop)
                    out = [out sprintf('%.20e ', vec(cnt))];
                else
                    out = [out sprintf('%.20e, ', vec(cnt))];
                end
            else
                break;
            end            
            cnt = cnt + 1;
        end
        out = [out '...'];
        disp(out);
    end
    disp(']');