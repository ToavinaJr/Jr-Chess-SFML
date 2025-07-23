#pragma once
#include <SFML/Graphics/Font.hpp>
#include <map>
#include <string>
#include <stdexcept>

namespace Jr {

    class FontManager {
    private:
        std::map<std::string, sf::Font> fonts;

    public:
        const sf::Font& getFont(const std::string& filename);
        void preloadFonts(const std::vector<std::string>& files);
        void clear();
    };

}