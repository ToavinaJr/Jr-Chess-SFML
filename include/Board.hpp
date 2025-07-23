#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>

#include "constants.hpp"
#include "TextureManager.hpp"
#include "FontManager.hpp"
#include "ChessLogicManager.hpp" // Board a besoin de la logique pour savoir quoi dessiner

namespace Jr {

    class Board {
    private:
        // Références aux managers
        TextureManager& textureManager;
        FontManager& fontManager;
        ChessLogic& chessLogic; // Référence au manager de logique

        // Éléments graphiques statiques (le plateau, les labels)
        std::array<std::array<sf::RectangleShape, 8>, 8> boxes;
        std::array<sf::Text, 32> labels; // Pour les coordonnées A-H, 1-8

        // Éléments graphiques dynamiques (les sprites des pièces, les highlight)
        std::vector<sf::Sprite> pieceSprites; // Sprites des pièces actuellement sur le plateau
        
        // États graphiques pour l'interaction utilisateur
        int selectedSquare = -1; // Case sélectionnée par le joueur
        std::vector<int> highlightedSquares; // Cases légales à surligner

        // Éléments pour la promotion
        std::vector<sf::Sprite> promotionChoicesSprites;
        sf::RectangleShape promotionFrame;

        // Méthodes privées pour le rendu
        void setupBoardSquares(); // Configure les sf::RectangleShape pour le plateau
        void setupLabels(); // Configure les sf::Text pour les coordonnées
        void updatePieceSprites(); // Met à jour pieceSprites en fonction de ChessLogic
        void preparePromotionDisplay(); // Prépare l'affichage des choix de promotion

    public:
        // Le constructeur prend les références aux managers nécessaires
        Board(TextureManager& tm, FontManager& fm, ChessLogic& cl);

        // Méthodes publiques d'interaction
        void draw(sf::RenderWindow& window);
        void handleMouseClick(int mouseX, int mouseY); // Gère le clic de souris

        // Getters pour l'état graphique (si d'autres classes en ont besoin)
        int getSelectedSquare() const { return selectedSquare; }
        const std::vector<int>& getHighlightedSquares() const { return highlightedSquares; }
    };
}