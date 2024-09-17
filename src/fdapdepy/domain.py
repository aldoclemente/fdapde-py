from ._domain import cpp_domain_2d, cpp_domain_3d

def domain(nodes, elements, boundary):
    local_dim = elements.shape[1] - 1
    embed_dim = nodes.shape[1]
    if(local_dim == 2 & embed_dim == 2):
        return domain2d(nodes, elements, boundary)
    elif(local_dim == 3 & embed_dim == 3):
        return domain3d(nodes, elements, boundary)
       
class _domain:
    def __init__(self, nodes, elements, boundary):
        self.__local_dim = elements.shape[1] - 1
        self.__embed_dim = nodes.shape[1]
    
    # child class has to set cpp_handler propperty .
    def nodes(self):
        return self.cpp_handler.nodes()

    def elements(self):
        return self.cpp_handler.elements()
    
    def boundary(self):
        return self.cpp_handler.boundary()
    
    def neighbors(self):
        return self.cpp_handler.neighbors()
    
    @property
    def local_dim(self):
        return self.__local_dim

    @property
    def embed_dim(self):
        return self.__embed_dim

class domain2d(_domain):
    def __init__(self, nodes, elements, boundary):
        super().__init__(nodes, elements, boundary)
        self.__cpp_handler = cpp_domain_2d(nodes, elements, boundary)
    
    def edges(self):
        return self.__cpp_handler.edges()
    
    @property
    def local_dim(self):
        return super().local_dim

    @property
    def embed_dim(self):
        return super().embed_dim
    
    @property
    def cpp_handler(self):
        return self.__cpp_handler
    
class domain3d(_domain):
    def __init__(self, nodes, elements, boundary):
        super().__init__(nodes, elements, boundary)
        self.__cpp_handler = cpp_domain_3d(nodes, elements, boundary)
    
    def faces(self):
        return self.__cpp_handler.faces()
    
    @property
    def local_dim(self):
        return super().local_dim

    @property
    def embed_dim(self):
        return super().embed_dim
    
    @property
    def cpp_handler(self):
        return self.__cpp_handler

    

