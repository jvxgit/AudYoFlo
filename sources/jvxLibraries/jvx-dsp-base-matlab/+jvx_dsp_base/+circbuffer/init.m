%% Function to allocate a circular buffer object
function [hdl] = init(numChannels, szFld)
    hdl.N = numChannels;
    hdl.M = szFld;
    hdl.memory = zeros(hdl.N, hdl.M);
    hdl.read = 0;
    hdl.fillheight = 0;