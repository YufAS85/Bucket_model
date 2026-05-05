#include <algorithm>
#include <iostream>
#include <limits>
#include <SFML/System.hpp>
#include <SFML/Config.hpp>
#include "BucketVisualizer.h"
#include "EulerODE.h"
#include "PlotVisualizer.h"
#include "RK4.h"

int main()
{
    std::cout << "Enter initial time, initial value, and step size (example: 0 100 1): ";
    double t0 = 0.0;
    double y0 = 100.0;
    double dt = 1.0;

    if (!(std::cin >> t0 >> y0 >> dt) || dt <= 0.0)
    {
        std::cerr << "Invalid input. Using default values: t0=0, y0=100, dt=1." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        t0 = 0.0;
        y0 = 100.0;
        dt = 1.0;
    }

    EulerODE eulerSolver;
    std::vector<MomentPoint> eulerSolution = eulerSolver.solution({t0, y0}, dt);

    std::cout << "\n=== Euler Method Results ===" << std::endl;
    for (const auto& point : eulerSolution)
    {
        std::cout << "t=" << point.t << " y=" << point.y << std::endl;
    }

    RK4 rk4Solver;
    std::vector<MomentPoint> rk4Solution = rk4Solver.solution({t0, y0}, dt);

    std::cout << "\n=== RK4 Method Results ===" << std::endl;
    for (const auto& point : rk4Solution)
    {
        std::cout << "t=" << point.t << " y=" << point.y << std::endl;
    }

    PlotVisualizerSFML3 graphVisualizer(800, 600, "ODE Solver - Numerical Methods Comparison (SFML 3)");
    BucketVisualizerSFML3 bucketVisualizer(420, 520, "Leaky Bucket Model");

    graphVisualizer.initialize(eulerSolution, rk4Solution);
    bucketVisualizer.initialize(eulerSolution, rk4Solution);

    size_t currentStep = 0;
    size_t maxSteps = std::max(eulerSolution.size(), rk4Solution.size());
    const float secondsPerStep = 0.4f;
    float frameTimeAccumulator = 0.0f;
    sf::Clock frameClock;

    while (graphVisualizer.isOpen() || bucketVisualizer.isOpen())
    {
        if (graphVisualizer.isOpen())
        {
#if SFML_VERSION_MAJOR >= 3
            while (auto event = graphVisualizer.pollEvent())
            {
                graphVisualizer.handleEvent(*event);
            }
#else
            sf::Event event;
            while (graphVisualizer.pollEvent(event))
            {
                graphVisualizer.handleEvent(event);
            }
#endif
        }

        if (bucketVisualizer.isOpen())
        {
#if SFML_VERSION_MAJOR >= 3
            while (auto event = bucketVisualizer.pollEvent())
            {
                bucketVisualizer.handleEvent(*event);
            }
#else
            sf::Event event;
            while (bucketVisualizer.pollEvent(event))
            {
                bucketVisualizer.handleEvent(event);
            }
#endif
        }

        float dtFrame = frameClock.restart().asSeconds();
        frameTimeAccumulator += dtFrame;
        while (currentStep + 1 < maxSteps && frameTimeAccumulator >= secondsPerStep)
        {
            frameTimeAccumulator -= secondsPerStep;
            ++currentStep;
        }

        if (!graphVisualizer.isOpen() && bucketVisualizer.isOpen())
            bucketVisualizer.close();
        if (!bucketVisualizer.isOpen() && graphVisualizer.isOpen())
            graphVisualizer.close();

        if (graphVisualizer.isOpen())
            graphVisualizer.render(eulerSolution, rk4Solution, currentStep);
        if (bucketVisualizer.isOpen())
            bucketVisualizer.render(eulerSolution, rk4Solution, currentStep);
    }

    return 0;
}
