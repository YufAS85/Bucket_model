#include "PlotVisualizer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

PlotVisualizerSFML3::PlotVisualizerSFML3(int width, int height, const std::string& title)
    : window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title)
{
    loadFont();
    if (!font.hasGlyph('A'))
    {
        std::cerr << "Warning: Could not load font for graph window" << std::endl;
    }
}

void PlotVisualizerSFML3::loadFont()
{
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc"
    };

    for (const auto& path : fontPaths)
    {
        if (font.openFromFile(path))
        {
            std::cout << "Font loaded: " << path << std::endl;
            return;
        }
    }
}

void PlotVisualizerSFML3::initialize(const std::vector<MomentPoint>& eulerPoints,
                                     const std::vector<MomentPoint>& rk4Points)
{
    if (eulerPoints.empty() && rk4Points.empty())
        return;

    double x_min = 0.0;
    double x_max = 0.0;
    double y_min = 0.0;
    double y_max = 0.0;
    bool first = true;

    auto updateBounds = [&](const std::vector<MomentPoint>& points)
    {
        for (const auto& p : points)
        {
            if (first)
            {
                x_min = x_max = p.t;
                y_min = y_max = p.y;
                first = false;
            }
            else
            {
                x_min = std::min(x_min, p.t);
                x_max = std::max(x_max, p.t);
                y_min = std::min(y_min, p.y);
                y_max = std::max(y_max, p.y);
            }
        }
    };

    updateBounds(eulerPoints);
    updateBounds(rk4Points);

    double x_padding = std::max(1.0, (x_max - x_min) * 0.1);
    double y_padding = std::max(1.0, (y_max - y_min) * 0.1);
    x_min -= x_padding;
    x_max += x_padding;
    y_min = std::min(0.0, y_min - y_padding);
    y_max += y_padding;

    if (x_max == x_min)
        x_max = x_min + 1.0;
    if (y_max == y_min)
        y_max = y_min + 1.0;

    x_center = (x_min + x_max) / 2.0;
    y_center = (y_min + y_max) / 2.0;
    x_half_range = (x_max - x_min) / 2.0;
    y_half_range = (y_max - y_min) / 2.0;
    zoom = 1.0;
    dragging = false;
    lastMousePos = {0, 0};
    initialized = true;
}

std::optional<sf::Event> PlotVisualizerSFML3::pollEvent()
{
    return window.pollEvent();
}

bool PlotVisualizerSFML3::isOpen() const
{
    return window.isOpen();
}

void PlotVisualizerSFML3::close()
{
    if (window.isOpen())
        window.close();
}

void PlotVisualizerSFML3::handleEvent(const sf::Event& event)
{
    if (!initialized)
        return;

    if (event.is<sf::Event::Closed>())
    {
        window.close();
    }
    else if (event.is<sf::Event::KeyPressed>())
    {
        if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyEvent->code == sf::Keyboard::Key::Escape)
                window.close();
        }
    }
    else if (event.is<sf::Event::MouseButtonPressed>())
    {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseEvent->button == sf::Mouse::Button::Left)
            {
                dragging = true;
                lastMousePos = sf::Vector2i(mouseEvent->position.x, mouseEvent->position.y);
            }
        }
    }
    else if (event.is<sf::Event::MouseButtonReleased>())
    {
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseEvent->button == sf::Mouse::Button::Left)
                dragging = false;
        }
    }
    else if (event.is<sf::Event::MouseMoved>())
    {
        if (dragging)
        {
            if (const auto* moveEvent = event.getIf<sf::Event::MouseMoved>())
            {
                double view_x_min, view_x_max, view_y_min, view_y_max;
                computeView(view_x_min, view_x_max, view_y_min, view_y_max);

                int dx = moveEvent->position.x - lastMousePos.x;
                int dy = moveEvent->position.y - lastMousePos.y;
                lastMousePos = sf::Vector2i(moveEvent->position.x, moveEvent->position.y);

                double xRange = view_x_max - view_x_min;
                double yRange = view_y_max - view_y_min;
                x_center -= dx * (xRange / static_cast<double>(window.getSize().x));
                y_center += dy * (yRange / static_cast<double>(window.getSize().y));
            }
        }
    }
    else if (event.is<sf::Event::MouseWheelScrolled>())
    {
        if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
        {
            double view_x_min, view_x_max, view_y_min, view_y_max;
            computeView(view_x_min, view_x_max, view_y_min, view_y_max);

            int mx = wheel->position.x;
            int my = wheel->position.y;
            int width = window.getSize().x;
            int height = window.getSize().y;

            double data_x = view_x_min + static_cast<double>(mx) / width * (view_x_max - view_x_min);
            double data_y = view_y_min + (static_cast<double>(height - my) / height) * (view_y_max - view_y_min);

            double newZoom = zoom;
            if (wheel->delta > 0)
                newZoom = std::max(0.2, zoom * 0.9);
            else if (wheel->delta < 0)
                newZoom = std::min(5.0, zoom * 1.1);

            double ratio = newZoom / zoom;
            zoom = newZoom;
            x_center = data_x + (x_center - data_x) * ratio;
            y_center = data_y + (y_center - data_y) * ratio;
        }
    }
}

void PlotVisualizerSFML3::render(const std::vector<MomentPoint>& eulerPoints,
                                 const std::vector<MomentPoint>& rk4Points,
                                 size_t step)
{
    if (!initialized)
        return;

    double view_x_min, view_x_max, view_y_min, view_y_max;
    computeView(view_x_min, view_x_max, view_y_min, view_y_max);

    window.clear(sf::Color::White);
    drawGrid(view_x_min, view_x_max, view_y_min, view_y_max);
    drawAxes(view_x_min, view_x_max, view_y_min, view_y_max);
    drawGraph(eulerPoints, sf::Color::Blue, view_x_min, view_x_max, view_y_min, view_y_max, step);
    drawGraph(rk4Points, sf::Color::Red, view_x_min, view_x_max, view_y_min, view_y_max, step);
    drawPoints(eulerPoints, sf::Color(0, 0, 150), view_x_min, view_x_max, view_y_min, view_y_max, step);
    drawPoints(rk4Points, sf::Color(150, 0, 0), view_x_min, view_x_max, view_y_min, view_y_max, step);
    drawLegend();
    drawInfo(eulerPoints, rk4Points, step);
    window.display();
}

sf::Vector2f PlotVisualizerSFML3::toScreen(double x, double y,
                                           double x_min, double x_max,
                                           double y_min, double y_max,
                                           int width, int height)
{
    float screen_x = static_cast<float>((x - x_min) / (x_max - x_min) * width);
    float screen_y = static_cast<float>(height - (y - y_min) / (y_max - y_min) * height);
    return {screen_x, screen_y};
}

void PlotVisualizerSFML3::computeView(double& left, double& right, double& bottom, double& top) const
{
    double view_half_x = std::max(0.1, x_half_range * zoom);
    double view_half_y = std::max(0.1, y_half_range * zoom);
    left = x_center - view_half_x;
    right = x_center + view_half_x;
    bottom = y_center - view_half_y;
    top = y_center + view_half_y;
    if (bottom > 0.0)
        bottom = std::min(0.0, bottom);
}

double PlotVisualizerSFML3::computeTickStep(double range, int maxTicks) const
{
    if (range <= 0.0)
        return 1.0;

    double rawStep = range / static_cast<double>(maxTicks);
    double exponent = std::floor(std::log10(rawStep));
    double base = std::pow(10.0, exponent);
    double normalized = rawStep / base;

    double niceNorm;
    if (normalized <= 1.0)
        niceNorm = 1.0;
    else if (normalized <= 2.0)
        niceNorm = 2.0;
    else if (normalized <= 5.0)
        niceNorm = 5.0;
    else
        niceNorm = 10.0;

    return niceNorm * base;
}

std::string PlotVisualizerSFML3::formatLabel(double value, double step) const
{
    std::ostringstream oss;
    if (std::abs(value) >= 1000.0 || step >= 1000.0)
    {
        oss << std::scientific << std::setprecision(2) << value;
    }
    else if (step < 1.0)
    {
        oss << std::fixed << std::setprecision(2) << value;
    }
    else if (step < 10.0)
    {
        oss << std::fixed << std::setprecision(1) << value;
    }
    else
    {
        oss << std::fixed << std::setprecision(0) << value;
    }
    return oss.str();
}

void PlotVisualizerSFML3::drawGrid(double x_min, double x_max, double y_min, double y_max)
{
    int width = window.getSize().x;
    int height = window.getSize().y;
    sf::Color gridColor(200, 200, 200);
    double x_step = computeTickStep(x_max - x_min, 10);
    double y_step = computeTickStep(y_max - y_min, 10);
    double x_start = std::ceil(x_min / x_step) * x_step;

    for (double x = x_start; x <= x_max; x += x_step)
    {
        sf::Vector2f screen_pos = toScreen(x, 0, x_min, x_max, y_min, y_max, width, height);
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = {screen_pos.x, 0};
        line[0].color = gridColor;
        line[1].position = {screen_pos.x, static_cast<float>(height)};
        line[1].color = gridColor;
        window.draw(line);

        if (font.hasGlyph('0'))
        {
            sf::Text text(font);
            text.setString(formatLabel(x, x_step));
            text.setCharacterSize(12);
            text.setFillColor(sf::Color::Black);
            text.setPosition({screen_pos.x - 20, static_cast<float>(height) - 20});
            window.draw(text);
        }
    }

    double y_start = std::ceil(y_min / y_step) * y_step;
    for (double y = y_start; y <= y_max; y += y_step)
    {
        sf::Vector2f screen_pos = toScreen(0, y, x_min, x_max, y_min, y_max, width, height);
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = {0, screen_pos.y};
        line[0].color = gridColor;
        line[1].position = {static_cast<float>(width), screen_pos.y};
        line[1].color = gridColor;
        window.draw(line);

        if (font.hasGlyph('0'))
        {
            sf::Text text(font);
            text.setString(formatLabel(y, y_step));
            text.setCharacterSize(12);
            text.setFillColor(sf::Color::Black);
            text.setPosition({5, screen_pos.y - 10});
            window.draw(text);
        }
    }
}

void PlotVisualizerSFML3::drawAxes(double x_min, double x_max, double y_min, double y_max)
{
    int width = window.getSize().x;
    int height = window.getSize().y;
    sf::Color axisColor(0, 0, 0);
    sf::Vector2f x_axis_start = toScreen(x_min, 0, x_min, x_max, y_min, y_max, width, height);
    sf::Vector2f x_axis_end = toScreen(x_max, 0, x_min, x_max, y_min, y_max, width, height);
    sf::VertexArray x_axis(sf::PrimitiveType::Lines, 2);
    x_axis[0].position = x_axis_start;
    x_axis[0].color = axisColor;
    x_axis[1].position = x_axis_end;
    x_axis[1].color = axisColor;
    window.draw(x_axis);
    sf::Vector2f y_axis_start = toScreen(0, y_min, x_min, x_max, y_min, y_max, width, height);
    sf::Vector2f y_axis_end = toScreen(0, y_max, x_min, x_max, y_min, y_max, width, height);
    sf::VertexArray y_axis(sf::PrimitiveType::Lines, 2);
    y_axis[0].position = y_axis_start;
    y_axis[0].color = axisColor;
    y_axis[1].position = y_axis_end;
    y_axis[1].color = axisColor;
    window.draw(y_axis);

    if (font.hasGlyph('0'))
    {
        sf::Text text(font);
        text.setString("t (time)");
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::Black);
        text.setPosition({static_cast<float>(width) - 60, static_cast<float>(height) - 30});
        window.draw(text);
        text.setString("y(t)");
        text.setPosition({10, 10});
        window.draw(text);
    }
}

void PlotVisualizerSFML3::drawGraph(const std::vector<MomentPoint>& points,
                                    const sf::Color& color,
                                    double x_min, double x_max, double y_min, double y_max,
                                    size_t step)
{
    size_t count = std::min(points.size(), step + 1);
    if (count < 2) return;
    int width = window.getSize().x;
    int height = window.getSize().y;
    sf::VertexArray lines(sf::PrimitiveType::LineStrip, count);
    for (size_t i = 0; i < count; ++i)
    {
        sf::Vector2f screen_pos = toScreen(points[i].t, points[i].y, x_min, x_max, y_min, y_max, width, height);
        lines[i].position = screen_pos;
        lines[i].color = color;
    }
    window.draw(lines);
}

void PlotVisualizerSFML3::drawPoints(const std::vector<MomentPoint>& points,
                                     const sf::Color& color,
                                     double x_min, double x_max, double y_min, double y_max,
                                     size_t step)
{
    size_t count = std::min(points.size(), step + 1);
    int width = window.getSize().x;
    int height = window.getSize().y;
    for (size_t i = 0; i < count; ++i)
    {
        sf::Vector2f screen_pos = toScreen(points[i].t, points[i].y, x_min, x_max, y_min, y_max, width, height);
        sf::CircleShape circle(4);
        circle.setPosition({screen_pos.x - 2, screen_pos.y - 2});
        circle.setFillColor(color);
        window.draw(circle);
    }
}

void PlotVisualizerSFML3::drawInfo(const std::vector<MomentPoint>& eulerPoints,
                                   const std::vector<MomentPoint>& rk4Points,
                                   size_t step)
{
    if (!font.hasGlyph('0')) return;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Black);
    std::string info = "Step: " + std::to_string(step) +
                      " | Euler points: " + std::to_string(std::min(eulerPoints.size(), step + 1)) +
                      " | RK4 points: " + std::to_string(std::min(rk4Points.size(), step + 1));
    text.setString(info);
    text.setPosition({10, static_cast<float>(window.getSize().y) - 30});
    window.draw(text);
    text.setString("Use mouse wheel to zoom");
    text.setCharacterSize(12);
    text.setFillColor(sf::Color(100, 100, 100));
    text.setPosition({10, static_cast<float>(window.getSize().y) - 50});
    window.draw(text);
    text.setString("Press ESC to exit");
    text.setPosition({static_cast<float>(window.getSize().x) - 150, static_cast<float>(window.getSize().y) - 30});
    window.draw(text);
}

void PlotVisualizerSFML3::drawLegend()
{
    if (!font.hasGlyph('0')) return;
    int width = window.getSize().x;
    float boxWidth = 180.0f;
    float boxHeight = 80.0f;
    float padding = 10.0f;
    float left = static_cast<float>(width) - boxWidth - padding;
    float top = padding;
    sf::RectangleShape legendBg({boxWidth, boxHeight});
    legendBg.setFillColor(sf::Color(255, 255, 255, 230));
    legendBg.setOutlineColor(sf::Color(100, 100, 100));
    legendBg.setOutlineThickness(1.0f);
    legendBg.setPosition({left, top});
    window.draw(legendBg);
    sf::RectangleShape marker({16.0f, 16.0f});
    marker.setPosition({left + 10.0f, top + 10.0f});
    marker.setFillColor(sf::Color::Blue);
    window.draw(marker);
    sf::Text label(font);
    label.setCharacterSize(14);
    label.setFillColor(sf::Color::Black);
    label.setString("Euler Method");
    label.setPosition({left + 32.0f, top + 8.0f});
    window.draw(label);
    marker.setPosition({left + 10.0f, top + 36.0f});
    marker.setFillColor(sf::Color::Red);
    window.draw(marker);
    label.setString("RK4 Method");
    label.setPosition({left + 32.0f, top + 34.0f});
    window.draw(label);
    label.setCharacterSize(12);
    label.setFillColor(sf::Color(80, 80, 80));
    label.setString("Scroll wheel to zoom");
    label.setPosition({left + 10.0f, top + 58.0f});
    window.draw(label);
}
