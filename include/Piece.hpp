#pragma once
#include <string>
#include <utility>

namespace Jr {

/**
 * @enum PieceType
 * @brief Enumération représentant le type d'une pièce d'échecs.
 */
enum class PieceType { 
    Pawn,   /**< Pion */
    Knight, /**< Cavalier */
    Bishop, /**< Fou */
    Rook,   /**< Tour */
    Queen,  /**< Dame */
    King,   /**< Roi */
    None    /**< Aucune pièce (case vide) */
};

/**
 * @enum PieceColor
 * @brief Enumération représentant la couleur d'une pièce d'échecs.
 */
enum class PieceColor { 
    White, /**< Pièce blanche */
    Black, /**< Pièce noire */
    None   /**< Aucune couleur (case vide) */
};

/**
 * @struct Piece
 * @brief Structure représentant une pièce d'échecs logique (sans aspects graphiques).
 * 
 * Contient le type et la couleur de la pièce, avec quelques méthodes utilitaires.
 */
struct Piece {
    /// Type de la pièce (Pion, Cavalier, etc.)
    PieceType type = PieceType::None;

    /// Couleur de la pièce (Blanc, Noir, ou None)
    PieceColor color = PieceColor::None;

    /**
     * @brief Constructeur par défaut, crée une pièce vide.
     */
    Piece() = default;

    /**
     * @brief Constructeur avec initialisation du type et de la couleur.
     * @param t Type de la pièce
     * @param c Couleur de la pièce
     */
    Piece(PieceType t, PieceColor c) : type(t), color(c) {}

    /**
     * @brief Indique si cette pièce est vide (pas de pièce).
     * @return true si la pièce est vide, false sinon.
     */
    bool isEmpty() const {
        return type == PieceType::None || color == PieceColor::None;
    }

    /**
     * @brief Retourne le nom du fichier de texture associé à la pièce.
     * 
     * Le nom est composé d'un préfixe 'w' ou 'b' selon la couleur, suivi d'une lettre 
     * représentant le type (P, N, B, R, Q, K), et de l'extension ".png".
     * 
     * Par exemple : "wP.png" pour un pion blanc, "bK.png" pour un roi noir.
     * 
     * @return Nom du fichier texture ou chaîne vide si pièce vide.
     */
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
            default: return "";
        }
        return s + ".png";
    }
    
    /**
     * @brief Retourne une chaîne représentant la pièce, par exemple "wP" ou "bK".
     * 
     * Utile pour identifier la pièce dans des maps ou pour le débogage.
     * 
     * @return Nom court de la pièce ou chaîne vide si pièce vide.
     */
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
#pragma once
#include <string>
#include <utility>

namespace Jr {

/**
 * @enum PieceType
 * @brief Enumération représentant le type d'une pièce d'échecs.
 */
enum class PieceType { 
    Pawn,   /**< Pion */
    Knight, /**< Cavalier */
    Bishop, /**< Fou */
    Rook,   /**< Tour */
    Queen,  /**< Dame */
    King,   /**< Roi */
    None    /**< Aucune pièce (case vide) */
};

/**
 * @enum PieceColor
 * @brief Enumération représentant la couleur d'une pièce d'échecs.
 */
enum class PieceColor { 
    White, /**< Pièce blanche */
    Black, /**< Pièce noire */
    None   /**< Aucune couleur (case vide) */
};

/**
 * @struct Piece
 * @brief Structure représentant une pièce d'échecs logique (sans aspects graphiques).
 * 
 * Contient le type et la couleur de la pièce, avec quelques méthodes utilitaires.
 */
struct Piece {
    /// Type de la pièce (Pion, Cavalier, etc.)
    PieceType type = PieceType::None;

    /// Couleur de la pièce (Blanc, Noir, ou None)
    PieceColor color = PieceColor::None;

    /**
     * @brief Constructeur par défaut, crée une pièce vide.
     */
    Piece() = default;

    /**
     * @brief Constructeur avec initialisation du type et de la couleur.
     * @param t Type de la pièce
     * @param c Couleur de la pièce
     */
    Piece(PieceType t, PieceColor c) : type(t), color(c) {}

    /**
     * @brief Indique si cette pièce est vide (pas de pièce).
     * @return true si la pièce est vide, false sinon.
     */
    bool isEmpty() const {
        return type == PieceType::None || color == PieceColor::None;
    }

    /**
     * @brief Retourne le nom du fichier de texture associé à la pièce.
     * 
     * Le nom est composé d'un préfixe 'w' ou 'b' selon la couleur, suivi d'une lettre 
     * représentant le type (P, N, B, R, Q, K), et de l'extension ".png".
     * 
     * Par exemple : "wP.png" pour un pion blanc, "bK.png" pour un roi noir.
     * 
     * @return Nom du fichier texture ou chaîne vide si pièce vide.
     */
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
            default: return "";
        }
        return s + ".png";
    }
    
    /**
     * @brief Retourne une chaîne représentant la pièce, par exemple "wP" ou "bK".
     * 
     * Utile pour identifier la pièce dans des maps ou pour le débogage.
     * 
     * @return Nom court de la pièce ou chaîne vide si pièce vide.
     */
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
