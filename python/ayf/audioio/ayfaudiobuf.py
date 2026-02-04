
import numpy as np
from enum import Enum

# ======================================================================
# ======================================================================

class ProcessingType(Enum):
    UNDEFINED = 0
    I_SINGLEBUF = 1 # interleaved, singlebuffer [0 0 0 1 1 1]
    NOI_SINGLEBUF = 2 # non-interleaved, singlebuffer [0 0 0 1 1 1]
    NOI_BUFARR = 3 # non-interleaved, buffer array [[0 0 0][1 1 1]]

# ======================================================================
# ======================================================================
 
class ayfaudio_buf_param:

    def __init__(self):
        self.tp = ProcessingType.UNDEFINED
        self.chans = 0
        self.bs = 0
        self.sr = 0

    def params_set(self, operation = ProcessingType.I_SINGLEBUF, chans = 1, bs = 1024, sr = 48000, arith = 'double'):
        
        self.tp = operation
        self.chans = chans
        self.bs = bs
        self.sr = sr
        self.arith = arith

class ayfaudio_buf:

    def __init__(self):
        
        self.params = ayfaudio_buf_param()

        self.c = 0
        self.l = 0
        self.fld = None
        self.fIdx = 0

    def allocate(self, operation, nChans, fsize, srate, arith):

        self.params.params_set(chans = nChans, operation = operation, bs = fsize, sr = srate, arith = arith)
        
        match self.params.tp:
            case ProcessingType.I_SINGLEBUF:
                self.c = 1
                self.l = nChans * fsize
                self.tp = ProcessingType.I_SINGLEBUF
                self.fld = np.zeros(self.l, dtype=arith)

            case ProcessingType.NOI_SINGLEBUF:
                self.c = 1
                self.l = nChans * fsize
                self.tp = ProcessingType.NOI_SINGLEBUF
                self.fld = np.zeros(self.l, dtype=arith)

            case ProcessingType.NOI_BUFARR:
                self.c = nChans
                self.l = fsize
                self.tp = ProcessingType.NOI_BUFARR
                self.fld = np.zeros(self.l, dtype=arith)

            case _:
                assert(False)


    def deallocate(self):
        self.params.tp = ProcessingType.UNDEFINED
        self.params.chans = 0
        self.params.bs = 0

        self.c = 0
        self.l = 0
        self.fld = None
        self.fIdx = 0

# ======================================================================
# ======================================================================

class ayfaudio_data:
    def __init__(self, srate = 48000, nChans = 2):
        self.sampling_rate = srate
        self.nChans = nChans
  
# ======================================================================
# ======================================================================

class ayfaudio_outbuf:

    def __init__(self, maxLength = 48000, nChans = 2, fsize = 128):
        self.buf = None
        self.data = ayfaudio_data()
        self.duration_samples = 0        
        self.maxLength = maxLength
        self.fsize = fsize
        self.nChans = nChans
        self.fIdx = 0

    def postprocess_oneBuf_noi_singlebuf(self, outBufHdl):
        cpToStart = 0
        idxStart = outBufHdl.fIdx * outBufHdl.bs
        idxStop =  (outBufHdl.fIdx+1) * outBufHdl.bs
        idxStart = min(idxStart, self.duration_samples)
        idxStop = min(idxStop, self.duration_samples)
        cpNum = idxStop - idxStart
        if(cpNum > 0):
            for idxChan in range(0, self.nChans):
                self.buf[idxChan][idxStart:idxStart + cpNum] = outBufHdl.fld[cpToStart:cpToStart + cpNum]
                cpToStart += outBufHdl.bs
            
    def postprocess_oneBuf_noi_bufarr(self, outBufHdl, idxFrame):
        assert(0)

    def postprocess_single_frame(self, outBufHdl, idxFrame):
        match outBufHdl.tp:
            case ProcessingType.UNDEFINED:
                print('Error')
                assert(0)

            case ProcessingType.NOI_SINGLEBUF:
                self.postprocess_oneBuf_noi_singlebuf(outBufHdl)

            case ProcessingType.NOI_BUFARR:
                self.postprocess_oneBuf_noi_bufarr(outBufHdl, idxFrame)
        
        self.fIdx = outBufHdl.fIdx

    def param_get(self):
        return self.data

    def start(self, arith, nFrames = -1):
        
        if(nFrames > 0):
            self.duration_samples = nFrames * self.fsize
        else:
            self.duration_samples = self.maxLength * self.fsize

        self.buf = np.empty((self.nChans, int(self.duration_samples)),dtype=arith)
