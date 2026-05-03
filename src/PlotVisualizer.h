#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "MomentPoint.h"

class PlotVisualizerSFML3
{
public:
    PlotVisualizerSFML3(int width = 800, int height = 600, const std::string& title = "ODE Solver");

    void initialize(const std::vector<MomentPoint>& eulerPoints,
                    const std::vector<MomentPoint>& rk4Points);
    std::optional<sf::Event> pollEvent();
    bool isOpen() const;
    void close();
    void handleEvent(const sf::Event& event);
    void render(const std::vector<MomentPoint>& eulerPoints,
                const std::vector<MomentPoint>& rk4Points,
                size_t step);

private:
    void loadFont();
    sf::Vector2f toScreen(double x, double y,
                          double x_min, double x_max,
                          double y_min, double y_max,
                          int width, int height);
    void computeView(double& left, double& right, double& bottom, double& top) const;
    double computeTickStep(double range, int maxTicks) const;
    std::string formatLabel(double value, double step) const;

    void drawGrid(double x_min, double x_max, double y_min, double y_max);
    void drawAxes(double x_min, double x_max, double y_min, double y_max);
    void drawGraph(const std::vector<MomentPoint>& points,
                   const sf::Color& color,
                   double x_min, double x_max, double y_min, double y_max,
                   size_t step);
    void drawPoints(const std::vector<MomentPoint>& points,
                    const sf::Color& color,
                    double x_min, double x_max, double y_min, double y_max,
                    size_t step);
    void drawLegend();
    void drawInfo(const std::vector<MomentPoint>& eulerPoints,
                  const std::vector<MomentPoint>& rk4Points,
                  size_t step);

private:
    sf::RenderWindow window;
    sf::Font font;
    double x_center = 0.0;
    double y_center = 0.0;
    double x_half_range = 1.0;
    double y_half_range = 1.0;
    double zoom = 1.0;
    bool dragging = false;
    sf::Vector2i lastMousePos{0, 0};
    bool initialized = false;
};
