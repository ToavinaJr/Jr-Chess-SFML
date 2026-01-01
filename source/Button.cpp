#include "../include/Button.hpp"

namespace Jr {

/**
 * @class Button
 * @brief Représente un bouton interactif avec trois états : normal, survolé et pressé.
 */

/**
 * @brief Constructeur du bouton.
 * @param textStr Texte affiché sur le bouton.
 * @param font Police utilisée pour le texte.
 * @param charSize Taille des caractères.
 * @param size Taille du bouton.
 * @param normal Couleur par défaut.
 * @param hover Couleur lors du survol.
 * @param pressed Couleur lors du clic.
 */
Button::Button(const std::string& textStr, const sf::Font& font, unsigned int charSize,
               sf::Vector2f size, sf::Color normal, sf::Color hover, sf::Color pressed)
    : normalColor(normal), hoverColor(hover), pressedColor(pressed) {

    shape.setSize(size);
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::Black);

    text.setFont(font);
    text.setString(textStr);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::White);

    centerText();
}

/**
 * @brief Définit la position du bouton et recalcule le centrage du texte.
 * @param pos Nouvelle position.
 */
void Button::setPosition(sf::Vector2f pos) {
    shape.setPosition(pos);
    centerText();
}

/**
 * @brief Centre le texte par rapport à la forme du bouton.
 */
void Button::centerText() {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(shape.getPosition() + shape.getSize() / 2.0f);
}

/**
 * @brief Met à jour l'état visuel du bouton selon la position de la souris.
 * @param mousePos Position actuelle de la souris.
 */
void Button::update(const sf::Vector2f& mousePos) {
    // Si le bouton est sélectionné, garder la couleur de sélection sauf en cas de survol
    if (selected) {
        shape.setFillColor(shape.getGlobalBounds().contains(mousePos) ? hoverColor : pressedColor);
    } else {
        shape.setFillColor(shape.getGlobalBounds().contains(mousePos) ? hoverColor : normalColor);
    }
}

/**
 * @brief Vérifie si le bouton a été cliqué.
 * @param event Événement SFML (clic souris).
 * @return true si le bouton a été cliqué.
 */
bool Button::isClicked(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(hoverColor);
            return true;
        }
    } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(pressedColor);
        }
    }
    return false;
}

/**
 * @brief Dessine le bouton sur la fenêtre.
 * @param window Fenêtre SFML cible.
 */
void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(text);
}

/**
 * @brief Retourne les limites globales du bouton (utile pour la détection de collisions).
 */
sf::FloatRect Button::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

/**
 * @brief Définit l'état de sélection du bouton (modifie la couleur de fond).
 * @param isSelected true si le bouton doit être marqué comme sélectionné.
 */
void Button::setSelected(bool isSelected) {
    selected = isSelected;
    if (selected) {
        shape.setFillColor(pressedColor); // Utilise la couleur pressée pour indiquer la sélection
    } else {
        shape.setFillColor(normalColor);
    }
}

/**
 * @brief Retourne si le bouton est actuellement sélectionné.
 * @return true si le bouton est sélectionné.
 */
bool Button::isSelected() const {
    return selected;
}

} // namespace Jr
