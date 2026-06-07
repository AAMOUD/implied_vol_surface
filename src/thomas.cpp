#include "thomas.h"
#include <stdexcept>

std::vector<double> thomas(
    const std::vector<double>& lower,
    const std::vector<double>& diag,
    const std::vector<double>& upper,
    const std::vector<double>& rhs)
{
    int n = diag.size();

    if (n == 0)
        throw std::invalid_argument("thomas: empty system");

    std::vector<double> c(n, 0.0);
    std::vector<double> d(n, 0.0);
    std::vector<double> x(n, 0.0);

    // forward sweep: eliminate the lower diagonal
    c[0] = upper[0] / diag[0];
    d[0] = rhs[0]   / diag[0];

    for (int i = 1; i < n; ++i)
    {
        double denom = diag[i] - lower[i] * c[i-1];

        if (denom == 0.0)
            throw std::runtime_error("thomas: zero pivot, system may be singular");

        if (i < n - 1)
            c[i] = upper[i] / denom;
        d[i] = (rhs[i] - lower[i] * d[i-1]) / denom;
    }

    // back substitution
    x[n-1] = d[n-1];

    for (int i = n-2; i >= 0; --i)
        x[i] = d[i] - c[i] * x[i+1];

    return x;
}
