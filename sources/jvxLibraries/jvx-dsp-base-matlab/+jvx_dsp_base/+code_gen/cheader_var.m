function cheader_var(CH, typename, varname, precision, value, forceType, comment, pragmas, varprefix, num_values)

if(nargin < 10)
    num_values = [];
end

if(nargin < 9)
    varprefix = '';
end

if(nargin < 8)
    pragmas = {};
end

if(nargin < 7)
    comment = '';
end

if(nargin < 6)
    forceType = '';
end

if(~isempty(comment))
    fprintf(CH.fid, '// %s\n', comment);
end

numPragmas = length(pragmas);
if(numPragmas > 0)
    for(ind=1:numPragmas)
        fprintf(CH.fid, '%s\n', pragmas{ind});
    end
end

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
if(isOctave)

    thetype = typeinfo(value);

    % correct type to vector
    if strcmp(thetype, 'matrix')
        if isvector(value)
            thetype = 'vector';
        end
    end

    if ~strcmp(forceType,'')
        thetype = forceType;
    end

    switch(thetype)
        case 'scalar'
            fprintf(CH.fid, ['%s %s = ' varprefix precision ';\n'], typename, varname, value);
        case 'vector'
            value = value(:)';
            if(isempty(num_values))
                fprintf(CH.fid, '%s %s[%i] = {', typename, varname, length(value));
            else
                fprintf(CH.fid, '%s %s[%s] = {', typename, varname, num_values);
            end
            dlmwrite(CH.fid, value, ', ', 'newline', '', 'precision', [varprefix precision]);
            fprintf(CH.fid, '};\n');
        case 'matrix'
            if(isempty(num_values))
                fprintf(CH.fid, '%s %s[%i*%i] = {\n', typename, varname, size(value,1), size(value,2));
            else
                fprintf(CH.fid, '%s %s[%s] = {\n', typename, varname, num_values);
            end
                
            for i = 1:size(value, 1)
                fprintf(CH.fid, '    ');
                dlmwrite(CH.fid, value(i,:), ', ', 'newline', '', 'precision', [varprefix precision]);
                if i < size(value, 1)
                    fprintf(CH.fid, ',\n');
                end
            end
            fprintf(CH.fid, '};\n');
        case 'int16 matrix'
            if(isempty(num_values))
                fprintf(CH.fid, '%s %s[%i*%i] = {\n', typename, varname, size(value,1), size(value,2));
            else
                fprintf(CH.fid, '%s %s[%s] = {\n', typename, varname, num_values);
            end
            for i = 1:size(value, 1)
                fprintf(CH.fid, '    ');
                dlmwrite(CH.fid, value(i,:), ', ', 'newline', '', 'precision', [varprefix precision]);
                if i < size(value, 1)
                    fprintf(CH.fid, ',\n');
                end
            end
            fprintf(CH.fid, '};\n');
        case 'sq_string'
            if(isempty(num_values))
                fprintf(CH.fid, 'char %s[%i] = "%s";\n', varname, length(value),value);
            else
                fprintf(CH.fid, 'char %s[%s] = "%s";\n', varname, num_values,value);
            end
        otherwise
            error('cheader_var.m: unsupported type "%s"', typeinfo(value))
    end
else

    if(isnumeric(value))

        M = size(value,1);
        N = size(value,2);

        if(isempty(precision))
            switch(class(value))
                case 'double'
                    precision = '%f';
                case 'single'
                    precision = '%f';
                case 'int16'
                    precision = '%i';
                case 'int32'
                    precision = '%i';
                case 'int64'
                    precision = '%i';
            end
        end

        if(M == 1)

            if(N == 1)

                % Scalar case..
                if ~strcmp(forceType, 'vector')
                    fprintf(CH.fid, ['%s %s = ' varprefix precision ';\n'], typename, varname, value);
                else
                    fprintf(CH.fid, ['%s %s[1] = {' varprefix precision '};\n'], typename, varname, value);
                end
            else

                % vector case
                if(isempty(num_values))
                    fprintf(CH.fid, '%s %s[%i] = {', typename, varname, N);
                else
                    fprintf(CH.fid, '%s %s[%s] = {', typename, varname, num_values);
                end
                
                fprintf(CH.fid, [varprefix precision ', '], value(1:end-1));
                fprintf(CH.fid, [varprefix precision], value(end));
                fprintf(CH.fid, '};\n');
            end

        else
            if(N == 1)
                % vector case
                if(isempty(num_values))
                    fprintf(CH.fid, '%s %s[%i] = {', typename, varname, M);
                else
                    fprintf(CH.fid, '%s %s[%s] = {', typename, varname, num_values);
                end

                fprintf(CH.fid, [varprefix precision ', '], value(1:end-1));
                fprintf(CH.fid, [varprefix precision], value(end));
                fprintf(CH.fid, '};\n');
            else
                % vector case
                if(isempty(num_values))
                    fprintf(CH.fid, '%s %s[%i * %i] = {\n', typename, varname, M, N);
                else
                    fprintf(CH.fid, '%s %s[%s] = {\n', typename, varname, num_values);
                end
                
                for(ind=1:M)
                    if(ind == M)
                        fprintf(CH.fid, '\t');
                        fprintf(CH.fid, [varprefix precision ', '], value(ind, 1:end-1));
                        fprintf(CH.fid, [varprefix precision '\n'], value(ind, end));
                    else
                        fprintf(CH.fid, '\t');
                        fprintf(CH.fid, [varprefix precision ', '], value(ind,:));
                        fprintf(CH.fid, ['\n']);
                    end
                end
                fprintf(CH.fid, '};\n');
            end
        end
    else
       if(ischar(value))
           fprintf(CH.fid, 'const char %s = "%s";\n', varname, value);
       else
            error('cheader_var.m: unsupported type %s', typeinfo(value))
       end
    end
end
