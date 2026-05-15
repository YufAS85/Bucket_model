#include "analytical_solution.h"

#include <cmath>
#include <fstream>

double AnalyticalSolution::y(double t, double y0)
{
    double sqrt_y0 = std::sqrt(y0);
    double result = sqrt_y0 - t / 2.0;
    return result > 0 ? result * result : 0.0;
}


double AnalyticalSolution::emptyingTime(double y0)
{
    return 2.0 * std::sqrt(y0);
}


void AnalyticalSolution::saveToFile(const std::string& filename, double y0, double t_max, int points)
{
    std::ofstream file(filename);
    file << "# t y" << std::endl;

    for (int i = 0; i <= points; i++)
    {
        double t = t_max * i / points;
        file << t << " " << y(t, y0) << std::endl;
    }
}
