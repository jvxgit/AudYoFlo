% This script mimicks a real-time audio processing chain. Random noise is
% generated, framed in input buffers and processed using the algorithm in 
% the matlab class jvx_myNewRtapLibrary.jvx_myNewProject. When the
% processing is done, the processed output is played back. 

% creation of object of class jvx_myNewRtapLibrary.jvx_myNewProject.
myNewProjectObject = jvx_myNewRtapLibrary.jvx_myNewProject;

blockSize = 1000;
numOfBlocks = 48;
numChannelsIn = 1;
numChannelsOut = 1;

fs = 48000;

% random noise as input signal
input = rand(numChannelsIn, blockSize * numOfBlocks); 

% matrix to store output signals
output = zeros(numChannelsOut, blockSize * numOfBlocks); 
   
% buffer-wise processing of input signal
for blockIndex = 0:(numOfBlocks-1)
    currentIndices = ( (blockIndex*blockSize) + 1 ) : ( (blockIndex+1) * blockSize );
    currentInputBlock = input(currentIndices);
	output(currentIndices) = myNewProjectObject.process(currentInputBlock, numChannelsIn, numChannelsOut, blockSize);
end

% play output
sound(output, fs);
