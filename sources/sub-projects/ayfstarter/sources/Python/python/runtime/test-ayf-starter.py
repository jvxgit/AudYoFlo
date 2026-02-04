import sys
import os
import numpy as np

# Better use the pylab plot as it shows also in debugger!
import pylab as plt
# from matplotlib import pyplot as plt

# Import the local references. This is a different version in the release
# compared to the "released" version
import local

sys.path.append('O:/ayfdevelop/AudYoFlo/python/ayf-packages/ayf/audioio')

import ayf_starter_python
import ayfaudioio as ayfio
import ayfaudioproc as ayfproc
import ayfaudiobuf as ayfbuf

## ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
## This file with the .in extension is the basis for code generation. Therefore the
## same file without .in extension should not be edited. Otherwise the changes may 
## be lost in case the C-part is rebuilt!
## ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !

class myProcessor(ayfproc.ayfaudio_processor):

    def __init__(self):
        self.hdl = None

    def prepare(self, numIn = 1, numOut = 1, bSize = 128, sRate = 48000):
        self.hdl = ayf_starter_python.init_ayf_starter(numIn, numOut, bSize, sRate, int(-2), int(-2))
        return 0
    
    # @abstractmethod
    def process(self, inBuf: ayfbuf.ayfaudio_buf, outBuf: ayfbuf.ayfaudio_buf):
        
        # Run core function
        res = ayf_starter_python.run_frame_ayf_starter(self.hdl, inBuf.params.chans, outBuf.params.chans, inBuf.params.bs, inBuf.fld, outBuf.fld)
        return res

    # @abstractmethod
    def postprocess(self) :
        res = ayf_starter_python.term_ayf_starter(self.hdl)
        return res    

# =================================================================================
# =================================================================================

if __name__ == "__main__":

    # Declare some processing parameters
    fsize = int(1024)
    nChansIn = int(1)
    nChansOut = int(1)
    srate = int(48000)

    # Output the current process id to identify process in VS
    print("PID:", os.getpid())

    # Print out the help text    
    # help(ayf_starter_python)

    ### Convert into interleaved
    # ininter = np.ascontiguousarray(oneBuf).ravel(order="C")

    ### From interleaved to no-interleaved
    # oneBuf = np.ascontiguousarray(outinter.reshape(fsize, nChansOut).T)       
    fwk = ayfio.ayfaudio(numIn = nChansIn, numOut = nChansOut, bSize = fsize, sRate = srate, operStr = 'INTERLEAVED', format = 'double')
    pp = myProcessor()

    # Prepare source and sink
    fwk.prepare(source = 'test.wav', sink = 'out.wav')

    # Framing loop
    fwk.run(pp)

    # Processing done
    fwk.postprocess()
        
    # Take out the data
    dat = fwk.output_data()

    # Output result
    plt.plot(dat)
    plt.show()

    print("Done!")
