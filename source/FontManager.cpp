#include "../include/FontManager.hpp"
#include <iostream>   // Nécessaire pour std::cerr

namespace Jr {

/**
 * @class FontManager
 * @brief Gestionnaire centralisé des polices (`sf::Font`).
 *
 * Cette classe gère le chargement, le cache et la libération des polices
 * utilisées dans l'application.  
 * - Les polices sont chargées à la demande et mises en cache pour éviter des rechargements coûteux.  
 * - Toute tentative d'accès à une police non encore chargée entraîne son chargement automatique.
 *
 * @note
 * Après un appel à `clear()`, **toutes les références précédemment obtenues via `getFont` deviennent invalides**.
 */

/**
 * @brief Récupère une police à partir de son fichier (chargement à la demande).
 *
 * Si la police est déjà en cache, elle est retournée directement.  
 * Sinon, elle est chargée depuis le fichier et stockée dans le cache.
 *
 * @param filename Chemin d'accès au fichier de la police (ex. `"arial.ttf"`).
 * @return Référence constante vers l'objet `sf::Font` chargé.
 * @throws std::runtime_error Si le chargement de la police échoue.
 */
const sf::Font& FontManager::getFont(const std::string& filename) {
    auto it = fonts.find(filename);
    if (it != fonts.end()) {
        return it->second; // Déjà en cache
    }

    sf::Font font;
    if (!font.loadFromFile(filename)) {
        std::cerr << "[FontManager] Erreur : Impossible de charger la police : " << filename << std::endl;
        throw std::runtime_error("Impossible de charger la police : " + filename);
    }

    fonts[filename] = std::move(font);
    return fonts[filename];
}

/**
 * @brief Précharge un ensemble de polices.
 *
 * Charge et met en cache toutes les polices listées pour éviter les retards
 * lors de leur premier usage dans l'interface.
 *
 * @param files Liste des chemins d'accès aux fichiers de police.
 */
void FontManager::preloadFonts(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        getFont(file); // Chargement à la demande
    }
}

/**
 * @brief Vide complètement le cache des polices.
 *
 * Libère toutes les ressources mémoire utilisées par les polices.  
 * À utiliser avec prudence, car **toutes les références `sf::Font` retournées
 * précédemment par `getFont()` deviendront invalides**.
 */
void FontManager::clear() {
    fonts.clear();
}

} // namespace Jr
