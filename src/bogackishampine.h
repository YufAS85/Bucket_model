#pragma once

#include "ode.h"

class BogackiShampine : public ODE
{
public:
    std::string getName() const override;
    std::vector<Point> solution(Point position, float dt) override;
};
