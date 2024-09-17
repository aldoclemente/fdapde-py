from .function import *
import numpy as np
import string as str 
import copy
from random import sample

def random_name(len=10):
    name = ''.join(sample(list(str.ascii_letters) + 
                          ["1", "2", "3", "4", "5", "6", "7", "8", "9"], len)) 
    return("<"+name+">")

class differential_op:
    def __init__(self, expr, params, function):
        self.__params = {}
        self.__params[expr] = params
        self.__function = function
    
    def __deepcopy__(self,memo):
        res = differential_op("", 0, self.__function)
        memo[id(self)] = res
        res.params = copy.deepcopy(self.__params, memo)
        return res

    def __add__(self, other):
        if self.function == other.function:
            res = copy.deepcopy(self)
            for key in {**self.__params, **other.__params}.keys(): 
                if key in other.__params.keys():
                    res.params[key] = self.__params[key] + other.__params[key] 
                else:
                    res.params[key] = other.__params[other.__expr]
            return res

    def __neg__(self):
        res = copy.deepcopy(self)
        for key, value in self.__params.items():
            res.params[key] = (-1.) * self.__params[key]
        return res

    def __pos__(self):
        res = copy.deepcopy(self)
        for key, value in self.__params.items():
            res.__params[key] = (1.0) * self.__params[key]
        return res
    
    def __sub__(self, other):
        return self.__add__(other.__neg__())
        

    def __mul__(self, other):
        res = copy.deepcopy(self)
        if isinstance(other, (int, float)):
            for key, value in self.__params.items():
                res.params[key] = other * self.__params[key] 
        return res     
    
    def __rmul__(self, other):
        return self.__mul__(other)

    @property
    def function(self):
        return self.__function
    
    @property
    def params(self):
        return self.__params
    
    @params.setter              # da capire se ogni diff deve avere un overload
    def params(self, value): 
        self.__params = value
 

class laplacian(differential_op):

    def __init__(self, function):
        differential_op.__init__(self, "laplacian", 1, function)
    
    @differential_op.params.setter
    def params(self, value):
        if isinstance(value, (int, float)):
            self.__params = float(value)


#def grad(function):
#    embed_dim = function.functional_basis.domain.nodes().shape[1]
#    local_dim = function.functional_basis.domain.triangles().shape[1]-1  # da cambiare in elemets 
#    return differential_op("diff", np.eye(local_dim, local_dim), function)


