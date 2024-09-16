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

#ifndef __PY_FUNCTIONAL_BASIS_H__
#define __PY_FUNCTIONAL_BASIS_H__

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>

#include "mesh.h"

#include <Eigen/Dense>
#include <fdaPDE/utils.h>
#include <fdaPDE/finite_elements.h>
#include <fdaPDE/geometry.h>
#include "utils.h"
using fdapde::core::Triangulation;
using fdapde::core::Integrator;
using fdapde::core::LagrangianBasis;
using fdapde::core::pointwise_evaluation;
using fdapde::core::areal_evaluation;
using fdapde::core::eval;
using fdapde::core::ct_binomial_coefficient;
using fdapde::core::FEM;
using fdapde::core::fem_order;
using fdapde::core::combinations;

namespace py = pybind11;

namespace fdapde{
namespace py{
// type-erasure wrapper for a function space object
struct I_FunctionalSpace {
    template <typename T>
    using fn_ptrs = fdapde::mem_fn_ptrs<
      &T::size, &T::operator(), &T::template eval<pointwise_evaluation>, &T::template eval<areal_evaluation>, &T::dofs_coords>;
    // forwardings
    decltype(auto) size() const { return fdapde::invoke<std::size_t, 0>(*this); }
    decltype(auto) operator()(const DVector<double>& c, const DMatrix<double>& locs) const {
      return fdapde::invoke<DVector<double>, 1>(*this, c, locs);
    }
    decltype(auto) eval(int evaluation_type, const DMatrix<double>& locs) const {
        using RetType = std::pair<SpMatrix<double>, DVector<double>>;
        switch (evaluation_type) {   // run-time switch based on evaluation strategy
        case eval::pointwise: // 0
            return fdapde::invoke<RetType, 2>(*this, locs).first;
        case eval::areal: // 1
            return fdapde::invoke<RetType, 3>(*this, locs).first;
        }
        return SpMatrix<double>{};
    }
    decltype(auto) dofs_coords() { return fdapde::invoke<DMatrix<double>, 4>(*this); }
};
using FunctionalSpace = fdapde::erase<fdapde::heap_storage, I_FunctionalSpace>;

// supported basis function types
enum space_type { fem_lagrange = 1 };

template <int M, int N, int R> class FunctionalBasis {
   private:
    using DomainType = Triangulation<M, N>;
    using FunctionalSpaceType = FunctionalSpace;
    using QuadratureRule = Integrator<FEM, DomainType::local_dim, R>; // exact for FEM spaces (TODO: generalize)

    // internal data
    DomainType domain_ {};
    FunctionalSpaceType fun_space_ {}; // wrapped functional space
    QuadratureRule integrator_ {};     // quadrature rule (exact for the provided fem order)
    int space_type_;
    
    //void enumerate_dofs(const DomainType& mesh);
   public:
    
    // constructor
    FunctionalBasis(pybind11::object mesh, int space_type) : space_type_(space_type) {
        // set domain
        using PyDomainType = py::Mesh<DomainType::local_dim, DomainType::embed_dim>;

        //try {
            //mesh.attr("cpp_handler")();
            //pybind11::object meshptr = mesh.attr("cpp_handler")();
            //PyDomainType* ptr = pybind11::cast<PyDomainType*>(meshptr);
            PyDomainType* ptr = get_obj_as<PyDomainType>(mesh, "cpp_handler");
	        domain_ = ptr->domain();
            switch (space_type_)
            {
            case (space_type::fem_lagrange):
                fun_space_ = LagrangianBasis<DomainType, R>(domain_);
                break;
            default:
                break;
            }
        
    }
    std::size_t size() const { return fun_space_.size(); }   // number of basis functions on phyiscal domain
    // given a coefficient vector c \in \mathbb{R}^size_, evaluates the corresponding basis expansion at locs
    DVector<double> operator()(const DVector<double>& c, const DMatrix<double>& locs) const {
        return fun_space_(c, locs);
    }
    // returns the \Psi is the matrix of basis functions evaluations according
    // to the given policy (see the specific policy for details)
    SpMatrix<double> eval(int evaluation_type, const DMatrix<double>& locs) const {
        return fun_space_.eval(evaluation_type, locs);
    }
    // given a coefficient vector c \in \mathbb{R}^size_, evaluates the integral of the corresponding basis expasion
    // over the whole domain
    double integrate(const DVector<double>& c) const {
      return integrator_.integrate(domain_, fun_space_(c, integrator_.quadrature_nodes(domain_)));
    }
    
    // returns the coordinates of the dofs
    DMatrix<double> dofs_coordinates() { return fun_space_.dofs_coords(); }
    
    // destructor
    ~FunctionalBasis() = default;
};

}   // namespace py
}   // namespace fdapde

#endif   // __PY_FUNCTIONAL_BASIS_H__
