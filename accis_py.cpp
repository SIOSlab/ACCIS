#include "accis.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(accis, a) {

    py::class_<accis_sat>(a, "sat")
        .def(py::init<>())
        .def("set_param",   &accis_sat::set_param)
        .def("get_param",   &accis_sat::get_param)
        .def("get_results", &accis_sat::get_results);

    a.def("run", &accis_run);

}
