
#include "include/functional_basis.h"

namespace py = pybind11;

using cpp_lagrange_basis_2d_fe1 = py_FunctionalBasis<2,2,1>;
using cpp_lagrange_basis_2d_fe2 = py_FunctionalBasis<2,2,2>;

PYBIND11_MODULE(_functional_basis, mod) {
    py::class_<cpp_lagrange_basis_2d_fe1>(mod, "cpp_lagrange_basis_2d_fe1") 
        .def(py::init<py::object, int>())
        .def("size"      	    , &cpp_lagrange_basis_2d_fe1::size            )
        .def("eval"       	    , &cpp_lagrange_basis_2d_fe1::eval            )
        .def("integrate" 	    , &cpp_lagrange_basis_2d_fe1::integrate       )
        .def("dofs_coordinates",  &cpp_lagrange_basis_2d_fe1::dofs_coordinates);

    py::class_<cpp_lagrange_basis_2d_fe2>(mod, "cpp_lagrange_basis_2d_fe2") 
        .def(py::init<py::object, int>())
        .def("size"      	    , &cpp_lagrange_basis_2d_fe2::size            )
        .def("eval"       	    , &cpp_lagrange_basis_2d_fe2::eval            )
        .def("integrate" 	    , &cpp_lagrange_basis_2d_fe2::integrate       )
        .def("dofs_coordinates",  &cpp_lagrange_basis_2d_fe2::dofs_coordinates);
}