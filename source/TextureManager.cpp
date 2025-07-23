#include "../include/TextureManager.hpp"

namespace Jr {

    const sf::Texture& TextureManager::getTexture(const std::string& filename) {
        auto it = textures.find(filename);
        if (it != textures.end()) {
            return it->second;
        }

        sf::Texture texture;
        if (!texture.loadFromFile(filename)) {
            throw std::runtime_error("Impossible de charger la texture : " + filename);
        }

        texture.setSmooth(true); // Optionnel : active le lissage

        // Insérer la texture dans la map en déplaçant pour éviter une copie
        textures[filename] = std::move(texture);
        return textures[filename];
    }

    void TextureManager::preloadTextures(const std::vector<std::string>& files) {
        for (const auto& file : files) {
            getTexture(file); // charge la texture si pas déjà chargée
        }
    }

    void TextureManager::clear() {
        textures.clear();
    }

}
