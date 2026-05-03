#pragma once

#include <vector>
#include "SolverODE.h"

class EulerODE : public SolverODE
{
public:
    EulerODE() = default;
    ~EulerODE() override = default;

    std::vector<MomentPoint> solution(MomentPoint start, double dt) override;
};
