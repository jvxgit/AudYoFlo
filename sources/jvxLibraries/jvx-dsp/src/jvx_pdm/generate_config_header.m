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
numOutputFiles = 2;

% Check for show filename option. In Octave, output name is
% just printed, in Matlab, we need to copy that token into a
% specific text file
if strcmp(arg_list{1}, 'show_filename')

	% Output all output filenames

	if(isOctave)
		disp('jvx_fdesign_I_octConfig.h');
		disp('jvx_fdesign_II_octConfig.h');
	else
		out_file
		FID = fopen(out_file, 'wt');
		fprintf(FID, '%s\n', 'jvx_fdesign_I_octConfig.h');
		fprintf(FID, '%s\n', 'jvx_fdesign_II_octConfig.h');
		fclose(FID);
	end

	return
else

	% If we are requested to output the file, the first
	% <numOutputFiles> arguments specify where to write the content
	out_file{1} = arg_list{1};
	out_file{2} = arg_list{2};
end

% All remaining entries are paths to find the base libraries for code generation
for(ind=numOutputFiles+1:numArgs)
	addpath(arg_list{ind});
	disp(['[jvx_pdm] Adding path ' arg_list{ind}]);
end

% ==================================================================
% End of this header. We have set the paths and got the name of the output file in
% <out_file>.
% ==================================================================
if(isOctave)
	% In octave, the script is not exectuted in the source code directory. 
	% We can obtain this folder as follows
	this_path = mfilename('fullpath');
	[filepath,name,ext] = fileparts(this_path);
	addpath(filepath);
end
	
disp('[jvx_pdm] Generating parameters...');

os = 16;

% Filter 1
ppfilterlength = 16;
FIRDESIGN_FAC = 6;
disp(['[jvx_pdm] Filter design I: OS = ' num2str(os) ', PPF = ' num2str(ppfilterlength) ', FIRFAC = ' num2str(FIRDESIGN_FAC) ' --> ' out_file{1}]);
jvx_design_pdm_fir(os, ppfilterlength, FIRDESIGN_FAC, out_file{1});


% Filter 2
ppfilterlength = 8;
FIRDESIGN_FAC = 9;
disp(['[jvx_pdm] Filter design II: OS = ' num2str(os) ', PPF = ' num2str(ppfilterlength) ', FIRFAC = ' num2str(FIRDESIGN_FAC) ' --> ' out_file{2}]);

jvx_design_pdm_fir(os, ppfilterlength, FIRDESIGN_FAC, out_file{2});


