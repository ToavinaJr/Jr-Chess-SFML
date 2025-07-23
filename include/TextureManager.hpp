#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>

namespace Jr {

/**
 * @class TextureManager
 * @brief Gestionnaire de textures SFML chargé de gérer le chargement, la mise en cache et la libération des textures.
 * 
 * Cette classe permet de charger des textures depuis le disque une seule fois,
 * et de les réutiliser via un cache interne afin d'éviter des chargements redondants coûteux.
 */
class TextureManager {
private:
    /// Map associant un nom de fichier à sa texture SFML correspondante
    std::map<std::string, sf::Texture> textures;

public:
    /**
     * @brief Récupère la texture correspondant au fichier spécifié.
     * 
     * Si la texture n'est pas encore chargée, elle est chargée depuis le disque
     * et mise en cache pour les appels suivants.
     * 
     * @param filename Chemin du fichier image de la texture.
     * @return Référence constante à la texture chargée.
     * @throws std::runtime_error si le chargement échoue.
     */
    const sf::Texture& getTexture(const std::string& filename);

    /**
     * @brief Charge à l'avance plusieurs textures données dans un vecteur de noms de fichiers.
     * 
     * Permet de précharger les textures afin d'éviter des temps de latence durant le jeu.
     * 
     * @param files Vecteur des chemins des fichiers à charger.
     * @throws std::runtime_error si une texture ne peut être chargée.
     */
    void preloadTextures(const std::vector<std::string>& files);

    /**
     * @brief Vide le cache de textures, libérant ainsi la mémoire associée.
     */
    void clear();
};

}
