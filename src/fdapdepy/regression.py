import numpy as np
from ._regression import *
from .pde import *
from .utils import parse_formula

def match_arg(x, lst):
    return [el for el in lst if x in el]

optimizer_ = ("grid", "newton", "gd", "bfgs")
edf_evaluation_ = ("stochastic", "exact")

def gcv(smooth_params = None, optimizer = "grid",  
        edf_evaluation = "stochastic", seed = None, n_mc_samples = 100, max_iter = 20,
                step = 1e-2, tolerance = 1e-4):
    out = {}
    if(optimizer is None):
        out ={"calibration": "off", "lambda": smooth_params if smooth_params is not None else np.array(1.0).reshape(1,1)}
    else:
        out = {"calibration": "gcv", "lambda": smooth_params, 
            "edf": edf_evaluation, "n_mc_samples": n_mc_samples,
            "seed": -1 if seed is None else seed,
            "optimizer": optimizer,
            "max_iter" : max_iter,
            "tolerance": tolerance,
            "step": step}
    return out

def model_parse_arguments(model, **kwargs):
    
    if(model == "srpde"):
        return {}
    elif (model=="gsrpde"):
        out = {}
        out["fpirls_tolerance"] = 1e-4
        out["fpirls_max_iter"] = 200
        if ("fpirls_params" in  locals()["kwargs"].keys()):
            out["fpirls_tolerance"] = locals()["kwargs"]["fpirls_params"]["fpirls_tolerance"] if "fpirls_tolerance" in locals()["kwargs"]["fpirls_params"].keys() else 1e-4
            out["fpirls_max_iter"] = locals()["kwargs"]["fpirls_params"]["fpirls_max_iter"] if "fpirls_max_iter" in locals()["kwargs"]["fpirls_params"].keys() else 200
        return out
    
distributions = {"poisson": 0, "bernulli": 1, "exponential": 2, "gamma": 3}

class  srpde:
    def __init__(self, formula=None, data=None, y=None, penalty=None,
                 family = "gaussian"):                          # TO DO check statsmodels (patsy formulas,  multipledispatch)
        
        if (family == "gaussian"):
            self.__model = "srpde"
        elif(family in distributions.keys()):
            self.__model = "gsrpde"
        else:
            print("Wrong 'family argument'")
            return

        self.__observations = None
        self.__covariates = None
        self.__locations = None
        sampling = {}
        # y,data
        if( y is not None):
            self.__observations = y
            self.__covariates = data
            sampling = 0
        # formula, data (str, pandas.Dataframe)
        elif(formula is not None):
            parsed = parse_formula(formula, data)
            self.__observations = data[parsed["observations"]]
            self.__covariates = None if len(parsed["covariates"])==0 else data[parsed["covariates"]]
            self.__locations = None

            if (len(parsed["locations"]) == 0 or len(parsed["locations"]) == 1): 
                sampling = 0 
            elif(len(parsed["locations"]) == 2 and all(elem in data.columns for elem in parsed["locations"])):
                sampling = 1
                self.__locations = data[parsed["locations"]]
        else:
            print("Wrong input arguments")
            return

        # if ( ! parsed["covariates"] in globals().keys()) ...
          
        self.__penalty = penalty # pde class

        # sampling type  = 0 -> pointwise (at mesh nodes)
        if ( self.model == "srpde"):
                self.__cpp_handler = cpp_srpde(penalty, sampling)
        else: 
                self.__cpp_handler = cpp_gsrpde(penalty, sampling, distributions[family])
        
        self.cpp_handler.set_observations(self.observations)
        
        if(self.covariates is not None):
            self.cpp_handler.set_covariates(self.covariates)

        if(self.locations is not None):
            self.cpp_handler.set_spatial_locations(self.locations)

    def set_lambda(self, value):
        self.cpp_handler.set_lambda(value)
    
    def fit(self, **kwargs):
        fit_data = {}
        
        kargs = locals()["kwargs"].keys()
        if ("calibration" in kargs):
            if isinstance(locals()["kwargs"]["calibration"], float):
                fit_data["calibration"] = "off"
                fit_data["lambda"] = np.array( locals()["kwargs"]["calibration"] ).reshape(1,1)
            elif(isinstance(locals()["kwargs"]["calibration"], dict)):
                fit_data = locals()["kwargs"]["calibration"]
            else:
                return
        else:
            fit_data["calibration"] = "off"
            fit_data["lambda"] = np.array(1.0).reshape(1,1)
        
        fpirls_params = model_parse_arguments(self.model, **kwargs)
        if(self.__model == "gsrpde"):
            self.cpp_handler.set_fpirls_tolerance(fpirls_params["fpirls_tolerance"])
            self.cpp_handler.set_fpirls_max_iter(fpirls_params["fpirls_max_iter"])

        self.cpp_handler.fit(fit_data)

    def fitted(self):
        return self.cpp_handler.fitted()

    def f(self):
        return self.cpp_handler.f()    
    
    def gcvs(self):
        return self.cpp_handler.gcvs()
    
    def edfs(self):
        return self.cpp_handler.edfs()
    
    def optimal_lambda(self):
        return self.cpp_handler.optimum()

    @property 
    def observations(self):
        return self.__observations
    
    @property 
    def covariates(self):
        return self.__covariates

    @property
    def locations(self):
        return self.__locations
    
    @property
    def model(self):
        return self.__model

    @property 
    def cpp_handler(self):
        return self.__cpp_handler
    