#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp" // Reste nécessaire pour PlayingState
#include "Button.hpp" // NOUVEAU : Inclure la classe Button
#include <vector>
#include <memory> // Pour std::unique_ptr pour les boutons

namespace Jr {

/**
 * @class MenuState
 * @brief Représente l'état du menu principal du jeu.
 * 
 * Cette classe dérive de GameState et gère l'affichage du menu principal, 
 * qui permet au joueur de sélectionner une option parmi Jouer, Aide, Auteur, etc.
 * 
 * Elle gère un titre et un ensemble de boutons interactifs.
 */
class MenuState : public GameState {
private:
    /// Texte affiché en haut du menu, généralement le titre du jeu
    sf::Text titleText;

    /// Liste des boutons du menu, gérés via des pointeurs uniques
    std::vector<std::unique_ptr<Button>> menuButtons;
    
    /// Référence au gestionnaire de polices pour l'affichage du texte
    FontManager& fontManager;

    /// Référence au gestionnaire de textures, utilisé notamment pour passer à PlayingState
    TextureManager& textureManager;

    /**
     * @brief Initialise les boutons du menu avec leurs positions, tailles et callbacks.
     */
    void setupButtons();

    /**
     * @brief Centre horizontalement un texte sur la fenêtre, avec un décalage vertical donné.
     * @param text Texte SFML à centrer.
     * @param yOffset Décalage vertical en pixels depuis le haut.
     */
    void centerText(sf::Text& text, float yOffset);

public:
    /**
     * @brief Constructeur du MenuState.
     * 
     * Initialise la fenêtre, le gestionnaire d'états, les gestionnaires de polices et textures,
     * et prépare les éléments graphiques du menu.
     * 
     * @param manager Référence au StateManager pour gérer les transitions d'états.
     * @param win Référence à la fenêtre SFML utilisée pour le rendu.
     * @param fm Référence au gestionnaire de polices.
     * @param tm Référence au gestionnaire de textures.
     */
    MenuState(StateManager& manager, sf::RenderWindow& win, FontManager& fm, TextureManager& tm);

    /// Destructeur virtuel par défaut.
    ~MenuState() override = default;

    /**
     * @brief Traite les entrées utilisateur (clavier, souris, etc.) spécifiques à cet état.
     * @param event Événement SFML reçu.
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Met à jour la logique du menu (animations, états des boutons, etc.).
     * @param deltaTime Temps écoulé depuis la dernière mise à jour (en secondes).
     */
    void update(float deltaTime) override;

    /**
     * @brief Dessine le menu principal (titre, boutons) sur la fenêtre.
     */
    void draw() override;

    /**
     * @brief Méthode appelée lors de l'activation de cet état.
     * Permet d'initialiser ou de réinitialiser les éléments du menu.
     */
    void onEnter() override;
};

} // namespace Jr
