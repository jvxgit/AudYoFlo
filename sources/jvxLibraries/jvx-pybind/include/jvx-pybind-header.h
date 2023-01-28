#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

// https://pybind11.readthedocs.io/en/stable/basics.html
// =========================================================================

namespace py = pybind11;

static std::string jvxDataString() {

    return JVX_DATA_DESCRIPTOR;
}

template <class T>
T* init_module()
{
    T* myModule = nullptr;
    myModule = new T();
    return myModule;
}

template <class T>
int term_module(T* myModule)
{
    int ret = JVX_ERROR_NOT_READY;
    if (myModule)
    {
        delete(myModule);
        ret = JVX_NO_ERROR;
    }
    return ret;
}
