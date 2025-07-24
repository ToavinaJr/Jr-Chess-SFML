#include "../include/GameOverState.hpp"
#include "../include/StateManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace Jr {

class PlayingState;
GameOverState::GameOverState(StateManager& manager, sf::RenderWindow& win,
                             FontManager& fm, TextureManager& tm,
                             const std::string& result)
    : GameState(manager, win),
      fontManager(fm),
      textureManager(tm),
      resultMessage(result)
{
    resultText.setFont(fontManager.getFont(FONT_PATH));
    resultText.setString(resultMessage);
    resultText.setCharacterSize(48);
    resultText.setFillColor(sf::Color::Red);
    centerText(resultText, window.getSize().y / 4.f);

    replayButton = std::make_unique<Button>(
        "Rejouer", fontManager.getFont(FONT_PATH), 18,
        sf::Vector2f(200.f, 50.f),
        sf::Color(50, 50, 60, 200),
        sf::Color(80, 80, 90, 220),
        sf::Color(30, 30, 40, 255)
    );

    menuButton = std::make_unique<Button>(
        "Menu", fontManager.getFont(FONT_PATH), 18,
        sf::Vector2f(200.f, 50.f),
        sf::Color(50, 50, 60, 200),
        sf::Color(80, 80, 90, 220),
        sf::Color(30, 30, 40, 255)
    );

    replayButton->setPosition({window.getSize().x / 2.f - replayButton.get()->getGlobalBounds().width / 2, window.getSize().y / 2.f});
    replayButton->centerText();
    menuButton->setPosition({window.getSize().x / 2.f - menuButton.get()->getGlobalBounds().width / 2, window.getSize().y / 2.f + 100.f});
    menuButton->centerText();
}

void GameOverState::centerText(sf::Text& text, float yOffset) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(window.getSize().x / 2.f, yOffset);
}

void GameOverState::handleInput(const sf::Event& event) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    replayButton->update(mousePos);
    menuButton->update(mousePos);

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {

        if (replayButton->isClicked(event)) {
            stateManager.pushState<PlayingState>(textureManager, fontManager);
        }

        if (menuButton->isClicked(event)) {
            stateManager.changeState<MenuState>(fontManager, textureManager);
        }
    }
}

void GameOverState::update(float deltaTime) {
    // Pas d'animation pour l'instant
}

void GameOverState::draw() {
    window.clear(sf::Color::Black);
    window.draw(resultText);
    replayButton->draw(window);
    menuButton->draw(window);
    window.display();
}

} // namespace Jr
