#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <SFML/Graphics/Color.hpp>

namespace Jr {
    
    // Box sizes
    constexpr const int BOX_SIZE = 64;

    // Margin
    constexpr const int MARGIN = 28;

    // Window sizing constants
    constexpr const int WINDOW_WIDTH = BOX_SIZE * 8 + 2 * MARGIN;
    constexpr const int WINDOW_HEIGHT = BOX_SIZE * 8 + 2 * MARGIN;

    // Window title
    constexpr const char* WINDOW_TITLE = "SFML Chess";
    
    // Box Colors
    const sf::Color BOX_COLOR_LIGHT = sf::Color(240, 217, 181);
    const sf::Color BOX_COLOR_DARK  = sf::Color(181, 136, 99);
    const sf::Color BACKGROUND_COLOR = sf::Color(200, 200, 200);

}

#endif // CONSTANTS_HPP