/**
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "main_header.h"
#include "jvx.h"
#include "SimpleKernel.h"
#ifdef JVX_WP_MODULE_EMSCRIPTEN
 // Declare the entry symbols
EMSCRIPTEN_BINDINGS(CLASS_SimpleKernel) {
    class_<SimpleKernel>("SimpleKernel")
        .constructor()
        .function("initialize",
            &SimpleKernel::initialize)
        .function("process",
            &SimpleKernel::Process,
            allow_raw_pointers())
        .function("setGain",
            &SimpleKernel::setGain)
        .function("snapshot",
            &SimpleKernel::snapshot)
        .function("getAvrg",
            &SimpleKernel::getAvrg)
        .function("getMax",
            &SimpleKernel::getMax)
        .function("getClip",
            &SimpleKernel::getClip);
}
#else

SimpleKernel* init_module()
{
    SimpleKernel* myModule = nullptr;
    myModule = new SimpleKernel();
    return myModule;
}

int term_module(SimpleKernel* myModule)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        delete(myModule);
        ret = JVX_NO_ERROR;
    }
    return ret;
}

int initialize(SimpleKernel* myModule, int bsize, int srate, int chans, int chansMax)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        myModule->initialize(bsize, srate, chans, chansMax);
        ret = JVX_NO_ERROR;
    }
    return ret;
}

int process_noi_onebuf(SimpleKernel* myModule, pybind11::array_t<jvxData> bufi, pybind11::array_t<jvxData> bufo, int chansIn, int chansOut, int fIdx)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        myModule->verifySize(bufi.size()/chansIn);
        myModule->verifySize(bufo.size() /chansOut);
        myModule->Process((uintptr_t)bufi.data(), (uintptr_t)bufo.mutable_data(), chansIn);
        ret = JVX_NO_ERROR;
    }
    return ret;
}

int process_no_multibuf(SimpleKernel* myModule, pybind11::array_t< pybind11::array_t<jvxData> >bufi, pybind11::array_t< pybind11::array_t<jvxData> >bufo, int chansIn, int chansOut, int fIdx)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        myModule->verifySize(bufi.size());
        myModule->verifySize(bufo.size());
        myModule->Process((uintptr_t)bufi.data(), (uintptr_t)bufo.mutable_data(), chansIn);
        ret = JVX_NO_ERROR;
    }
    return ret;
}

int setgain(SimpleKernel* myModule, jvxData val)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        myModule->setGain(val);
        ret = JVX_NO_ERROR;
    }
    return ret;
}

jvxData getgain(SimpleKernel* myModule)
{
    jvxData val = -1;
    if (myModule)
    {
        val =  myModule->getGain();
    }
    return val;
}

namespace py = pybind11;

PYBIND11_MODULE(SimpleKernel, m) {

    // Here is our opaque type: we no longer may return it as a "void*", therefore,
    // we need to define it as specific class,
    // 
    // https://stackoverflow.com/questions/50641461/dealing-with-opaque-pointers-in-pybind11/55991847#55991847
    // 
    py::class_<SimpleKernel>(m, "SimpleKernel");
    m.doc() = R"pbdoc(
        Pybind11 bind of webassembly SimpleKernel
        -----------------------
    )pbdoc";

    // The allocated object must be deleted by the moduloe, therefore the rerutn value of this functions
    // is tagged to be a "reference",
    //  
    // https://pybind11.readthedocs.io/en/stable/advanced/functions.html
    //
    m.def("init_module", &init_module, py::return_value_policy::reference, R"pbdoc(
        Initialize the processing
    )pbdoc");

    m.def("term_module", &term_module, R"pbdoc(
        Terminate the processing
    )pbdoc");

    m.def("initialize", &initialize, py::return_value_policy::reference, R"pbdoc(
        Initialize the processing
    )pbdoc", py::arg("module"), py::arg("bsize"), py::arg("srate"), py::arg("channels"), py::arg("chansMax"));

    m.def("process_noi_onebuf", &process_noi_onebuf, R"pbdoc(
        Process one buffer, input fdata is non-interleaved on one buffer
    )pbdoc",
        py::arg("module"), py::arg("input buffer"), py::arg("output buffer"), py::arg("num chans in"),
        py::arg("num chans out"), py::arg("fIdx"));

    m.def("setgain", &setgain, R"pbdoc(
        Set processing gain
    )pbdoc");

    m.def("getgain", &getgain, R"pbdoc(
        Get processing gain
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
#endif
