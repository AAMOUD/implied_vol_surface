#ifndef CUBIC_SPLINE_H
#define CUBIC_SPLINE_H

#include <vector>

// natural cubic spline (second derivative is 0 at both ends)
class CubicSpline
{
public:
    void build(const std::vector<double>& xs, const std::vector<double>& ys);
    double eval(double t) const;

private:
    std::vector<double> x;   // knot positions
    std::vector<double> y;   // knot values
    std::vector<double> M;   // second derivatives at each knot
};

#endif
