from .domain import *
from ._functional_basis import *

class functional_basis:
    def __init__(self, domain, order):
        self.__order = order
        if order == 1:
            self.__domain = domain # controllare se viene passato per riferimento (si spera)
            self.__cpp_handler = cpp_lagrange_basis_2d_fe1(domain, 1)
        elif order == 2:
            self.__cpp_handler = cpp_lagrange_basis_2d_fe2(domain, 1)

    def size(self):
        return self.cpp_handler.size()

    def eval(self, locs):
        return self.cpp_handler.eval(0, locs)

    def dofs_coordinates(self):
        return self.cpp_handler.dofs_coordinates()
    
    @property
    def order(self):
        return self.__order

    @property
    def domain(self):
        return self.__domain
    
    @property
    def cpp_handler(self):
        return self.__cpp_handler

