#pragma once

#include <string>
#include <vector>

#include "ode.h"

class GnuplotGenerator
{
public:
    explicit GnuplotGenerator(double analytical);

    void addData(double dt, double rk4_err, double adams_err, double bs_err,
                 int rk4_pts, int adams_pts, int bs_pts);

    void saveDataFiles();
    void generateGnuplotScript();
    void saveTrajectories(ODE* solver, Point start, double dt, const std::string& filename);
    void generateTrajectoryScript();

private:
    std::vector<double> dts;
    std::vector<double> rk4_errors;
    std::vector<double> adams_errors;
    std::vector<double> bs_errors;
    std::vector<int> rk4_points;
    std::vector<int> adams_points;
    std::vector<int> bs_points;
    double analytical_time;
};
