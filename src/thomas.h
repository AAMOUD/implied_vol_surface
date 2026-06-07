#ifndef THOMAS_H
#define THOMAS_H

#include <vector>

// O(n) solver for tridiagonal systems of the form:
// lower[i]*x[i-1] + diag[i]*x[i] + upper[i]*x[i+1] = rhs[i]
std::vector<double> thomas(
    const std::vector<double>& lower,
    const std::vector<double>& diag,
    const std::vector<double>& upper,
    const std::vector<double>& rhs
);

#endif
