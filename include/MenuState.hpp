#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp" // Reste nécessaire pour PlayingState
#include "Button.hpp" // NOUVEAU : Inclure la classe Button
#include <vector>
#include <memory> // Pour std::unique_ptr pour les boutons

namespace Jr {

/**
 * @brief Représente l'état du menu principal du jeu.
 * Permet au joueur de choisir entre Jouer, Aide et Auteur.
 */
class MenuState : public GameState {
private:
    sf::Text titleText;
    // Utilisation de std::vector de unique_ptr pour gérer les boutons
    std::vector<std::unique_ptr<Button>> menuButtons;
    
    FontManager& fontManager;
    TextureManager& textureManager; // Reste nécessaire pour passer à PlayingState

    void setupButtons(); // Renommé de setupMenuItems
    void centerText(sf::Text& text, float yOffset); // Utile pour le titre

public:
    MenuState(StateManager& manager, sf::RenderWindow& win, FontManager& fm, TextureManager& tm);
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
    void onEnter() override;
};

} // namespace Jr