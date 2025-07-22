#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <array>
#include <map>
#include <string>
#include <vector>

namespace Jr {
    class Board {
        private:
            std::array<std::array<sf::RectangleShape, 8>, 8>    boxes;
            std::array<sf::Text, 32>                            labels;
            std::map<std::string, sf::Texture>                  pieceTextures;
            std::vector<sf::Sprite>                             pieceSprites;
            sf::Font                                            font;
            std::map<std::string, uint64_t>                     bitboards; 
            uint64_t                                            bitboardPieces;
            
            int                                                 selectedSquare = -1;
            std::vector<int>                                    highlightedSquares;
            bool                                                whiteTurn = true;
            int                                                 enPassantSquare = -1;

            // Variables pour savoirs la possibilité de roquer
            bool                                                whiteKingMoved             =    false;
            bool                                                whiteRookKingsideMoved     =    false;
            bool                                                whiteRookQueensideMoved    =    false;

            bool                                                blackKingMoved             =    false;
            bool                                                blackRookKingsideMoved     =    false;
            bool                                                blackRookQueensideMoved    =    false;

            // Variables pour la promotion des pièces
            bool                                                promotionPending           = false;
            bool                                                promotionWhite             = true;
            int                                                 promotionSquare            = -1;
            std::vector<sf::Sprite>                             promotionChoices;

            void                                                loadTextures();
            void                                                updateSpritesFromBitboards();
            bool                                                movePiece(int from, int to);
            std::vector<int>                                    getLegalMoves(const std::string& pieceName, int from, bool forCheck = false);

            bool                                                isKingInCheck(bool whiteKing);
            bool                                                wouldBeInCheck(int from, int to, bool whiteKing);

        public:
            Board();
            void                                                draw(sf::RenderWindow& window);
            void                                                handleClick(int mouseX, int mouseY);
    };
}

#endif
