#include <pybind11/pybind11.h>
#include "heymath.hpp"

namespace py = pybind11;

PYBIND11_MODULE(python_example, m) {
    m.def("add", &add);
    m.def("sub",&sub);
}

