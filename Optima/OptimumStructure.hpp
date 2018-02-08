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
#include <functional>

// Optima includes
#include <Optima/Index.hpp>
#include <Optima/Matrix.hpp>

namespace Optima {

/// The requirements in the evaluation of the objective function.
class ObjectiveRequirement
{
public:
    /// The boolean flag that indicates the need for the objective value.
    bool val = true;

    /// The boolean flag that indicates the need for the objective gradient.
    bool grad = true;

    /// The boolean flag that indicates the need for the objective Hessian.
    bool hessian = true;
};

/// The evaluated state of an objective function.
class ObjectiveState
{
public:
    /// The evaluated value of the objective function.
    double val;

    /// The evaluated gradient of the objective function.
    VectorXd grad;

    /// The evaluated Hessian of the objective function.
    MatrixXd hessian;

    /// The requirements in the evaluation of the objective function.
    ObjectiveRequirement requires;

    /// The boolean flag that indicates if the objective function evaluation failed.
    bool failed = false;
};

/// The functional signature of an objective function.
/// @param x The values of the variables \eq{x}.
/// @param f The evaluated state of the objective function.
using ObjectiveFunction = std::function<void(VectorXdConstRef, ObjectiveState&)>;

// todo Implement this as a class using the pimpl idiom
// OptimumStructure structure;
// structure.setNumVariables(10);
// structure.setNumEqualityConstraints(5);
// structure.setNumInequalityConstraints(3);
// structure.setConstraintMatrix(A);
// structure.setObjective(obj);

/// The structure of an optimization problem that changes with less frequency.
class OptimumStructure
{
public:
    /// Construct an OptimumStructure instance.
    /// @param f The objective function \eq{f} in the optimization problem.
    /// @param n The number of variables in \eq{x} in the optimization problem.
    /// @param m The number of linear equality constraints in the optimization problem.
    OptimumStructure(ObjectiveFunction f, Index n, Index m);

    /// Construct an OptimumStructure instance.
    /// @param f The objective function \eq{f} in the optimization problem.
    /// @param A The linear equality constraint matrix \eq{A} in the optimization problem.
    OptimumStructure(ObjectiveFunction f, MatrixXdConstRef A);

    /// Set the coefficient matrix \eq{A} of the linear equality constraints.
    /// This method does not allow changing the dimensions of the equality constraint matrix \eq{A}.
    auto setEqualityConstraintMatrix(MatrixXdConstRef A) -> void;

    /// Set the indices of the variables in \eq{x} with lower bounds.
    auto setVariablesWithLowerBounds(VectorXiConstRef indices) -> void;

    /// Set all variables in \eq{x} with lower bounds.
    auto allVariablesHaveLowerBounds() -> void;

    /// Set the indices of the variables in \eq{x} with upper bounds.
    auto setVariablesWithUpperBounds(VectorXiConstRef indices) -> void;

    /// Set all variables in \eq{x} with upper bounds.
    auto allVariablesHaveUpperBounds() -> void;

    /// Set the indices of the variables in \eq{x} with fixed values.
    auto setVariablesWithFixedValues(VectorXiConstRef indices) -> void;

    /// Return the number of variables.
    auto numVariables() const -> Index { return m_n; }

    /// Return the number of linear equality constraints.
    auto numEqualityConstraints() const -> Index { return m_A.rows(); }

    /// Return the indices of the variables with lower bounds.
    auto variablesWithLowerBounds() const -> VectorXiConstRef { return m_lowerpartition.tail(m_nlower); }

    /// Return the indices of the variables with upper bounds.
    auto variablesWithUpperBounds() const -> VectorXiConstRef { return m_upperpartition.tail(m_nupper); }

    /// Return the indices of the variables with fixed values.
    auto variablesWithFixedValues() const -> VectorXiConstRef { return m_fixedpartition.tail(m_nfixed); }

    /// Return the indices of the variables without lower bounds.
    auto variablesWithoutLowerBounds() const -> VectorXiConstRef { return m_lowerpartition.head(m_n - m_nlower); }

    /// Return the indices of the variables without upper bounds.
    auto variablesWithoutUpperBounds() const -> VectorXiConstRef { return m_upperpartition.head(m_n - m_nupper); }

    /// Return the indices of the variables without fixed values.
    auto variablesWithoutFixedValues() const -> VectorXiConstRef { return m_fixedpartition.head(m_n - m_nfixed); }

    /// Return the indices of the variables partitioned in [without, with] lower bounds.
    auto orderingLowerBounds() const -> VectorXiConstRef { return m_lowerpartition; }

    /// Return the indices of the variables partitioned in [without, with] upper bounds.
    auto orderingUpperBounds() const -> VectorXiConstRef { return m_upperpartition; }

    /// Return the indices of the variables partitioned in [without, with] fixed values.
    auto orderingFixedValues() const -> VectorXiConstRef { return m_fixedpartition; }

    /// Return the objective function.
    auto objectiveFunction() const -> const ObjectiveFunction& { return m_objective; }

    /// Return the coefficient matrix \eq{A} of the linear equality constraints.
    auto equalityConstraintMatrix() const -> MatrixXdConstRef { return m_A; }

    /// Evaluate the objective function.
    /// @param x The values of the variables \eq{x}.
    /// @param f The evaluated state of the objective function.
    auto objective(VectorXdConstRef x, ObjectiveState& f) const -> void { m_objective(x, f); }

    /// Return the coefficient matrix \eq{A} of the linear equality constraints.
    /// This is an alias to method @ref equalityConstraintMatrix.
    auto A() const -> MatrixXdConstRef { return m_A; }

private:
    /// The objective function in the optimization problem.
    ObjectiveFunction m_objective;

    /// The number of variables in the optimization problem.
    Index m_n;

    /// The number of linear equality constraints in the optimization problem.
    Index m_m;

    /// The coefficient matrix of the linear equality constraint \eq{Ax = a}.
    MatrixXd m_A;

    /// The number of variables with lower bounds.
    Index m_nlower;

    /// The number of variables with upper bounds.
    Index m_nupper;

    /// The number of variables with fixed values.
    Index m_nfixed;

    /// The indices of the variables partitioned in [with, without] lower bounds.
    VectorXi m_lowerpartition;

    /// The indices of the variables partitioned in [with, without] upper bounds.
    VectorXi m_upperpartition;

    /// The indices of the variables partitioned in [with, without] fixed values.
    VectorXi m_fixedpartition;
};

} // namespace Optima