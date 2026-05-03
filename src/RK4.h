#pragma once

#include <vector>
#include "SolverODE.h"

class RK4 : public SolverODE
{
public:
    RK4() = default;
    ~RK4() override = default;

    std::vector<MomentPoint> solution(MomentPoint start, double dt) override;
};
