#pragma once

#include <vector>
#include "MomentPoint.h"

class SolverODE
{
public:
    virtual ~SolverODE() = default;

    virtual std::vector<MomentPoint> solution(MomentPoint start, double dt) = 0;

protected:
    double f(MomentPoint pos);
};
