#pragma once

#include "GameState.hpp"
#include "Board.hpp"
#include "ChessLogic.hpp"
#include "TextureManager.hpp"
#include "FontManager.hpp"

namespace Jr {

/**
 * @class PlayingState
 * @brief État du jeu représentant une partie d'échecs en cours.
 * 
 * Cette classe dérive de GameState et gère la logique et l'affichage
 * du plateau d'échecs pendant une partie active. Elle coordonne les
 * interactions utilisateur, la mise à jour de la logique de jeu,
 * et le rendu graphique via les gestionnaires de textures et polices.
 */
class PlayingState : public GameState {
private:
    /// Référence au gestionnaire de textures partagé pour charger et dessiner les sprites
    TextureManager& textureManager;

    /// Référence au gestionnaire de polices partagé pour gérer les polices de texte
    FontManager& fontManager;

    /// Instance de la logique d'échecs propre à cette partie
    ChessLogic chessLogic;

    /// Instance du plateau graphique associé à la partie
    Board board;

public:
    /**
     * @brief Constructeur de PlayingState.
     * 
     * Initialise les références aux gestionnaires, crée la logique et le plateau,
     * et prépare l'état pour une nouvelle partie.
     * 
     * @param manager Référence au StateManager pour gérer les transitions d'état.
     * @param win Référence à la fenêtre SFML pour le rendu.
     * @param tm Référence au gestionnaire de textures.
     * @param fm Référence au gestionnaire de polices.
     */
    PlayingState(StateManager& manager, sf::RenderWindow& win, TextureManager& tm, FontManager& fm);

    /// Destructeur virtuel par défaut.
    ~PlayingState() override = default;

    /**
     * @brief Traite les entrées utilisateur spécifiques à la partie en cours.
     * 
     * Gère les clics, mouvements de souris, et autres événements.
     * 
     * @param event Événement SFML à traiter.
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Met à jour la logique du jeu (animations, état des pièces, règles).
     * 
     * @param deltaTime Temps écoulé depuis la dernière mise à jour (en secondes).
     */
    void update(float deltaTime) override;

    /**
     * @brief Dessine le plateau et tous les éléments graphiques liés à la partie.
     */
    void draw() override;

    /**
     * @brief Méthode appelée lors de l'entrée dans l'état PlayingState.
     * 
     * Permet d'initialiser ou réinitialiser la partie, le plateau et la logique.
     */
    void onEnter() override;
};

} // namespace Jr
