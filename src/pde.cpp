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

#include "include/pde.h"

namespace fdapde {
namespace py {
  
#define pde_pybind_interface(LocalDim, EmbedDim, Order)                                                             \
      .def(pybind11::init<pybind11::object, pybind11::int_, pybind11::object>())                                    \
      .def("quadrature_nodes" , &PDE_<LocalDim, EmbedDim, Order>::quadrature_nodes )                                \
      .def("dofs_coordinates" , &PDE_<LocalDim, EmbedDim, Order>::dofs_coordinates )                                \
      .def("mass"                 , &PDE_<LocalDim, EmbedDim, Order>::mass                 )                        \
      .def("stiff"                , &PDE_<LocalDim, EmbedDim, Order>::stiff                )                        \
      .def("force"                , &PDE_<LocalDim, EmbedDim, Order>::force                )                        \
      .def("set_dirichlet_bc"     , &PDE_<LocalDim, EmbedDim, Order>::set_dirichlet_bc     )                        \
      .def("set_forcing"          , &PDE_<LocalDim, EmbedDim, Order>::set_forcing          )                        \
      .def("set_initial_condition", &PDE_<LocalDim, EmbedDim, Order>::set_initial_condition)                        \
      .def("init"                 , &PDE_<LocalDim, EmbedDim, Order>::init                 )

typedef PDE_<2, 2, 1> cpp_pde_2_2_1;
typedef PDE_<2, 2, 2> cpp_pde_2_2_2;
typedef PDE_<3, 3, 1> cpp_pde_3_3_1;
// typedef PDE_<3, 3, 2> cpp_pde_3_3_2; NOT available ReferenceElements::bary_coords

PYBIND11_MODULE(_pde, mod){ 
    pybind11::class_<PDE>(mod, "cpp_pde");                      

    pybind11::class_<PDE_<2, 2, 1>, PDE>(mod, "cpp_pde_2_2_1") // Specify the Parent Class!
        pde_pybind_interface(2, 2, 1);

    pybind11::class_<PDE_<2, 2, 2>, PDE>(mod, "cpp_pde_2_2_2") 
        pde_pybind_interface(2, 2, 2); 

    pybind11::class_<PDE_<3, 3, 1>, PDE>(mod, "cpp_pde_3_3_1")
        pde_pybind_interface(3, 3, 1);

    //pybind11::class_<PDE_<3, 3, 1>, PDE>(mod, "cpp_pde_3_3_1")
    //    pde_pybind_interface(3, 3, 2); 
}

}   // namespace py
}   // namespace fdapde
