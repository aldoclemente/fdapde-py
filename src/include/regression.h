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

#ifndef __PY_REGRESSION_H__
#define __PY_REGRESSION_H__

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>

#include <regression.h>
#include <fdaPDE/optimization.h>

#include <variant>

#include "calibration/gcv.h"
#include "calibration/kfold_cv.h"
#include "calibration/rmse.h"
#include "pde.h"
#include "utils.h"

namespace fdapde {
namespace py {

template <typename Objective> core::Optimizer<Objective> parse_optimizer(const pybind11::dict py_input) { //dict
    core::Optimizer<Objective> opt;
    if (py_input["optimizer"].is_none()) { return opt; }   // return empty optimizer
    std::string opt_flag = pybind11::cast<std::string>(py_input["optimizer"]);              // optimizer name

    // newton/quasi-netwon optimization parameters
    int max_iter = pybind11::cast<std::size_t>(py_input["max_iter"]); // int -> std::size_t, compile complains
    double tolerance = pybind11::cast<double>(py_input["tolerance"]);
    double step = pybind11::cast<double>(py_input["step"]);
    // instantiate optimization algorithms
    if (opt_flag == "grid") { opt = core::Grid<fdapde::Dynamic> {}; }
    if (opt_flag == "newton") { opt = core::Newton<fdapde::Dynamic> {max_iter, tolerance, step}; }
    if (opt_flag == "gd") { opt = core::GradientDescent<fdapde::Dynamic> {max_iter, tolerance, step}; }
    if (opt_flag == "bfgs") { opt = core::BFGS<fdapde::Dynamic> {max_iter, tolerance, step}; }

    // handle step adaptivity in optimizers

    return opt;
}

template <typename ModelType> class RegressionModel {
   protected:
    using RegularizationType = typename ModelType::RegularizationType;
    using DataContainer = BlockFrame<double, int>;
    ModelType model_;      // wrapped model
    DataContainer data_;   // data storage (can be removed if we initialize the data stack in init())
    // calibration
    DVector<double> optim_lambda_;   // optimal selected level of smoothing
    calibration::GCV<
      std::conditional_t<std::is_same_v<RegularizationType, models::SpaceOnly>, models::SpaceOnly, models::SpaceTime>>
      gcv_;
    calibration::KCV kcv_;

    // calibrate regression model according to provided input
    void calibrate(const pybind11::dict py_input) {
        // recover calibration type
        std::string calibration = pybind11::cast<std::string>(py_input["calibration"]);
        if (calibration == "off") { optim_lambda_ = pybind11::cast<DVector<double>>(py_input["lambda"]); }
        if (calibration == "gcv") {
            std::string edf = pybind11::cast<std::string>(py_input["edf"]);
            if (edf == "exact") {
                gcv_.set_edf_strategy(models::ExactEDF());
            } else {   // stochastic approximation of edf
                int n_mc_samples = pybind11::cast<int>(py_input["n_mc_samples"]);
                int seed = pybind11::cast<int>(py_input["seed"]);
                gcv_.set_edf_strategy(models::StochasticEDF(n_mc_samples, seed));
            }
            gcv_.set_optimizer(parse_optimizer<models::GCV>(py_input));
            optim_lambda_ = gcv_(pybind11::cast<DMatrix<double>>(py_input["lambda"])).fit(model_);
        }
        if (calibration == "kcv") {
            int n_folds = pybind11::cast<int>(py_input["n_folds"]);
            int seed = py_input["seed"].is_none() ? fdapde::random_seed : pybind11::cast<int>(py_input["seed"]);
            kcv_ = calibration::KCV(n_folds, seed);
            optim_lambda_ = kcv_(pybind11::cast<DMatrix<double>>(py_input["lambda"]), calibration::RMSE()).fit(model_);
        }
        model_.set_lambda(optim_lambda_);
        return;
    }
    // TODO:
    // need to supply the spatial locations and the temporal locations
   public:
    // constructor
    RegressionModel() = default;
    void fit(const pybind11::dict py_input) {
        model_.set_data(data_);
        calibrate(py_input);   // find optimal tuning parameter according to desired strategy
        model_.init();        // initialize model with optimal parameters
        model_.solve();       // final fitting
    }
    // setters
    void set_observations(const DMatrix<double>& y) { data_.template insert<double>(OBSERVATIONS_BLK, y); }
    void set_covariates(const DMatrix<double>& X) { data_.template insert<double>(DESIGN_MATRIX_BLK, X); }
    void set_lambda(const DVector<double>& lambda) { model_.set_lambda(lambda); }
    // getters
    const DVector<double>& f() const { return model_.f(); }
    DMatrix<double> fitted() const { return model_.fitted(); }
    const std::vector<double>& gcvs() const { return gcv_.gcvs(); }
    const std::vector<double>& edfs() const { return gcv_.edfs(); }
    const DVector<double>& optimum() const { return optim_lambda_; }

    // destructor
    ~RegressionModel() = default;
};

struct SRPDE : public RegressionModel<models::SRPDE> {
    using ModelType = models::SRPDE;
    using Base = RegressionModel<ModelType>;
    SRPDE(pybind11::object py_obj, int sampling_type) {
        Base::model_ = ModelType( get_obj_as<py::PDE>(py_obj)->pde, Sampling(sampling_type));
    }
};

template <typename RegularizationType> struct GSRPDE : public RegressionModel<models::GSRPDE<RegularizationType>> {
    using ModelType = models::GSRPDE<RegularizationType>;
    using Base = RegressionModel<ModelType>;
    // space-only, space-time parabolic
    GSRPDE(pybind11::object py_obj, int sampling_type, int distribution)
    requires(models::is_space_only<ModelType>::value || models::is_space_time_parabolic<ModelType>::value) {
        init_(distribution, get_obj_as<py::PDE>(py_obj)->pde, Sampling(sampling_type));
    }

    // space-time separable
    GSRPDE(pybind11::object py_obj1, pybind11::object py_obj2, int sampling_type, int distribution)
    requires(models::is_space_time_separable<ModelType>::value) {
        init_(distribution, get_obj_as<py::PDE>(py_obj1)->pde, get_obj_as<py::PDE>(py_obj2)->pde, Sampling(sampling_type));
    }
    void set_fpirls_tolerance(double tol) { Base::model_.set_fpirls_tolerance(tol); }
    void set_fpirls_max_iter(std::size_t max_iter) { Base::model_.set_fpirls_max_iter(max_iter); }
   private:
    // instantiate model depending on distribution type
    template <typename... Args> void init_(int distribution, Args&&... args) {
        if (distribution == 0) Base::model_ = ModelType(std::forward<Args>(args)..., models::Poisson());
        if (distribution == 1) Base::model_ = ModelType(std::forward<Args>(args)..., models::Bernulli());
        if (distribution == 2) Base::model_ = ModelType(std::forward<Args>(args)..., models::Exponential());
        if (distribution == 3) Base::model_ = ModelType(std::forward<Args>(args)..., models::Gamma());
    }
};

/*
template <typename RegularizationType, typename SolutionPolicy>
struct STRPDE : public RegressionModel<models::STRPDE<RegularizationType, SolutionPolicy>> {
    using ModelType = models::STRPDE<RegularizationType, SolutionPolicy>;
    using Base = RegressionModel<ModelType>;
    // space-time parabolic
    STRPDE(pybind11::object py_obj, int sampling_type, int distribution)
    requires(models::is_space_time_parabolic<ModelType>::value) {
        Base::model_ = ModelType(get_obj_as<py::PDE>(py_obj)->pde, Sampling(sampling_type));
    }
    // space-time separable
    STRPDE(pybind11::object py_obj1, pybind11::object py_obj2, int sampling_type)
    requires(models::is_space_time_separable<ModelType>::value) {
        Base::model_ =
          ModelType(get_obj_as<py::PDE>(py_obj1)->pde, get_obj_as<py::PDE>(py_obj2)->pde, Sampling(sampling_type));
    }
};
*/

}   // namespace py
}   // namespace fdapde

#endif   // __PY_REGRESSION_H__
