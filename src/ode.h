#pragma once

#include <string>
#include <vector>

struct Point
{
    double t;
    double y;
};

class ODE
{
public:
    virtual ~ODE() = default;
    virtual std::vector<Point> solution(Point position, float dt) = 0;
    virtual std::string getName() const = 0;

protected:
    double f(const Point& position) const;
    double findZero(const Point& left, const Point& right, double precision = 1e-10) const;
};
