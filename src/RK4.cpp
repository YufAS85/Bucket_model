#include "rk4.h"

#include <vector>

std::string RK4::getName() const
{
    return "RK4";
}

std::vector<Point> RK4::solution(Point position, float dt)
{
    std::vector<Point> Result = {position};

    while (position.y > 0)
    {
        double half_dt = dt / 2.0;
        double t = position.t;
        double y = position.y;

        double k1 = f({t, y});
        double k2 = f({t + half_dt, y + half_dt * k1});
        double k3 = f({t + half_dt, y + half_dt * k2});
        double k4 = f({t + dt, y + dt * k3});

        double next_y = y + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        double next_t = t + dt;

        position.y = next_y;
        position.t = next_t;

        if (position.y < 0)
        {
            double exact_t = findZero(Result.back(), position);
            Result.push_back({exact_t, 0.0});
            break;
        }

        Result.push_back(position);
    }

    return Result;
}
