from ._domain import cpp_domain_2d, cpp_domain_3d

class domain:
    def __init__(self, nodes, elements, boundary):
        self.__cpp_handler = cpp_domain_2d(nodes, elements, boundary)
    
    def nodes(self):
        return self.__cpp_handler.nodes()
    
    def triangles(self):
        return self.__cpp_handler.triangles()
    
    def boundary(self):
        return self.__cpp_handler.boundary()
    
    def edges(self):
        return self.__cpp_handler.edges()
    
    def neighbors(self):
        return self.__cpp_handler.neighbors()
    
    def cpp_handler(self):
        return self.__cpp_handler


    

