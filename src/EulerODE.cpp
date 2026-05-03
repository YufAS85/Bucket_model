#include "EulerODE.h"

std::vector<MomentPoint> EulerODE::solution(MomentPoint start, double dt)
{
    std::vector<MomentPoint> result = {start};

    while (start.y > 0.0)
    {
        start.y += dt * f(start);
        start.t += dt;
        result.push_back(start);
    }

    return result;
}
