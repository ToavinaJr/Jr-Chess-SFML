#pragma once
#include "ChessLogic.hpp"
#include "Piece.hpp"
#include <utility>

namespace Jr {

struct AIMove {
    int from = -1;
    int to = -1;
    int score = 0;
};

/**
 * @class AIPlayer
 * @brief Moteur d'IA pour jouer aux échecs avec algorithme Minimax et élagage alpha-bêta
 */
class AIPlayer {
public:
    explicit AIPlayer(int depth = 3);

    // Trouve le meilleur coup pour la position donnée
    AIMove findBestMove(const ChessLogic& logic);

    void setDepth(int d) { maxDepth = d; }
    int getDepth() const { return maxDepth; }

private:
    int maxDepth;

    AIMove minimax(ChessLogic node, int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluate(const ChessLogic& logic) const;
    int pieceValue(PieceType t) const;
    
    // Simule un coup et gère automatiquement la promotion
    static void simulateMoveAndResolve(ChessLogic& logicCopy, int from, int to);
};

} // namespace Jr
