import numpy as np

class ayfaudio_processor:
    # @abstractmethod
    def prepare(self, numIn = 1, numOut = 1, bSize = 128, sRate = 48000) -> int: ...

    # @abstractmethod
    def process(self, inBuf, outBuf) -> int: ...

    # @abstractmethod
    def postprocess(self) -> int: ...

# ======================================================================
# ======================================================================