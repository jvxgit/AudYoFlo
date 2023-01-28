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

	% In Octave, receive command arguments here
	arg_list = argv ();
	pkg load signal;
else

	% In Matlab, receive arguments in a global variable
	global arg_list;
	global out_file;
end

numArgs = length(arg_list);
numOutputFiles = 1;

% Check for show filename option. In Octave, output name is
% just printed, in Matlab, we need to copy that token into a
% specific text file
if strcmp(arg_list{1}, 'show_filename')

	% Output all output filenames

	if(isOctave)
		disp('jvx_startHere_octConfig.h')
	else
		out_file
		FID = fopen(out_file, 'wt');
		fprintf(FID, '%s\n', 'jvx_startHere_octConfig.h');
		fclose(FID);
	end

	return
else

	% If we are requested to output the file, the first
	% <numOutputFiles> arguments specify where to write the content
	out_file = arg_list{1};
end

% All remaining entries are paths to find the base libraries for code generation
for(ind=numOutputFiles+1:numArgs)
	addpath(arg_list{ind});
	disp(['[jvx_startHere] Adding path ' arg_list{ind}]);
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


hGlobal.doPlot = false; % plot computation results
if(hGlobal.doPlot)
    graphics_toolkit('gnuplot');
end
hGlobal.floatPrecision = '%.25e';

disp('[jvx_startHere] Generating parameters...')


%% algorithm parameters

% global parameters
hGlobal.fs = 16000; % sampling rate (Hz)
hGlobal.nIn = 2; % number of input channels (must not be changed!)
hGlobal.D = 0.018; % microphone distance
hGlobal.c = 343.0; % speed of sound

% input high pass
HP.fpass = 160; % passband frequency (Hz)
HP.fstop = 80; % stopband frequency (Hz)
HP.Astop = 20; % min stopband attenuation (dB)
HP.Rpass = 1;  % max passband ripple (dB)


%% computations & header generation
CH = jvx_dsp_base.code_gen.cheader_begin(out_file, mfilename('fullpathext'));

% some global parameters
jvx_dsp_base.code_gen.cheader_add(CH, '// global parameters');
jvx_dsp_base.code_gen.cheader_define(CH, 'JVX_STARTHERE_SPEEDOFSOUND', hGlobal.c, '%.1f');
jvx_dsp_base.code_gen.cheader_emptyline(CH);

% some random matrix
jvx_dsp_base.code_gen.cheader_add(CH, '// pseudo-random data');
randData = randn(1024,1);
forceType = 'matrix';
jvx_dsp_base.code_gen.cheader_var(CH, 'static jvxData', 'JVX_STARTHERE_SOME_COEFFS', hGlobal.floatPrecision, randData, forceType);

jvx_dsp_base.code_gen.cheader_end(CH);

if(hGlobal.doPlot)
    plot(randData);
    disp('... press any key to continue ...')
    pause
end
