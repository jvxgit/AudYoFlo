import numpy as np
import pyximport
pyximport.install(setup_args={"script_args" : ["--verbose"],"include_dirs":np.get_include()})
from haukesTest import method

if __name__ == "__main__":
    out = method(np.array([1.0, 2.0, 3.0],dtype=np.float64))    

