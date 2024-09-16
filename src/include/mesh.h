// This file is part of fdaPDE, a C++ library for physics-informed
// spatial and functional data analysis.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __PY_MESH_H__
#define __PY_MESH_H__

#include <pybind11/pybind11.h>
#include<pybind11/eigen.h>
#include <fdaPDE/utils.h>
#include <fdaPDE/geometry.h>

namespace fdapde{
namespace py{

template<int M, int N> class Mesh {

    protected:
        using DomainType = fdapde::core::Triangulation<M, N>;
        DomainType domain_ {};
    public:
        Mesh(Eigen::Ref<const DMatrix<double>> nodes, 
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
        ~Mesh() = default;

}; 

}   // namespace py
}   // namespace fdapde

#endif   // __PY_MESH_H__