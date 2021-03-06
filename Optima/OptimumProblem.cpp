//// Optima is a C++ library for solving linear and non-linear constrained optimization problems
////
//// Copyright (C) 2014-2018 Allan Leal
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
//#include "OptimumProblem.hpp"
//
//namespace Optima {
//
//
//OptimumProblem::OptimumProblem(const OptimumStructure& structure)
//: m_n(structure.n), m_m(structure.A.rows()),
//  m_nlower(0), m_nupper(0), m_nfixed(0), m_b(m_m),
//  m_xlower(m_n), m_xupper(m_n), m_xfixed(m_n)
//{
//    m_lowerpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_upperpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_fixedpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_xlower.fill(0.0);
//    m_xupper.fill(0.0);
//    m_xfixed.fill(0.0);
//}
//
//auto OptimumProblem::xlower(double val) -> void
//{
//    m_xlower.fill(val);
//    m_nlower = m_n;
//}
//
//auto OptimumProblem::xlower(VectorConstRef values) -> void
//{
//    m_xlower.head(m_n) = values;
//    m_nlower = m_n;
//}
//
//auto OptimumProblem::xlower(IndicesConstRef indices, VectorConstRef values) -> void
//{
//    m_xlower(indices) = values;
//    m_nlower = indices.size();
//    m_lowerpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_lowerpartition.head(m_nlower).swap(m_lowerpartition(indices));
//}
//
//auto OptimumProblem::xlower() const -> VectorConstRef
//{
//    return m_xlower.head(m_nlower);
//}
//
//auto OptimumProblem::xupper(double val) -> void
//{
//    m_xupper.fill(val);
//    m_nupper = m_n;
//}
//
//auto OptimumProblem::xupper(VectorConstRef values) -> void
//{
//    m_xupper.head(m_n) = values;
//    m_nupper = m_n;
//}
//
//auto OptimumProblem::xupper(IndicesConstRef indices, VectorConstRef values) -> void
//{
//    m_xupper(indices) = values;
//    m_nupper = indices.size();
//    m_upperpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_upperpartition.head(m_nlower).swap(m_upperpartition(indices));
//}
//
//auto OptimumProblem::xupper() const -> VectorConstRef
//{
//    return m_xupper.head(m_nupper);
//}
//
//auto OptimumProblem::xfixed(double val) -> void
//{
//    m_xfixed.fill(val);
//    m_nfixed = m_n;
//}
//
//auto OptimumProblem::xfixed(VectorConstRef values) -> void
//{
//    m_xfixed.head(m_n) = values;
//    m_nfixed = m_n;
//}
//
//auto OptimumProblem::xfixed(IndicesConstRef indices, VectorConstRef values) -> void
//{
//    m_xfixed(indices) = values;
//    m_nfixed = indices.size();
//    m_fixedpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_fixedpartition.head(m_nlower).swap(m_fixedpartition(indices));
//}
//
//auto OptimumProblem::xfixed() const -> VectorConstRef
//{
//    return m_xfixed.head(m_nfixed);
//}
//
//auto OptimumProblem::iwithlower() const -> IndicesConstRef
//{
//    return m_lowerpartition.head(m_nlower);
//}
//
//auto OptimumProblem::variablesWithUpperBounds() const -> IndicesConstRef
//{
//    return m_upperpartition.head(m_nupper);
//}
//
//auto OptimumProblem::variablesWithFixedValues() const -> IndicesConstRef
//{
//    return m_fixedpartition.head(m_nfixed);
//}
//
//auto OptimumProblem::variablesWithoutLowerBounds() const -> IndicesConstRef
//{
//    return m_lowerpartition.tail(m_n - m_nlower);
//}
//
//auto OptimumProblem::variablesWithoutUpperBounds() const -> IndicesConstRef
//{
//    return m_upperpartition.tail(m_n - m_nupper);
//}
//
//auto OptimumProblem::variablesWithoutFixedValues() const -> IndicesConstRef
//{
//    return m_fixedpartition.tail(m_n - m_nfixed);
//}
//
//auto OptimumProblem::lowerpartition() const -> IndicesConstRef
//{
//    return m_lowerpartition;
//}
//
//auto OptimumProblem::upperpartition() const -> IndicesConstRef
//{
//    return m_upperpartition;
//}
//
//auto OptimumProblem::fixedpartition() const -> IndicesConstRef
//{
//    return m_fixedpartition;
//}
//
//} // namespace Optima
