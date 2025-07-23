#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace Jr {

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressedColor;

public:
    // Constructeur
    Button(const std::string& textStr, const sf::Font& font, unsigned int charSize,
           sf::Vector2f size, sf::Color normal, sf::Color hover, sf::Color pressed);

    // Positionnement du bouton
    void setPosition(sf::Vector2f pos);

    // Ajuste l'origine du texte pour le centrer dans le bouton
    void centerText();

    // Met à jour l'état visuel du bouton en fonction de la position de la souris
    void update(const sf::Vector2f& mousePos);

    // Vérifie si le bouton est cliqué (doit être appelé lors d'un événement MouseButtonPressed)
    bool isClicked(const sf::Event& event);

    // Dessine le bouton sur la fenêtre
    void draw(sf::RenderWindow& window) const;

    // Accesseur pour les limites globales du bouton (utile pour le clic)
    sf::FloatRect getGlobalBounds() const;
};

} // namespace Jr