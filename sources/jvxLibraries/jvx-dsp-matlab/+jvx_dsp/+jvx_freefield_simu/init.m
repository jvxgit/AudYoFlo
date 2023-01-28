function [hdl] = init(c, num_mics)

    % Space definition:
    %                        | +z (90°/undefined)
    %                        | 
    %                        |     (0°/0°)
    %                        |    / +y
    %                        |   /
    %                        |  /
    %                        | /
    %                        | ---------------- +x (0°/270°)
    
    hdl.c = c;
    hdl.num_mics = num_mics;
    
    for(ind=1:hdl.num_mics)
        hdl.mics{ind}.x = 0;
        hdl.mics{ind}.y = 0;
        hdl.mics{ind}.z = 0;
    end
    
    
    