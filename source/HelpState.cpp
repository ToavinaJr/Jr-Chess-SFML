#include "../include/HelpState.hpp"
#include "../include/StateManager.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

HelpState::HelpState(StateManager& manager, sf::RenderWindow& win, FontManager& fm)
    : GameState(manager, win), fontManager(fm) {
    // Le setup est fait dans onEnter.
}

void HelpState::onEnter() {
    std::cout << "Entering HelpState." << std::endl;

    // === Titre ===
    titleText.setFont(fontManager.getFont(FONT_PATH));
    titleText.setString("Aide - Comment Jouer");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    sf::FloatRect textRect = titleText.getLocalBounds();
    titleText.setOrigin(textRect.left + textRect.width / 2.f,
                        textRect.top + textRect.height / 2.f);
    titleText.setPosition(WINDOW_WIDTH / 2.f, 100);

    // === Contenu ===
    contentText.setFont(fontManager.getFont(FONT_PATH));
    std::string helpContent =
        "Bienvenue au jeu d'echecs !\n\n"
        "Pour apprendre mieux comment jouer,\n"
        "Consulter: https://en.wikipedia.org/wiki/Chess\n\n";
    
    contentText.setString(helpContent);
    contentText.setCharacterSize(16);
    contentText.setFillColor(sf::Color::White);
    contentText.setLineSpacing(1.2f);
    contentText.setPosition(50, titleText.getPosition().y + titleText.getCharacterSize() * 2);

    // === Bouton Retour ===
    sf::Color btnNormal(50, 50, 60, 200);
    sf::Color btnHover(80, 80, 90, 220);
    sf::Color btnPressed(30, 30, 40, 255);
    sf::Vector2f btnSize(200, 50);

    backButton = std::make_unique<Button>("Retour", fontManager.getFont(FONT_PATH), 28,
                                          btnSize, btnNormal, btnHover, btnPressed);

    backButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - btnSize.x / 2.f,
                                         WINDOW_HEIGHT - 100));
}

void HelpState::centerText(sf::Text& text, float yOffset) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f,
                   textRect.top + textRect.height / 2.f);
    text.setPosition(WINDOW_WIDTH / 2.f, yOffset);
}

void HelpState::handleInput(const sf::Event& event) {
    // ✅ Gestion du clic sur le bouton Retour
    if (event.type == sf::Event::MouseButtonReleased && backButton->isClicked(event)) {
        std::cout << "Back button clicked in HelpState. Returning to Menu." << std::endl;
        stateManager.popState();
        return;
    }

    // ✅ Gestion touche Échap
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "Escape pressed in HelpState. Returning to Menu." << std::endl;
        stateManager.popState();
    }
}

void HelpState::update(float deltaTime) {
    if (backButton) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        backButton->update(mousePos);
    }
}

void HelpState::draw() {
    window.draw(titleText);
    window.draw(contentText);
    if (backButton) {
        backButton->draw(window);
    }
}

} // namespace Jr
