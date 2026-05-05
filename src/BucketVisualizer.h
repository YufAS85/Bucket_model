#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <optional>
#include <string>
#include <vector>
#include "MomentPoint.h"

class BucketVisualizerSFML3
{
public:
    BucketVisualizerSFML3(int width = 420, int height = 520, const std::string& title = "Leaky Bucket Model");

    void initialize(const std::vector<MomentPoint>& eulerPoints,
                    const std::vector<MomentPoint>& rk4Points);
#if SFML_VERSION_MAJOR >= 3
    std::optional<sf::Event> pollEvent();
#else
    bool pollEvent(sf::Event& event);
#endif
    bool isOpen() const;
    void close();
    void handleEvent(const sf::Event& event);
    void render(const std::vector<MomentPoint>& eulerPoints,
                const std::vector<MomentPoint>& rk4Points,
                size_t step);

private:
    void loadFont();
    void drawBucket(float left, float top, float width, float height, float fillHeight,
                    const sf::Color& waterColor, const std::string& label);

private:
    sf::RenderWindow window;
    sf::Font font;
    double maxHeight = 1.0;
    float bucketWidth = 140.0f;
    float bucketHeight = 360.0f;
    float bucketTop = 80.0f;
    float bucketLeftX = 40.0f;
    float bucketRightX = 240.0f;
    float bucketBaseY = bucketTop + bucketHeight;
    bool initialized = false;
};
