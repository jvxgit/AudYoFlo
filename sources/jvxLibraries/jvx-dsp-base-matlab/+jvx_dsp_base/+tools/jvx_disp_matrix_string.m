function [] = dispMatrixString(mat, outFormatArg)

    outFormat = '%.20e ';
    if(nargin > 1)
        outFormat = outFormatArg;
    end
    out = '[';
    for(ii = 1:size(mat,1))
        vv = mat(ii,:);
        if(ii > 1)
            out = [out ';'];
        end
        out = [out sprintf(outFormat, vv)];
    end
    out = [out ']'];
    disp(out);