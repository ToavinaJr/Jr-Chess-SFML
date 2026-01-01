#include "../include/AboutState.hpp"
#include "../include/StateManager.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

/**
 * @brief Constructeur de l'état "À propos" (AboutState).
 *
 * Initialise l'arrière-plan et stocke les références nécessaires.
 *
 * @param manager Référence vers le StateManager pour gérer les états.
 * @param win Fenêtre principale SFML dans laquelle sera rendu l'état.
 * @param fm Gestionnaire de polices (FontManager) pour charger les polices utilisées.
 */
AboutState::AboutState(StateManager& manager, sf::RenderWindow& win, FontManager& fm)
    : GameState(manager, win), fontManager(fm) {
    backgroundShape.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    backgroundShape.setFillColor(sf::Color(0, 0, 0));
}

/**
 * @brief Méthode appelée à l'entrée de l'état.
 *
 * Configure tous les éléments graphiques : titre, ligne de séparation, textes d'information,
 * et bouton de retour.
 */
void AboutState::onEnter() {
    std::cout << "Entering AboutState." << std::endl;

    // === Titre ===
    titleText.setFont(fontManager.getFont(FONT_PATH));
    titleText.setString("Auteur");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color(255, 215, 0));
    centerText(titleText, 100);

    // === Ligne de séparation sous le titre ===
    titleSeparator.setSize(sf::Vector2f(titleText.getGlobalBounds().width + 40, 3));
    titleSeparator.setFillColor(sf::Color(255, 215, 0, 150));
    titleSeparator.setOrigin(titleSeparator.getLocalBounds().width / 2.f,
                             titleSeparator.getLocalBounds().height / 2.f);
    titleSeparator.setPosition(WINDOW_WIDTH / 2.f,
                               titleText.getPosition().y + titleText.getCharacterSize() / 2.f + 30);

    // === Textes d'information centrés sous le titre ===
    float startY = titleSeparator.getPosition().y + 80; ///< Point de départ vertical

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
    footer3.setString("Email : toavinasylvianno14@gmail.com");
    footer3.setCharacterSize(16);
    footer3.setFillColor(sf::Color(200, 200, 200));
    centerText(footer3, startY + 100);

    footer4.setFont(fontManager.getFont(FONT_PATH));
    footer4.setString("Portfolio : https://portfolio-toavinajr.vercel.app/");
    footer4.setCharacterSize(16);
    footer4.setFillColor(sf::Color(180, 220, 255));
    centerText(footer4, startY + 150);

    // === Bouton de retour centré en bas ===
    sf::Color btnNormal(50, 50, 60, 200);
    sf::Color btnHover(80, 80, 90, 220);
    sf::Color btnPressed(30, 30, 40, 255);
    sf::Vector2f btnSize(200, 50);

    backButton = std::make_unique<Button>("Retour", fontManager.getFont(FONT_PATH), 28,
                                          btnSize, btnNormal, btnHover, btnPressed);
    backButton->setPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - btnSize.x / 2.f,
                                         footer4.getPosition().y + 100));
}

/**
 * @brief Centre horizontalement un texte par rapport à la fenêtre.
 *
 * @param text Référence vers l'objet sf::Text à centrer.
 * @param yOffset Position verticale (axe Y) où placer le texte.
 */
void AboutState::centerText(sf::Text& text, float yOffset) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f,
                   textRect.top + textRect.height / 2.f);
    text.setPosition(WINDOW_WIDTH / 2.f, yOffset);
}

/**
 * @brief Gère les entrées clavier et souris.
 *
 * @param event Événement SFML (clics ou touches).
 *
 * @details
 * - Si le bouton "Retour" est cliqué → Retourne au menu (popState).
 * - Si la touche Échap est pressée → Retourne également au menu.
 */
void AboutState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && backButton->isClicked(event)) {
        stateManager.popState();
        return;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.popState();
    }
}

/**
 * @brief Met à jour les éléments dynamiques de l'état.
 *
 * @param deltaTime Temps écoulé depuis la dernière mise à jour (en secondes).
 *
 * @details
 * Actuellement, seule la mise à jour visuelle du bouton "Retour" est effectuée.
 */
void AboutState::update(float deltaTime) {
    backButton->update(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}

/**
 * @brief Dessine tous les éléments graphiques de l'état.
 *
 * @details
 * L'ordre de dessin est : fond → titre → ligne de séparation → textes → bouton "Retour".
 */
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
