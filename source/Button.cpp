#include "../include/Button.hpp"

namespace Jr {

Button::Button(const std::string& textStr, const sf::Font& font, unsigned int charSize,
               sf::Vector2f size, sf::Color normal, sf::Color hover, sf::Color pressed)
    : normalColor(normal), hoverColor(hover), pressedColor(pressed) {
    // Initialisation de la forme du bouton
    shape.setSize(size);
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::Black); // Une petite bordure pour le contraste

    // Initialisation du texte du bouton
    text.setFont(font);
    text.setString(textStr);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::White); // Couleur du texte

    centerText(); // Centre le texte par rapport à la forme du bouton
}

void Button::setPosition(sf::Vector2f pos) {
    shape.setPosition(pos);
    centerText(); // Recentrer le texte après avoir déplacé la forme
}

void Button::centerText() {
    // Centrer l'origine du texte pour qu'il soit bien au milieu du bouton
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    
    // Positionner le texte au centre de la forme
    sf::Vector2f shapeCenter = shape.getPosition() + shape.getSize() / 2.0f;
    text.setPosition(shapeCenter);
}

void Button::update(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        shape.setFillColor(hoverColor); // Survolé
    } else {
        shape.setFillColor(normalColor); // Normal
    }
}

bool Button::isClicked(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        // Vérifie si le bouton a été relâché et si la souris était sur le bouton
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(hoverColor); // Retour à l'état survolé après le clic
            return true;
        }
    } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(pressedColor); // État pressé
        }
    }
    return false;
}


void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(text);
}

sf::FloatRect Button::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

} // namespace Jr