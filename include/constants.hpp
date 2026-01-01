#pragma once
#include <SFML/Graphics/Color.hpp>

namespace Jr {
    // Box sizes
    inline constexpr int BOX_SIZE = 64;
    inline constexpr int MARGIN = 28;
    inline constexpr int SIDEBAR_WIDTH = 320; // Panneau latéral pour captures, horloge, historique

    // Window sizing - élargi pour inclure le sidebar
    inline constexpr int BOARD_WIDTH = BOX_SIZE * 8 + 2 * MARGIN;
    inline constexpr int WINDOW_WIDTH = BOARD_WIDTH + SIDEBAR_WIDTH;
    inline constexpr int WINDOW_HEIGHT = BOX_SIZE * 8 + 2 * MARGIN;

    // Title
    inline constexpr const char* WINDOW_TITLE = "SFML Chess";

    // Colors - style moderne chess.com
    inline const sf::Color BOX_COLOR_LIGHT = sf::Color(240, 217, 181);
    inline const sf::Color BOX_COLOR_DARK  = sf::Color(181, 136, 99);
    inline const sf::Color BACKGROUND_COLOR = sf::Color(38, 36, 33); // Fond sombre moderne
    inline const sf::Color PANEL_BG_COLOR = sf::Color(48, 46, 43);   // Couleur panneaux
    inline const sf::Color TEXT_COLOR = sf::Color(220, 220, 220);    // Texte clair
    inline const sf::Color ACCENT_COLOR = sf::Color(129, 182, 76);   // Vert accent

    constexpr const char* FONT_PATH = "../assets/fonts/SpaceMono-Regular.ttf";
}
