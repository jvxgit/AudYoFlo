import sys
import os

# Add path where to look for the pybind library
sys.path.append('O:/audyoflo/audio-win64/release/runtime/python/library')
# sys.path.append('../library')

# Add path where to look for additional libraries - note: the typical approach (PATH, local folder)
# seems not to work in Python!
# -> https://stackoverflow.com/questions/39734176/load-a-dll-with-dependencies-in-python
os.add_dll_directory('O:/audyoflo/audio-win64/release/runtime/bin')
#os.add_dll_directory('../../bin')
