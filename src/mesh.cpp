#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>

#include <fdaPDE/utils.h>
#include <fdaPDE/geometry.h>

namespace py = pybind11;

using mesh2d = fdapde::core::Triangulation<2, 2>;

PYBIND11_MODULE(fdapdepy, mesh) {
    py::class_<mesh2d>(mesh, "mesh2d")
        .def(py::init< Eigen::Ref<const DMatrix<double>>, 
                       Eigen::Ref<const DMatrix<int>>, 
                       Eigen::Ref<const DMatrix<int>> >())
        .def("nodes",       &mesh2d::nodes)
        .def("triangles",   &mesh2d::cells)
        .def("edges",       &mesh2d::edges)
        .def("neighbors",   &mesh2d::neighbors);
}

