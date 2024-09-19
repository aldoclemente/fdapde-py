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

#include "include/regression.h"

namespace fdapde {
namespace py {
  
#define regression_rcpp_interface(Model)                                                                            \
      .def("set_lambda"      , &RegressionModel<Model>::set_lambda      )                                           \
      .def("set_covariates"  , &RegressionModel<Model>::set_covariates  )                                           \
      .def("set_observations", &RegressionModel<Model>::set_observations)                                           \
      .def("fit"             , &RegressionModel<Model>::fit             )                                           \
      .def("fitted"          , &RegressionModel<Model>::fitted          )                                           \
      .def("f"               , &RegressionModel<Model>::f               )                                           \
      .def("gcvs"            , &RegressionModel<Model>::gcvs            )                                           \
      .def("edfs"            , &RegressionModel<Model>::edfs            )                                           \
      .def("optimum"         , &RegressionModel<Model>::optimum         ) 

//using cpp_srpde = SRPDE;

PYBIND11_MODULE(_regression, mod) {
    pybind11::class_< RegressionModel<models::SRPDE>>(mod, "cpp_srpde_base")
        regression_rcpp_interface(models::SRPDE);

    pybind11::class_<SRPDE, RegressionModel<models::SRPDE>>(mod, "cpp_srpde")
        .def(pybind11::init<pybind11::object, int>());

    pybind11::class_< RegressionModel<models::GSRPDE<models::SpaceOnly>> >(mod, "cpp_gsrpde_base")
        regression_rcpp_interface(models::GSRPDE<models::SpaceOnly>);

    pybind11::class_<GSRPDE<models::SpaceOnly>, RegressionModel<models::GSRPDE<models::SpaceOnly>>>(mod, "cpp_gsrpde")
        .def(pybind11::init<pybind11::object, int ,int>())
        .def("set_fpirls_tolerance", &GSRPDE<models::SpaceOnly>::set_fpirls_tolerance)
        .def("set_fpirls_max_iter" , &GSRPDE<models::SpaceOnly>::set_fpirls_max_iter );
}

}
}