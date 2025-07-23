#pragma once
#include <string>
#include <utility>

namespace Jr {

    // Enumérations pour le type et la couleur de la pièce
    enum class PieceType { Pawn, Knight, Bishop, Rook, Queen, King, None };
    enum class PieceColor { White, Black, None };

    // Structure pour représenter une pièce logique (non graphique)
    struct Piece {
        PieceType type = PieceType::None;
        PieceColor color = PieceColor::None;

        // Constructeur par défaut
        Piece() = default;

        // Constructeur avec type et couleur
        Piece(PieceType t, PieceColor c) : type(t), color(c) {}

        // Vérifie si la pièce est "vide"
        bool isEmpty() const {
            return type == PieceType::None || color == PieceColor::None;
        }

        // Retourne le nom de fichier de la texture pour cette pièce
        std::string getTextureFileName() const {
            if (isEmpty()) return "";

            std::string s = "";
            s += (color == PieceColor::White ? 'w' : 'b');
            switch (type) {
                case PieceType::Pawn:   s += 'P'; break;
                case PieceType::Knight: s += 'N'; break;
                case PieceType::Bishop: s += 'B'; break;
                case PieceType::Rook:   s += 'R'; break;
                case PieceType::Queen:  s += 'Q'; break;
                case PieceType::King:   s += 'K'; break;
                default: return ""; // Ne devrait pas arriver
            }
            return s + ".png";
        }
        
        // Retourne le nom de la pièce (ex: "wP", "bK")
        std::string getName() const {
            if (isEmpty()) return "";
            std::string s = "";
            s += (color == PieceColor::White ? 'w' : 'b');
            switch (type) {
                case PieceType::Pawn:   s += 'P'; break;
                case PieceType::Knight: s += 'N'; break;
                case PieceType::Bishop: s += 'B'; break;
                case PieceType::Rook:   s += 'R'; break;
                case PieceType::Queen:  s += 'Q'; break;
                case PieceType::King:   s += 'K'; break;
                default: return "";
            }
            return s;
        }
    };
}