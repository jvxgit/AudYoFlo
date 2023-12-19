function [] = jvx_disp_matrix_string(mat, outFormatArg, toClipboardArg)

    toClipboard = false;
    outFormat = '%g ';
    
    if(nargin > 2)
        toClipboard = toClipboardArg;
    end
    
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
    if(toClipboard)
        clipboard('copy',out);
        disp('Content copied to clipboard!');
    else
        disp(out);
    end