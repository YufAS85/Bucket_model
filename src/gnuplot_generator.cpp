#include "gnuplot_generator.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

GnuplotGenerator::GnuplotGenerator(double analytical)
    : analytical_time(analytical)
{
}

void GnuplotGenerator::addData(double dt, double rk4_err, double adams_err, double bs_err,
                               int rk4_pts, int adams_pts, int bs_pts)
{
    dts.push_back(dt);
    rk4_errors.push_back(rk4_err);
    adams_errors.push_back(adams_err);
    bs_errors.push_back(bs_err);
    rk4_points.push_back(rk4_pts);
    adams_points.push_back(adams_pts);
    bs_points.push_back(bs_pts);
}

void GnuplotGenerator::saveDataFiles()
{
    std::ofstream rk4_file("rk4_data.txt");
    rk4_file << "# dt error points" << std::endl;
    for (size_t i = 0; i < dts.size(); i++)
        rk4_file << dts[i] << " " << rk4_errors[i] << " " << rk4_points[i] << std::endl;

    std::ofstream adams_file("adams4_data.txt");
    adams_file << "# dt error points" << std::endl;
    for (size_t i = 0; i < dts.size(); i++)
        adams_file << dts[i] << " " << adams_errors[i] << " " << adams_points[i] << std::endl;

    std::ofstream bs_file("bs_data.txt");
    bs_file << "# dt error points" << std::endl;
    for (size_t i = 0; i < dts.size(); i++)
        bs_file << dts[i] << " " << bs_errors[i] << " " << bs_points[i] << std::endl;

    std::ofstream all_file("all_data.txt");
    all_file << "# dt rk4_error adams4_error bs_error" << std::endl;
    for (size_t i = 0; i < dts.size(); i++)
        all_file << dts[i] << " " << rk4_errors[i] << " " << adams_errors[i] << " " << bs_errors[i] << std::endl;

    std::cout << "Data files saved successfully!" << std::endl;
}

void GnuplotGenerator::generateGnuplotScript()
{
    std::ofstream script("plot_accuracy.gnu");

    script << "# Gnuplot script for accuracy analysis\n";
    script << "# Run with: gnuplot -persist plot_accuracy.gnu\n\n";
    script << "set terminal wxt size 1200,800 enhanced\n";
    script << "set title 'Leaky Bucket Problem - Accuracy Analysis'\n";
    script << "set xlabel 'Time step dt'\n";
    script << "set ylabel 'Absolute error'\n";
    script << "set logscale xy\n";
    script << "set grid\n";
    script << "set key top left\n\n";
    script << "plot \\\n";
    script << "     'rk4_data.txt' using 1:2 with linespoints linewidth 2 pointtype 1 title 'RK4', \\\n";
    script << "     'adams4_data.txt' using 1:2 with linespoints linewidth 2 pointtype 2 title 'Adams4', \\\n";
    script << "     'bs_data.txt' using 1:2 with linespoints linewidth 2 pointtype 3 title 'Bogacki-Shampine'\n\n";
    script << "pause -1 'Press Enter to continue...'\n\n";
    script << "set title 'Relative Error (%)'\n";
    script << "set ylabel 'Relative error (%)'\n";
    script << "plot \\\n";
    script << "     'rk4_data.txt' using 1:($2/" << analytical_time << "*100) with linespoints linewidth 2 pointtype 1 title 'RK4', \\\n";
    script << "     'adams4_data.txt' using 1:($2/" << analytical_time << "*100) with linespoints linewidth 2 pointtype 2 title 'Adams4', \\\n";
    script << "     'bs_data.txt' using 1:($2/" << analytical_time << "*100) with linespoints linewidth 2 pointtype 3 title 'Bogacki-Shampine'\n\n";
    script << "pause -1 'Press Enter to continue...'\n\n";
    script << "set title 'Convergence Rate (log-log scale)'\n";
    script << "set ylabel 'Error'\n";
    script << "plot \\\n";
    script << "     'rk4_data.txt' using 1:2 with linespoints linewidth 2 pointtype 1 title 'RK4', \\\n";
    script << "     'adams4_data.txt' using 1:2 with linespoints linewidth 2 pointtype 2 title 'Adams4', \\\n";
    script << "     'bs_data.txt' using 1:2 with linespoints linewidth 2 pointtype 3 title 'Bogacki-Shampine', \\\n";
    script << "     x**4 * 0.01 with lines linewidth 2 dashtype 2 title 'O(h^4) reference', \\\n";
    script << "     x**3 * 0.001 with lines linewidth 2 dashtype 3 title 'O(h^3) reference'\n\n";
    script << "pause -1 'Press Enter to continue...'\n\n";
    script << "set title 'Computational Cost'\n";
    script << "set ylabel 'Number of steps'\n";
    script << "unset logscale y\n";
    script << "plot \\\n";
    script << "     'rk4_data.txt' using 1:3 with linespoints linewidth 2 pointtype 1 title 'RK4 steps', \\\n";
    script << "     'adams4_data.txt' using 1:3 with linespoints linewidth 2 pointtype 2 title 'Adams4 steps', \\\n";
    script << "     'bs_data.txt' using 1:3 with linespoints linewidth 2 pointtype 3 title 'BS steps'\n\n";
    script << "pause -1 'Press Enter to exit...'\n";

    std::cout << "Gnuplot script generated: plot_accuracy.gnu" << std::endl;
}

void GnuplotGenerator::saveTrajectories(ODE* solver, Point start, double dt, const std::string& filename)
{
    auto solution = solver->solution(start, dt);
    std::ofstream file(filename);
    file << "# t y" << std::endl;
    for (const auto& p : solution)
        file << p.t << " " << p.y << std::endl;
}

void GnuplotGenerator::generateTrajectoryScript()
{
    std::ofstream script("plot_trajectories.gnu");

    script << "# Gnuplot script for trajectory comparison\n";
    script << "set terminal wxt size 1000,600 enhanced\n";
    script << "set title 'Numerical vs Analytical Solution (dt=0.05)'\n";
    script << "set xlabel 'Time t'\n";
    script << "set ylabel 'y(t)'\n";
    script << "set grid\n";
    script << "set key top right\n\n";
    script << "plot \\\n";
    script << "     'analytical.txt' with lines linewidth 3 title 'Analytical solution', \\\n";
    script << "     'rk4_trajectory.txt' with linespoints linewidth 2 pointtype 1 pointsize 0.5 title 'RK4', \\\n";
    script << "     'adams4_trajectory.txt' with linespoints linewidth 2 pointtype 2 pointsize 0.5 title 'Adams4', \\\n";
    script << "     'bs_trajectory.txt' with linespoints linewidth 2 pointtype 3 pointsize 0.5 title 'Bogacki-Shampine'\n\n";
    script << "pause -1 'Press Enter to exit...'\n";

    std::cout << "Trajectory script generated: plot_trajectories.gnu" << std::endl;
}
