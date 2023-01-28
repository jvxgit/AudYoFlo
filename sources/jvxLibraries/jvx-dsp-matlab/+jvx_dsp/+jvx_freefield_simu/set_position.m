function [hdl] = set_position(hdl, iscart, v0,v1,v2, id_mic_mat)
    
    if(id_mic_mat <= size(hdl.mics,2))
        if(iscart)
            hdl.mics{id_mic_mat}.x = v0;
            hdl.mics{id_mic_mat}.y = v1;
            hdl.mics{id_mic_mat}.z = v2;
        else
            [hdl.mics{id_mic_mat}.z, hdl.mics{id_mic_mat}.y, hdl.mics{id_mic_mat}.x] = ...
                jvx_dsp_base.math.jvx_sperical_to_cart(v0, v1, v2);
        end
    else
        error('Id out of bounds');
    end
            