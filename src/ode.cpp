#include "ode.h"

#include <cmath>

double ODE::f(const Point& position) const
{
    if (position.y < 0)
        return 0;
    return -std::sqrt(position.y);
}


double ODE::findZero(const Point& left, const Point& right, double precision) const
{
    double t_mid = (left.t + right.t) / 2.0;
    double y_mid = left.y + (right.y - left.y) * (t_mid - left.t) / (right.t - left.t);

    if (right.t - left.t < precision)
        return t_mid;

    if (y_mid > 0)
    {
        Point mid = {t_mid, y_mid};
        return findZero(mid, right, precision);
    }

    Point mid = {t_mid, y_mid};
    return findZero(left, mid, precision);
}
