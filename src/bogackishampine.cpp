#include "bogackishampine.h"

#include <algorithm>
#include <cmath>
#include <vector>

std::string BogackiShampine::getName() const
{
    return "Bogacki-Shampine";
}

std::vector<Point> BogackiShampine::solution(Point position, float dt)
{
    std::vector<Point> Result = {position};
    double t = position.t;
    double y = position.y;
    double h = dt;
    double tolerance = 1e-6;

    while (y > 0 && t < 2 * std::sqrt(position.y) + 1)
    {
        double k1 = f({t, y});
        double k2 = f({t + h / 2.0, y + h / 2.0 * k1});
        double k3 = f({t + 3.0 * h / 4.0, y + 3.0 * h / 4.0 * k2});

        double y3 = y + h * (2.0 / 9.0 * k1 + 1.0 / 3.0 * k2 + 4.0 / 9.0 * k3);
        double k4 = f({t + h, y3});
        double y2 = y + h * (7.0 / 24.0 * k1 + 1.0 / 4.0 * k2 + 1.0 / 3.0 * k3 + 1.0 / 8.0 * k4);

        double error = std::abs(y3 - y2);

        if (error > tolerance && h > 1e-8)
        {
            h *= 0.9 * std::pow(tolerance / error, 1.0 / 3.0);
            continue;
        }

        double t_next = t + h;

        if (y3 < 0)
        {
            double exact_t = findZero(Result.back(), {t_next, y3});
            Result.push_back({exact_t, 0.0});
            break;
        }

        t = t_next;
        y = y3;
        Result.push_back({t, y});

        if (error < tolerance / 10.0)
            h *= 1.2;

        h = std::min(h, 1.0);
    }

    return Result;
}
