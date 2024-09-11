#ifndef __PY_FUNCTIONAL_BASIS_H__
#define __PY_FUNCTIONAL_BASIS_H__

#include <pybind11/pybind11.h>
#include "mesh.h"

#include <Eigen/Dense>
#include <fdaPDE/utils.h>
#include <fdaPDE/finite_elements.h>
#include <fdaPDE/geometry.h>
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
enum space_type { fem_lagrange };

template <int M, int N, int R> class py_FunctionalBasis {
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
    py_FunctionalBasis(py::object mesh, int space_type) : space_type_(space_type) {
        // set domain
        
        //try {
            mesh.attr("cpp_handler")();
            py::object meshptr = mesh.attr("cpp_handler")();
            py_Mesh<M, N>* ptr = py::cast<py_Mesh<M, N>*>(meshptr);
	        domain_ = ptr->domain();
        //} catch (py::error_already_set &e) {
        //    py::print(domain_.nodes());
        //    py::print("Python exception caught:", e.what());
        //}

            fun_space_ = LagrangianBasis<DomainType, R>(domain_);
        
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
    ~py_FunctionalBasis() = default;
};

#endif   // __PY_FUNCTIONAL_BASIS_H__
