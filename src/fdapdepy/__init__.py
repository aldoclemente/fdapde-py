#from ._domain import cpp_domain_2d, cpp_domain_3d # esponi SOLO i wrapper python :)
from .domain import domain # esponi SOLO i wrapper python :)
from .functional_basis import functional_basis
from .function import function
from .operators import laplacian
from .pde import pde
from ._regression import *

#__all__ = [""] public interface