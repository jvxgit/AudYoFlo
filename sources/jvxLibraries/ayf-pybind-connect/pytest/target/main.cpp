#include "jvx.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "CjvxCToPythonConverter.h"
#include "CjvxPythonToCConverter.h"

// Smoke test module for ayf-pybind-connect. Exercises a handful of round-trips
// (scalar/list/string/selection-list) end to end through the ported converter
// classes, without requiring a full jvx component/property host.
// https://pybind11.readthedocs.io/en/stable/basics.html
// =========================================================================

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

static pybind11::object
roundtrip_data_list(pybind11::object arr)
{
	// Read the incoming numpy array via pyArgument2Type-style buffer access,
	// then hand it straight back out through pyReturnDataList to prove both
	// directions work against the same jvxData buffer.
	if (!py::isinstance<py::array_t<jvxData>>(arr))
	{
		throw std::runtime_error("expected a jvxData numpy array");
	}
	py::array_t<jvxData> typed = arr.cast<py::array_t<jvxData>>();
	jvxSize n = (jvxSize)typed.size();
	return CjvxCToPythonConverter::pyReturnDataList(typed.data(), n);
}

static jvxData
roundtrip_scalar(std::vector<pybind11::object> args, jvxSize idx)
{
	jvxData value = 0.0;
	jvxErrorType res = CjvxPythonToCConverter::pyArgument2Data(value, args, idx, (jvxSize)args.size());
	if (res != JVX_NO_ERROR)
	{
		throw std::runtime_error("pyArgument2Data failed: " + std::string(jvxErrorType_descr(res)));
	}
	return value;
}

static pybind11::object
roundtrip_selection_list(std::vector<std::string> options, int selectedIdx, int exclusiveIdx)
{
	jvxSelectionList selList((jvxSize)options.size());
	selList.strList.assign(options);
	jvx_bitZSet(selList.bitFieldSelected(), selectedIdx);
	if (exclusiveIdx >= 0)
	{
		jvx_bitZSet(selList.bitFieldExclusive, exclusiveIdx);
	}
	return CjvxCToPythonConverter::pyReturnSelectionListStruct(selList);
}

static pybind11::object
roundtrip_string_list(std::vector<std::string> values)
{
	return CjvxCToPythonConverter::pyReturnStringList(values);
}

PYBIND11_MODULE(ayf_pybind_connect_smoketest, m) {
	m.doc() = R"pbdoc(
		Smoke test for ayf-pybind-connect
		----------------------------------

		.. currentmodule:: ayf_pybind_connect_smoketest

	)pbdoc";

	m.def("roundtrip_data_list", &roundtrip_data_list, R"pbdoc(
		Take a jvxData numpy array in, hand it back out via pyReturnDataList.
	)pbdoc");

	m.def("roundtrip_scalar", &roundtrip_scalar, R"pbdoc(
		Read the idx-th positional argument as a jvxData scalar via pyArgument2Data.
	)pbdoc");

	m.def("roundtrip_selection_list", &roundtrip_selection_list, R"pbdoc(
		Build a jvxSelectionList in C++ and return it as a python dict via
		pyReturnSelectionListStruct.
	)pbdoc");

	m.def("roundtrip_string_list", &roundtrip_string_list, R"pbdoc(
		Return a list of strings via pyReturnStringList.
	)pbdoc");

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = "dev";
#endif
}
