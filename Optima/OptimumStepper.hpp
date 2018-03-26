// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2018 Allan Leal
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

#pragma once

// C++ includes
#include <memory>

// Optima includes
#include <Optima/Matrix.hpp>

namespace Optima {

// Forward declarations
class OptimumOptions;
class OptimumParams;
class OptimumState;
class OptimumStructure;
class Result;

/// The class that implements the step calculation.
class OptimumStepper
{
public:
    /// Construct a default OptimumStepper instance.
    OptimumStepper(const OptimumStructure& structure);

    /// Construct a copy of an OptimumStepper instance.
    OptimumStepper(const OptimumStepper& other);

    /// Destroy this OptimumStepper instance.
    virtual ~OptimumStepper();

    /// Assign an OptimumStepper instance to this.
    auto operator=(OptimumStepper other) -> OptimumStepper&;

    /// Set the options for the step calculation.
    auto setOptions(const OptimumOptions& options) -> void;

    /// Decompose the interior-point saddle point matrix used to compute the step vectors.
    auto decompose(const OptimumParams& params, const OptimumState& state) -> Result;

    /// Solve the interior-point saddle point matrix used to compute the step vectors.
    /// @warning Before calling method OptimumStepper::solve, one has first to call
    ///          method OptimumStepper::decompose.
    auto solve(const OptimumParams& params, const OptimumState& state) -> Result;

    /// Return the step vector.
    auto step() const -> VectorConstRef;

    /// Return the step vector for the *x* variables.
    auto dx() const -> VectorConstRef;

    /// Return the step vector for the *y* variables.
    auto dy() const -> VectorConstRef;

    /// Return the step vector for the *z* variables.
    auto dz() const -> VectorConstRef;

    /// Return the step vector for the *w* variables.
    auto dw() const -> VectorConstRef;

    /// Return the residual of all conditions for the optimum solution.
    auto residual() const -> VectorConstRef;

    /// Return the residual of the optimality conditions.
    auto residualOptimality() const -> VectorConstRef;

    /// Return the residual of the feasibility conditions.
    auto residualFeasibility() const -> VectorConstRef;

    /// Return the residual of the complementarity conditions for the lower bounds.
    auto residualComplementarityLowerBounds() const -> VectorConstRef;

    /// Return the residual of the complementarity conditions for the upper bounds.
    auto residualComplementarityUpperBounds() const -> VectorConstRef;

    /// Return the residual of the complementarity conditions for the inequality constraints.
    auto residualComplementarityInequality() const -> VectorConstRef;

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Optima
