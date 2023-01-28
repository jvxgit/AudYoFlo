import sys
import os

# Add path where to look for the pybind library
sys.path.append('../library')

# Add path where to look for additional libraries - note: the typical approach (PATH, local folder)
# seems not to work in Python!
# -> https://stackoverflow.com/questions/39734176/load-a-dll-with-dependencies-in-python
cwd = os.getcwd()
cwdupup = cwd.rsplit(os.sep, 2)[0]
cwddll = os.path.join(cwdupup, 'bin')

os.add_dll_directory(cwddll)
