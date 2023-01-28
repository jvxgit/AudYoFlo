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
    error('please use Matlab, this generator uses the function firpm which is not yet available in the octave signal package');
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
    'jvx_fx_resampler_design_us192_2_audio16_octConfig.h' ...
    'jvx_fx_resampler_design_audio16_2_us192_octConfig.h' ...
    'jvx_fx_resampler_design_us192_2_audio48_octConfig.h' ...
    'jvx_fx_resampler_design_audio48_2_us192_octConfig.h' ...
    'jvx_fx_resampler_design_audio32_2_audio8_octConfig.h' ...
    'jvx_fx_resampler_design_audio8_2_audio32_octConfig.h' ...
    'jvx_fx_resampler_design_audio16_2_audio8_octConfig.h' ...
    'jvx_fx_resampler_design_audio8_2_audio16_octConfig.h' ...
              };

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
    out_file{4} = arg_list{4};
    out_file{5} = arg_list{5};
    out_file{6} = arg_list{6};
	out_file{7} = arg_list{7};
    out_file{8} = arg_list{8};
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

disp('[jvx_fixed_resampler] Generating parameters...');

OSMPLE = 12;
bsize_16kHz = 128;
stopbandatten = 60;
socket = 0;
TUNING_FACTOR = 1;
lfilt = 64;
bsizein = OSMPLE * bsize_16kHz;

jvx_design_fixed_resampler_one_design(bsizein, bsize_16kHz,lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{1});
jvx_design_fixed_resampler_one_design(bsize_16kHz,bsizein, lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{2});

OSMPLE = 4;
bsize_48kHz = 128;
stopbandatten = 60;
socket = 0;
TUNING_FACTOR = 1;
lfilt = 64;
bsizein = OSMPLE * bsize_48kHz;

jvx_design_fixed_resampler_one_design(bsizein, bsize_48kHz,lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{3});
jvx_design_fixed_resampler_one_design(bsize_48kHz,bsizein, lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{4});

OSMPLE = 4;
bsize_8kHz = 40;
stopbandatten = 60;
socket = 0;
TUNING_FACTOR = 1;
lfilt = 64;
bsizein = OSMPLE * bsize_8kHz;

jvx_design_fixed_resampler_one_design(bsizein, bsize_8kHz,lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{5});
jvx_design_fixed_resampler_one_design(bsize_8kHz,bsizein, lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{6});

OSMPLE = 2;
bsize_8kHz = 128;
stopbandatten = 60;
socket = 0;
TUNING_FACTOR = 1;
lfilt = 64;
bsizein = OSMPLE * bsize_8kHz;

jvx_design_fixed_resampler_one_design(bsizein, bsize_8kHz,lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{7});
jvx_design_fixed_resampler_one_design(bsize_8kHz,bsizein, lfilt,stopbandatten, socket, TUNING_FACTOR, out_file{8});