import local
import SimpleKernel
import os

class AudioProcess:
    def __init__(self):
        help(SimpleKernel)
        pid = os.getpid()
        print("Current pID = <{!r}".format(pid))
        self.modInst = None
        self.framesize = 128
        self.srate = 48000

    def initialize(self):
        self.modInst = SimpleKernel.init_module()
        SimpleKernel.initialize(self.modInst, self.framesize, self.srate, 2, 2)

    def terminate(self):
        SimpleKernel.term_module(self.modInst)
        modInst = None

    def process(self, input, output, frameIdx = -1):
        #  output.fld = input.fld

        # Run the core data copy function here!
        SimpleKernel.process_noi_onebuf(self.modInst, input.fld, output.fld, input.chans, output.chans, frameIdx)
        output.fIdx = input.fIdx
        return 0

    def progress_step(self, progress):
        print('Progress: {!r}'.format( progress))