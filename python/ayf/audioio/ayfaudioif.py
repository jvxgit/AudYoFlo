import numpy as np
import ayfaudiobuf as ayfbuf

class ayfaudio_in:
    # @abstractmethod
    def prepare_data(self, formatSystem) -> None: ...

    # @abstractmethod
    def clear_data(self) -> None: ...

    # @abstractmethod
    def term(self) -> None: ...

    # @abstractmethod
    def param_get(self) -> ayfbuf.ayfaudio_data: ...

    # @abstractmethod
    def prepare_single_frame(self, inBufHdl) -> bool: ...

    # @abstractmethod
    def start(self, inBufHdl) -> int: ...

    # @abstractmethod
    def progress(self, inBufHdl) -> float: ...

    # =============================================================================
    # =============================================================================

class ayfaudio_processor:
    # @abstractmethod
    def prepare(self, numIn = 1, numOut = 1, bSize = 128, sRate = 48000) -> int: ...

    # @abstractmethod
    def process(self, inBuf, outBuf) -> int: ...

    # @abstractmethod
    def postprocess(self) -> int: ...

# ======================================================================
# ======================================================================