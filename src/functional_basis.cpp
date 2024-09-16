// This file is part of fdaPDE, a C++ library for physics-informed
// spatial and functional data analysis.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "include/functional_basis.h"

namespace fdapde{
namespace py{

#define functional_basis_pybind_interface(LocalDim, EmbedDim, Order)                         \
    .def(pybind11::init<pybind11::object, int>())                                            \
    .def("size"      	    , &FunctionalBasis<LocalDim, EmbedDim, Order>::size            ) \
    .def("eval"       	    , &FunctionalBasis<LocalDim, EmbedDim, Order>::eval            ) \
    .def("integrate" 	    , &FunctionalBasis<LocalDim, EmbedDim, Order>::integrate       ) \
    .def("dofs_coordinates",  &FunctionalBasis<LocalDim, EmbedDim, Order>::dofs_coordinates)

typedef FunctionalBasis<2,2,1> cpp_lagrange_basis_2d_fe1;
typedef FunctionalBasis<2,2,2> cpp_lagrange_basis_2d_fe2;

typedef FunctionalBasis<3,3,1> cpp_lagrange_basis_3d_fe1;
// using cpp_lagrange_basis_3d_fe2 = FunctionalBasis<3,3,2>; NOT available ReferenceElements::bary_coords

PYBIND11_MODULE(_functional_basis, mod) {
    pybind11::class_<cpp_lagrange_basis_2d_fe1>(mod, "cpp_lagrange_basis_2d_fe1") 
        functional_basis_pybind_interface(2,2,1);

    pybind11::class_<cpp_lagrange_basis_2d_fe2>(mod, "cpp_lagrange_basis_2d_fe2") 
        functional_basis_pybind_interface(2,2,2);

    pybind11::class_<cpp_lagrange_basis_3d_fe1>(mod, "cpp_lagrange_basis_3d_fe1") 
        functional_basis_pybind_interface(3,3,1);

    //pybind11::class_<cpp_lagrange_basis_3d_fe2>(mod, "cpp_lagrange_basis_3d_fe2") 
    //    functional_basis_pybind_interface(3,3,2);
}

}   // namespace py
}   // namespace fdapde
