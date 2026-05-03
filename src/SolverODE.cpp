#include "SolverODE.h"

#include <cmath>

double SolverODE::f(MomentPoint pos)
{
    if (pos.y <= 0.0)
        return 0.0;
    return -std::sqrt(pos.y);
}
