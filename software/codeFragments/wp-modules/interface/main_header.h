#ifdef JVX_WP_MODULE_EMSCRIPTEN

#include "emscripten/bind.h"
using namespace emscripten;

#else

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#endif