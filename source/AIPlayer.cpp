#include "../include/AIPlayer.hpp"
#include <limits>
#include <algorithm>

namespace Jr {

AIPlayer::AIPlayer(int depth) : maxDepth(depth) {}

AIMove AIPlayer::findBestMove(const ChessLogic& logic) {
    ChessLogic copy = logic;
    bool maximizing = copy.getWhiteTurn(); // Si c'est aux blancs de jouer, maximiser
    return minimax(copy, maxDepth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), maximizing);
}

AIMove AIPlayer::minimax(ChessLogic node, int depth, int alpha, int beta, bool maximizingPlayer) {
    AIMove best;
    
    // Nœud terminal ou profondeur 0
    if (depth == 0) {
        best.score = evaluate(node);
        return best;
    }

    // Générer tous les coups légaux
    bool toMoveIsWhite = node.getWhiteTurn();
    int bestScore = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (int from = 0; from < 64; ++from) {
        Piece p = node.getPieceAtSquare(from);
        if (p.isEmpty()) continue;
        if ((p.color == PieceColor::White) != toMoveIsWhite) continue;

        auto moves = node.getLegalMoves(from);
        for (int to : moves) {
            ChessLogic child = node;
            simulateMoveAndResolve(child, from, to);

            // Si promotion en attente, promouvoir en dame
            if (child.isPromotionPending()) {
                child.promotePawn(child.getPromotionSquare(), PieceType::Queen);
            }

            // Évaluer récursivement
            AIMove reply = minimax(child, depth - 1, alpha, beta, !maximizingPlayer);
            int score = reply.score;

            if (maximizingPlayer) {
                if (score > bestScore) {
                    bestScore = score;
                    best.from = from;
                    best.to = to;
                    best.score = score;
                }
                alpha = std::max(alpha, bestScore);
                if (beta <= alpha) break; // Coupure bêta
            } else {
                if (score < bestScore) {
                    bestScore = score;
                    best.from = from;
                    best.to = to;
                    best.score = score;
                }
                beta = std::min(beta, bestScore);
                if (beta <= alpha) break; // Coupure alpha
            }
        }
        if (beta <= alpha) break;
    }

    // Si aucun coup trouvé (mat/pat), évaluer la position
    if (best.from == -1 && best.to == -1) {
        best.score = evaluate(node);
    }
    return best;
}

void AIPlayer::simulateMoveAndResolve(ChessLogic& logicCopy, int from, int to) {
    logicCopy.makeMove(from, to);
    if (logicCopy.isPromotionPending()) {
        logicCopy.promotePawn(logicCopy.getPromotionSquare(), PieceType::Queen);
    }
}

int AIPlayer::evaluate(const ChessLogic& logic) const {
    // Évaluation simple : matériel + bonus mobilité
    auto board = logic.getCurrentBoardState();
    int material = 0;
    
    for (const auto& [sq, piece] : board) {
        int val = pieceValue(piece.type);
        material += (piece.color == PieceColor::White) ? val : -val;
    }

    // Mobilité : nombre de coups légaux
    int mobility = 0;
    bool whiteToMove = logic.getWhiteTurn();
    
    for (int from = 0; from < 64; ++from) {
        Piece p = logic.getPieceAtSquare(from);
        if (p.isEmpty()) continue;
        
        auto list = logic.getLegalMoves(from);
        int moveCount = static_cast<int>(list.size());
        
        if (p.color == PieceColor::White) {
            mobility += whiteToMove ? moveCount : -moveCount;
        } else {
            mobility += whiteToMove ? -moveCount : moveCount;
        }
    }

    // Bonus de position pour les pions centraux
    int positional = 0;
    for (const auto& [sq, piece] : board) {
        if (piece.type == PieceType::Pawn) {
            int file = sq % 8;
            int rank = sq / 8;
            // Bonus pour pions centraux
            if (file >= 2 && file <= 5) {
                int bonus = (piece.color == PieceColor::White) ? rank : (7 - rank);
                positional += (piece.color == PieceColor::White) ? bonus : -bonus;
            }
        }
    }

    // Score combiné avec pondérations
    int score = material * 100 + mobility * 3 + positional * 2;
    return score;
}

int AIPlayer::pieceValue(PieceType t) const {
    switch (t) {
        case PieceType::Pawn:   return 1;
        case PieceType::Knight: return 3;
        case PieceType::Bishop: return 3;
        case PieceType::Rook:   return 5;
        case PieceType::Queen:  return 9;
        case PieceType::King:   return 0;
        default: return 0;
    }
}

} // namespace Jr
