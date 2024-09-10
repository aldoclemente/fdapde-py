#ifndef __PY_MESH2D_H__
#define __PY_MESHD_H__

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>
#include <fdaPDE/utils.h>
#include <fdaPDE/geometry.h>

template<int M, int N> class py_Mesh {

    protected:
        using DomainType = fdapde::core::Triangulation<M, N>;
        DomainType domain_ {};
    public:
        py_Mesh(Eigen::Ref<const DMatrix<double>> nodes, 
                Eigen::Ref<const DMatrix<int>> cells, 
                Eigen::Ref<const DMatrix<int>> boundary) : domain_(nodes, cells, boundary){}
        
        // getters
        const DomainType& domain() const { return domain_; }
        const DMatrix<double>& nodes() const { return domain_.nodes(); }
        const DMatrix<int> cells() const { return domain_.cells(); }

        //  SFINAE
        
        template<bool is_2d=fdapde::core::is_2d<M,N>::value >
        typename std::enable_if<is_2d, const DMatrix<int, Eigen::RowMajor> >::type
        edges() const { return domain_.edges(); }
        
        //  SFINAE
        template<bool is_3d=fdapde::core::is_3d<M,N>::value >
        typename std::enable_if<is_3d, const DMatrix<int, Eigen::RowMajor> >::type
        faces() const { return domain_.faces(); }
 
        const DMatrix<int> neighbors() const { return domain_.neighbors(); }
        const DMatrix<int> boundary() const { return domain_.boundary(); }
        // destructor
        ~py_Mesh() = default;

}; 

#endif   // __PY_MESH_H__