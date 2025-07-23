#include "../include/FontManager.hpp"
#include <iostream>   // Nécessaire pour std::cerr

namespace Jr {

    /**
     * @brief Récupère une police à partir de son nom de fichier.
     *
     * Cette fonction est le point d'accès principal pour obtenir une police.
     * Elle vérifie d'abord si la police a déjà été chargée et est en cache.
     * Si ce n'est pas le cas, elle tente de charger la police depuis le fichier spécifié.
     * En cas d'échec de chargement, une erreur est affichée sur `std::cerr` et une `std::runtime_error` est lancée.
     *
     * @param filename Le chemin d'accès au fichier de la police (par exemple, "arial.ttf").
     * @return Une référence constante à l'objet `sf::Font` chargé.
     * @throws std::runtime_error Si la police ne peut pas être chargée depuis le fichier spécifié.
     */
    const sf::Font& FontManager::getFont(const std::string& filename) {
        // Tente de trouver la police dans la map `fonts` (le cache).
        auto it = fonts.find(filename);
        if (it != fonts.end()) {
            // La police est déjà chargée, retourne une référence à la police existante.
            return it->second;
        }

        // La police n'est pas encore chargée, tente de la charger.
        sf::Font font;
        if (!font.loadFromFile(filename)) {
            // Échec du chargement de la police. Affiche une erreur et lance une exception.
            std::cerr << "Erreur : Impossible de charger la police : " << filename << std::endl;
            throw std::runtime_error("Impossible de charger la police : " + filename);
        }
        
        // La police a été chargée avec succès. La stocke dans la map en utilisant `std::move`
        // pour un déplacement efficace de l'objet `sf::Font`.
        fonts[filename] = std::move(font);
        // Retourne une référence à la police nouvellement chargée et stockée.
        return fonts[filename];
    }

    /**
     * @brief Précharge un ensemble de polices.
     *
     * Cette fonction parcourt une liste de noms de fichiers de polices
     * et appelle `getFont` pour chaque fichier. Cela permet de s'assurer
     * que ces polices sont chargées et mises en cache avant d'être réellement utilisées,
     * évitant ainsi des retards potentiels lors du premier accès.
     * En cas d'échec de chargement d'une police, `getFont` gérera l'erreur.
     *
     * @param files Un vecteur de chaînes de caractères, où chaque chaîne est le chemin d'accès à un fichier de police.
     */
    void FontManager::preloadFonts(const std::vector<std::string>& files) {
        for (const auto& file : files) {
            getFont(file); // Appelle getFont, qui chargera ou récupérera la police du cache.
        }
    }

    /**
     * @brief Vide le cache de polices.
     *
     * Cette fonction libère toutes les ressources mémoire occupées par les polices
     * actuellement chargées dans le gestionnaire. Elle doit être appelée avec prudence,
     * car toutes les références `sf::Font` précédemment obtenues via `getFont`
     * deviendront invalides après cet appel.
     */
    void FontManager::clear() {
        fonts.clear(); // Efface tous les éléments de la map, libérant les polices.
    }

} // namespace Jr