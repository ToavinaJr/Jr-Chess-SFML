#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>
#include <cstdint> // Pour uint64_t
#include "Piece.hpp" // Inclure la nouvelle structure Piece

namespace Jr {

    /**
     * @class ChessLogic
     * @brief Gestion complète de la logique d'un jeu d'échecs.
     * 
     * Cette classe maintient l'état interne du plateau sous forme de bitboards,
     * gère le tour des joueurs, les règles spéciales (roque, prise en passant, promotion)
     * et valide les coups selon les règles officielles.
     * 
     * Elle fournit des méthodes pour interagir avec la logique du jeu, comme vérifier la validité
     * d'un coup, générer les coups légaux pour une pièce donnée, et appliquer un mouvement.
     */
    class ChessLogic {
    private:
        /// Bitboards représentant la position de chaque type de pièce, identifiées par une chaîne (ex: "wP" pour pions blancs)
        std::map<std::string, uint64_t> bitboards;

        /// Bitboard représentant toutes les cases occupées par une pièce (blanche ou noire)
        uint64_t bitboardPieces;

        /// Indique si c'est au tour des blancs de jouer (true) ou des noirs (false)
        bool whiteTurn = true;

        /// Index de la case cible d'une prise en passant possible, ou -1 si aucune
        int enPassantSquare = -1;

        /// Indique si le roi blanc a déjà bougé (affecte la possibilité de roque)
        bool whiteKingMoved = false;

        /// Indique si la tour blanche côté roi a déjà bougé
        bool whiteRookKingsideMoved = false;

        /// Indique si la tour blanche côté dame a déjà bougé
        bool whiteRookQueensideMoved = false;

        /// Indique si le roi noir a déjà bougé
        bool blackKingMoved = false;

        /// Indique si la tour noire côté roi a déjà bougé
        bool blackRookKingsideMoved = false;

        /// Indique si la tour noire côté dame a déjà bougé
        bool blackRookQueensideMoved = false;

        /// Indique si une promotion de pion est en attente
        bool promotionPending = false;

        /// Indique si la promotion concerne un pion blanc (true) ou noir (false)
        bool promotionWhite = true;

        /// Case où la promotion doit avoir lieu (index), -1 si aucune promotion en attente
        int promotionSquare = -1;

        /**
         * @brief Vérifie si la case spécifiée est vide.
         * @param sq Index de la case (0-63)
         * @return true si la case est vide, false sinon.
         */
        bool isEmpty(int sq) const;

        /**
         * @brief Vérifie si la pièce à la case donnée est ennemie par rapport à la couleur spécifiée.
         * @param sq Index de la case (0-63)
         * @param whitePiece true si la pièce de référence est blanche, false si noire
         * @return true si la pièce sur la case est adverse, false sinon.
         */
        bool isEnemy(int sq, bool whitePiece) const;
        
        /**
         * @brief Génère les déplacements "bruts" possibles pour une pièce à une case donnée.
         * 
         * Ces mouvements ne tiennent pas compte de la mise en échec du roi ni d'autres règles avancées.
         * 
         * @param piece La pièce concernée.
         * @param from La case d'origine (0-63).
         * @return Vecteur d'indices de cases accessibles.
         */
        std::vector<int> getRawMoves(const Piece& piece, int from) const;
        
    public:
        /**
         * @brief Obtient la pièce située à une case donnée.
         * @param square Index de la case (0-63)
         * @return Objet Piece représentant la pièce, ou une pièce "vide" si aucune.
         */
        Piece getPieceAtSquare(int square) const;

        /**
         * @brief Constructeur par défaut.
         * Initialise l'état interne pour une partie fraîche.
         */
        ChessLogic();

        /**
         * @brief Initialise la position de départ du plateau (placement initial des pièces).
         * Réinitialise aussi toutes les variables d'état (roque, promotion, etc).
         */
        void initializeBoard();

        /**
         * @brief Vérifie si un mouvement est valide selon les règles du jeu.
         * 
         * Cette méthode valide la légalité complète, y compris la non-mise en échec du roi.
         * 
         * @param from Case d'origine du mouvement (0-63).
         * @param to Case de destination (0-63).
         * @return true si le mouvement est légal, false sinon.
         */
        bool isValidMove(int from, int to) const;

        /**
         * @brief Effectue un coup valide et met à jour l'état du jeu.
         * 
         * Met à jour les bitboards, le tour, les droits au roque, prise en passant, promotion, etc.
         * 
         * @param from Case d'origine (0-63).
         * @param to Case de destination (0-63).
         * @return true si le mouvement a été effectué avec succès, false sinon (ex: coup illégal).
         */
        bool makeMove(int from, int to);
        
        /**
         * @brief Retourne la liste des coups légaux pour la pièce située à la case donnée.
         * @param from Case de la pièce (0-63).
         * @return Vecteur des cases accessibles légalement.
         */
        std::vector<int> getLegalMoves(int from) const;

        /**
         * @brief Indique si le roi de la couleur spécifiée est en échec.
         * @param whiteKing true pour roi blanc, false pour roi noir.
         * @return true si le roi est en échec, false sinon.
         */
        bool isKingInCheck(bool whiteKing) const;

        /**
         * @brief Vérifie si un coup hypothétique mettrait le roi en échec.
         * 
         * Utilisé pour valider la légalité des coups.
         * 
         * @param from Case d'origine (0-63).
         * @param to Case de destination (0-63).
         * @param whiteKing Couleur du roi à protéger (true = blanc).
         * @return true si le roi serait en échec après ce coup, false sinon.
         */
        bool wouldBeInCheck(int from, int to, bool whiteKing) const;

        /**
         * @brief Obtient le tour actuel.
         * @return true si c'est le tour des blancs, false pour les noirs.
         */
        bool getWhiteTurn() const { return whiteTurn; }

        /**
         * @brief Indique si une promotion est en attente de choix.
         * @return true si une promotion est en attente, false sinon.
         */
        bool isPromotionPending() const { return promotionPending; }

        /**
         * @brief Indique la couleur du pion en promotion.
         * @return true pour blanc, false pour noir.
         */
        bool getPromotionWhite() const { return promotionWhite; }

        /**
         * @brief Retourne la case où la promotion doit être effectuée.
         * @return Index de la case (0-63), -1 si aucune promotion en attente.
         */
        int getPromotionSquare() const { return promotionSquare; }
        
        /**
         * @brief Effectue la promotion d'un pion à une case donnée vers un nouveau type de pièce.
         * @param square Case où la promotion doit avoir lieu (0-63).
         * @param newType Type de pièce choisie pour la promotion (ex: Dame, Tour, Fou, Cavalier).
         */
        void promotePawn(int square, PieceType newType);

        /**
         * @brief Récupère l'état complet du plateau sous forme de map case -> pièce.
         * 
         * Utile pour l'affichage ou l'export du plateau.
         * 
         * @return Map des cases (0-63) associées à la pièce présente.
         */
        std::map<int, Piece> getCurrentBoardState() const;
    };
}
