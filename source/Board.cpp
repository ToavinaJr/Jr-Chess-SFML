#include "../include/Board.hpp"
#include "../include/constants.hpp"
#include <stdexcept>
#include <iostream>
#include <map>

namespace Jr {

    Board::Board() {
        // === Bitboard initial ===
        bitboardPieces = 0ULL;
        for (int i = 8; i < 16; i++) bitboardPieces |= (1ULL << i);
        for (int i = 48; i < 56; i++) bitboardPieces |= (1ULL << i);
        for (int i = 0; i < 8; i++) bitboardPieces |= (1ULL << i);
        for (int i = 56; i < 64; i++) bitboardPieces |= (1ULL << i);

        // === Bitboards individuels ===
        bitboards["wP"] = 0x000000000000FF00ULL;
        bitboards["wR"] = 0x0000000000000081ULL;
        bitboards["wN"] = 0x0000000000000042ULL;
        bitboards["wB"] = 0x0000000000000024ULL;
        bitboards["wQ"] = 0x0000000000000008ULL;
        bitboards["wK"] = 0x0000000000000010ULL;

        bitboards["bP"] = 0x00FF000000000000ULL;
        bitboards["bR"] = 0x8100000000000000ULL;
        bitboards["bN"] = 0x4200000000000000ULL;
        bitboards["bB"] = 0x2400000000000000ULL;
        bitboards["bQ"] = 0x0800000000000000ULL;
        bitboards["bK"] = 0x1000000000000000ULL;

        // === Police ===
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            throw std::runtime_error("Impossible de charger la police !");
        }

        // === Cases + légendes ===
        int index = 0;
        auto makeLabel = [](sf::Font& font, const std::string& str, float x, float y,
                            bool center = true, float rotation = 0.f) -> sf::Text {
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(16);
            text.setFillColor(sf::Color::Black);
            text.setString(str);
            if (center) {
                text.setOrigin(text.getLocalBounds().width / 2.f,
                               text.getLocalBounds().height / 2.f);
            }
            text.setPosition(x, y);
            text.setRotation(rotation);
            return text;
        };

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                boxes[i][j].setSize(sf::Vector2f(BOX_SIZE, BOX_SIZE));
                boxes[i][j].setPosition(MARGIN + j * BOX_SIZE, MARGIN + i * BOX_SIZE);
                boxes[i][j].setFillColor(((i + j) % 2 == 0) ? BOX_COLOR_LIGHT : BOX_COLOR_DARK);
            }

            labels[index++] = makeLabel(font, std::to_string(8 - i),
                                        MARGIN / 2.f, MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f);
            labels[index++] = makeLabel(font, std::to_string(8 - i),
                                        MARGIN + BOX_SIZE * 8 + MARGIN / 2.f,
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f, true, 180.f);
            labels[index++] = makeLabel(font, std::string(1, 'a' + i),
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                        MARGIN + BOX_SIZE * 8 + MARGIN / 4.f);
            labels[index++] = makeLabel(font, std::string(1, 'a' + i),
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                        MARGIN / 2.f, true, 180.f);
        }

        loadTextures();
        updateSpritesFromBitboards();
    }

    // === DESSIN ===
    void Board::draw(sf::RenderWindow& window) {
        window.clear(BACKGROUND_COLOR);

        for (auto& row : boxes)
            for (auto& box : row)
                window.draw(box);

        for (auto& label : labels)
            window.draw(label);

        for (auto& sprite : pieceSprites)
            window.draw(sprite);

        // === Case sélectionnée ===
        if (selectedSquare != -1) {
            int row = selectedSquare / 8;
            int col = selectedSquare % 8;

            sf::RectangleShape highlight(sf::Vector2f(BOX_SIZE, BOX_SIZE));
            highlight.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
            highlight.setFillColor(sf::Color(255, 255, 0, 100));
            window.draw(highlight);
        }

        // === Coups possibles ===
        for (int sq : highlightedSquares) {
            int row = sq / 8;
            int col = sq % 8;

            sf::CircleShape circle(BOX_SIZE / 6.f);
            circle.setFillColor(sf::Color(0, 255, 0, 120));
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(
                MARGIN + col * BOX_SIZE + BOX_SIZE / 2.f,
                MARGIN + (7 - row) * BOX_SIZE + BOX_SIZE / 2.f
            );
            window.draw(circle);
        }

        // === Roi en échec ===
        for (bool white : {true, false}) {
            if (isKingInCheck(white)) {
                std::string kingName = white ? "wK" : "bK";
                int sq = __builtin_ctzll(bitboards[kingName]);
                int row = sq / 8;
                int col = sq % 8;

                sf::RectangleShape highlight(sf::Vector2f(BOX_SIZE, BOX_SIZE));
                highlight.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
                highlight.setFillColor(sf::Color(255, 0, 0, 120));
                window.draw(highlight);
            }
        }

        if (promotionPending) {
            window.draw(promotionFrame);
            for (auto& choice : promotionChoices)
                window.draw(choice);
        }

    }

    void Board::preparePromotionChoices() {
        promotionChoices.clear();

        std::vector<std::string> pieces = promotionWhite
            ? std::vector<std::string>{"wQ", "wR", "wB", "wN"}
            : std::vector<std::string>{"bQ", "bR", "bB", "bN"};

        float startX = WINDOW_WIDTH / 2.f - 2 * BOX_SIZE;
        float startY = WINDOW_HEIGHT / 2.f - BOX_SIZE / 2.f;

        // === ✅ Cadre global pour toutes les pièces ===
        promotionFrame.setSize(sf::Vector2f(4 * BOX_SIZE, BOX_SIZE));
        promotionFrame.setPosition(startX, startY);
        promotionFrame.setFillColor(sf::Color(200, 200, 0, 180)); // Jaune semi-transparent
        promotionFrame.setOutlineThickness(3.f);
        promotionFrame.setOutlineColor(sf::Color::Black);

        // === ✅ Pièces à dessiner par-dessus ===
        for (int i = 0; i < 4; i++) {
            sf::Sprite sprite;
            sprite.setTexture(pieceTextures[pieces[i]]);
            sf::Vector2u texSize = pieceTextures[pieces[i]].getSize();
            sprite.setScale((float)BOX_SIZE / texSize.x, (float)BOX_SIZE / texSize.y);
            sprite.setPosition(startX + i * BOX_SIZE, startY);
            promotionChoices.push_back(sprite);
        }
    }


    std::vector<int> Board::getLegalMoves(const std::string& pieceName, int from, bool forCheck) {
        std::vector<int> moves;
        int row = from / 8;
        int col = from % 8;
        bool isWhite = (pieceName[0] == 'w');

        auto isEmpty = [&](int sq) {
            for (auto& [name, bb] : bitboards)
                if (bb & (1ULL << sq)) return false;
            return true;
        };

        auto isEnemy = [&](int sq, bool whitePiece) {
            for (auto& [name, bb] : bitboards)
                if (bb & (1ULL << sq)) return (name[0] == 'w') != whitePiece;
            return false;
        };

        auto addIfValid = [&](int r, int c, bool whitePiece) {
            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                int sq = r * 8 + c;
                if (isEmpty(sq) || isEnemy(sq, whitePiece)) moves.push_back(sq);
            }
        };

        // === PIONS ===
        if (pieceName == "wP" || pieceName == "bP") {
            int dir = isWhite ? 1 : -1;
            int startRow = isWhite ? 1 : 6;
            int fwd = (row + dir) * 8 + col;

            if (row + dir >= 0 && row + dir < 8 && isEmpty(fwd)) {
                moves.push_back(fwd);
                if (row == startRow) {
                    int fwd2 = (row + 2 * dir) * 8 + col;
                    if (isEmpty(fwd2)) moves.push_back(fwd2);
                }
            }

            for (int dc : {-1, 1}) {
                int r = row + dir, c = col + dc;
                if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                    int sq = r * 8 + c;
                    if (isEnemy(sq, isWhite)) moves.push_back(sq);
                    if (!forCheck && sq == enPassantSquare) moves.push_back(sq);
                }
            }
        }

        // === ROI ===
        if (pieceName[1] == 'K') {
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++)
                    if (!(dr == 0 && dc == 0)) addIfValid(row + dr, col + dc, isWhite);

            if (!forCheck && !isKingInCheck(isWhite)) {
                int backRank = isWhite ? 0 : 7;

                // Roque côté roi (court)
                if (isWhite) {
                    if (!whiteKingMoved && !whiteRookKingsideMoved) {
                        if (isEmpty(backRank * 8 + 5) && isEmpty(backRank * 8 + 6)) {
                            if (!wouldBeInCheck(from, backRank * 8 + 4, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 5, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 6, isWhite)) {
                                moves.push_back(backRank * 8 + 6);
                            }
                        }
                    }
                    // Roque côté dame (long)
                    if (!whiteKingMoved && !whiteRookQueensideMoved) {
                        if (isEmpty(backRank * 8 + 1) && isEmpty(backRank * 8 + 2) && isEmpty(backRank * 8 + 3)) {
                            if (!wouldBeInCheck(from, backRank * 8 + 4, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 3, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 2, isWhite)) {
                                moves.push_back(backRank * 8 + 2);
                            }
                        }
                    }
                } else {
                    // Noirs
                    if (!blackKingMoved && !blackRookKingsideMoved) {
                        if (isEmpty(backRank * 8 + 5) && isEmpty(backRank * 8 + 6)) {
                            if (!wouldBeInCheck(from, backRank * 8 + 4, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 5, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 6, isWhite)) {
                                moves.push_back(backRank * 8 + 6);
                            }
                        }
                    }
                    if (!blackKingMoved && !blackRookQueensideMoved) {
                        if (isEmpty(backRank * 8 + 1) && isEmpty(backRank * 8 + 2) && isEmpty(backRank * 8 + 3)) {
                            if (!wouldBeInCheck(from, backRank * 8 + 4, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 3, isWhite) &&
                                !wouldBeInCheck(from, backRank * 8 + 2, isWhite)) {
                                moves.push_back(backRank * 8 + 2);
                            }
                        }
                    }
                }
            }
        }

        // === CAVALIER, FOU, TOUR, DAME ===
        auto slide = [&](std::vector<std::pair<int,int>> dirs) {
            for (auto [dr, dc] : dirs) {
                int r = row + dr, c = col + dc;
                while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                    int sq = r * 8 + c;
                    if (isEmpty(sq)) moves.push_back(sq);
                    else { if (isEnemy(sq, isWhite)) moves.push_back(sq); break; }
                    r += dr; c += dc;
                }
            }
        };

        if (pieceName[1] == 'N') {
            for (auto [dr,dc] : std::vector<std::pair<int,int>>{
                {2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}})
                addIfValid(row+dr, col+dc, isWhite);
        }

        if (pieceName[1] == 'B') slide({{1,1},{1,-1},{-1,1},{-1,-1}});
        if (pieceName[1] == 'R') slide({{1,0},{-1,0},{0,1},{0,-1}});
        if (pieceName[1] == 'Q') slide({{1,1},{1,-1},{-1,1},{-1,-1},{1,0},{-1,0},{0,1},{0,-1}});

        if (!forCheck) {
            std::vector<int> filtered;
            for (int sq : moves)
                if (!wouldBeInCheck(from, sq, isWhite))
                    filtered.push_back(sq);
            return filtered;
        }
        return moves;
    }

    
    bool Board::isKingInCheck(bool whiteKing) {
        std::string kingName = whiteKing ? "wK" : "bK";
        if (!bitboards[kingName]) return false;
        int kingSquare = __builtin_ctzll(bitboards[kingName]);

        for (auto& [name, bbOriginal] : bitboards) {
            if (name == kingName) continue;
            bool isWhitePiece = (name[0] == 'w');
            if (isWhitePiece == whiteKing) continue;

            uint64_t bb = bbOriginal;
            while (bb) {
                int from = __builtin_ctzll(bb);
                bb &= bb - 1;
                auto moves = getLegalMoves(name, from, true); // ✅ mode simplifié
                for (int sq : moves) {
                    if (sq == kingSquare) return true;
                }
            }
        }
        return false;
    }

    void Board::updateSpritesFromBitboards() {
        pieceSprites.clear();

        for (const auto& [pieceName, bitboard] : bitboards) {
            uint64_t bb = bitboard;

            while (bb) {
                int square = __builtin_ctzll(bb);
                bb &= bb - 1;

                // ✅ Plus besoin de conversion, on utilise directement pieceName
                sf::Sprite sprite;
                sprite.setTexture(pieceTextures[pieceName]);

                // Scale automatique
                sf::Vector2u texSize = pieceTextures[pieceName].getSize();
                float scaleX = static_cast<float>(BOX_SIZE) / texSize.x;
                float scaleY = static_cast<float>(BOX_SIZE) / texSize.y;
                sprite.setScale(scaleX, scaleY);

                // Position correcte avec marge
                int row = square / 8;
                int col = square % 8;
                sprite.setPosition(
                    static_cast<float>(MARGIN + col * BOX_SIZE),
                    static_cast<float>(MARGIN + (7 - row) * BOX_SIZE)
                );

                pieceSprites.push_back(sprite);
            }
        }
    }

    bool Board::movePiece(int from, int to) {
        if (from < 0 || from >= 64 || to < 0 || to >= 64)
            return false;

        // Trouver la pièce à déplacer
        std::string movingPiece;
        for (auto& [pieceName, bb] : bitboards) {
            if (bb & (1ULL << from)) {
                movingPiece = pieceName;
                break;
            }
        }
        if (movingPiece.empty()) return false;

        // Vérifier que le coup est légal
        auto legalMoves = getLegalMoves(movingPiece, from);
        if (std::find(legalMoves.begin(), legalMoves.end(), to) == legalMoves.end())
            return false;

        // === Gestion de la prise en passant ===
        if ((movingPiece == "wP" || movingPiece == "bP") && to == enPassantSquare) {
            int dir = (movingPiece == "wP") ? -8 : 8;
            int capturedPawnSquare = to + dir;
            for (auto& [pieceName, bb] : bitboards) {
                if (bb & (1ULL << capturedPawnSquare)) {
                    bb &= ~(1ULL << capturedPawnSquare);
                    break;
                }
            }
        }

        // === Retirer la pièce capturée normalement ===
        for (auto& [pieceName, bb] : bitboards) {
            if (bb & (1ULL << to)) {
                bb &= ~(1ULL << to);
            }
        }

        // === Détection et gestion du roque ===
        if (movingPiece[1] == 'K') {
            int fromRow = from / 8;
            int fromCol = from % 8;
            int toRow = to / 8;
            int toCol = to % 8;

            // Roque côté roi (court)
            if (toCol == 6 && fromRow == toRow) {
                int rookFrom = fromRow * 8 + 7;
                int rookTo = fromRow * 8 + 5;

                std::string rookName = (movingPiece[0] == 'w') ? "wR" : "bR";
                bitboards[rookName] &= ~(1ULL << rookFrom);
                bitboards[rookName] |= (1ULL << rookTo);
            }
            // Roque côté dame (long)
            else if (toCol == 2 && fromRow == toRow) {
                int rookFrom = fromRow * 8 + 0;
                int rookTo = fromRow * 8 + 3;

                std::string rookName = (movingPiece[0] == 'w') ? "wR" : "bR";
                bitboards[rookName] &= ~(1ULL << rookFrom);
                bitboards[rookName] |= (1ULL << rookTo);
            }
        }

        // === Déplacer la pièce ===
        bitboards[movingPiece] &= ~(1ULL << from);
        bitboards[movingPiece] |= (1ULL << to);

        // === Mettre à jour enPassantSquare ===
        if (movingPiece == "wP" && from / 8 == 1 && to / 8 == 3) {
            enPassantSquare = from + 8;
        } else if (movingPiece == "bP" && from / 8 == 6 && to / 8 == 4) {
            enPassantSquare = from - 8;
        } else {
            enPassantSquare = -1;
        }

        // === Mettre à jour le flag "moved" du roi ou des tours ===
        if (movingPiece == "wK") whiteKingMoved = true;
        if (movingPiece == "bK") blackKingMoved = true;

        if (movingPiece == "wR") {
            if (from == 7) whiteRookKingsideMoved = true;
            else if (from == 0) whiteRookQueensideMoved = true;
        }
        if (movingPiece == "bR") {
            if (from == 63) blackRookKingsideMoved = true;
            else if (from == 56) blackRookQueensideMoved = true;
        }

        // === Détection de promotion ===
        if (movingPiece == "wP" && to / 8 == 7) {
            promotionPending = true;
            promotionWhite = true;
            promotionSquare = to;
            preparePromotionChoices();
        } else if (movingPiece == "bP" && to / 8 == 0) {
            promotionPending = true;
            promotionWhite = false;
            promotionSquare = to;
            preparePromotionChoices();
        }

        // Changer le tour
        if (!promotionPending) {
            whiteTurn = !whiteTurn;
        }
        

        // Mettre à jour les sprites
        updateSpritesFromBitboards();

        return true;
    }

    void Board::handleClick(int mouseX, int mouseY) {
        // Conversion des coordonnées de la souris en case (0..63)
        int col = (mouseX - MARGIN) / BOX_SIZE;
        int row = 7 - ((mouseY - MARGIN) / BOX_SIZE); // inversé pour SFML

        if (col < 0 || col >= 8 || row < 0 || row >= 8) return;

        int clickedSquare = row * 8 + col;

        if (promotionPending) {
            for (int i = 0; i < (int)promotionChoices.size(); i++) {
                if (promotionChoices[i].getGlobalBounds().contains(mouseX, mouseY)) {
                    // === Choix de la nouvelle pièce ===
                    std::string newPiece = promotionWhite
                        ? std::vector<std::string>{"wQ", "wR", "wB", "wN"}[i]
                        : std::vector<std::string>{"bQ", "bR", "bB", "bN"}[i];

                    std::string pawn = promotionWhite ? "wP" : "bP";

                    bitboards[pawn] &= ~(1ULL << promotionSquare);
                    bitboards[newPiece] |= (1ULL << promotionSquare);

                    promotionPending = false;
                    promotionChoices.clear();
                    promotionSquare = -1;

                    whiteTurn = !whiteTurn;

                    updateSpritesFromBitboards();
                    return;
                }
            }
            return; // ✅ On bloque tout autre clic tant que la promotion n'est pas finie
        }

        if (selectedSquare == -1) {
            // === Sélection d'une pièce ===
            std::string pieceOnSquare;
            for (const auto& [pieceName, bb] : bitboards) {
                if (bb & (1ULL << clickedSquare)) {
                    pieceOnSquare = pieceName;
                    break;
                }
            }

            if (!pieceOnSquare.empty()) {
                bool isWhitePiece = (pieceOnSquare[0] == 'w');
                if ((isWhitePiece && whiteTurn) || (!isWhitePiece && !whiteTurn)) {
                    selectedSquare = clickedSquare;
                    highlightedSquares = getLegalMoves(pieceOnSquare, selectedSquare);
                }
            }
        } else {
            // === Tentative de déplacement ===
            if (movePiece(selectedSquare, clickedSquare)) {
                // Déplacement réussi
                selectedSquare = -1;
                highlightedSquares.clear();
            } else {
                // Soit on change de sélection, soit on annule
                std::string pieceOnSquare;
                for (const auto& [pieceName, bb] : bitboards) {
                    if (bb & (1ULL << clickedSquare)) {
                        pieceOnSquare = pieceName;
                        break;
                    }
                }
                if (!pieceOnSquare.empty()) {
                    bool isWhitePiece = (pieceOnSquare[0] == 'w');
                    if ((isWhitePiece && whiteTurn) || (!isWhitePiece && !whiteTurn)) {
                        selectedSquare = clickedSquare;
                        highlightedSquares = getLegalMoves(pieceOnSquare, selectedSquare);
                    } else {
                        selectedSquare = -1;
                        highlightedSquares.clear();
                    }
                } else {
                    selectedSquare = -1;
                    highlightedSquares.clear();
                }
            }
        }
    }


    bool Board::wouldBeInCheck(int from, int to, bool whiteKing) {
        // Sauvegarder les bitboards originaux
        auto backupBitboards = bitboards;

        // Trouver la pièce à déplacer
        std::string movingPiece;
        for (const auto& [pieceName, bb] : bitboards) {
            if ((bb & (1ULL << from)) != 0) {
                movingPiece = pieceName;
                break;
            }
        }
        if (movingPiece.empty()) {
            // Pas de pièce à déplacer => ne devrait pas arriver
            return false;
        }

        // Simuler le déplacement dans les bitboards
        bitboards[movingPiece] &= ~(1ULL << from);

        // Enlever la pièce capturée, s'il y en a une sur 'to'
        for (auto& [pieceName, bb] : bitboards) {
            if ((bb & (1ULL << to)) != 0) {
                bb &= ~(1ULL << to);
            }
        }

        bitboards[movingPiece] |= (1ULL << to);

        // Tester si roi est en échec
        bool inCheck = isKingInCheck(whiteKing);

        // Restaurer les bitboards originaux
        bitboards = backupBitboards;

        return inCheck;
    }

    void Board::loadTextures() {
        std::map<std::string, std::string> pieceFiles = {
            {"wP", "wP.png"},
            {"wR", "wR.png"},
            {"wN", "wN.png"},
            {"wB", "wB.png"},
            {"wQ", "wQ.png"},
            {"wK", "wK.png"},
            {"bP", "bP.png"},
            {"bR", "bR.png"},
            {"bN", "bN.png"},
            {"bB", "bB.png"},
            {"bQ", "bQ.png"},
            {"bK", "bK.png"}
        };

        std::string basePath = "../assets/images/pieces/chess_maestro_bw/";

        for (const auto& [pieceName, fileName] : pieceFiles) {
            sf::Texture texture;
            if (!texture.loadFromFile(basePath + fileName)) {
                std::cerr << "Erreur chargement texture : " << basePath + fileName << std::endl;
            } else {
                pieceTextures[pieceName] = std::move(texture);
            }
        }
    }


}   
