import setuptools
import Cython.Build as cb
import numpy

setuptools.setup(
	name='Haukes Cython Test Project', 
	ext_modules=cb.cythonize('haukesTest.pyx'),
    compiler_directives={'language_level' : "3"}, 
	include_dirs=[numpy.get_include()])

