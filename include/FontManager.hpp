#pragma once
#include <SFML/Graphics/Font.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace Jr {

/**
 * @class FontManager
 * @brief Gestion centralisée du chargement et du cache des polices.
 *
 * Fournit un système de cache pour éviter de recharger les polices plusieurs fois.
 */
class FontManager {
public:
    const sf::Font& getFont(const std::string& filename);
    void preloadFonts(const std::vector<std::string>& files);
    void clear();

private:
    std::unordered_map<std::string, sf::Font> fonts; ///< Cache des polices chargées.
};

} // namespace Jr
