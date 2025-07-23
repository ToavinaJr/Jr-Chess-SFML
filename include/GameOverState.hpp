#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp"
#include "Button.hpp"
#include "PlayingState.hpp" // Pour lancer une nouvelle partie
#include "MenuState.hpp"    // Pour revenir au menu

#include <string>
#include <memory>

namespace Jr {

class GameOverState : public GameState {
private:
    FontManager& fontManager;
    TextureManager& textureManager;

    sf::Text resultText;
    std::unique_ptr<Button> replayButton;
    std::unique_ptr<Button> menuButton;

    std::string resultMessage;

    void centerText(sf::Text& text, float yOffset);

public:
    GameOverState(StateManager& manager, sf::RenderWindow& win,
                  FontManager& fm, TextureManager& tm,
                  const std::string& result);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
};

} // namespace Jr
