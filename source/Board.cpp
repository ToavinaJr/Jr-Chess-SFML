#include "../include/Board.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

/**
 * @class Board
 * @brief Gère l'affichage graphique du plateau d'échecs et les interactions utilisateur.
 */

/**
 * @brief Constructeur du plateau.
 * @param tm Gestionnaire des textures.
 * @param fm Gestionnaire des polices.
 * @param cl Logique du jeu d'échecs.
 */
Board::Board(TextureManager& tm, FontManager& fm, ChessLogic& cl)
    : textureManager(tm), fontManager(fm), chessLogic(cl) {

    fontManager.getFont(FONT_PATH);
    setupBoardSquares();
    setupLabels();
    updatePieceSprites();
}

/**
 * @brief Configure les cases du plateau (taille, position, couleurs alternées).
 */
void Board::setupBoardSquares() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            boxes[i][j].setSize(sf::Vector2f(BOX_SIZE, BOX_SIZE));
            boxes[i][j].setPosition(MARGIN + j * BOX_SIZE, MARGIN + i * BOX_SIZE);
            boxes[i][j].setFillColor(((i + j) % 2 == 0) ? BOX_COLOR_LIGHT : BOX_COLOR_DARK);
        }
    }
}

/**
 * @brief Ajoute les labels des coordonnées (A-H, 1-8) autour du plateau.
 */
void Board::setupLabels() {
    int index = 0;
    const sf::Font& currentFont = fontManager.getFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    auto makeLabel = [&](const std::string& str, float x, float y,
                         bool center = true, float rotation = 0.f) {
        sf::Text text;
        text.setFont(currentFont);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);
        text.setString(str);
        if (center) {
            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height / 2.f);
        }
        text.setPosition(x, y);
        text.setRotation(rotation);
        return text;
    };

    for (int i = 0; i < 8; ++i) {
        labels[index++] = makeLabel(std::to_string(8 - i),
                                    MARGIN / 2.f, MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f);
        labels[index++] = makeLabel(std::to_string(8 - i),
                                    MARGIN + BOX_SIZE * 8 + MARGIN / 2.f,
                                    MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f, true, 180.f);
        labels[index++] = makeLabel(std::string(1, 'a' + i),
                                    MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                    MARGIN + BOX_SIZE * 8 + MARGIN / 4.f);
        labels[index++] = makeLabel(std::string(1, 'a' + i),
                                    MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                    MARGIN / 2.f, true, 180.f);
    }
}

/**
 * @brief Met à jour les sprites des pièces selon l'état actuel du jeu.
 * @details Parcourt l'état du plateau fourni par ChessLogic et place chaque sprite.
 */
void Board::updatePieceSprites() {
    pieceSprites.clear();
    auto currentBoardState = chessLogic.getCurrentBoardState();

    for (const auto& [square, piece] : currentBoardState) {
        if (piece.isEmpty()) continue;

        sf::Sprite sprite;
        const sf::Texture& tex = textureManager.getTexture(
            "../assets/images/pieces/chess_maestro_bw/" + piece.getTextureFileName());
        sprite.setTexture(tex);

        float scale = static_cast<float>(BOX_SIZE) / tex.getSize().x;
        sprite.setScale(scale, scale);

        int row = square / 8;
        int col = square % 8;
        sprite.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);

        pieceSprites.push_back(sprite);
    }
}

/**
 * @brief Prépare l'affichage des choix de promotion.
 */
void Board::preparePromotionDisplay() {
    promotionChoicesSprites.clear();

    auto pieceNames = chessLogic.getPromotionWhite()
        ? std::vector<std::string>{"wQ", "wR", "wB", "wN"}
        : std::vector<std::string>{"bQ", "bR", "bB", "bN"};

    float startX = WINDOW_WIDTH / 2.f - 2 * BOX_SIZE;
    float startY = WINDOW_HEIGHT / 2.f - BOX_SIZE / 2.f;

    promotionFrame.setSize(sf::Vector2f(4 * BOX_SIZE, BOX_SIZE));
    promotionFrame.setPosition(startX, startY);
    promotionFrame.setFillColor(sf::Color(200, 200, 0, 180));
    promotionFrame.setOutlineThickness(3.f);
    promotionFrame.setOutlineColor(sf::Color::Black);

    for (int i = 0; i < 4; i++) {
        sf::Sprite sprite;
        const sf::Texture& tex = textureManager.getTexture(
            "../assets/images/pieces/chess_maestro_bw/" + pieceNames[i] + ".png");
        sprite.setTexture(tex);
        sprite.setScale((float)BOX_SIZE / tex.getSize().x, (float)BOX_SIZE / tex.getSize().y);
        sprite.setPosition(startX + i * BOX_SIZE, startY);
        promotionChoicesSprites.push_back(sprite);
    }
}

/**
 * @brief Dessine le plateau, les pièces et les surbrillances.
 * @param window Fenêtre SFML où tout est dessiné.
 */
void Board::draw(sf::RenderWindow& window) {
    window.clear(BACKGROUND_COLOR);

    for (auto& row : boxes) for (auto& box : row) window.draw(box);
    for (auto& label : labels) window.draw(label);
    for (auto& sprite : pieceSprites) window.draw(sprite);

    // Surbrillance de la case sélectionnée
    if (selectedSquare != -1) {
        int row = selectedSquare / 8;
        int col = selectedSquare % 8;
        sf::RectangleShape highlight(sf::Vector2f(BOX_SIZE, BOX_SIZE));
        highlight.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
        highlight.setFillColor(sf::Color(255, 255, 0, 100));
        window.draw(highlight);
    }

    // Cercles pour les coups possibles
    for (int sq : highlightedSquares) {
        int row = sq / 8;
        int col = sq % 8;
        sf::CircleShape circle(BOX_SIZE / 6.f);
        circle.setFillColor(sf::Color(0, 255, 0, 120));
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setPosition(MARGIN + col * BOX_SIZE + BOX_SIZE / 2.f,
                           MARGIN + (7 - row) * BOX_SIZE + BOX_SIZE / 2.f);
        window.draw(circle);
    }

    // Roi en échec
    for (bool white : {true, false}) {
        if (!chessLogic.isKingInCheck(white)) continue;
        std::string kingName = white ? "wK" : "bK";
        for (auto const& [sq, piece] : chessLogic.getCurrentBoardState()) {
            if (piece.getName() != kingName) continue;
            int row = sq / 8, col = sq % 8;
            sf::RectangleShape check(sf::Vector2f(BOX_SIZE, BOX_SIZE));
            check.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
            check.setFillColor(sf::Color(255, 0, 0, 120));
            window.draw(check);
        }
    }

    if (chessLogic.isPromotionPending()) {
        window.draw(promotionFrame);
        for (auto& choice : promotionChoicesSprites) window.draw(choice);
    }
}

/**
 * @brief Gère les clics de souris pour sélectionner ou déplacer une pièce.
 * @param mouseX Coordonnée X de la souris.
 * @param mouseY Coordonnée Y de la souris.
 */
void Board::handleMouseClick(int mouseX, int mouseY) {
    int col = (mouseX - MARGIN) / BOX_SIZE;
    int row = 7 - ((mouseY - MARGIN) / BOX_SIZE);
    if (col < 0 || col >= 8 || row < 0 || row >= 8) return;

    int clickedSquare = row * 8 + col;

    // Gestion de la promotion
    if (chessLogic.isPromotionPending()) {
        for (int i = 0; i < (int)promotionChoicesSprites.size(); ++i) {
            if (!promotionChoicesSprites[i].getGlobalBounds().contains((float)mouseX, (float)mouseY)) continue;
            Jr::PieceType newType = (i == 0) ? Jr::PieceType::Queen :
                                    (i == 1) ? Jr::PieceType::Rook :
                                    (i == 2) ? Jr::PieceType::Bishop : Jr::PieceType::Knight;
            chessLogic.promotePawn(chessLogic.getPromotionSquare(), newType);
            updatePieceSprites();
            selectedSquare = -1;
            highlightedSquares.clear();
            return;
        }
        return;
    }

    // Sélection et déplacement classique
    if (selectedSquare == -1) {
        Jr::Piece pieceOnSquare = chessLogic.getPieceAtSquare(clickedSquare);
        if (!pieceOnSquare.isEmpty() &&
            ((pieceOnSquare.color == Jr::PieceColor::White && chessLogic.getWhiteTurn()) ||
             (pieceOnSquare.color == Jr::PieceColor::Black && !chessLogic.getWhiteTurn()))) {
            selectedSquare = clickedSquare;
            highlightedSquares = chessLogic.getLegalMoves(selectedSquare);
        }
    } else {
        if (chessLogic.makeMove(selectedSquare, clickedSquare)) {
            selectedSquare = -1;
            highlightedSquares.clear();
            updatePieceSprites();
            if (chessLogic.isPromotionPending()) preparePromotionDisplay();
        } else {
            Jr::Piece newPiece = chessLogic.getPieceAtSquare(clickedSquare);
            if (!newPiece.isEmpty() &&
                ((newPiece.color == Jr::PieceColor::White && chessLogic.getWhiteTurn()) ||
                 (newPiece.color == Jr::PieceColor::Black && !chessLogic.getWhiteTurn()))) {
                selectedSquare = clickedSquare;
                highlightedSquares = chessLogic.getLegalMoves(selectedSquare);
            } else {
                selectedSquare = -1;
                highlightedSquares.clear();
            }
        }
    }
}

void Board::clearSelection() {
    selectedSquare = -1;
    highlightedSquares.clear();
}

} // namespace Jr
