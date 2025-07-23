#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>
#include <cstdint> // Pour uint64_t
#include "Piece.hpp" // Inclure la nouvelle structure Piece

namespace Jr {

    // Forward declaration pour éviter les dépendances circulaires si ChessLogic a besoin de Board (peu probable après refactor)
    // class Board; 

    class ChessLogic {
    private:
        // Toutes les variables d'état du jeu qui étaient dans Board
        std::map<std::string, uint64_t> bitboards;
        uint64_t bitboardPieces; // Représente toutes les cases occupées

        // Etat du jeu
        bool whiteTurn = true;
        int enPassantSquare = -1; // Case cible d'une prise en passant possible, -1 si aucune

        // Variables pour le roque
        bool whiteKingMoved = false;
        bool whiteRookKingsideMoved = false;
        bool whiteRookQueensideMoved = false;
        bool blackKingMoved = false;
        bool blackRookKingsideMoved = false;
        bool blackRookQueensideMoved = false;

        // Variables pour la promotion
        bool promotionPending = false;
        bool promotionWhite = true;
        int promotionSquare = -1; // Case où la promotion a lieu

        // Méthodes privées de validation et de logique de mouvement
        bool isEmpty(int sq) const;
        bool isEnemy(int sq, bool whitePiece) const;
        
        // Fonction pour obtenir les mouvements bruts (sans vérifier les échecs)
        std::vector<int> getRawMoves(const Piece& piece, int from) const;
        
    public:
        Piece getPieceAtSquare(int square) const; // Aide à récupérer la pièce à une case donnée
        ChessLogic();
        void initializeBoard(); // Initialise les bitboards au début d'une partie

        // Méthodes publiques pour interagir avec la logique du jeu
        bool isValidMove(int from, int to) const; // Vérifie si un coup est légal (inclut les échecs)
        bool makeMove(int from, int to); // Effectue un coup et met à jour l'état du jeu
        
        std::vector<int> getLegalMoves(int from) const; // Retourne les cases légales pour une pièce donnée

        bool isKingInCheck(bool whiteKing) const;
        bool wouldBeInCheck(int from, int to, bool whiteKing) const;

        // Gestion du roque et de la prise en passant
        void updateCastlingRights(const std::string& pieceName, int from);
        void handleEnPassantCapture(const std::string& movingPiece, int to);
        void resetEnPassantSquare(const std::string& movingPiece, int from, int to);
        void handleCastlingMove(const std::string& movingPiece, int from, int to);

        // Getters pour l'état du jeu
        bool getWhiteTurn() const { return whiteTurn; }
        bool isPromotionPending() const { return promotionPending; }
        bool getPromotionWhite() const { return promotionWhite; }
        int getPromotionSquare() const { return promotionSquare; }
        
        // Méthode pour la promotion
        void promotePawn(int square, PieceType newType);

        // Obtenir l'état complet du plateau pour l'affichage (map de Piece pour chaque case)
        std::map<int, Piece> getCurrentBoardState() const;
    };
}