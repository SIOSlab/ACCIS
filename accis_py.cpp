#include "accis.hpp"
#include "cam_observer.hpp"
#include "generator.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(accis, a) {

    py::class_<accis_sat>(a, "sat")
        .def(py::init<>())
        .def("set_param",   &accis_sat::set_param)
        .def("get_param",   &accis_sat::get_param)
        .def("get_results", &accis_sat::get_results)
        .def("step",        &accis_sat::step)
        .def("transmit",    &accis_sat::transmit);

    py::class_<generator>(a, "generator")
        .def(py::init<>())
        .def("set_param", &generator::set_param)
        .def("get_param", &generator::get_param)
        .def("run",       &generator::run); 

    py::class_<cam_observer>(a, "cam_observer")
        .def(py::init<>())
        .def("set_param", &cam_observer::set_param)
        .def("set_pts", &cam_observer::set_pts)
        .def("h", &cam_observer::h); 

}
