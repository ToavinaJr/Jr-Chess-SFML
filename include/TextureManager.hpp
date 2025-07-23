#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>

namespace Jr {

    class TextureManager {
    private:
        std::map<std::string, sf::Texture> textures;

    public:
        // Récupère la texture si déjà chargée, sinon la charge depuis le disque
        const sf::Texture& getTexture(const std::string& filename);

        // Charge plusieurs textures à l'avance
        void preloadTextures(const std::vector<std::string>& files);

        // Vide toutes les textures chargées
        void clear();
    };

}
