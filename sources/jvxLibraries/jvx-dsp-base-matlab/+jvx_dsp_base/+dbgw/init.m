function [hdl] = init(hdl, token, ext)
    fName = token;
    if(nargin >=3)
        fName = [fName '.' ext];
    end
    command = ['hdl.fH = fopen(''' fName ''', ''wb'');'];
    eval(command);
    
    
    