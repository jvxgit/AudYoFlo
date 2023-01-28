% Parameters:
% fprintf(CH.fid, [ prefix '#define %s %s' format ' \n' ], token, convert_prefix, value);
% Modifications:
% -> prefix = '//' if commented == true
% -> format auto selected if format == ''
function cheader_define(CH, token, value, format, commented, comment, convert_prefix)

if(nargin < 7)
    convert_prefix = '';
end

if(nargin < 6)
    comment = '';
end

if(nargin < 5)
    commented = false;
end

if(nargin < 4)
    format = '';
end

if(nargin < 3)
    value = '';
end

if(~isempty(comment))
    fprintf(CH.fid, ['// %s ' '\n' ], comment);
end

if commented
    prefix = '// ';
else
    prefix ='';
end

isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
if(isOctave)
    switch(typeinfo(value))
      case 'bool'
        fprintf(CH.fid, [ prefix '#define %s %s\n' ], token, mat2str(value));
      case 'scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %s\n' ], token, convert_prefix, mat2str(value));
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
      case 'int16 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
      case 'int32 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
     case 'int64 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
      case 'uint16 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
      case 'uint32 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
     case 'uint64 scalar'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format '\n' ], token, convert_prefix, value);
        end
      case 'null_sq_string'
        fprintf(CH.fid, [ prefix '#define %s\n' ], token);
      case 'sq_string'
        fprintf(CH.fid, [ prefix '#define %s "%s"\n' ], token, value);
      case 'string'
        fprintf(CH.fid, [ prefix '#define %s "%s"\n' ], token, value);        
      otherwise
        error('cheader_define.m: unsupported type %s', typeinfo(value))
    end
else
    % Matlab
    switch(class(value))
      case 'logical'
        fprintf(CH.fid, [ prefix '#define %s %s\n' ], token, mat2str(value));

      case 'double'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %f\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s' format ' \n' ], token, convert_prefix, value);
        end

      case 'single'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %f\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end

      case 'int16'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end

      case 'int32'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end

      case 'int64'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %i\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end
   case 'uint16'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end

      case 'uint32'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end

      case 'uint64'
        if(strcmp(format, ''))
            fprintf(CH.fid, [ prefix '#define %s %s %u\n' ], token, convert_prefix, value);
        else
            fprintf(CH.fid, [ prefix '#define %s %s ' format ' \n' ], token, convert_prefix, value);
        end
      case 'char'
        fprintf(CH.fid, [ prefix '#define %s "%s"\n' ], token, value);
      case 'string'
        fprintf(CH.fid, [ prefix '#define %s "%s"\n' ], token, value);
      otherwise
        error('cheader_define.m: unsupported type %s', class(value))
    end
end