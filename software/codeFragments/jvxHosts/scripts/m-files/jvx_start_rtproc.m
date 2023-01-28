function [rtproc] = jvx_start_rtproc(pathCComponents, pathMatlabSources, namesDemo, startPath, commandsDemo)

    % Handle case if someone calls this function without any start
    % information
    this_path = fileparts(mfilename('fullpath'));
    
    if(nargin < 5)
        commandsDemo = {};
    end
    
    if(nargin < 4)
        startPath = this_path;
    end
    
    if(nargin < 3)
        namesDemo = {'Realtime Processing'}
		commandsDemo = {...
            ['jvxHost(''rt-audio'', ''--config'', ''' ...
            this_path '/config.jvx'', ''--jvxdir'', ''' ...
            this_path '/jvxComponents'');'];...
            ['Offline Processing: jvxHost(''off-audio'', ''--config'', '''...
            this_path '/config.jvx'', ''--jvxdir'', '''...
            this_path '/jvxComponents'');']}
    end
    
    if(nargin < 2)
        pathMatlabSources = {...
            [startPath '/matlab/m-files'],...
            [startPath '/matlab/m-files/subprojects-audionode'],...
            [startPath '/matlab/m-files/audioProperties']...
            };       
    end
    if(nargin < 1)
        pathCComponents = [startPath '/matlab'];       
    end
    
    display('=============================================');
    display('==> Matlab started for use with Ayf Tools.<==');
    display('=============================================');
   
    % Copnfigure system to use RTProc host remotely
    rtproc.pathCComponents = pathCComponents;
    rtproc.pathMatlabSources = pathMatlabSources;
    
    disp(['Adding directory ' rtproc.pathCComponents]);
    addpath(rtproc.pathCComponents);

	for(ind=1:size(rtproc.pathMatlabSources,2))
        if(exist(rtproc.pathMatlabSources{ind}, 'dir'))
    		disp(['Adding directory ' rtproc.pathMatlabSources{ind}]);
        	addpath(rtproc.pathMatlabSources{ind});
        end
    end
    
    if(~isempty(namesDemo))
        disp(['Usage: ']);
        for(ind = 1:size(namesDemo,1))
            if(ind <= size(commandsDemo,1))
                disp([num2str(ind) ') ' namesDemo{ind} ': ' commandsDemo{ind}]);
            else
                disp([num2str(ind) ') ' namesDemo{ind}]);
            end
        end
    end
    
	global jvx_start_path;
	jvx_start_path = startPath;
    
    if(size(commandsDemo, 1) == 1)
        eval(commandsDemo{1});
    end