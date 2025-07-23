#include "../include/MenuState.hpp"
#include "../include/PlayingState.hpp"
#include "../include/HelpState.hpp"
#include "../include/AboutState.hpp"
#include "../include/StateManager.hpp"
#include "../include/constants.hpp"

#include <iostream>

namespace Jr {

MenuState::MenuState(StateManager& manager, sf::RenderWindow& win, FontManager& fm, TextureManager& tm)
    : GameState(manager, win), fontManager(fm), textureManager(tm) {
    // Le constructeur ne fait pas le setup complet, on le fait dans onEnter.
}

void MenuState::onEnter() {
    // Initialisation des éléments du menu lors de l'entrée dans cet état
    setupButtons(); // Appel de la nouvelle fonction setupButtons

    // Configuration du titre
    titleText.setFont(fontManager.getFont(FONT_PATH));
    titleText.setString("Jeu d'Echecs");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::White);
    centerText(titleText, WINDOW_HEIGHT / 4.0f - 50);
}

void MenuState::setupButtons() {
    menuButtons.clear(); // S'assurer que les éléments ne sont pas dupliqués si on y revient

    const sf::Font& font = fontManager.getFont(FONT_PATH);
    unsigned int charSize = 36; // Taille de texte un peu plus petite pour les boutons
    sf::Vector2f buttonSize(250, 60); // Taille uniforme pour les boutons
    float startY = WINDOW_HEIGHT / 2.0f - 100; // Position de départ pour les options
    float spacing = buttonSize.y + 20; // Espacement entre les boutons

    // Couleurs des boutons
    sf::Color normalColor(70, 70, 70, 200);   // Gris foncé semi-transparent
    sf::Color hoverColor(100, 100, 100, 220); // Gris un peu plus clair au survol
    sf::Color pressedColor(50, 50, 50, 255);  // Gris encore plus foncé au clic

    // Création des boutons
    // Bouton Jouer
    auto playButton = std::make_unique<Button>("Jouer", font, charSize, buttonSize, normalColor, hoverColor, pressedColor);
    playButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY));
    menuButtons.push_back(std::move(playButton));

    // Bouton Aide
    auto helpButton = std::make_unique<Button>("Aide", font, charSize, buttonSize, normalColor, hoverColor, pressedColor);
    helpButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + spacing));
    menuButtons.push_back(std::move(helpButton));

    // Bouton Auteur
    auto aboutButton = std::make_unique<Button>("Auteur", font, charSize, buttonSize, normalColor, hoverColor, pressedColor);
    aboutButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + 2 * spacing));
    menuButtons.push_back(std::move(aboutButton));

    // Bouton Quitter
    auto exitButton = std::make_unique<Button>("Quitter", font, charSize, buttonSize, normalColor, hoverColor, pressedColor);
    exitButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + 3 * spacing));
    menuButtons.push_back(std::move(exitButton));
}

void MenuState::centerText(sf::Text& text, float yOffset) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(WINDOW_WIDTH / 2.0f, yOffset);
}

void MenuState::handleInput(const sf::Event& event) {
    // La mise à jour des couleurs au survol se fait dans update()
    // La gestion du clic se fait ici, au moment où le bouton est relâché
    if (event.type == sf::Event::MouseButtonReleased) {
        for (size_t i = 0; i < menuButtons.size(); ++i) {
            if (menuButtons[i]->isClicked(event)) {
                switch (i) { // Utilise l'index comme identifiant du bouton
                    case 0: // Jouer
                        std::cout << "Option 'Jouer' clicked." << std::endl;
                        stateManager.pushState<PlayingState>(textureManager, fontManager);
                        break;
                    case 1: // Aide
                        std::cout << "Option 'Aide' clicked." << std::endl;
                        stateManager.pushState<HelpState>(fontManager);
                        break;
                    case 2: // Auteur
                        std::cout << "Option 'Auteur' clicked." << std::endl;
                        stateManager.pushState<AboutState>(fontManager);
                        break;
                    case 3: // Quitter
                        std::cout << "Option 'Quitter' clicked." << std::endl;
                        window.close();
                        break;
                }
                return; // Gérer un seul clic
            }
        }
    }
}

void MenuState::update(float deltaTime) {
    // Mettre à jour l'état visuel des boutons (couleur au survol)
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto& button : menuButtons) {
        button->update(mousePos);
    }
}

void MenuState::draw() {
    window.draw(titleText);
    for (const auto& button : menuButtons) {
        button->draw(window);
    }
}

} // namespace Jr