#include "BucketVisualizer.h"

#include <iomanip>
#include <iostream>
#include <sstream>

BucketVisualizerSFML3::BucketVisualizerSFML3(int width, int height, const std::string& title)
#if SFML_VERSION_MAJOR >= 3
    : window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title)
#else
    : window(sf::VideoMode(width, height), title)
#endif
{
    loadFont();
    if (!font.hasGlyph('A'))
    {
        std::cerr << "Warning: Could not load font for bucket window" << std::endl;
    }
}

void BucketVisualizerSFML3::loadFont()
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
#if SFML_VERSION_MAJOR >= 3
        if (font.openFromFile(path))
#else
        if (font.loadFromFile(path))
#endif
            return;
    }
}

void BucketVisualizerSFML3::initialize(const std::vector<MomentPoint>& eulerPoints,
                                       const std::vector<MomentPoint>& rk4Points)
{
    if (eulerPoints.empty() && rk4Points.empty())
        return;

    maxHeight = 1.0;
    for (const auto& p : eulerPoints)
        maxHeight = std::max(maxHeight, p.y);
    for (const auto& p : rk4Points)
        maxHeight = std::max(maxHeight, p.y);
    initialized = true;
}

#if SFML_VERSION_MAJOR >= 3
std::optional<sf::Event> BucketVisualizerSFML3::pollEvent()
{
    return window.pollEvent();
}
#else
bool BucketVisualizerSFML3::pollEvent(sf::Event& event)
{
    return window.pollEvent(event);
}
#endif

bool BucketVisualizerSFML3::isOpen() const
{
    return window.isOpen();
}

void BucketVisualizerSFML3::close()
{
    if (window.isOpen())
        window.close();
}

void BucketVisualizerSFML3::handleEvent(const sf::Event& event)
{
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
}

void BucketVisualizerSFML3::render(const std::vector<MomentPoint>& eulerPoints,
                                   const std::vector<MomentPoint>& rk4Points,
                                   size_t step)
{
    if (!initialized)
        return;

    double eulerLevel = step < eulerPoints.size() ? eulerPoints[step].y : 0.0;
    double rk4Level = step < rk4Points.size() ? rk4Points[step].y : 0.0;
    float eulerFill = static_cast<float>(std::clamp(eulerLevel / maxHeight, 0.0, 1.0) * bucketHeight);
    float rk4Fill = static_cast<float>(std::clamp(rk4Level / maxHeight, 0.0, 1.0) * bucketHeight);

    window.clear(sf::Color::White);
    drawBucket(bucketLeftX, bucketTop, bucketWidth, bucketHeight, eulerFill, sf::Color(120, 180, 255), "Euler");
    drawBucket(bucketRightX, bucketTop, bucketWidth, bucketHeight, rk4Fill, sf::Color(140, 200, 255), "RK4");

    if (font.hasGlyph('0'))
    {
        sf::Text title(font);
        title.setCharacterSize(18);
        title.setFillColor(sf::Color::Black);
        title.setString("Leaky bucket model");
        title.setPosition({80.0f, 20.0f});
        window.draw(title);

        sf::Text info(font);
        info.setCharacterSize(14);
        info.setFillColor(sf::Color(70, 70, 70));
        std::ostringstream oss;
        oss << "Step: " << step << "   y_euler=" << std::fixed << std::setprecision(1) << eulerLevel
            << "   y_rk4=" << std::fixed << std::setprecision(1) << rk4Level;
        info.setString(oss.str());
        info.setPosition({20.0f, bucketBaseY + 10.0f});
        window.draw(info);

        sf::Text hint(font);
        hint.setCharacterSize(12);
        hint.setFillColor(sf::Color(120, 120, 120));
        hint.setString("Press ESC to exit");
        hint.setPosition({window.getSize().x - 140.0f, bucketBaseY + 10.0f});
        window.draw(hint);
    }

    window.display();
}

void BucketVisualizerSFML3::drawBucket(float left, float top, float width, float height,
                                       float fillHeight,
                                       const sf::Color& waterColor,
                                       const std::string& label)
{
    sf::RectangleShape outline({width, height});
    outline.setPosition({left, top});
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color::Black);
    outline.setOutlineThickness(3.0f);
    window.draw(outline);

    if (fillHeight > 0.0f)
    {
        sf::RectangleShape water({width - 12.0f, fillHeight});
        water.setPosition({left + 6.0f, top + height - fillHeight});
        water.setFillColor(waterColor);
        window.draw(water);
    }

    if (font.hasGlyph('A'))
    {
        sf::Text labelText(font);
        labelText.setCharacterSize(16);
        labelText.setFillColor(sf::Color::Black);
        labelText.setString(label);
        labelText.setPosition({left + 12.0f, top + height + 8.0f});
        window.draw(labelText);
    }
}
