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

#ifndef __PY_PDE_H__
#define __PY_PDE_H__

// we only include RcppEigen.h which pulls Rcpp.h in for us

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>
#include <unordered_set>
#include <Eigen/Dense>
#include <fdaPDE/pde.h>
#include <fdaPDE/utils/symbols.h>

#include "mesh.h"
#include "utils.h"
using fdapde::core::FEM;

namespace fdapde {
namespace py {

// supported pde instantiations
enum pde_type {
    simple_laplacian = 1,        // \mu * \Delta f
    second_order_elliptic = 2,   // div(K * \nabla f) + dot(b, \nabla f) + c * f
    second_order_parabolic = 3   // df/dt + div(K * \nabla f) + dot(b, \nabla f) + c * f
};

// given a py::PDE_<M, N, R>, we can always cast to an py::PDE to recover the pde backend
struct PDE {
    using PDEType = erase<heap_storage, core::PDE__>;
    PDEType pde;
};

template <int M, int N, int R> class PDE_ : public PDE {
   private:
    using DomainType = core::Triangulation<M, N>;
    using QuadratureRule = core::Integrator<FEM, DomainType::local_dim, R>;
    template <typename L> using PDEType = core::PDE<DomainType, L, DMatrix<double>, core::FEM, core::fem_order<R>>;
    // internal data
    DomainType domain_ {};           // triangulation
    QuadratureRule integrator_ {};   // quadrature rule (exact for the provided fem order)
    int pde_type_ = 0;               // one of the pde_type enum values
    bool init_ = false;              // asserted true only after execution of init()
   public:
    // constructor
    PDE_(pybind11::object py_mesh, pybind11::int_ pde_type, pybind11::object pde_params) : pde_type_(pybind11::cast<int>(pde_type)) {
        domain_ = get_obj_as<py::Mesh<M, N>>(py_mesh, "cpp_handler")->domain();
        pde_type_ = pybind11::cast<int>(pde_type);
        // unpack pde parameters
        pybind11::dict pde_params_ = pybind11::cast<pybind11::dict>(pde_params);
        // instantiate pde template based on pde type
        switch (pde_type_) {
        case pde_type::simple_laplacian: {   // special acceleration for simple laplacian penalty
            double mu = pybind11::cast<double>(pde_params_["diff"]);
            auto L = mu * core::laplacian<FEM>();
            pde = PDEType<decltype(L)>(domain_, L);
        } break;
        case pde_type::second_order_elliptic: {
            SMatrix<M> K = pybind11::cast<DMatrix<double>>(pde_params_["diff"]);
	        SVector<M> b = pybind11::cast<DMatrix<double>>(pde_params_["tran"]);
	        double c = pybind11::cast<double>(pde_params_["reac"]);
            auto L = core::diffusion<FEM>(K) + core::advection<FEM>(b) + core::reaction<FEM>(c);
            pde = PDEType<decltype(L)>(domain_, L);
        } break;
        case pde_type::second_order_parabolic: {
            SMatrix<M> K = pybind11::cast<DMatrix<double>>(pde_params_["diff"]);
	        SVector<M> b = pybind11::cast<DMatrix<double>>(pde_params_["tran"]);
	        double c = pybind11::cast<double>(pde_params_["reac"]);
            DVector<double> time_nodes = pybind11::cast<DVector<double>>(pde_params_["time_nodes"]);
            auto L = core::dt<FEM>() + core::diffusion<FEM>(K) + core::advection<FEM>(b) + core::reaction<FEM>(c);
            pde = PDEType<decltype(L)>(domain_, time_nodes, L);
        } break;
        }
    }
    // setters
    void set_dirichlet_bc(const DMatrix<double>& data) { pde.set_dirichlet_bc(data); }
    void set_forcing(const DMatrix<double>& data) { pde.set_forcing(data); }
    void set_initial_condition(const DVector<double>& data) {
        fdapde_assert(pde_type_ == pde_type::second_order_parabolic);
        pde.set_initial_condition(data);
    }
    // getters
    DMatrix<double> quadrature_nodes() const { return integrator_.quadrature_nodes(domain_); };
    DMatrix<double> dofs_coordinates() const { return pde.dof_coords(); };
    const SpMatrix<double>& mass() const {
        fdapde_assert(init_ == true);
        return pde.mass();
    }
    const SpMatrix<double>& stiff() const {
        fdapde_assert(init_ == true);
        return pde.stiff();
    }
    const DMatrix<double>& force() const {
        fdapde_assert(init_ == true);
        return pde.force();
    }
    void init() {   // initialize internal pde status
        pde.init();
        init_ = true;
    }
    // destructor
    ~PDE_() = default;
};

}   // namespace r
}   // namespace fdapde

#endif   // __PY_PDE_H__
