#pragma once
#include <SFML/Graphics/Color.hpp>

namespace Jr {
    // Box sizes
    inline constexpr int BOX_SIZE = 64;
    inline constexpr int MARGIN = 28;

    // Window sizing
    inline constexpr int WINDOW_WIDTH = BOX_SIZE * 8 + 2 * MARGIN;
    inline constexpr int WINDOW_HEIGHT = BOX_SIZE * 8 + 2 * MARGIN;

    // Title
    inline constexpr const char* WINDOW_TITLE = "SFML Chess";

    // Colors
    inline const sf::Color BOX_COLOR_LIGHT = sf::Color(240, 217, 181);
    inline const sf::Color BOX_COLOR_DARK  = sf::Color(181, 136, 99);
    inline const sf::Color BACKGROUND_COLOR = sf::Color(200, 200, 200);

    constexpr const char* FONT_PATH = "../assets/fonts/SpaceMono-Regular.ttf";
}
