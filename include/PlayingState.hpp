#pragma once

#include "GameState.hpp"
#include "Board.hpp"
#include "ChessLogic.hpp"
#include "TextureManager.hpp"
#include "FontManager.hpp"

namespace Jr {

/**
 * @brief Représente l'état de jeu actif où une partie d'échecs se déroule.
 */
class PlayingState : public GameState {
private:
    // Ces membres ne sont pas créés ici, mais passés par référence
    // pour que PlayingState puisse utiliser les gestionnaires du Game.
    TextureManager& textureManager;
    FontManager& fontManager;
    ChessLogic chessLogic; // Chaque partie a sa propre logique de jeu
    Board board;

public:
    PlayingState(StateManager& manager, sf::RenderWindow& win, TextureManager& tm, FontManager& fm);
    ~PlayingState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
    void onEnter() override; // Initialisation de la partie quand on entre dans cet état
};

} // namespace Jr