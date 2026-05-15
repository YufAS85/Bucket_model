#pragma once

#include <string>

class AnalyticalSolution
{
public:
    static double y(double t, double y0);
    static double emptyingTime(double y0);
    static void saveToFile(const std::string& filename, double y0, double t_max, int points);
};
