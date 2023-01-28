function [success] = jvx_constants_c_hp(directory, fName, prefix, varargin)

numArguments = nargin-3;
success = false;

fileH = fopen([directory '\' fName], 'w');

tt = upper(fName);
tt = strrep(tt,'.','__');
fprintf(fileH, ['#ifndef __' tt '\n']);
fprintf(fileH, ['#define __' tt '\n\n']);

if(fileH ~= -1)
    fprintf(fileH, ['// Writing Matlab variables for reuse in C/C++ (header file)\n']);
    fprintf(fileH, '\n');
    numVariables = numArguments/2;
    success = true;
    for(indV = 1:numVariables)
        var = varargin{(indV-1)*2+1};
        nameVar = varargin{(indV)*2};
        
        dimy = size(var,1);
        dimx = size(var,2);

        if(dimy == 1) && (dimx == 1)
            fprintf(fileH, '// ============ scalar ==============\n');
            fprintf(fileH, ['static DATATYPE_ARITHMETIC_USED ' prefix '_' nameVar ' = CONVERT_DOUBLE_2_DATATYPE_ARITHMETIC_USED(%.30e);\n'], var);
            fprintf(fileH, '\n');
        elseif(dimy == 1)
            fprintf(fileH, ['// ============ vector (1x' num2str(dimx) ')==============\n']);
            fprintf(fileH, ['static int ' prefix '_' nameVar '_dimx = %d;\n'], dimx);
            fprintf(fileH, '\n');
            fprintf(fileH, ['static DATATYPE_ARITHMETIC_USED ' prefix '_' nameVar '[' num2str(dimx) '] = {']);
            for(ind2 = 1:dimx)
                if(ind2 ~= 1)
                    fprintf(fileH, [', ']);
                end
                fprintf(fileH, 'CONVERT_DOUBLE_2_DATATYPE_ARITHMETIC_USED(%.30e)', var(ind2));
            end
            fprintf(fileH, ['};\n']);
            fprintf(fileH, '\n');
        else
            fprintf(fileH, ['// ============ matrix(' num2str(dimy) 'x' num2str(dimx) '==============\n']);
            fprintf(fileH, ['static int ' prefix '_' nameVar '_dimx = %d;\n'], dimx);
            fprintf(fileH, ['static int ' prefix '_' nameVar '_dimy = %d;\n'], dimy);
            fprintf(fileH, '\n');
            for(ind = 1:dimy)
                fprintf(fileH, ['static DATATYPE_ARITHMETIC_USED ' prefix '_' nameVar '_' num2str(ind) '[] = {']);
                for(ind2 = 1:dimx)
                    if(ind2 ~= 1)
                        fprintf(fileH, [', ']);
                    end
                    fprintf(fileH, 'CONVERT_DOUBLE_2_DATATYPE_ARITHMETIC_USED(%.30e)', var(ind, ind2));
                end
                fprintf(fileH, ['};\n']);
            end
            
            fprintf(fileH, ['static DATATYPE_ARITHMETIC_USED* ' prefix '_' nameVar '[] = {']);
            for(ind = 1:dimy)
                 fprintf(fileH, [prefix '_' nameVar '_' num2str(ind)]);
                if(ind ~= dimy)
                    fprintf(fileH, ', ');
                end
            end
            fprintf(fileH, '};\n');
        end
    end
    
    fprintf(fileH, '#endif\n');
    fclose(fileH);
end
