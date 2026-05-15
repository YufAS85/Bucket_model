#pragma once

#include "ode.h"

class Adams4 : public ODE
{
public:
    std::string getName() const override;
    std::vector<Point> solution(Point position, float dt) override;

private:
    std::vector<Point> getStartPoints(Point start, float dt);
};
