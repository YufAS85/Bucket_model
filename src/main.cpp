#include "adams4.h"
#include "analytical_solution.h"
#include "bogackishampine.h"
#include "gnuplot_generator.h"
#include "rk4.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    float init = 15.0f;
    std::cout << std::fixed << std::setprecision(10);
    std::cout << "=== LEAKY BUCKET PROBLEM ===" << std::endl;
    std::cout << "Equation: dy/dt = -sqrt(y)" << std::endl;
    std::cout << "Enter Initial condition: y(0) = ... (default = 15.0): ";

    std::string input;
    std::cin >> input;
    if (!input.empty())
    {
        init = std::stof(input);
    }

    double analytical_time = AnalyticalSolution::emptyingTime(init);
    std::cout << "Analytical emptying time: t = 2√" << init << " = " << analytical_time << std::endl;
    std::cout << std::endl;

    Point start{0.0, init};
    std::vector<double> dts = {1.0, 0.5, 0.2, 0.1, 0.05, 0.02, 0.01, 0.005};

    RK4 rk4;
    Adams4 adams;
    BogackiShampine bs;

    GnuplotGenerator generator(analytical_time);

    std::cout << "Collecting data..." << std::endl;

    for (double dt : dts)
    {
        auto rk4_sol = rk4.solution(start, dt);
        auto adams_sol = adams.solution(start, dt);
        auto bs_sol = bs.solution(start, dt);

        double rk4_error = std::abs(rk4_sol.back().t - analytical_time);
        double adams_error = std::abs(adams_sol.back().t - analytical_time);
        double bs_error = std::abs(bs_sol.back().t - analytical_time);

        generator.addData(dt, rk4_error, adams_error, bs_error,
                          static_cast<int>(rk4_sol.size()),
                          static_cast<int>(adams_sol.size()),
                          static_cast<int>(bs_sol.size()));

        std::cout << "dt = " << dt << " -> RK4 error: " << rk4_error
                  << ", Adams error: " << adams_error
                  << ", BS error: " << bs_error << std::endl;
    }

    generator.saveDataFiles();
    generator.generateGnuplotScript();

    std::cout << "\nSaving trajectories with dt=0.05..." << std::endl;
    generator.saveTrajectories(&rk4, start, 0.05, "rk4_trajectory.txt");
    generator.saveTrajectories(&adams, start, 0.05, "adams4_trajectory.txt");
    generator.saveTrajectories(&bs, start, 0.05, "bs_trajectory.txt");

    double t_max = analytical_time + 0.5;
    AnalyticalSolution::saveToFile("analytical.txt", init, t_max, 1000);
    generator.generateTrajectoryScript();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "To visualize results, run:" << std::endl;
    std::cout << "  gnuplot -persist plot_accuracy.gnu" << std::endl;
    std::cout << "  gnuplot -persist plot_trajectories.gnu" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    return 0;
}
