// Optima is a C++ library for solving linear and non-linear constrained optimization problems
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

#include "Canonicalizer.hpp"

// C++ includes
#include <cassert>

// Eigen includes
#include <Optima/deps/eigen3/Eigen/Dense>

// Optima includes
#include <Optima/IndexUtils.hpp>
#include <Optima/Utils.hpp>

namespace Optima {

struct Canonicalizer::Impl
{
    /// The full-pivoting LU decomposition of A so that P*A*Q = L*U;
    Eigen::FullPivLU<Matrix> lu;

    /// The matrix `S` in the canonical form `C = [I S]`.
    Matrix S;

    /// The permutation matrix `P`.
    Indices P;

    /// The transpose of the permutation matrix `P`.
    Indices Ptr;

    /// The permutation matrix `Q`.
    Indices Q;

    /// The auxiliary permutation matrix `Q`.
    Indices Qaux;

    /// The inverse permutation of the new ordering of the variables
    Indices inv_ordering;

    /// The canonicalizer matrix `R`.
    Matrix R;

    /// The matrix `M` used in the swap operation.
    Vector M;

    /// The permutation matrix `Kb` used in the weighted update method.
    PermutationMatrix Kb;

    /// The permutation matrix `Kn` used in the weighted update method.
    PermutationMatrix Kn;

    /// The threshold used to compare numbers.
    double threshold;

    /// Compute the canonical matrix of the given matrix.
    auto compute(MatrixConstRef A) -> void
    {
        // The number of rows and columns of A
        const Index m = A.rows();
        const Index n = A.cols();

        // Check if number of columns is greater/equal than number of rows
        assert(n >= m && "Could not canonicalize the given matrix. "
            "The given matrix has more rows than columns.");

        /// Initialize the current ordering of the variables
        inv_ordering = indices(n);

        // Compute the full-pivoting LU of A so that P*A*Q = L*U
        lu.compute(A);

        // Get the rank of matrix A
        const Index r = lu.rank();

        // Get the LU factors of matrix A
        const auto L   = lu.matrixLU().leftCols(m).triangularView<Eigen::UnitLower>();
        const auto Ubb = lu.matrixLU().topLeftCorner(r, r).triangularView<Eigen::Upper>();
        const auto Ubn = lu.matrixLU().topRightCorner(r, n - r);

        // Set the permutation matrices P and Q
        P = lu.permutationP().indices().cast<Index>();
        Q = lu.permutationQ().indices().cast<Index>();

        // Initialize the permutation matrix Q(aux)
        Qaux = Q;

        Ptr.resize(m);
        Ptr(P) = indices(m);

        // Calculate the regularizer matrix R
        R = P.asPermutation();
        R = L.solve(R);
        R.topRows(r) = Ubb.solve(R.topRows(r));

        // Calculate matrix S
        S = Ubn;
        S = Ubb.solve(S);

        // Initialize the permutation matrices Kb and Kn
        Kb.setIdentity(r);
        Kn.setIdentity(n - r);

        // Initialize the threshold value
        threshold = std::abs(lu.maxPivot()) * lu.threshold() * std::max(A.rows(), A.cols());
    }

    /// Rationalize the entries in the canonical form.
    auto rationalize(Index maxdenominator) -> void
    {
        auto rational = [&](double val) -> double
        {
            auto pair = Optima::rationalize(val, maxdenominator);
            return static_cast<double>(std::get<0>(pair))/std::get<1>(pair);
        };

        std::transform(S.data(), S.data() + S.size(), S.data(), rational);
        std::transform(R.data(), R.data() + R.size(), R.data(), rational);
    }

    /// Swap a basic variable by a non-basic variable.
    auto updateWithSwapBasicVariable(Index ib, Index in) -> void
    {
        // Check if ib < rank(A)
        assert(ib < lu.rank() &&
            "Could not swap basic and non-basic variables. "
                "Expecting an index of basic variable below `r`, where `r = rank(A)`.");

        // Check if in < n - rank(A)
        assert(in < lu.cols() - lu.rank() &&
            "Could not swap basic and non-basic variables. "
                "Expecting an index of non-basic variable below `n - r`, where `r = rank(A)`.");

        // Check if S(ib, in) is different than zero
        assert(std::abs(S(ib, in)) > threshold &&
            "Could not swap basic and non-basic variables. "
                "Expecting a non-basic variable with non-zero pivot.");

        // Initialize the matrix M
        M = S.col(in);

        // Auxiliary variables
        const Index m = S.rows();
        const double aux = 1.0/S(ib, in);

        // Update the canonicalizer matrix R (only its `r` upper rows, where `r = rank(A)`)
        R.row(ib) *= aux;
        for(Index i = 0; i < m; ++i)
            if(i != ib) R.row(i) -= S(i, in) * R.row(ib);

        // Update matrix S
        S.row(ib) *= aux;
        for(Index i = 0; i < m; ++i)
            if(i != ib) S.row(i) -= S(i, in) * S.row(ib);
        S.col(in) = -M*aux;
        S(ib, in) = aux;

        // Update the permutation matrix Q
        std::swap(Q[ib], Q[m + in]);
    }

    /// Update the existing canonical form with given priority weights for the columns.
    auto updateWithPriorityWeights(VectorConstRef w) -> void
    {
        // Assert there are as many weights as there are variables
        assert(w.rows() == lu.cols() &&
            "Could not update the canonical form."
                "Mismatch number of variables and given priority weights.");

        // The rank and number of columns of matrix A
        const Index r = lu.rank();
        const Index n = lu.cols();

        // The number of basic and non-basic variables
        const Index nb = r;
        const Index nn = n - r;

        // The upper part of R corresponding to linearly independent rows of A
        auto Rb = R.topRows(nb);

        // The indices of the basic and non-basic variables
        auto ibasic = Q.head(nb);
        auto inonbasic = Q.tail(nn);

        // Find the non-basic variable with maximum proportional weight with respect to a basic variable
        auto find_nonbasic_candidate = [&](Index i, Index& j)
        {
            j = 0; double max = -infinity();
            double tmp = 0.0;
            for(Index k = 0; k < nn; ++k) {
                if(std::abs(S(i, k)) <= threshold) continue;
                tmp = w[inonbasic[k]] * std::abs(S(i, k));
                if(tmp > max) {
                    max = tmp;
                    j = k;
                }
            }
            return max;
        };

        // Check if there are basic variables to be swapped with non-basic variables with higher priority
        if(nn > 0) for(Index i = 0; i < nb; ++i)
        {
            Index j;
            const double wi = w[ibasic[i]];
            const double wj = find_nonbasic_candidate(i, j);
            if(wi < wj)
                updateWithSwapBasicVariable(i, j);
        }

        // Sort the basic variables in descend order of weights
        std::sort(Kb.indices().data(), Kb.indices().data() + nb,
            [&](Index l, Index r) { return w[ibasic[l]] > w[ibasic[r]]; });

        // Sort the non-basic variables in descend order of weights
        std::sort(Kn.indices().data(), Kn.indices().data() + nn,
            [&](Index l, Index r) { return w[inonbasic[l]] > w[inonbasic[r]]; });

        // Rearrange the rows of S based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(S);

        // Rearrange the columns of S based on the new order of non-basic variables
        Kn.applyThisOnTheRight(S);

        // Rearrange the top `nb` rows of R based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(Rb);

        // Rearrange the permutation matrix Q based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(ibasic);

        // Rearrange the permutation matrix Q based on the new order of non-basic variables
        Kn.transpose().applyThisOnTheLeft(inonbasic);
    }
};

Canonicalizer::Canonicalizer()
: pimpl(new Impl())
{}

Canonicalizer::Canonicalizer(MatrixConstRef A)
: pimpl(new Impl())
{
    compute(A);
}

Canonicalizer::Canonicalizer(const Canonicalizer& other)
: pimpl(new Impl(*other.pimpl))
{}

Canonicalizer::~Canonicalizer()
{}

auto Canonicalizer::operator=(Canonicalizer other) -> Canonicalizer&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto Canonicalizer::numVariables() const -> Index
{
    return pimpl->lu.cols();
}

auto Canonicalizer::numEquations() const -> Index
{
    return pimpl->lu.rows();
}

auto Canonicalizer::numBasicVariables() const -> Index
{
    return pimpl->lu.rank();
}

auto Canonicalizer::numNonBasicVariables() const -> Index
{
    return numVariables() - numBasicVariables();
}

auto Canonicalizer::S() const -> MatrixConstRef
{
    return pimpl->S;
}

auto Canonicalizer::R() const -> MatrixConstRef
{
    return pimpl->R;
}

auto Canonicalizer::Q() const -> IndicesConstRef
{
    return pimpl->Q;
}

auto Canonicalizer::C() const -> Matrix
{
    const Index m  = numEquations();
    const Index n  = numVariables();
    const Index nb = numBasicVariables();
    Matrix res = zeros(m, n);
    res.topRows(nb) << identity(nb, nb), S();
    return res;
}

auto Canonicalizer::indicesLinearlyIndependentEquations() const -> IndicesConstRef
{
    return pimpl->Ptr;
}

auto Canonicalizer::indicesBasicVariables() const -> IndicesConstRef
{
    return Q().head(numBasicVariables());
}

auto Canonicalizer::indicesNonBasicVariables() const -> IndicesConstRef
{
    return Q().tail(numNonBasicVariables());
}

auto Canonicalizer::compute(MatrixConstRef A) -> void
{
    pimpl->compute(A);
}

auto Canonicalizer::updateWithSwapBasicVariable(Index ibasic, Index inonbasic) -> void
{
    pimpl->updateWithSwapBasicVariable(ibasic, inonbasic);
}

auto Canonicalizer::updateWithPriorityWeights(VectorConstRef weights) -> void
{
    pimpl->updateWithPriorityWeights(weights);
}

auto Canonicalizer::rationalize(Index maxdenominator) -> void
{
    pimpl->rationalize(maxdenominator);
}

} // namespace Optima
