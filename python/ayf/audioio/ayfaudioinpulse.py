import pathlib
import numpy as np
import math
import audiofile as af

import ayfaudioif as ayfif
import ayfaudioio as ayfio
import ayfaudiobuf as ayfbuf

# ========================================================================
class ayfaudio_inbuffered(ayfif.ayfaudio_in):

    def prepare_data(self, formatSystem):
            
        if(formatSystem == "single"):
            if(self.buf.dtype == "float64"):
                self.buf = np.float32(self.buf)
        elif(formatSystem == "double"):
            if(self.buf.dtype == "float32"):
                self.buf = np.float64(self.buf)
        else:
            assert(False)

        self.fIdx = 0

    def clear_data(self):
        self.data = ayfbuf.ayfaudio_data()
        self.duration_samples = 0
        self.fIdx = 0
        self.buf = None

    def param_get(self):
        return self.data
        
    def prepare_oneBuf_noi_singlebuf(self, inBufHdl):
        inBufHdl.fld.fill(0)
        cpToStart = 0
        if(inBufHdl.fIdx * inBufHdl.bs >= self.duration_samples):
            return False

        idxStart = inBufHdl.fIdx * inBufHdl.bs
        idxStop =  (inBufHdl.fIdx+1) * inBufHdl.bs
        idxStop = min(idxStop, self.duration_samples)        
        cpNum = idxStop - idxStart
        chansLim = min(self.data.nChans, inBufHdl.chans)
        for idxChan in range(0, chansLim):
            inBufHdl.fld[cpToStart:cpToStart + cpNum] = self.buf[idxChan][idxStart:idxStart + cpNum]
            cpToStart += inBufHdl.bs        
        return True


    def prepare_oneBuf_noi_bufarr(self, inBufHdl, frameIdx):
        assert(0)

    def prepare_oneBuf_i_singlebuf(self, inBufHdl: ayfbuf.ayfaudio_buf):
        
        inBufHdl.fld.fill(0)
        nSamples = inBufHdl.params.bs * inBufHdl.params.chans
        
        # Only full frames. We need samples from frame 0 to frame 1 - therefore "+1"
        if((self.fIdx + 1) * nSamples >= self.duration_samples):
            return False
        
        # Extract buffer here!
        idxStart = self.fIdx * nSamples
        idxStop =  (self.fIdx+1) * nSamples
        idxStop = min(idxStop, (self.duration_samples * inBufHdl.params.chans)) 
        nSamples = idxStop - idxStart
                
        # Might be a 1D-array or a 2D-array
        firstBuf = self.buf   
        if(len(self.buf) == 2):
            firstBuf = self.buf[0]

        # Copy content and the frame
        inBufHdl.fld[0:nSamples] = firstBuf[idxStart:idxStop]  
        inBufHdl.fIdx = self.fIdx
        return True
    
    def prepare_single_frame(self, inBufHdl, frameIdx):
        match inBufHdl.tp:
            case ayfbuf.ProcessingType.NOI_SINGLEBUF:
                retVal = self.prepare_oneBuf_noi_singlebuf(inBufHdl) # No frameindex required as it handles that internally

            case ayfbuf.ProcessingType.NOI_BUFARR:
                retVal = self.prepare_oneBuf_noi_bufarr(inBufHdl, frameIdx)

            case ayfbuf.ProcessingType.I_SINGLEBUF:
                retVal = self.prepare_oneBuf_i_singlebuf(inBufHdl) # No frameindex required as it handles that internally
            case _:
                print('Error')
                assert(0)

        if(retVal == True):
            inBufHdl.fIdx = self.fIdx
            self.fIdx = self.fIdx + 1    
        return retVal

    def start(self, inBufHdl):
        return math.ceil( self.duration_samples/inBufHdl.params.bs)

    def stop(self) -> int: ...
        # Do nothing                

    def progress(self, inBufHdl: ayfbuf.ayfaudio_buf):
        return (self.fIdx* inBufHdl.params.bs)/ self.duration_samples
    
# ===========================================================================
# ===========================================================================

class ayfaudio_inpulse(ayfaudio_inbuffered):

    def __init__(self, duration_secs = 10, samplerate = 48000, nChans = 1, format = 'double'):

        self.data = ayfbuf.ayfaudio_data()
        self.data.sampling_rate = samplerate
        self.duration_samples = duration_secs * self.data.sampling_rate 
        self.fIdx = 0
        self.buf = None

        print("### AYFAUDIOINPULSE -- Creating pulse input.")

        if(format == "single"):
            self.buf = np.zeros(nChans * self.duration_samples, dtype="float32") 

        elif(format == "double"):
            
        # self.buf = np.float64(self.buf)
            self.buf = np.zeros(nChans * self.duration_samples, dtype="float64")
        else:
            assert(False)

        # Fill very first sample to realize the pulse
        for idxChan in range(0, nChans):
            self.buf[idxChan] = 1.0
                    
        szArr = self.buf.shape
        if(len(szArr) == 2):

            self.duration_samples = szArr[1]
            self.data.nChans = szArr[0]
        else:
            self.duration_samples = szArr[0]
            self.data.nChans = 1

# ==========================================================================
# ==========================================================================