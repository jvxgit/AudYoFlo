#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "ayf_starter_python.h"

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

PYBIND11_MODULE(ayf_starter_python, m) {
    m.doc() = R"pbdoc(
        Pybind11 AudYoFlo component connect plugin for Starter project
        -----------------------

        .. currentmodule:: ayf_starter_python

        .. autosummary::
           :toctree: _generate

           init_ayf_starter
           term_ayf_starter
           run_frame_ayf_starter
    )pbdoc";

    m.def("init_ayf_starter", &init_ayf_starter, 
        py::arg("nChannelsIn"),
        py::arg("nChannelsOut"), 
        py::arg("fsize"),
        py::arg("samplerate"),
        py::arg("ayfIdentSlot_node"),
        py::arg("ayfIdentSlot_dev"),
        py::arg("nmIniFile"),
        py::arg("cbOnEvent"),
        R"pbdoc(Initialize the module)pbdoc");

    m.def("term_ayf_starter", &term_ayf_starter, R"pbdoc(
        Terminate the processing
    )pbdoc");

    m.def("run_frame_ayf_starter", &run_frame_ayf_starter, 
        py::arg("hdlArg"), 
        py::arg("nChannelsIn"), 
        py::arg("nChannelsOut"), 
        py::arg("fSize"), 
        py::arg("ptrsIn"), 
        py::arg("ptrsOut"),        
        R"pbdoc(Run the processing)pbdoc");

    // =====================================================

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
