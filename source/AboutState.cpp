#include "../include/AboutState.hpp"
#include "../include/StateManager.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

AboutState::AboutState(StateManager& manager, sf::RenderWindow& win, FontManager& fm)
    : GameState(manager, win), fontManager(fm) {
    backgroundShape.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    backgroundShape.setFillColor(sf::Color(20, 20, 30)); // Bleu nuit
}

void AboutState::onEnter() {
    std::cout << "Entering AboutState." << std::endl;

    // === Titre ===
    titleText.setFont(fontManager.getFont(FONT_PATH));
    titleText.setString("Auteur");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color(255, 215, 0));
    centerText(titleText, 100);

    // === Ligne de séparation ===
    titleSeparator.setSize(sf::Vector2f(titleText.getGlobalBounds().width + 40, 3));
    titleSeparator.setFillColor(sf::Color(255, 215, 0, 150));
    titleSeparator.setOrigin(titleSeparator.getLocalBounds().width / 2.f,
                             titleSeparator.getLocalBounds().height / 2.f);
    titleSeparator.setPosition(WINDOW_WIDTH / 2.f,
                               titleText.getPosition().y + titleText.getCharacterSize() / 2.f + 30);

    // === Textes centrés sous le titre ===
    float startY = titleSeparator.getPosition().y + 80; // Point de départ

    footer1.setFont(fontManager.getFont(FONT_PATH));
    footer1.setString("Cree par Toavina Jr");
    footer1.setCharacterSize(24);
    footer1.setFillColor(sf::Color(255, 215, 0));
    centerText(footer1, startY);

    footer2.setFont(fontManager.getFont(FONT_PATH));
    footer2.setString("En utilisant SFML");
    footer2.setCharacterSize(16);
    footer2.setFillColor(sf::Color(200, 200, 200));
    centerText(footer2, startY + 50);

    footer3.setFont(fontManager.getFont(FONT_PATH));
    footer3.setString("Email : toavina@gmail.com");
    footer3.setCharacterSize(16);
    footer3.setFillColor(sf::Color(200, 200, 200));
    centerText(footer3, startY + 100);

    footer4.setFont(fontManager.getFont(FONT_PATH));
    footer4.setString("Portfolio : https://portfolio-toavinajr.vercel.app/");
    footer4.setCharacterSize(16);
    footer4.setFillColor(sf::Color(180, 220, 255));
    centerText(footer4, startY + 150);

    // === Bouton retour ===
    sf::Color btnNormal(50, 50, 60, 200);
    sf::Color btnHover(80, 80, 90, 220);
    sf::Color btnPressed(30, 30, 40, 255);
    sf::Vector2f btnSize(200, 50);

    backButton = std::make_unique<Button>("Retour", fontManager.getFont(FONT_PATH), 28,
                                          btnSize, btnNormal, btnHover, btnPressed);
    backButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - btnSize.x / 2.f,
                                         footer4.getPosition().y + 100));
}

void AboutState::centerText(sf::Text& text, float yOffset) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f,
                   textRect.top + textRect.height / 2.f);
    text.setPosition(WINDOW_WIDTH / 2.f, yOffset);
}

void AboutState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && backButton->isClicked(event)) {
        stateManager.popState();
        return;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.popState();
    }
}

void AboutState::update(float deltaTime) {
    backButton->update(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}

void AboutState::draw() {
    window.draw(backgroundShape);
    window.draw(titleText);
    window.draw(titleSeparator);

    // Bloc de textes centrés
    window.draw(footer1);
    window.draw(footer2);
    window.draw(footer3);
    window.draw(footer4);

    backButton->draw(window);
}

} // namespace Jr
