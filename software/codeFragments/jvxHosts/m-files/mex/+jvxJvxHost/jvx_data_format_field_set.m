function [out] = jvx_data_format_field_set(in, format_id, to,compiledForDouble)
    
    if(to)
        switch(format_id)
            case 1
                %{'JVX_DATAFORMAT_DATA', 1, '--'}, ...
                if(compiledForDouble)
                    out = double(in);
                else
                    out = single(in);
                end
            case 2
                % {'JVX_DATAFORMAT_16BIT_LE', 3, '--'}, ...
                in = in * 2^15;
                out = int16(in);
            case 3
                %{'JVX_DATAFORMAT_24BIT_LE', 4, '--'}, ...
                in = in * 2^23;
                out = int32(in);
            case 4
                %{'JVX_DATAFORMAT_32BIT_LE', 5, '--'}, ...
                in = in * 2^31;
                out = int32(in);
            case 5
                % {'JVX_DATAFORMAT_64BIT_LE', 6, '--'}, ...
                in = in * 2^63;
                out = int64(in);
            case 6
                %{'JVX_DATAFORMAT_8BIT', 7, '--'}, ...
                in = in * 2^7;
                out = int8(in);
            otherwise
                error('Invalid dataformat');
        end
    else
         if(compiledForDouble)
            in = double(in);
         else
              in = single(in);
         end
         
        switch(format_id)
            case 1
                %{'JVX_DATAFORMAT_DOUBLE', 1, '--'}, ...
            case 2
                % {'JVX_DATAFORMAT_FLOAT', 2, '--'}, ...
            case 3
                % {'JVX_DATAFORMAT_16BIT_LE', 3, '--'}, ...
                in = in / 2^15;
            case 4
                %{'JVX_DATAFORMAT_24BIT_LE', 4, '--'}, ...
                in = in / 2^23;
            case 5
                %{'JVX_DATAFORMAT_32BIT_LE', 5, '--'}, ...
                in = in / 2^31;
            case 6
                % {'JVX_DATAFORMAT_64BIT_LE', 6, '--'}, ...
                in = in / 2^63;
            case 7
                %{'JVX_DATAFORMAT_8BIT', 7, '--'}, ...
                in = in / 2^7;
            otherwise
                error('Invalid dataformat');
        end
       out = in;
    end
        