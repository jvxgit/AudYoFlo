from dataclasses import dataclass
import sys
from turtle import end_fill
import numpy as np
import pathlib
from enum import Enum
import math
import ayfaudioif as ayfif
import ayfaudiooutwav as ayfoutwav
import ayfaudioinwav as ayfinwav
import ayfaudioinpulse as ayfinpls
import ayfaudiobuf as ayfbuf

# ==========================================================
# ==========================================================

class ayfaudio:
        
    def __init__(self, numIn = 1, numOut = 1, bSize = 128, sRate = 48000, operStr = 'NONINTERLEAVED', format = 'double'):
        
        self.fsize = 128
        self.inChans = 1
        self.outChans = 1
        self.sRate = 48000
        self.operation = ayfbuf.ProcessingType.NOI_SINGLEBUF
        self.format = 'double'
        self.arith = 'float64'

        if(bSize != None):
            self.fsize = bSize

        if(numIn != None):
            self.inChans = numIn

        if(numOut != None):
            self.outChans = numOut
        
        if(sRate != None):
            self.sRate = sRate

        if(isinstance(operStr, str) == True):
            if(operStr == 'NINTERLEAVED'):
                self.operation = ayfbuf.ProcessingType.NOI_SINGLEBUF
            elif(operStr == 'NINTERLEAVEDB'):
                self.operation = ayfbuf.ProcessingType.NOI_SINGLEBUF
            elif(operStr == 'INTERLEAVED'):
                self.operation = ayfbuf.ProcessingType.I_SINGLEBUF
            else:
                assert(False)

        if(isinstance(format, str) == True):
            if(format == 'single'):
                self.format = 'single'
                self.arith = 'float32'
        
        self.dataIn = ayfbuf.ayfaudio_data()
        self.dataOut = ayfbuf.ayfaudio_data()

        self.core = None
        self.corein = None
        self.coreout = None

        self.ibuf = ayfbuf.ayfaudio_buf()
        self.obuf = ayfbuf.ayfaudio_buf()
        self.progressStep = 0.05
        self.progressShowNext = 0.0

    def prepare(self, source = None, sink = None):
        
        print("### AYFAUDIOIO -- Running Prepare Function")
        
        # Input side
        if(isinstance(source, str) == True):

            if(source == "pulse"):
                self.corein = ayfinpls.ayfaudio_inpulse(duration_secs = 10, samplerate =  self.sRate, nChans = self.inChans, format = self.format )
            else :
                self.corein = ayfinwav.ayfaudio_inwav(source)

        else: 
            self.corein = source
        
        # Run the core allocation
        self.corein.prepare_data(self.format)

        if(isinstance(sink, str) == True):
            self.coreout = ayfoutwav.ayfaudio_outwav(sink)

        if(isinstance(sink, ayfbuf.ayfaudio_outbuf) == True): 
            self.coreout = sink

        # Get the input parameters from source
        self.dataIn = self.corein.param_get()
        if(self.inChans == -1):
            self.inChans = self.dataIn.nChans

        #self.dataOut = self.coreout.param_get()
        #if(self.outChans == -1):
        #    self.outChans = self.dataOut.nChans

        #if(self.dataIn.sampling_rate != self.dataOut.sampling_rate):
        #    print('WARNING: Samplerate mismatch input vs output {!r} <-> {!r}'.format(self.dataIn.sampling_rate,
        #        self.dataOut.sampling_rate) )

        # self.fsize = bsize

        # ============================================
        self.ibuf.allocate(self.operation, self.inChans, self.fsize, self.sRate, self.arith)
        self.obuf.allocate(self.operation, self.outChans, self.fsize, self.sRate, self.arith)
    
    def run(self, processor):
        print('### AYFAUDIOIO -- Staring Run Loop')
        nFrames = self.corein.start(self.ibuf)
        self.coreout.start(self.arith, nFrames, self.obuf)
        frameIdx = 0

        ## Prepare processor for processing loop
        processor.prepare(numIn = self.ibuf.params.chans, numOut = self.obuf.params.chans, bSize = self.ibuf.params.bs, sRate = self.ibuf.params.sr)
        
        ## Main Loop
        while(1):
            if(self.corein.prepare_single_frame(self.ibuf, frameIdx)):
                progress = self.corein.progress(self.ibuf)
                if(progress > self.progressShowNext ):
                    # processor.progress_step(progress)
                    self.progressShowNext = self.progressShowNext + self.progressStep
                    print("\rProgress: ", math.floor(progress*100 + 0.5), "%", end='')

                # Unset frame Idx - we may set it in the process function OR right after it
                self.obuf.fIdx = -1
                # Process data - you may also copy the frame idx
                processor.process(self.ibuf, self.obuf)

                # If lower than 0 we need to set it automatically
                if(self.obuf.fIdx < 0):
                    self.obuf.fIdx =  self.ibuf.fIdx

                self.coreout.postprocess_single_frame( self.obuf)
                frameIdx += 1
            else:
                break

        ## Postprocess processor
        processor.postprocess()

        self.corein.stop()
        self.coreout.stop()
            
    def postprocess(self):
        
        print("### AYFAUDIOIO -- Running Postprocess Function")

        self.ibuf.deallocate()
        self.obuf.deallocate()
        self.corein.clear_data()

    def output_data(self):
        return self.coreout.buf





