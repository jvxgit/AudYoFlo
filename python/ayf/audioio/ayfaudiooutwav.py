import pathlib
import numpy as np
import math
import soundfile as sf

import ayfaudioio as ayfio
import ayfaudiobuf as ayfbuf

class ayfaudio_outwav:

    def __init__(self, fName):
        self.fname = fName
        self.data = ayfbuf.ayfaudio_data()
        self.duration_samples = 0
        self.fIdx = 0
        self.buf = None
        self.params = ayfbuf.ayfaudio_buf_param()        
        path_output = pathlib.Path(self.fname)
        self.duration_samples = 0

        # self.buf,  self.data.sampling_rate = af.read(str(path_input))        
        # szArr = self.buf.shape
        # self.duration_samples = szArr[1]
        # self.data.nChans = szArr[0]
    
    def start(self, arith, nFrames, obuf):
        
        self.params = obuf.params
        self.duration_samples = nFrames
        match self.params.tp:

            case ayfbuf.ProcessingType.NOI_SINGLEBUF:
                assert(0) # retVal = self.prepare_oneBuf_noi_singlebuf(inBufHdl) # No frameindex required as it handles that internally

            case ayfbuf.ProcessingType.NOI_BUFARR:
                assert(0) # retVal = self.prepare_oneBuf_noi_bufarr(inBufHdl, frameIdx)

            case ayfbuf.ProcessingType.I_SINGLEBUF:

                self.buf = np.zeros(self.params.bs * self.params.chans * self.duration_samples , dtype=arith)       

            case _:
                print('Error')
                assert(0)
                
        # self.buf = np.zeros( * nFrames, dtype=float)
        # Create the space for output buffers
 
    def postprocess_single_frame(self, outBufHdl: ayfbuf.ayfaudio_buf):
        match outBufHdl.tp:
            
            case ayfbuf.ProcessingType.NOI_SINGLEBUF:
                retVal = 0 # self.prepare_oneBuf_noi_singlebuf(inBufHdl) # No frameindex required as it handles that internally

            case ayfbuf.ProcessingType.NOI_BUFARR:
                retVal = 0 # self.prepare_oneBuf_noi_bufarr(inBufHdl, frameIdx)

            case ayfbuf.ProcessingType.I_SINGLEBUF:

                firstBuf = outBufHdl.fld  
                if(len(outBufHdl.fld) == 2):
                    firstBuf = outBufHdl.fld[0]
                
                nSamples = self.params.bs * self.params.chans

                idxStart = self.fIdx * nSamples
                idxStop = (self.fIdx + 1) * nSamples

                self.buf[idxStart:idxStop] = firstBuf[0:nSamples]
                retVal = True # self.prepare_oneBuf_i_singlebuf(inBufHdl) # No frameindex required as it handles that internally

            case _:
                print('Error')
                assert(0)

        # Forward sample counter
        if(retVal == True):
            outBufHdl.fIdx = self.fIdx
            self.fIdx = self.fIdx + 1    
        return retVal    
    
    def stop(self):
        # Store collected buffer to file. Convert interleaved buffers first to non.nterleaved array
        buf = np.asarray(self.buf)        
        frames = math.ceil(buf.size/self.params.chans)
        data = buf.reshape(frames, self.params.chans) 

        #b = np.asarray(self.buf)
        #print("buf type:", type(self.buf))
        #print("buf shape:", b.shape, "dtype:", b.dtype)
        #print("sr:", self.params.sr, type(self.params.sr))        
        sf.write(self.fname, data, samplerate= self.params.sr, subtype="PCM_16", format="wav")
        print("### AYFAUDIOOUTWAV -- Saved data in file {self.fname}")