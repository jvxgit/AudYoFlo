classdef jvx_myNewProject < handle
    %jvx_myNewProject Algorithm library template
    %   provides process method, which can be called by the audio node
    
    properties
        myVolume;
    end
    
    methods
        function this = jvx_myNewProject
            this.myVolume = 1.0;
        end
        
		function outputs = process(this, inputs, numChannelsIn, numChannelsOut, bSize)            
			assert(all(size(inputs) == [numChannelsIn, bSize]), 'Incorrect block size or number of input channels');
			outputs = zeros(numChannelsOut, bSize);
	
			for channelIdxOut = 1:numChannelsOut
				channelIdxIn = mod(channelIdxOut-1, numChannelsIn) + 1;
				outputs(channelIdxOut,:) = inputs(channelIdxIn,:) * this.myVolume;
			end
	
		end
    end
    
end

