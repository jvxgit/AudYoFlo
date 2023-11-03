function [suc] = writeH5File(fName, str, entryNameArg, overrideArg)    
    
    override = true;
    entryName = 'data';
    if(nargin >= 4)
        override = overrideArg;
    end
    if(nargin >= 3)
        entryName = entryNameArg;
    end
    
    if(override)
        if(exist(fName) == 2)
            delete(fName);
        end
    end
    
    path = ['/' entryName];    
    suc = writeH5FileCore(fName, path, str);
end

function [suc] = writeH5FileCore(fName, path, str)
        
suc = false;
    % h5create and h5write
    if(isstruct(str))
        content = struct2cell(str);
        fnames = fieldnames(str);
        suc = true;
        for(idx=1:length(fnames))
            
            pathNext = [path '/' fnames{idx}];
            sucLoc = writeH5FileCore(fName, pathNext, content{idx});
            if(sucLoc == false)
                suc = false;
                break;
            end
        end
    else
        if(isnumeric(str))
            suc = true;
            dd = size(str);
            h5create(fName, path, dd);
            h5write(fName, path, str);
        elseif(ischar(str))
            suc = true;
            fld = uint8(str);
            dd = size(fld);            
            h5create(fName, path, dd, 'Datatype','uint8');
            h5write(fName, path, fld);
        else
            % suc = false;
            disp('Invalid data format');
        end
    end
end