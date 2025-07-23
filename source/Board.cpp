#include "../include/Board.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

    /**
     * @brief Constructeur de la classe Board.
     * * Initialise le plateau de jeu graphique, charge les textures et les polices,
     * et met en place les liens vers les gestionnaires de ressources et la logique du jeu.
     * * @param tm Référence au gestionnaire de textures.
     * @param fm Référence au gestionnaire de polices.
     * @param cl Référence à l'objet ChessLogic gérant la logique du jeu.
     */
    Board::Board(TextureManager& tm, FontManager& fm, ChessLogic& cl)
        : textureManager(tm), fontManager(fm), chessLogic(cl) {
        
        // Initialisation de la police (via FontManager).
        // Il est recommandé de précharger la police au début de l'application (e.g., dans main)
        // pour éviter des rechargements multiples si le chemin n'est pas mis en cache.
        fontManager.getFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        setupBoardSquares();  // Configure les formes SFML pour les cases du plateau.
        setupLabels();        // Configure les labels pour les coordonnées (A1-H8).

        // Met à jour les sprites des pièces après l'initialisation de la logique dans ChessLogic.
        updatePieceSprites();
    }

    /**
     * @brief Configure les objets sf::RectangleShape pour toutes les cases du plateau.
     * * Définit la taille, la position et la couleur de chaque case du plateau d'échecs.
     * Les couleurs alternent entre clair et sombre comme sur un échiquier standard.
     */
    void Board::setupBoardSquares() {
        for (int i = 0; i < 8; ++i) { // Itération sur les rangées (y)
            for (int j = 0; j < 8; ++j) { // Itération sur les colonnes (x)
                boxes[i][j].setSize(sf::Vector2f(BOX_SIZE, BOX_SIZE));
                // Positionnement des cases, en tenant compte de la marge
                boxes[i][j].setPosition(MARGIN + j * BOX_SIZE, MARGIN + i * BOX_SIZE);
                // Définition de la couleur de la case (alternance clair/sombre)
                boxes[i][j].setFillColor(((i + j) % 2 == 0) ? BOX_COLOR_LIGHT : BOX_COLOR_DARK);
            }
        }
    }

    /**
     * @brief Configure les labels textuels pour les coordonnées du plateau (A-H, 1-8).
     * * Crée des objets sf::Text pour afficher les rangées (1-8) et les colonnes (a-h)
     * autour du plateau, avec des options de centrage et de rotation.
     */
    void Board::setupLabels() {
        int index = 0; // Index pour stocker les labels dans le tableau `labels`
        // Récupère la police chargée via le FontManager.
        const sf::Font& currentFont = fontManager.getFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        
        /**
         * @brief Lambda fonction pour créer un objet sf::Text avec des paramètres communs.
         * * @param str La chaîne de caractères à afficher.
         * @param x La coordonnée X de la position du texte.
         * @param y La coordonnée Y de la position du texte.
         * @param center Indique si l'origine du texte doit être centrée.
         * @param rotation L'angle de rotation du texte en degrés.
         * @return Un objet sf::Text configuré.
         */
        auto makeLabel = [&](const std::string& str, float x, float y,
                            bool center = true, float rotation = 0.f) -> sf::Text {
            sf::Text text;
            text.setFont(currentFont);
            text.setCharacterSize(16);
            text.setFillColor(sf::Color::Black);
            text.setString(str);
            if (center) {
                // Centre l'origine du texte pour faciliter le positionnement
                text.setOrigin(text.getLocalBounds().width / 2.f,
                               text.getLocalBounds().height / 2.f);
            }
            text.setPosition(x, y);
            text.setRotation(rotation);
            return text;
        };

        // Création des labels pour les rangées et les colonnes
        for (int i = 0; i < 8; ++i) {
            // Labels des rangées sur le côté gauche (1-8)
            labels[index++] = makeLabel(std::to_string(8 - i),
                                        MARGIN / 2.f, MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f);
            // Labels des rangées sur le côté droit (1-8), inversés
            labels[index++] = makeLabel(std::to_string(8 - i),
                                        MARGIN + BOX_SIZE * 8 + MARGIN / 2.f,
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f, true, 180.f);
            // Labels des colonnes en bas (a-h)
            labels[index++] = makeLabel(std::string(1, 'a' + i),
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                        MARGIN + BOX_SIZE * 8 + MARGIN / 4.f);
            // Labels des colonnes en haut (a-h), inversés
            labels[index++] = makeLabel(std::string(1, 'a' + i),
                                        MARGIN + i * BOX_SIZE + BOX_SIZE / 2.f,
                                        MARGIN / 2.f, true, 180.f);
        }
    }

    /**
     * @brief Met à jour tous les sprites des pièces affichées sur le plateau.
     * * Cette fonction efface les sprites existants et les recrée en interrogeant
     * l'état actuel du plateau auprès de la logique du jeu (ChessLogic).
     * Elle gère le chargement des textures et le positionnement correct des sprites.
     */
    void Board::updatePieceSprites() {
        pieceSprites.clear(); // Efface tous les sprites de pièces existants.
        // Récupère l'état actuel du plateau depuis ChessLogic sous forme de map (case -> Piece).
        std::map<int, Piece> currentBoardState = chessLogic.getCurrentBoardState();

        // Parcourt toutes les pièces présentes sur le plateau
        for (const auto& pair : currentBoardState) {
            int square = pair.first;   // L'index de la case (0-63)
            Piece piece = pair.second; // L'objet Piece (type et couleur)

            if (piece.isEmpty()) continue; // Ignore les cases vides (ne devrait pas arriver ici si bien géré par ChessLogic)

            sf::Sprite sprite;
            // Utilise le TextureManager pour obtenir la texture de la pièce.
            // Le chemin de la texture est construit à partir du nom du fichier de texture de la pièce.
            const sf::Texture& tex = textureManager.getTexture(
                "../assets/images/pieces/chess_maestro_bw/" + piece.getTextureFileName());
            sprite.setTexture(tex);

            // Redimensionne le sprite pour qu'il s'adapte à la taille d'une case (BOX_SIZE).
            sf::Vector2u texSize = tex.getSize();
            float scaleX = static_cast<float>(BOX_SIZE) / texSize.x;
            float scaleY = static_cast<float>(BOX_SIZE) / texSize.y;
            sprite.setScale(scaleX, scaleY);

            // Calcule la position du sprite sur l'écran.
            // Les coordonnées SFML commencent en haut à gauche, les rangées d'échecs sont inversées (0-7 de bas en haut).
            int row = square / 8;
            int col = square % 8;
            sprite.setPosition(
                static_cast<float>(MARGIN + col * BOX_SIZE),
                static_cast<float>(MARGIN + (7 - row) * BOX_SIZE) // Inverser la rangée pour l'affichage SFML
            );
            pieceSprites.push_back(sprite); // Ajoute le sprite à la liste pour le dessin.
        }
    }

    /**
     * @brief Prépare les sprites et le cadre pour l'affichage des options de promotion.
     * * Cette fonction est appelée lorsqu'un pion atteint la dernière rangée et doit être promu.
     * Elle crée un cadre et les sprites des quatre pièces possibles (Dame, Tour, Fou, Cavalier)
     * pour que le joueur puisse choisir.
     */
    void Board::preparePromotionDisplay() {
        promotionChoicesSprites.clear(); // Efface les options de promotion précédentes.

        // Détermine les noms des textures des pièces de promotion en fonction du joueur actuel.
        std::vector<std::string> pieceNames = chessLogic.getPromotionWhite()
            ? std::vector<std::string>{"wQ", "wR", "wB", "wN"} // Pièces blanches
            : std::vector<std::string>{"bQ", "bR", "bB", "bN"}; // Pièces noires

        // Calcule la position de départ pour l'affichage de la promotion (centré à l'écran).
        float startX = WINDOW_WIDTH / 2.f - 2 * BOX_SIZE; // Pour 4 pièces, total 4*BOX_SIZE de large
        float startY = WINDOW_HEIGHT / 2.f - BOX_SIZE / 2.f; // Centré verticalement sur une case

        // Configure le cadre rectangulaire qui englobe les options de promotion.
        promotionFrame.setSize(sf::Vector2f(4 * BOX_SIZE, BOX_SIZE));
        promotionFrame.setPosition(startX, startY);
        promotionFrame.setFillColor(sf::Color(200, 200, 0, 180)); // Jaune semi-transparent
        promotionFrame.setOutlineThickness(3.f);
        promotionFrame.setOutlineColor(sf::Color::Black);

        // Crée et positionne les sprites pour chaque option de promotion.
        for (int i = 0; i < 4; i++) {
            sf::Sprite sprite;
            // Charge la texture de la pièce via le TextureManager.
            const sf::Texture& tex = textureManager.getTexture(
                "../assets/images/pieces/chess_maestro_bw/" + pieceNames[i] + ".png");
            sprite.setTexture(tex);

            // Redimensionne le sprite pour qu'il s'adapte à la taille d'une case.
            sf::Vector2u texSize = tex.getSize();
            sprite.setScale((float)BOX_SIZE / texSize.x, (float)BOX_SIZE / texSize.y);
            // Positionne le sprite horizontalement dans le cadre de promotion.
            sprite.setPosition(startX + i * BOX_SIZE, startY);
            promotionChoicesSprites.push_back(sprite); // Ajoute le sprite à la liste des choix.
        }
    }


    /**
     * @brief Dessine tous les éléments du plateau de jeu sur la fenêtre.
     * * Inclut le fond, les cases du plateau, les labels de coordonnées, les pièces,
     * la surbrillance de la case sélectionnée, les coups possibles et les rois en échec.
     * Gère également l'affichage du menu de promotion si nécessaire.
     * * @param window Référence à la fenêtre de rendu SFML.
     */
    void Board::draw(sf::RenderWindow& window) {
        window.clear(BACKGROUND_COLOR); // Efface l'écran avec la couleur de fond.

        // Dessiner les cases du plateau
        for (auto& row : boxes) {
            for (auto& box : row) {
                window.draw(box);
            }
        }
        
        // Dessiner les labels (coordonnées)
        for (auto& label : labels) {
            window.draw(label);
        }

        // Dessiner les pièces
        // Note: Les sprites sont stockés dans un std::vector.
        for (auto& sprite : pieceSprites) {
            window.draw(sprite);
        }

        // Mettre en surbrillance la case sélectionnée par le joueur.
        if (selectedSquare != -1) {
            int row = selectedSquare / 8;
            int col = selectedSquare % 8;
            sf::RectangleShape highlight(sf::Vector2f(BOX_SIZE, BOX_SIZE));
            // Positionnement inversé pour SFML
            highlight.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
            highlight.setFillColor(sf::Color(255, 255, 0, 100)); // Jaune semi-transparent
            window.draw(highlight);
        }

        // Dessiner des cercles pour indiquer les coups possibles pour la pièce sélectionnée.
        for (int sq : highlightedSquares) {
            int row = sq / 8;
            int col = sq % 8;
            sf::CircleShape circle(BOX_SIZE / 6.f); // Un cercle d'un sixième de la taille de la case
            circle.setFillColor(sf::Color(0, 255, 0, 120)); // Vert semi-transparent
            circle.setOrigin(circle.getRadius(), circle.getRadius()); // Centre l'origine pour un positionnement facile
            // Positionne le cercle au centre de la case
            circle.setPosition(
                MARGIN + col * BOX_SIZE + BOX_SIZE / 2.f,
                MARGIN + (7 - row) * BOX_SIZE + BOX_SIZE / 2.f
            );
            window.draw(circle);
        }

        // Mettre en surbrillance le roi en échec (rouge).
        for (bool white : {true, false}) { // Vérifie pour le roi blanc puis pour le roi noir
            if (chessLogic.isKingInCheck(white)) {
                // Détermine le nom de la pièce roi (ex: "wK" ou "bK")
                std::string kingName = white ? "wK" : "bK";
                int kingSquare = -1;
                // Obtient l'état actuel du plateau pour trouver la position du roi.
                auto boardState = chessLogic.getCurrentBoardState();
                for(auto const& [sq, piece] : boardState) {
                    if (piece.getName() == kingName) { // Si le nom de la pièce correspond au roi recherché
                        kingSquare = sq;
                        break;
                    }
                }

                if (kingSquare != -1) { // Si le roi est trouvé
                    int row = kingSquare / 8;
                    int col = kingSquare % 8;
                    sf::RectangleShape highlight(sf::Vector2f(BOX_SIZE, BOX_SIZE));
                    // Positionnement inversé pour SFML
                    highlight.setPosition(MARGIN + col * BOX_SIZE, MARGIN + (7 - row) * BOX_SIZE);
                    highlight.setFillColor(sf::Color(255, 0, 0, 120)); // Rouge semi-transparent
                    window.draw(highlight);
                }
            }
        }

        // Gérer l'affichage du menu de promotion si une promotion est en attente.
        if (chessLogic.isPromotionPending()) {
            window.draw(promotionFrame); // Dessine le cadre de promotion
            for (auto& choice : promotionChoicesSprites) {
                window.draw(choice); // Dessine les options de pièces pour la promotion
            }
        }
    }

    /**
     * @brief Gère les clics de souris sur le plateau de jeu.
     * * Cette fonction est le cœur de l'interaction utilisateur. Elle convertit les coordonnées
     * de la souris en case du plateau et gère la sélection des pièces, le déplacement,
     * et le processus de promotion si applicable.
     * * @param mouseX Coordonnée X de la souris.
     * @param mouseY Coordonnée Y de la souris.
     */
    void Board::handleMouseClick(int mouseX, int mouseY) {
        // Convertir les coordonnées de la souris en index de case (0..63).
        // La colonne est directe, mais la rangée est inversée pour correspondre au système SFML (Y=0 en haut).
        int col = (mouseX - MARGIN) / BOX_SIZE;
        int row = 7 - ((mouseY - MARGIN) / BOX_SIZE); 

        // Vérifie si le clic est en dehors du plateau.
        if (col < 0 || col >= 8 || row < 0 || row >= 8) return;

        int clickedSquare = row * 8 + col; // L'index de la case cliquée (0-63).

        // Si une promotion est en cours, le clic est géré différemment.
        if (chessLogic.isPromotionPending()) {
            // Parcourt les sprites des options de promotion pour voir si l'une d'elles a été cliquée.
            for (int i = 0; i < (int)promotionChoicesSprites.size(); ++i) {
                if (promotionChoicesSprites[i].getGlobalBounds().contains(static_cast<float>(mouseX), static_cast<float>(mouseY))) {
                    // Clic sur l'une des options de promotion.
                    Jr::PieceType newType; // Variable pour stocker le type de pièce choisi.
                    switch (i) {
                        case 0: newType = Jr::PieceType::Queen; break;
                        case 1: newType = Jr::PieceType::Rook;  break;
                        case 2: newType = Jr::PieceType::Bishop; break;
                        case 3: newType = Jr::PieceType::Knight; break;
                        default: return; // Cas inattendu, ne devrait pas arriver.
                    }
                    // Demande à la logique du jeu de promouvoir le pion.
                    chessLogic.promotePawn(chessLogic.getPromotionSquare(), newType);
                    updatePieceSprites(); // Met à jour l'affichage après la promotion.
                    selectedSquare = -1;     // Réinitialise la sélection.
                    highlightedSquares.clear(); // Efface les mises en surbrillance.
                    return; // Le clic a été géré.
                }
            }
            return; // Bloque tout autre clic tant que la promotion n'est pas finalisée (clic en dehors des options).
        }

        // Logique principale de sélection et de déplacement des pièces.
        if (selectedSquare == -1) {
            // Aucune pièce n'est actuellement sélectionnée, tenter d'en sélectionner une.
            Jr::Piece pieceOnSquare = chessLogic.getPieceAtSquare(clickedSquare); // Obtient la pièce sur la case cliquée.
            if (!pieceOnSquare.isEmpty()) { // S'il y a une pièce sur la case.
                // Vérifie si la pièce appartient au joueur dont c'est le tour.
                bool isWhitePiece = (pieceOnSquare.color == Jr::PieceColor::White);
                if ((isWhitePiece && chessLogic.getWhiteTurn()) || (!isWhitePiece && !chessLogic.getWhiteTurn())) {
                    selectedSquare = clickedSquare; // Sélectionne la pièce.
                    // Obtient et stocke les coups légaux pour la pièce sélectionnée pour les mettre en surbrillance.
                    highlightedSquares = chessLogic.getLegalMoves(selectedSquare);
                    // preparePromotionDisplay(); // Cette ligne n'est pas nécessaire ici, elle est déclenchée par makeMove
                }
            }
        } else {
            // Une pièce est déjà sélectionnée, tenter un déplacement vers `clickedSquare`.
            if (chessLogic.makeMove(selectedSquare, clickedSquare)) {
                // Le déplacement a été un succès (il était légal et effectué par ChessLogic).
                selectedSquare = -1; // Désélectionne la pièce.
                highlightedSquares.clear(); // Efface les coups en surbrillance.
                updatePieceSprites(); // Met à jour l'affichage après le coup.

                // Si une promotion est en attente après ce coup (déplacement d'un pion jusqu'au bout)
                if (chessLogic.isPromotionPending()) {
                    preparePromotionDisplay(); // Prépare l'affichage du menu de promotion.
                }
            } else {
                // Le coup n'était pas légal, ou le joueur a cliqué sur une autre pièce.
                Jr::Piece newPieceOnSquare = chessLogic.getPieceAtSquare(clickedSquare);
                // Vérifie si la nouvelle case contient une pièce de la même couleur que le joueur actuel.
                bool isWhiteNewPiece = (newPieceOnSquare.color == Jr::PieceColor::White);
                
                if (!newPieceOnSquare.isEmpty() && 
                    ((isWhiteNewPiece && chessLogic.getWhiteTurn()) || (!isWhiteNewPiece && !chessLogic.getWhiteTurn()))) {
                    // Si on a cliqué sur une autre pièce de la même couleur, on change la sélection.
                    selectedSquare = clickedSquare;
                    highlightedSquares = chessLogic.getLegalMoves(selectedSquare);
                } else {
                    // Si on a cliqué ailleurs ou sur une pièce ennemie, on désélectionne la pièce actuelle.
                    selectedSquare = -1;
                    highlightedSquares.clear();
                }
            }
        }
    }
} // namespace Jr