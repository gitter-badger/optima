// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2017 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <../PyOptima/PyOptima/Optimization/PyOptimumMethod.hpp"

#include <boost/python.hpp>
namespace py = boost::python;

// Optima includes
#include <Optima/Core/OptimumMethod.hpp>

namespace Optima {

auto export_OptimumMethod() -> void
{
    py::enum_<OptimumMethod>("OptimumMethod")
        .value("IpAction", OptimumMethod::IpAction)
        .value("IpNewton", OptimumMethod::IpNewton)
        .value("IpOpt", OptimumMethod::IpOpt)
        .value("Karpov", OptimumMethod::Karpov)
        .value("Simplex", OptimumMethod::Simplex)
        ;
}

} // namespace Optima
