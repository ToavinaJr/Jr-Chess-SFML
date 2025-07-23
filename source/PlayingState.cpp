#include "../include/PlayingState.hpp"
#include "../include/StateManager.hpp"
#include "../include/ChessLogic.hpp"
#include "../include/GameOverState.hpp"

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
    // Ici tu peux aussi mettre à jour les animations ou l'IA si tu en as

    // Récupérer l'état actuel du jeu via ChessLogic
    ChessGameStatus state = chessLogic.getGameState();

    switch (state) {
        case ChessGameStatus::Checkmate:
            stateManager.changeState<GameOverState>(fontManager, textureManager, "Échec et mat !");
            std::cout << "Checkmate! Game Over." << std::endl;
            break;

        case ChessGameStatus::Stalemate:
            stateManager.changeState<GameOverState>(fontManager, textureManager, "Pat !");
            break;

        case ChessGameStatus::Draw50Move:
            stateManager.changeState<GameOverState>(fontManager, textureManager, "Match nul (règle des 50 coups) !");
            std::cout << "Draw by 50-move rule! Game Over." << std::endl;
            break;

        case ChessGameStatus::DrawRepetition:
            stateManager.changeState<GameOverState>(fontManager, textureManager, "Match nul (répétition de position) !");
            std::cout << "Draw by repetition! Game Over." << std::endl;
            break;

        case ChessGameStatus::DrawMaterial:
            stateManager.changeState<GameOverState>(fontManager, textureManager, "Pat !");
            // std::cout << "Draw by insufficient material! Game Over." << std::endl;
            break;

        case ChessGameStatus::Playing:
        default:
            // Rien à faire, la partie continue
            // std::cout << "Game is still ongoing." << std::endl;
            break;
    }
}


void PlayingState::draw() {
    board.draw(window); // Dessiner le plateau de jeu
}

} // namespace Jr