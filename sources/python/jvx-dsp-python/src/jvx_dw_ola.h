#include "jvx.h"

void* init_processing_dw_ola(int hopsize, int framesize);
int term_processing_dw_ola(void* hdlArg);
int run_processing_dw_ola(void* hdlArg, pybind11::array_t<jvxData> bufio, pybind11::array_t<std::complex<jvxData>> weights);
int run_processing_dw_ola_raw(void* hdlArg, jvxData* bufio, const std::complex<jvxData>* weights);