#include "../include/PlayingState.hpp"
#include "../include/StateManager.hpp" // Pour le popState si nécessaire
#include <iostream>

namespace Jr {

PlayingState::PlayingState(StateManager& manager, sf::RenderWindow& win, TextureManager& tm, FontManager& fm)
    : GameState(manager, win),
      textureManager(tm), // Références aux managers globaux
      fontManager(fm),
      chessLogic(), // Initialise la logique du jeu pour cette partie
      board(textureManager, fontManager, chessLogic) // Le board est construit avec les managers et la logique
{
    // Le constructeur ne fait pas le setup complet, on le fait dans onEnter.
}

void PlayingState::onEnter() {
    chessLogic.initializeBoard(); // Réinitialise le plateau à chaque nouvelle partie
    // Le board sera mis à jour automatiquement lors de son prochain draw()
    std::cout << "Entering PlayingState." << std::endl;
}

void PlayingState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        board.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
    }
    // Gérer d'autres entrées, comme un retour au menu
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            std::cout << "Escape pressed in PlayingState. Returning to Menu." << std::endl;
            stateManager.popState(); // Revenir à l'état précédent (le menu)
        }
    }
}

void PlayingState::update(float deltaTime) {
    // Mettre à jour la logique de jeu si nécessaire (animations, IA, etc.)
}

void PlayingState::draw() {
    board.draw(window); // Dessiner le plateau de jeu
}

} // namespace Jr