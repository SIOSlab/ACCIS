#include "accis_io.hpp"
#include "accis_sim.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(accis, a) {
    
    py::class_<sat_info>(a, "sat_info", py::dynamic_attr())
        .def(py::init<>())
        .def_readwrite("filter_type", &sat_info::filter_type);

}
