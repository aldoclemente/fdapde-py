from .function import function
from .functional_basis import functional_basis
from .operators import *
from .domain import domain2d, domain3d
from ._pde import *

pde_type = {"laplacian": 1, "elliptic": 2, "parabolic": 3}

def classify_pde(differential_op):
    if  "dt" in differential_op.params.keys():
        return pde_type["parabolic"]
    elif len(set(differential_op.params.keys()) & set(("laplacian", ""))) == 1: # non può esserci un unico elemento...
        return pde_type["laplacian"]
    elif len(set(differential_op.params.keys()) & set(("diff", "adv", "reac"))) >= 1:
        return pde_type["elliptic"]
    raise Exception("Unrecognized differential operator.")

def pde_params(differential_op):
    type = classify_pde(differential_op)
    pde_params = {}

    if type == 1:
        pde_params["diff"] = differential_op.params["laplacian"]
    
    # tempo 

    return pde_params

class pde:
    def __init__(self, operator, force=None):
        self.__operator = operator
        self.__force = force
        domain = self.__operator.function.functional_basis.domain
        #local_dim = domain.local_dim
        #embed_dim = domain.embed_dim
        order = self.__operator.function.functional_basis.order
        type = classify_pde(operator)
        params = pde_params(operator)

        if (isinstance(domain, domain2d) & order == 1):
            self.__cpp_handler = cpp_pde_2_2_1(domain, type, params)
        elif (isinstance(domain, domain2d) & order == 2):
            self.__cpp_handler = cpp_pde_2_2_2(domain, type, params)

        quad_nodes = self.cpp_handler.quadrature_nodes()

        if(force==None):
            self.cpp_handler.set_forcing(np.zeros((quad_nodes.shape[0],1)))
        elif(callable(force)):
            self.cpp_handler.set_forcing(force(quad_nodes))

        self.cpp_handler.init()

    def quadrature_nodes(self):
        return self.cpp_handler.quadrature_nodes()

    def dofs_coordinates(self):
        return self.cpp_handler.dofs_coordinates()

    def stiff(self):
        return self.cpp_handler.stiff()
    
    def mass(self):
        return self.cpp_handler.mass()
    
    def force(self):
        return self.cpp_handler.force()
    
    def set_dirichlet_bc(self, value):
        self.cpp_handler.set_dirichlet_bc(value)
    
    @property
    def operator(self):
        return self.__operator
    
    @property 
    def cpp_handler(self):
        return self.__cpp_handler



