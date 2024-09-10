#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>

#include "include/mesh.h"

namespace py = pybind11;

typedef py_Mesh<2,2> cpp_domain_2d;
typedef py_Mesh<3,3> cpp_domain_3d;

PYBIND11_MODULE(fdapdepy, mod) {
    py::class_<py_Mesh<2,2>>(mod, "cpp_domain_2d") // auto domain = 
        .def(py::init< Eigen::Ref<const DMatrix<double>>, 
                       Eigen::Ref<const DMatrix<int>>, 
                       Eigen::Ref<const DMatrix<int>> >())
        .def("nodes",       &cpp_domain_2d::nodes)
        .def("triangles",   &cpp_domain_2d::cells)
        .def("neighbors",   &cpp_domain_2d::neighbors)
        .def("edges", 
                static_cast<const DMatrix<int, Eigen::RowMajor> (cpp_domain_2d::*)() const>(&cpp_domain_2d::edges));

    py::class_<py_Mesh<3,3>>(mod, "cpp_domain_3d") // auto domain = 
        .def(py::init< Eigen::Ref<const DMatrix<double>>, 
                       Eigen::Ref<const DMatrix<int>>, 
                       Eigen::Ref<const DMatrix<int>> >())
        .def("nodes",       &cpp_domain_3d::nodes)
        .def("tetrahedrons",   &cpp_domain_3d::cells)
        .def("neighbors",   &cpp_domain_3d::neighbors)
        .def("faces", 
                static_cast<const DMatrix<int, Eigen::RowMajor> (cpp_domain_3d::*)() const>(&cpp_domain_3d::faces));

}

//PYBIND11_MODULE(fdapdepy, mod3d) {
    

//}       
     
