#include "cubic_spline.h"
#include "thomas.h"
#include <stdexcept>

void CubicSpline::build(const std::vector<double>& xs, const std::vector<double>& ys)
{
    int n = xs.size();

    if (n < 2)
        throw std::invalid_argument("CubicSpline: need at least 2 points");
    if (xs.size() != ys.size())
        throw std::invalid_argument("CubicSpline: xs and ys must have the same size");

    x = xs;
    y = ys;
    M.assign(n, 0.0);  // second derivatives, boundary ones stay 0 for natural spline

    // only 2 points means its just a straight line, nothing to solve
    if (n == 2)
        return;

    // h[i] is the gap between consecutive knots
    std::vector<double> h(n - 1);
    for (int i = 0; i < n - 1; ++i)
    {
        h[i] = x[i+1] - x[i];
        if (h[i] <= 0.0)
            throw std::invalid_argument("CubicSpline: xs must be strictly increasing");
    }

    // one equation per interior knot to solve for the second derivatives
    int sz = n - 2;

    std::vector<double> lower(sz, 0.0);
    std::vector<double> diag(sz,  0.0);
    std::vector<double> upper(sz, 0.0);
    std::vector<double> rhs(sz,   0.0);

    for (int i = 0; i < sz; ++i)
    {
        int k = i + 1;

        lower[i] = h[k-1];
        diag[i]  = 2.0 * (h[k-1] + h[k]);
        upper[i] = h[k];

        rhs[i] = 6.0 * (
            (y[k+1] - y[k])   / h[k] -
            (y[k]   - y[k-1]) / h[k-1]
        );
    }

    std::vector<double> interior_M = thomas(lower, diag, upper, rhs);

    for (int i = 0; i < sz; ++i)
        M[i+1] = interior_M[i];
}


double CubicSpline::eval(double t) const
{
    int n = x.size();

    if (n < 2)
        throw std::runtime_error("CubicSpline: spline not built yet");
    if (t < x[0] || t > x[n-1])
        throw std::runtime_error("CubicSpline: t is outside the interpolation range");

    // find the interval that contains t
    int i = 0;
    while (i < n - 2 && t >= x[i+1])
        i++;

    double hi = x[i+1] - x[i];

    // a and b are local coordinates inside the interval, they always add up to 1
    double a = (x[i+1] - t) / hi;
    double b = (t - x[i])   / hi;

    double term1 = a * y[i];
    double term2 = b * y[i+1];
    double term3 = (a*a*a - a) * M[i]   * (hi*hi) / 6.0;
    double term4 = (b*b*b - b) * M[i+1] * (hi*hi) / 6.0;

    return term1 + term2 + term3 + term4;
}
