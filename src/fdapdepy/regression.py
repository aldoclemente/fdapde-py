import numpy as np
from ._regression import *
from .pde import *

def match_arg(x, lst):
    return [el for el in lst if x in el]

optimizer_ = ("grid", "newton", "gd", "bfgs")
edf_evaluation_ = ("stochastic", "exact")

def gcv(lambda_ = None, optimizer = "grid", # non puoi usare lambda perché è una keyword 
        edf_evaluation = "stochastic", seed = None, n_mc_samples = 100, max_iter = 20,
                step = 1e-2, tolerance = 1e-4):
  
    out = {"calibration": "gcv", "lambda": lambda_, 
            "edf": edf_evaluation, "n_mc_samples": n_mc_samples,
            "seed": -1 if seed == None else seed,
            "optimizer": optimizer,
            "max_iter" : max_iter,
            "tolerance": tolerance,
            "step": step}
    return out

class  srpde:
    def __init__(self, y, data, penalty): # TO DO check statsmodels (formulas...)
        self.__observations = y
        self.__covariates = data
        self.__penalty = penalty # pde class

        # sampling type  = 0 -> pointwise
        self.__cpp_handler = cpp_srpde(penalty, 0)
        self.cpp_handler.set_observations(self.observations)
        
        if(self.covariates != None):
            self.cpp_handler.set_covariates(self.covariates)

    def set_lambda(self, value):
        self.cpp_handler.set_lambda(value)
    
    def fit(self, **kwargs):
        fit_data = {}
        
        kargs = locals()["kwargs"].keys()
        print(kargs)
        if ("calibration" in kargs):
            print("ciao")
            if isinstance(locals()["kwargs"]["calibration"], float):
                fit_data["calibration"] = "off"
                fit_data["lambda"] = locals()["kwargs"]["calibration"]
                print("caso scalare")
            elif(isinstance(locals()["kwargs"]["calibration"], dict)):
                fit_data = locals()["kwargs"]["calibration"]
                print("caso dict")
            else:
                print("invalid type bounded to calibration argument.")
                return
        else:
            fit_data["calibration"] = "off"
            fit_data["lambda"] = 1.0
        
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
    def cpp_handler(self):
        return self.__cpp_handler