#include "accis_sat.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(accis, s) {
    py::class_<accis_sat>(s, "satellite")
        .def(py::init<const std::unordered_map<std::string,int>&,
            const std::unordered_map<std::string,double>&>())
        .def("get_params_i", &accis_sat::get_params_i)
        .def("get_params_d", &accis_sat::get_params_d)
        .def("step", &accis_sat::step)
        .def("transmit", &accis_sat::transmit);
}


