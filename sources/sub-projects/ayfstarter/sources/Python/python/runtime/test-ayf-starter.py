import sys
import os
import numpy

# Better use the pylab plot as it shows also in debugger!
import pylab as plt
# from matplotlib import pyplot as plt

# Import the local references. This is a different version in the release
# compared to the "released" version
import local

import ayf_starter_python

if __name__ == "__main__":

    fsize = int(1024)
    nChansIn = int(1)
    nChansOut = int(1)
    nFrames = 100

    # Print out the help text
    help(ayf_starter_python)
    
    # Create a numpy array with increasing float values
    data = numpy.arange(256*nFrames, dtype=float)
    out = numpy.zeros(256 * nFrames, dtype=float)
    oneBuf = numpy.zeros(256, dtype=float)
    weights = numpy.ones(fft2p1, dtype=complex)
    cnt = 0

    # Run the function for dw_ola
    hdl = jvx_dsp_python.init_processing_dw_ola(hop_size, fft_size)
    for j in range(nFrames):
        oneBuf = data[cnt:cnt+hop_size].copy()
        res = jvx_dsp_python.run_processing_dw_ola(hdl, oneBuf, weights)
        out[cnt:cnt+hop_size] = oneBuf
        cnt += hop_size

    res = jvx_dsp_python.term_processing_dw_ola(hdl)

    plt.plot(data, out)
    plt.show()
