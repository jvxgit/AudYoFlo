function [res] = readH5File(fName)    
    
    res =  [];
    toc = h5info(fName);

    res = processGroups(fName, toc.Groups, res);
    res = processDatasets(fName, toc.Datasets, '', res);

    % We return the struct in the file as a substruct of the returned
    % value. At first not implemented like this I noticed that wrapping it
    % allows us to see what the originally name was which is important if
    % we write it afterwards!    
end

function res = processGroups(fName, groups, res)
    
    for(idx = 1: size(groups,1))
        oneGroup = groups(idx);
        [dummy, nameLoc] = fileparts(oneGroup.Name);
        newStr = [];
        if(~isempty(oneGroup.Groups))
            
            % Attach new struct -kind of call by reference
            newStr = processGroups(fName, oneGroup.Groups, newStr);
        end
        if(~isempty(oneGroup.Datasets))

            % Attach new struct -kind of call by reference
            newStr = processDatasets(fName, oneGroup.Datasets, oneGroup.Name, newStr);
        end
        
        command = [ 'res.' nameLoc ' = newStr;'];
        eval(command);
    end
end
    % names = {[Q{1}; Q{2}]};
    
function res = processDatasets(fName, datasets, name, res)
    
       for(jdx = 1:size(datasets, 1))
           oneSet = datasets(jdx);
           nameSet = [name '/' oneSet.Name];
           content = h5read(fName, nameSet);
           
           % Special rule for strings
           if(strcmp(oneSet.Datatype.Type, 'H5T_STD_U8LE'))
               content = char(content);
           end
           command = ['res.' oneSet.Name ' = content;'];
           eval(command);
       end
end

