#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "jvx_dw_ola.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

// https://pybind11.readthedocs.io/en/stable/basics.html
// =========================================================================

void add(pybind11::array_t<jvxData> vv, int j) {
    // return i + j;
    int i;
    pybind11::ssize_t sz = vv.size();
    //const jvxData* buf_in = vv.data();
    jvxData* buf_in = vv.mutable_data();
    for (i = 0; i < sz; i++)
    {
        buf_in[i] += j;
    }
}

namespace py = pybind11;

PYBIND11_MODULE(jvx_dsp_python, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: jvx_dsp_python

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    m.def("init_processing_dw_ola", &init_processing_dw_ola, R"pbdoc(
        Initialize the processing
    )pbdoc");

    m.def("term_processing_dw_ola", &term_processing_dw_ola, R"pbdoc(
        Terminate the processing
    )pbdoc");

    m.def("run_processing_dw_ola", &run_processing_dw_ola, R"pbdoc(
        Run the processing
    )pbdoc");

    // =====================================================

    m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

    m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
