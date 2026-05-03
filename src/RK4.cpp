#include "RK4.h"

std::vector<MomentPoint> RK4::solution(MomentPoint start, double dt)
{
    std::vector<MomentPoint> result = {start};

    while (start.y > 0.0)
    {
        double t = start.t;
        double y = start.y;
        double half_dt = dt * 0.5;

        double k1 = f({t, y});
        double k2 = f({t + half_dt, y + half_dt * k1});
        double k3 = f({t + half_dt, y + half_dt * k2});
        double k4 = f({t + dt, y + dt * k3});

        start.y = y + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        start.t = t + dt;
        result.push_back(start);
    }

    return result;
}
