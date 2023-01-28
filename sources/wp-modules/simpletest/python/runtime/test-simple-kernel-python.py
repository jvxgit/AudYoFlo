
import sys
 
import numpy

# Better use the pylab plot as it shows also in debugger!
import pylab as plt
# from matplotlib import pyplot as plt

# Import the local references. This is a different version in the release
# compared to the "released" version
import local
import os

import jvx_helper_python
import simple_kernel_processor

pathAyf = os.environ["AYF_PACKAGES_PATH"]
if(pathAyf):
    sys.path.append(pathAyf)
    import ayf.audioio.ayfident as ayfident
    import ayf.audioio.ayfaudioio as ayfaudioio    

if __name__ == "__main__":

    framesize = 128

    ## help(jvx_helper_python)
    
    # Print out the help text
     

    # Create audio processing unit
    aio = ayfaudioio.ayfaudio()

    # Create the processor
    apr = simple_kernel_processor.AudioProcess()
    apr.initialize()

    # Create input device
    inAudio = ayfaudioio.ayfaudio_inwav("./testWav.wav")

    # Create output "device" as a buffer
    outAudio = ayfaudioio.ayfaudio_outbuf(maxLength=480000, nChans = 2)

    # Initialize module
    aio.initialize(bsize=128, format= jvx_helper_python.jvxDataString(), 
        source = inAudio, sink = outAudio)

    # Run the processing loop
    aio.run(apr)
   
    aio.terminate()

    apr.terminate()

    # Output result
    data = outAudio.buf[0]
    plt.plot(data)
    plt.show(block = True)
