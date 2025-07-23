#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory> // Pour std::unique_ptr

// Incluez tous les autres en-têtes nécessaires
#include "TextureManager.hpp"
#include "FontManager.hpp"
#include "ChessLogic.hpp" // Renommé de ChessLogicManager.hpp
#include "Board.hpp"
#include "constants.hpp"

// Nouveaux includes pour le StateManager
#include "StateManager.hpp"
#include "MenuState.hpp"      // Initial state
#include "PlayingState.hpp"   // Possible state
#include "HelpState.hpp"      // Possible state
#include "AboutState.hpp"     // Possible state

namespace Jr {

class Game {
private:
    sf::RenderWindow window;
    TextureManager textureManager;
    FontManager fontManager;
    
    // Le StateManager est maintenant responsable de la gestion des états
    StateManager stateManager;

    // Méthodes privées
    void loadAssets();
    void handleEvents();
    void update(float deltaTime);
    void render();

public:
    Game();
    int run();
};

} // namespace Jr