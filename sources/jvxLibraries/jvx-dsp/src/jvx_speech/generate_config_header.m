%% header generation with octave or Matlab
%
% the output filename with full path must be passed as an argument; when the
% argument "show_filename" is given, the preferred output filename must be
% printed instead
% The mechansm works slightly different in Matlab and Octave:
% 1) In Octave, we may specify command arguments which are then obtained
%	 in the command "argv()"
% 2) In Matlab, the command arguments are pshed into the workspace BEFORE the
%    actual script is called.

% ==================================================================
% Header for handling of Octave as well as Matlab entries. Since the
% following fragment contains the code to provide Matlab library functions,
% we can not put this into a library function. Hence, in case of a new file,
% just copy the header part.
% ==================================================================

% Distinguish between Octave and Matlab entry
isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
if(isOctave)
    %error('please use Matlab, this generator uses the function firpm which is not yet available in the octave signal package');
    % In Octave, receive command arguments here
    arg_list = argv ();
    pkg load signal;
else

    % In Matlab, receive arguments in a global variable
    global arg_list;
    global out_file;
end

numArgs = length(arg_list);
outputFiles = {...
    'jvx_irs_p48_8.h' ...
    'jvx_irs_p48_16.h' ...
    'jvx_irs_p341_16.h' };

numOutputFiles = size(outputFiles,2); % This must match

% Check for show filename option. In Octave, output name is
% just printed, in Matlab, we need to copy that token into a
% specific text file
if strcmp(arg_list{1}, 'show_filename')

    % Output all output filenames

    if(isOctave)
        for(ind=1:size(outputFiles,2))
            disp(outputFiles{ind});
        end
    else
        out_file
        FID = fopen(out_file, 'wt');
        for(ind=1:size(outputFiles,2))
            fprintf(FID, '%s\n', outputFiles{ind});
        end
        fclose(FID);
    end

    return
else

    % If we are requested to output the file, the first
    % <numOutputFiles> arguments specify where to write the content
    out_file{1} = arg_list{1};
    out_file{2} = arg_list{2};
    out_file{3} = arg_list{3};
end

% All remaining entries are paths to find the base libraries for code generation
for(ind=numOutputFiles+1:numArgs)
    addpath(arg_list{ind});
    disp(['[jvx_resampler] Adding path ' arg_list{ind}]);
end

% ==================================================================
% End of this header. We have set the paths and got the name of the output file in
% <out_file>.
% ==================================================================
% ==================================================================
% ==================================================================
% ==================================================================
% ==================================================================
% ==================================================================

disp('[jvx_irs_filter] Generating parameters...');

token = 'P48_8';
[num, den] = jvx_dsp.speech.irs_filter(token);
fH =  jvx_dsp_base.code_gen.cheader_begin(out_file{1}, 'irs_filter');
jvx_dsp_base.code_gen.cheader_define(fH, ['JVX_IRS_' token '_ORDER'], int32((size(num,2) -1)), '%i');
jvx_dsp_base.code_gen.cheader_var(fH, 'static jvxData', ['jvx_irs_' token], '%.20f', num, '', '', {},'', ['JVX_IRS_' token '_ORDER + 1'] );
jvx_dsp_base.code_gen.cheader_end(fH);

token = 'P48_16';
[num, den] = jvx_dsp.speech.irs_filter(token);
fH =  jvx_dsp_base.code_gen.cheader_begin(out_file{2}, 'irs_filter');
jvx_dsp_base.code_gen.cheader_define(fH, ['JVX_IRS_' token '_ORDER'], int32((size(num,2) -1)), '%i');
jvx_dsp_base.code_gen.cheader_var(fH, 'static jvxData', ['jvx_irs_' token], '%.20f', num, '', '', {},'', ['JVX_IRS_' token '_ORDER + 1'] );
jvx_dsp_base.code_gen.cheader_end(fH);

token = 'P341_16';
[num, den] = jvx_dsp.speech.irs_filter(token);
fH =  jvx_dsp_base.code_gen.cheader_begin(out_file{3}, 'irs_filter');
jvx_dsp_base.code_gen.cheader_define(fH, ['JVX_IRS_' token '_ORDER'], int32((size(num,2) -1)), '%i');
jvx_dsp_base.code_gen.cheader_var(fH, 'static jvxData', ['jvx_irs_' token], '%.20f', num, '', '', {},'', ['JVX_IRS_' token '_ORDER + 1'] );
jvx_dsp_base.code_gen.cheader_end(fH);

