#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "jvx.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

// https://pybind11.readthedocs.io/en/stable/basics.html
// =========================================================================

std::string jvxDataString() {
    
    return JVX_DATA_DESCRIPTOR;
}

namespace py = pybind11;

PYBIND11_MODULE(jvx_helper_python, m) {
    m.doc() = R"pbdoc(
        Pybind11 jvxrt helpers
        -----------------------

        .. currentmodule:: jvx_helper_python

    )pbdoc";

    m.def("jvxDataString", &jvxDataString, R"pbdoc(
        Return String to find out the <jvxData> arithmetic
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
