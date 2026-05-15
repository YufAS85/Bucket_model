#include "adams4.h"

#include <vector>

std::string Adams4::getName() const
{
    return "Adams4";
}

std::vector<Point> Adams4::solution(Point position, float dt)
{
    std::vector<Point> Result = {position};
    std::vector<Point> startPoints = getStartPoints(position, dt);

    if (startPoints.size() < 4)
        return startPoints;

    Result = startPoints;

    double f0 = f(Result[0]);
    double f1 = f(Result[1]);
    double f2 = f(Result[2]);
    double f3 = f(Result[3]);

    while (Result.back().y > 0)
    {
        double t_n = Result.back().t;
        double y_n = Result.back().y;
        double t_next = t_n + dt;

        double y_pred = y_n + dt / 24.0 * (55.0 * f3 - 59.0 * f2 + 37.0 * f1 - 9.0 * f0);
        if (y_pred < 0)
            y_pred = 0;

        double f_pred = f({t_next, y_pred});
        double y_corr = y_n + dt / 24.0 * (9.0 * f_pred + 19.0 * f3 - 5.0 * f2 + f1);
        double f_corr = f({t_next, y_corr});
        y_corr = y_n + dt / 24.0 * (9.0 * f_corr + 19.0 * f3 - 5.0 * f2 + f1);

        if (y_corr < 0)
        {
            double exact_t = findZero(Result.back(), {t_next, y_corr});
            Result.push_back({exact_t, 0.0});
            break;
        }

        Result.push_back({t_next, y_corr});

        f0 = f1;
        f1 = f2;
        f2 = f3;
        f3 = f_corr;
    }

    return Result;
}

std::vector<Point> Adams4::getStartPoints(Point start, float dt)
{
    std::vector<Point> points;
    double t = start.t;
    double y = start.y;

    points.push_back({t, y});

    for (int i = 0; i < 3; i++)
    {
        double half_dt = dt / 2.0;

        double k1 = f({t, y});
        double k2 = f({t + half_dt, y + half_dt * k1});
        double k3 = f({t + half_dt, y + half_dt * k2});
        double k4 = f({t + dt, y + dt * k3});

        y = y + dt / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        t += dt;

        if (y < 0)
            y = 0;

        points.push_back({t, y});
    }

    return points;
}
