#include "../include/Game.hpp"
#include <iostream>
#include <stdexcept>

namespace Jr {

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE),
      textureManager(),
      fontManager(),
      stateManager(window)
{
    window.setFramerateLimit(60);

    try {
        loadAssets();
    } catch (const std::runtime_error& e) {
        std::cerr << "Erreur critique au démarrage du jeu: " << e.what() << std::endl;
        exit(1);
    }

    stateManager.pushState<MenuState>(fontManager, textureManager);
}

void Game::loadAssets() {
    // Chemins des textures de pièces
    std::vector<std::string> pieceTexturePaths = {
        "../assets/images/pieces/chess_maestro_bw/wP.png",
        "../assets/images/pieces/chess_maestro_bw/wR.png",
        "../assets/images/pieces/chess_maestro_bw/wN.png",
        "../assets/images/pieces/chess_maestro_bw/wB.png",
        "../assets/images/pieces/chess_maestro_bw/wQ.png",
        "../assets/images/pieces/chess_maestro_bw/wK.png",
        "../assets/images/pieces/chess_maestro_bw/bP.png",
        "../assets/images/pieces/chess_maestro_bw/bR.png",
        "../assets/images/pieces/chess_maestro_bw/bN.png",
        "../assets/images/pieces/chess_maestro_bw/bB.png",
        "../assets/images/pieces/chess_maestro_bw/bQ.png",
        "../assets/images/pieces/chess_maestro_bw/bK.png"
    };
    
    textureManager.preloadTextures(pieceTexturePaths);
    fontManager.getFont(FONT_PATH); 
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        // Déléguer la gestion de l'événement à l'état actif
        stateManager.handleInput(event);
    }
}

void Game::update(float deltaTime) {
    // Déléguer la mise à jour à l'état actif
    stateManager.update(deltaTime);
}

void Game::render() {
    window.clear();
    // Déléguer le dessin à l'état actif
    stateManager.draw();
    window.display();
}

int Game::run() {
    sf::Clock clock; // Pour calculer le deltaTime
    while (window.isOpen() && !stateManager.isEmpty()) { // Le jeu continue tant qu'il y a des états
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();
    }
    return 0;
}

} // namespace Jr