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

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>

#include "include/mesh.h"

namespace fdapde{
namespace py{

#define mesh_pybind_interface(LocalDim, EmbedDim)                                                                                  \
      .def(pybind11::init< Eigen::Ref<const DMatrix<double>>, Eigen::Ref<const DMatrix<int>>, Eigen::Ref<const DMatrix<int>> > ()) \
      .def("nodes",       &Mesh<LocalDim, EmbedDim>::nodes)                                                                        \
      .def("neighbors",   &Mesh<LocalDim, EmbedDim>::neighbors)                                                                
      

typedef Mesh<2,2> cpp_domain_2d;
typedef Mesh<3,3> cpp_domain_3d;

PYBIND11_MODULE(_domain, mod) {
    pybind11::class_<Mesh<2,2>>(mod, "cpp_domain_2d")
         mesh_pybind_interface(2,2)  
        .def("triangles",   &cpp_domain_2d::cells)
        .def("edges", 
                static_cast<const DMatrix<int, Eigen::RowMajor> (cpp_domain_2d::*)() const>(&cpp_domain_2d::edges));

    pybind11::class_<Mesh<3,3>>(mod, "cpp_domain_3d") // auto domain = 
         mesh_pybind_interface(3,3)
        .def("tetrahedrons",   &cpp_domain_3d::cells)
        .def("faces", 
                static_cast<const DMatrix<int, Eigen::RowMajor> (cpp_domain_3d::*)() const>(&cpp_domain_3d::faces));

}

}   // namespace py
}   // namespace fdapde
     
