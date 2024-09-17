from .functional_basis import functional_basis
import numpy as np

class function:
    def __init__(self, functional_basis):
        self.__functional_basis = functional_basis
        self.__coefficients = np.zeros((self.__functional_basis.size(),1))

    def eval(self, locs):
        return self.__functional_basis.eval(locs) * self.coefficients

    def integrate(self):
        return self.__functional_basis.integrate(self.coefficients)

    @property
    def coefficients(self):
        return self.__coefficients

    @coefficients.setter
    def coefficients(self,coeffs):
        if coeffs.shape[0] != self.functional_basis.size():
            print("wrong input size.")
            return 
        self.__coefficients = coeffs
    
    @property
    def functional_basis(self):
        return self.__functional_basis     