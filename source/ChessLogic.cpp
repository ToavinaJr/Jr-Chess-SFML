#include "../include/ChessLogic.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <random>

// --- Utilitaires de Bitboard ---

#ifdef __GNUC__
#define CUSTOM_CTZLL(x) __builtin_ctzll(x)
#elif defined(_MSC_VER) // For Microsoft Visual C++
#include <intrin.h>
inline int msvc_ctzll(uint64_t n) {
    unsigned long index;
    _BitScanForward64(&index, n);
    return index;
}
#define CUSTOM_CTZLL(x) msvc_ctzll(x)
#else
// Fallback for other compilers
#warning "__builtin_ctzll is a GCC extension. Consider implementing an alternative for other compilers like MSVC."
inline int custom_ctzll_fallback(uint64_t n) {
    if (n == 0) return 64; // Convention: 64 if all bits are zero
    int count = 0;
    while ((n & 1) == 0) { // While the least significant bit is 0
        n >>= 1;          // Shift right
        count++;          // Increment count
    }
    return count;
}
#define CUSTOM_CTZLL(x) custom_ctzll_fallback(x)
#endif

namespace Jr {

    /**
     * @brief Constructeur de la classe ChessLogic.
     * Initialise l'état du plateau de jeu à sa configuration de départ standard.
     */
    ChessLogic::ChessLogic() {
        generateZobristKeys();
        initializeBoard();
    }

    /**
     * @brief Initialise les bitboards et les variables d'état du jeu pour une nouvelle partie.
     * Définit la position de départ de toutes les pièces sur le plateau,
     * et réinitialise les drapeaux de tour, de roque, de prise en passant et de promotion.
     */
    void ChessLogic::initializeBoard() {
        // Efface tous les bitboards existants.
        bitboards.clear();
        // Initialisation des bitboards pour chaque type de pièce à leur position de départ standard.
        // Chaque valeur hexadécimale représente un masque de 64 bits (uint64_t).
        bitboards["wP"] = 0x000000000000FF00ULL; // Pions blancs sur la 2ème rangée (index 8-15)
        bitboards["wR"] = 0x0000000000000081ULL; // Tours blanches sur a1 (0) et h1 (7)
        bitboards["wN"] = 0x0000000000000042ULL; // Cavaliers blancs sur b1 (1) et g1 (6)
        bitboards["wB"] = 0x0000000000000024ULL; // Fous blancs sur c1 (2) et f1 (5)
        bitboards["wQ"] = 0x0000000000000008ULL; // Dame blanche sur d1 (3)
        bitboards["wK"] = 0x0000000000000010ULL; // Roi blanc sur e1 (4)

        bitboards["bP"] = 0x00FF000000000000ULL; // Pions noirs sur la 7ème rangée (index 48-55)
        bitboards["bR"] = 0x8100000000000000ULL; // Tours noires sur a8 (56) et h8 (63)
        bitboards["bN"] = 0x4200000000000000ULL; // Cavaliers noirs sur b8 (57) et g8 (62)
        bitboards["bB"] = 0x2400000000000000ULL; // Fous noirs sur c8 (58) et f8 (61)
        bitboards["bQ"] = 0x0800000000000000ULL; // Dame noire sur d8 (59)
        bitboards["bK"] = 0x1000000000000000ULL; // Roi noir sur e8 (60)

        // Met à jour le bitboard général `bitboardPieces` qui représente toutes les cases occupées.
        bitboardPieces = 0ULL;
        for (const auto& pair : bitboards) {
            bitboardPieces |= pair.second; // Effectue un OR bit à bit pour combiner toutes les positions de pièces.
        }

        // Réinitialisation des variables d'état du jeu pour un nouveau départ.
        whiteTurn = true;           // Les blancs commencent.
        enPassantSquare = -1;       // Aucune prise en passant possible au début.
        promotionPending = false;   // Aucune promotion en attente.
        promotionWhite = true;      // Par défaut, promotion pour les blancs si elle arrive.
        promotionSquare = -1;       // Aucune case de promotion définie.

        // Réinitialisation des drapeaux de roque.
        whiteKingMoved = false;
        whiteRookKingsideMoved = false;
        whiteRookQueensideMoved = false;
        blackKingMoved = false;
        blackRookKingsideMoved = false;
        blackRookQueensideMoved = false;

        fiftyMoveCounter = 0;       // Réinitialise le compteur de 50 coups
        positionHistory.clear();    // Efface l'historique des positions
        currentZobristHash = calculateZobristHash(); // Calcule le hash initial
        positionHistory.push_back(currentZobristHash);
    }

    /**
     * @brief Récupère l'objet Piece (type et couleur) à une case donnée.
     * Parcourt les bitboards pour identifier quelle pièce se trouve à la position spécifiée.
     * @param square L'index de la case (0-63).
     * @return L'objet Piece trouvé à la case. Si la case est vide ou invalide, retourne une Piece vide.
     */
    Piece ChessLogic::getPieceAtSquare(int square) const {
        if (square < 0 || square >= 64) {
            return Piece(); // Case invalide, retourne une pièce vide.
        }

        // Itère sur tous les bitboards (chaque type de pièce)
        for (const auto& pair : bitboards) {
            // Vérifie si le bit correspondant à 'square' est défini dans le bitboard de cette pièce.
            if ((pair.second & (1ULL << square)) != 0) {
                // La pièce est trouvée. Extrait sa couleur et son type à partir de son nom (ex: "wP", "bK").
                PieceColor color = (pair.first[0] == 'w') ? PieceColor::White : PieceColor::Black;
                PieceType type;
                switch (pair.first[1]) {
                    case 'P': type = PieceType::Pawn; break;
                    case 'N': type = PieceType::Knight; break;
                    case 'B': type = PieceType::Bishop; break;
                    case 'R': type = PieceType::Rook; break;
                    case 'Q': type = PieceType::Queen; break;
                    case 'K': type = PieceType::King; break;
                    default: return Piece(); // Cas inattendu, retourne une pièce vide.
                }
                return Piece(type, color); // Retourne l'objet Piece complet.
            }
        }
        return Piece(); // Aucune pièce trouvée à cette case, elle est vide.
    }

    /**
     * @brief Vérifie si une case spécifique est vide.
     * Utilise le bitboard `bitboardPieces` qui représente toutes les cases occupées.
     * @param sq L'index de la case (0-63).
     * @return True si la case est vide, False sinon.
     */
    bool ChessLogic::isEmpty(int sq) const {
        return (bitboardPieces & (1ULL << sq)) == 0;
    }

    /**
     * @brief Vérifie si une case contient une pièce ennemie par rapport à une pièce d'une couleur donnée.
     * @param sq L'index de la case (0-63).
     * @param whitePiece La couleur de la pièce de référence (True si blanche, False si noire).
     * @return True si la case contient une pièce de couleur opposée à `whitePiece`, False sinon.
     */
    bool ChessLogic::isEnemy(int sq, bool whitePiece) const {
        Piece pieceAtSq = getPieceAtSquare(sq); // Récupère la pièce à la case donnée.
        if (pieceAtSq.isEmpty()) {
            return false; // Si la case est vide, il n'y a pas d'ennemi.
        }
        // Compare la couleur de la pièce trouvée avec la couleur de référence.
        return (pieceAtSq.color == PieceColor::White) != whitePiece;
    }

    /**
     * @brief Génère la liste des mouvements "bruts" (non filtrés par les règles d'échec) pour une pièce donnée.
     * Cette fonction calcule toutes les cases où une pièce pourrait se déplacer ou capturer,
     * sans prendre en compte si le mouvement mettrait le roi en échec.
     * @param piece L'objet Piece (type et couleur) pour laquelle générer les mouvements.
     * @param from L'index de la case de départ de la pièce.
     * @return Un vecteur d'entiers représentant les index des cases cibles possibles.
     */
    std::vector<int> ChessLogic::getRawMoves(const Piece& piece, int from) const {
        std::vector<int> moves;
        int row = from / 8; // Rangée de la case de départ.
        int col = from % 8; // Colonne de la case de départ.
        bool isWhite = (piece.color == PieceColor::White); // Couleur de la pièce.

        /**
         * @brief Lambda fonction utilitaire pour ajouter une case à la liste des mouvements.
         * Vérifie si la case est dans les limites du plateau et si elle est vide
         * ou occupée par une pièce ennemie.
         * @param r La rangée de la case cible.
         * @param c La colonne de la case cible.
         */
        auto addIfValid = [&](int r, int c) {
            if (r >= 0 && r < 8 && c >= 0 && c < 8) { // Vérifie les limites du plateau.
                int sq = r * 8 + c; // Calcule l'index de la case cible.
                if (isEmpty(sq) || isEnemy(sq, isWhite)) { // Si vide ou contient un ennemi.
                    moves.push_back(sq); // Ajoute la case aux mouvements possibles.
                }
            }
        };

        // Logique de mouvement spécifique à chaque type de pièce.
        switch (piece.type) {
            case PieceType::Pawn: {
                int dir = isWhite ? 1 : -1;     // Direction du mouvement du pion (haut pour blancs, bas pour noirs).
                int startRow = isWhite ? 1 : 6; // Rangée de départ pour le double pas initial.

                // Mouvement simple en avant (1 case).
                int fwd_sq = (row + dir) * 8 + col;
                if (row + dir >= 0 && row + dir < 8 && isEmpty(fwd_sq)) {
                    moves.push_back(fwd_sq);
                    // Mouvement de deux cases au premier coup.
                    if (row == startRow) {
                        int fwd2_sq = (row + 2 * dir) * 8 + col;
                        if (isEmpty(fwd2_sq)) { // Le chemin (la case intermédiaire) doit être libre.
                            moves.push_back(fwd2_sq);
                        }
                    }
                }

                // Captures diagonales.
                for (int dc : {-1, 1}) { // Vérifie les colonnes adjacentes (gauche et droite).
                    int targetRow = row + dir;
                    int targetCol = col + dc;
                    if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {
                        int sq = targetRow * 8 + targetCol;
                        if (isEnemy(sq, isWhite)) {
                            moves.push_back(sq); // Capture normale.
                        }
                        // Prise en passant: la case `enPassantSquare` est la cible où le pion attérira.
                        // La validation que le pion ennemi est bien derrière la case cible est faite dans makeMove.
                        if (sq == enPassantSquare) {
                            moves.push_back(sq);
                        }
                    }
                }
                break;
            }
            case PieceType::Knight: {
                // Les déplacements du cavalier sont fixes (L-shape).
                for (auto [dr, dc] : std::vector<std::pair<int, int>>{
                    {2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}}) {
                    addIfValid(row + dr, col + dc);
                }
                break;
            }
            case PieceType::Bishop: {
                // Mouvements diagonaux (glissant).
                for (auto [dr, dc] : std::vector<std::pair<int, int>>{
                    {1,1}, {1,-1}, {-1,1}, {-1,-1}}) { // Directions diagonales.
                    int r = row + dr, c = col + dc;
                    while (r >= 0 && r < 8 && c >= 0 && c < 8) { // Tant que dans le plateau.
                        int sq = r * 8 + c;
                        if (isEmpty(sq)) {
                            moves.push_back(sq); // Case vide, peut continuer à glisser.
                        } else {
                            if (isEnemy(sq, isWhite)) moves.push_back(sq); // Capture, puis arrête la glissade.
                            break; // Arrête la glissade si une pièce est rencontrée (amie ou ennemie).
                        }
                        r += dr; c += dc; // Passe à la case suivante dans la même direction.
                    }
                }
                break;
            }
            case PieceType::Rook: {
                // Mouvements orthogonaux (glissant).
                for (auto [dr, dc] : std::vector<std::pair<int, int>>{
                    {1,0}, {-1,0}, {0,1}, {0,-1}}) { // Directions orthogonales (haut, bas, gauche, droite).
                    int r = row + dr, c = col + dc;
                    while (r >= 0 && r < 8 && c >= 0 && c < 8) { // Tant que dans le plateau.
                        int sq = r * 8 + c;
                        if (isEmpty(sq)) {
                            moves.push_back(sq);
                        } else {
                            if (isEnemy(sq, isWhite)) moves.push_back(sq);
                            break; // Arrête la glissade.
                        }
                        r += dr; c += dc;
                    }
                }
                break;
            }
            case PieceType::Queen: {
                // La dame combine les mouvements du fou et de la tour.
                for (auto [dr, dc] : std::vector<std::pair<int, int>>{
                    {1,1}, {1,-1}, {-1,1}, {-1,-1}, // Diagonales
                    {1,0}, {-1,0}, {0,1}, {0,-1}}) { // Orthogonaux
                    int r = row + dr, c = col + dc;
                    while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                        int sq = r * 8 + c;
                        if (isEmpty(sq)) {
                            moves.push_back(sq);
                        } else {
                            if (isEnemy(sq, isWhite)) moves.push_back(sq);
                            break; // Arrête la glissade.
                        }
                        r += dr; c += dc;
                    }
                }
                break;
            }
            case PieceType::King: {
                // Le roi se déplace d'une case dans toutes les directions.
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (!(dr == 0 && dc == 0)) {
                            addIfValid(row + dr, col + dc);
                        }
                    }
                }                
                break;
            }
            default: break; // PieceType::None (aucune action pour ce type).
        }
        return moves;
    }

    /**
     * @brief Vérifie si un roi est en échec.
     * Détermine si le roi de la couleur spécifiée est attaqué par une pièce adverse.
     * @param whiteKing True pour vérifier le roi blanc, False pour le roi noir.
     * @return True si le roi est en échec, False sinon.
     */
    bool ChessLogic::isKingInCheck(bool whiteKing) const {
        std::string kingName = whiteKing ? "wK" : "bK"; // Nom du roi à vérifier.
        
        // Trouve la case actuelle du roi en utilisant son bitboard.
        int kingSquare = -1;
        // Vérifie si le bitboard du roi existe et n'est pas vide.
        if (bitboards.count(kingName) == 0 || bitboards.at(kingName) == 0ULL) {
            // Le roi n'est pas sur le plateau (cas anormal ou fin de partie).
            return false;
        }
        kingSquare = CUSTOM_CTZLL(bitboards.at(kingName)); // Trouve l'index du bit défini (la position du roi).

        // Parcourt toutes les pièces sur le plateau pour voir si elles attaquent le roi.
        for (const auto& pair : bitboards) {
            const std::string& pieceName = pair.first;
            uint64_t bbOriginal = pair.second;

            // Vérifie si la pièce appartient à l'adversaire du roi.
            bool isWhitePiece = (pieceName[0] == 'w');
            if (isWhitePiece == whiteKing) {
                continue; // Ignore les pièces de la même couleur que le roi.
            }

            uint64_t bb = bbOriginal;
            while (bb) {
                int from = CUSTOM_CTZLL(bb);
                bb &= bb - 1;

                // Récupère l'objet Piece de l'attaquant.
                Piece attackingPiece = getPieceAtSquare(from);
                if (attackingPiece.isEmpty()) {
                    continue; // S'il y a une erreur et la pièce est vide, ignore.
                }

                // Génère les mouvements bruts de cette pièce attaquante.
                std::vector<int> moves = getRawMoves(attackingPiece, from); 

                // Vérifie si la case du roi est parmi les mouvements bruts de l'attaquant.
                for (int sq : moves) {
                    if (sq == kingSquare) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * @brief Simule un coup pour vérifier si le roi serait en échec après ce coup.
     * Crée une copie temporaire de l'état du plateau, effectue le coup simulé dessus,
     * puis vérifie si le roi de la couleur spécifiée est en échec dans cet état simulé.
     * L'état original du jeu n'est pas modifié.
     * @param from L'index de la case de départ de la pièce.
     * @param to L'index de la case d'arrivée de la pièce.
     * @param whiteKing La couleur du roi dont on vérifie l'échec (True si blanc, False si noir).
     * @return True si le roi serait en échec après le coup simulé, False sinon.
     */
    bool ChessLogic::wouldBeInCheck(int from, int to, bool whiteKing) const {
        // Sauvegarde de l'état actuel des bitboards pour restauration.
        std::map<std::string, uint64_t> backupBitboards = bitboards;
        uint64_t backupBitboardPieces = bitboardPieces;

        int fromCol = from % 8; // Colonne de départ, utile pour la logique de roque.

        // --- Simuler le déplacement ---

        // 1. Trouver la pièce à déplacer.
        std::string movingPieceName;
        Piece movingPiece;
        for (const auto& pair : backupBitboards) {
            if ((pair.second & (1ULL << from)) != 0) {
                movingPieceName = pair.first;
                movingPiece = getPieceAtSquare(from);
                break;
            }
        }

        if (movingPieceName.empty() || movingPiece.isEmpty()) {
            return false;
        }

        // 2. Supprimer la pièce de sa position de départ dans la copie des bitboards.
        backupBitboards[movingPieceName] &= ~(1ULL << from);
        backupBitboardPieces &= ~(1ULL << from);

        // 3. Gérer la capture si une pièce est sur la case cible (`to`).
        // Cette étape doit être effectuée avant de placer la pièce à `to`.
        for (auto& pair : backupBitboards) {
            if ((pair.second & (1ULL << to)) != 0) {
                pair.second &= ~(1ULL << to);
                backupBitboardPieces &= ~(1ULL << to);
                break;
            }
        }
        
        // 4. Gérer la prise en passant pour la simulation.
        // Lors d'une prise en passant, la case 'to' est vide, mais un pion est capturé.
        if (movingPiece.type == PieceType::Pawn && to == enPassantSquare && (backupBitboardPieces & (1ULL << to)) == 0) {
            int capturedPawnSq = (movingPiece.color == PieceColor::White) ? (to - 8) : (to + 8);
            std::string pawnName = (movingPiece.color == PieceColor::White) ? "bP" : "wP";

            // Vérifie que le pion à capturer existe bien à cette position dans le backup.
            if ((backupBitboards.count(pawnName) && (backupBitboards[pawnName] & (1ULL << capturedPawnSq)) != 0)) {
                 backupBitboards[pawnName] &= ~(1ULL << capturedPawnSq);
                 backupBitboardPieces &= ~(1ULL << capturedPawnSq);
            }
        }

        // 5. Placer la pièce sur sa nouvelle position (`to`) dans la copie des bitboards.
        backupBitboards[movingPieceName] |= (1ULL << to);
        backupBitboardPieces |= (1ULL << to);

        // 6. Gérer le roque pour la simulation (déplacement de la tour associé).
        // Ceci est crucial car le roi ne peut pas roquer à travers une case attaquée.
        if (movingPiece.type == PieceType::King && std::abs(fromCol - (to % 8)) == 2) {
            int fromRow = from / 8;
            int rookFrom, rookTo;
            std::string rookName = (movingPiece.color == PieceColor::White) ? "wR" : "bR";

            if ((to % 8) == 6) { // Roque côté roi (court): roi de e à g, tour de h à f.
                rookFrom = fromRow * 8 + 7; // Position de la tour sur h.
                rookTo = fromRow * 8 + 5;   // Position d'arrivée de la tour sur f.
            } else { // Roque côté dame (long): roi de e à c, tour de a à d.
                rookFrom = fromRow * 8 + 0; // Position de la tour sur a.
                rookTo = fromRow * 8 + 3;   // Position d'arrivée de la tour sur d.
            }
            // Déplacer la tour dans les bitboards de backup.
            backupBitboards[rookName] &= ~(1ULL << rookFrom);
            backupBitboards[rookName] |= (1ULL << rookTo);
            backupBitboardPieces &= ~(1ULL << rookFrom);
            backupBitboardPieces |= (1ULL << rookTo);
        }

        // --- Vérifier l'échec avec l'état simulé ---
        // Crée une instance temporaire de ChessLogic en copiant l'état actuel (`*this`).
        // Puis, elle applique les bitboards simulés à cette instance temporaire.
        ChessLogic simulatedLogic = *this;
        simulatedLogic.bitboards = backupBitboards;
        simulatedLogic.bitboardPieces = backupBitboardPieces;

        // Appelle `isKingInCheck` sur l'instance simulée pour vérifier l'échec.
        bool inCheck = simulatedLogic.isKingInCheck(whiteKing);

        return inCheck;
    }


    /**
     * @brief Récupère tous les coups légaux pour une pièce à partir d'une case donnée.
     * Cette fonction combine les mouvements bruts avec les vérifications d'échec
     * pour s'assurer que seuls les coups valides (qui ne laissent pas le roi en échec) sont retournés.
     * Inclut également la logique spécifique du roque.
     * @param from L'index de la case de départ de la pièce.
     * @return Un vecteur d'entiers représentant les index des cases cibles légales.
     */
    std::vector<int> ChessLogic::getLegalMoves(int from) const {
        std::vector<int> legalMoves;
        Piece piece = getPieceAtSquare(from);

        if (piece.isEmpty()) {
            return legalMoves;
        }

        bool isWhite = (piece.color == PieceColor::White);
        if (isWhite != whiteTurn) {
            return legalMoves;
        }

        // Obtient les mouvements bruts de la pièce.
        std::vector<int> candidateMoves = getRawMoves(piece, from);

        // Filtre les mouvements bruts en vérifiant s'ils mettraient le roi en échec.
        for (int to : candidateMoves) {
            if (!wouldBeInCheck(from, to, isWhite)) {
                legalMoves.push_back(to); // Le coup est légal s'il ne met pas le roi en échec.
            }
        }
        
        // Ajout spécifique de la logique de roque.
        // Le roque a des règles supplémentaires qui ne sont pas entièrement couvertes par `getRawMoves`
        // et `wouldBeInCheck` pour les cases intermédiaires.
        if (piece.type == PieceType::King) {
            int backRank = isWhite ? 0 : 7; // Rangée de départ du roi (0 pour blancs, 7 pour noirs).
            int kingStartCol = 4;           // Colonne de départ du roi (e-file).

            // Roque côté roi (court)
            // Conditions : Le roi et la tour côté roi n'ont pas bougé.
            if (!(isWhite ? whiteKingMoved : blackKingMoved) && 
                !(isWhite ? whiteRookKingsideMoved : blackRookKingsideMoved)) {
                // Vérifier que les cases intermédiaires entre le roi et la tour sont vides (f1/f8, g1/g8).
                if (isEmpty(backRank * 8 + 5) && isEmpty(backRank * 8 + 6)) {
                    // Le roi ne doit pas être en échec AVANT le roque,
                    // ni passer par une case attaquée, ni arriver en échec.
                    if (!isKingInCheck(isWhite) && // Le roi n'est pas déjà en échec.
                        !wouldBeInCheck(from, backRank * 8 + kingStartCol + 1, isWhite) && // Vérifie case f.
                        !wouldBeInCheck(from, backRank * 8 + kingStartCol + 2, isWhite)) { // Vérifie case g.
                        legalMoves.push_back(backRank * 8 + kingStartCol + 2); // Ajoute le coup de roque (case g).
                    }
                }
            }

            // Roque côté dame (long)
            // Conditions : Le roi et la tour côté dame n'ont pas bougé.
            if (!(isWhite ? whiteKingMoved : blackKingMoved) && 
                !(isWhite ? whiteRookQueensideMoved : blackRookQueensideMoved)) {
                // Vérifier que les cases intermédiaires entre le roi et la tour sont vides (b1/b8, c1/c8, d1/d8).
                if (isEmpty(backRank * 8 + 1) && isEmpty(backRank * 8 + 2) && isEmpty(backRank * 8 + 3)) {
                    // Le roi ne doit pas être en échec AVANT le roque,
                    // ni passer par une case attaquée, ni arriver en échec.
                    if (!isKingInCheck(isWhite) && // Le roi n'est pas déjà en échec.
                        !wouldBeInCheck(from, backRank * 8 + kingStartCol - 1, isWhite) && // Vérifie case d.
                        !wouldBeInCheck(from, backRank * 8 + kingStartCol - 2, isWhite)) { // Vérifie case c.
                        legalMoves.push_back(backRank * 8 + kingStartCol - 2); // Ajoute le coup de roque (case c).
                    }
                }
            }
        }

        return legalMoves;
    }


    /**
     * @brief Vérifie si un coup est légal avant de l'effectuer.
     * Cette fonction utilise `getLegalMoves` pour déterminer si la case cible `to`
     * est une destination valide pour la pièce à `from`.
     * @param from L'index de la case de départ.
     * @param to L'index de la case d'arrivée.
     * @return True si le coup est légal, False sinon.
     */
    bool ChessLogic::isValidMove(int from, int to) const {
        // Obtient tous les coups légaux pour la pièce à 'from'.
        std::vector<int> moves = getLegalMoves(from);
        // Vérifie si la case 'to' est présente dans la liste des coups légaux.
        return std::find(moves.begin(), moves.end(), to) != moves.end();
    }

    /**
     * @brief Effectue un coup sur le plateau et met à jour l'état du jeu.
     * Avant d'effectuer le coup, il vérifie sa légalité via `isValidMove`.
     * Gère les captures, la prise en passant, le roque, la promotion des pions
     * et le changement de tour.
     * @param from L'index de la case de départ de la pièce.
     * @param to L'index de la case d'arrivée de la pièce.
     * @return True si le coup a été effectué avec succès, False si le coup n'était pas légal.
     */
    bool ChessLogic::makeMove(int from, int to) {
        if (!isValidMove(from, to)) {
            // Le coup n'est pas légal selon les règles des échecs (y compris les échecs au roi).
            return false;
        }
        
        // Réinitialisation des drapeaux de promotion et de prise en passant pour le prochain coup.
        promotionPending = false;
        promotionSquare = -1;
        
        // Trouver la pièce à déplacer et son nom dans les bitboards.
        std::string movingPieceName;
        Piece movingPiece;
        for (const auto& pair : bitboards) {
            if ((pair.second & (1ULL << from)) != 0) { // Si le bit de la case 'from' est défini dans ce bitboard
                movingPieceName = pair.first;           // Récupère le nom de la pièce (ex: "wP")
                movingPiece = getPieceAtSquare(from);   // Récupère l'objet Piece (type et couleur)
                break;
            }
        }

        if (movingPieceName.empty() || movingPiece.isEmpty()) {
            return false; // Erreur logique : aucune pièce trouvée, ou pièce invalide.
                          // Ceci devrait normalement être intercepté par `isValidMove`.
        }

        // --- Gérer la capture ---
        // Parcourt tous les bitboards pour voir si une pièce est sur la case cible (`to`).
        for (auto& pair : bitboards) {
            if ((pair.second & (1ULL << to)) != 0) {
                pair.second &= ~(1ULL << to); // Retire la pièce capturée de son bitboard spécifique.
                break; // Une seule pièce peut être capturée à la fois.
            }
        }
        
        // --- Gestion de la prise en passant (En Passant) ---
        // Le cas où un pion capture un autre pion via la règle de la prise en passant.
        // La case `to` est vide lors d'une prise en passant, car le pion capturé est sur une autre case.
        if (movingPiece.type == PieceType::Pawn && to == enPassantSquare && (bitboardPieces & (1ULL << to)) == 0) {
            // Détermine la case où se trouve le pion adverse capturé (derrière la case `to`).
            int capturedPawnSquare = (movingPiece.color == PieceColor::White) ? (to - 8) : (to + 8);
            std::string pawnName = (movingPiece.color == PieceColor::White) ? "bP" : "wP"; // Nom du pion adverse.
            // Vérifie que le pion à capturer existe bien à cette position.
            if ((bitboards.count(pawnName) && (bitboards[pawnName] & (1ULL << capturedPawnSquare)) != 0)) {
                bitboards[pawnName] &= ~(1ULL << capturedPawnSquare); // Retire le pion capturé.
            }
        }

        // --- Déplacer la pièce (mise à jour des bitboards) ---
        bitboards[movingPieceName] &= ~(1ULL << from); // Retire la pièce de sa position de départ.
        bitboards[movingPieceName] |= (1ULL << to);   // Place la pièce à sa nouvelle position.

        // --- Gestion du roque (déplacement de la tour associé) ---
        // Détecte si le coup du roi est un roque (déplacement horizontal de 2 cases).
        int fromCol = from % 8;
        int toCol = to % 8;
        if (movingPiece.type == PieceType::King && std::abs(fromCol - toCol) == 2) {
            int fromRow = from / 8; // Rangée du roi.
            int rookFrom, rookTo;   // Positions de la tour.
            std::string rookName = (movingPiece.color == PieceColor::White) ? "wR" : "bR"; // Nom de la tour.

            if (toCol == 6) { // Roque côté roi (court) : tour de h à f.
                rookFrom = fromRow * 8 + 7;
                rookTo = fromRow * 8 + 5;
            } else { // Roque côté dame (long) : tour de a à d.
                rookFrom = fromRow * 8 + 0;
                rookTo = fromRow * 8 + 3;
            }
            bitboards[rookName] &= ~(1ULL << rookFrom); // Retire la tour de sa position de départ.
            bitboards[rookName] |= (1ULL << rookTo);   // Place la tour à sa nouvelle position.
        }

        // --- Mettre à jour les droits de roque ---
        // Si le roi ou une tour a bougé, leurs droits de roque correspondants sont perdus.
        if (movingPiece.type == PieceType::King) {
            if (movingPiece.color == PieceColor::White) whiteKingMoved = true;
            else blackKingMoved = true;
        } else if (movingPiece.type == PieceType::Rook) {
            // Vérifie si la tour a bougé de sa position initiale de roque.
            if (movingPiece.color == PieceColor::White) {
                if (from == 7) whiteRookKingsideMoved = true;  // Tour blanche côté roi (h1).
                else if (from == 0) whiteRookQueensideMoved = true; // Tour blanche côté dame (a1).
            } else { // Tour noire.
                if (from == 63) blackRookKingsideMoved = true; // Tour noire côté roi (h8).
                else if (from == 56) blackRookQueensideMoved = true; // Tour noire côté dame (a8).
            }
        }

        // --- Mettre à jour la case de prise en passant pour le prochain tour ---
        enPassantSquare = -1; // Réinitialise la case de prise en passant par défaut.
        if (movingPiece.type == PieceType::Pawn) {
            // Si un pion a avancé de deux cases (depuis sa rangée de départ),
            // la case derrière lui devient une case de prise en passant potentielle.
            if (movingPiece.color == PieceColor::White && (from / 8) == 1 && (to / 8) == 3) {
                enPassantSquare = from + 8; // Case derrière le pion blanc.
            } else if (movingPiece.color == PieceColor::Black && (from / 8) == 6 && (to / 8) == 4) {
                enPassantSquare = from - 8; // Case derrière le pion noir.
            }
        }

        // --- Vérifier la promotion du pion ---
        if (movingPiece.type == PieceType::Pawn) {
            // Si un pion atteint la dernière rangée de l'adversaire.
            if ((movingPiece.color == PieceColor::White && (to / 8) == 7) || // Pion blanc atteint la 8ème rangée.
                (movingPiece.color == PieceColor::Black && (to / 8) == 0)) { // Pion noir atteint la 1ère rangée.
                promotionPending = true;     // Marque qu'une promotion est en attente.
                promotionWhite = (movingPiece.color == PieceColor::White); // Couleur du pion à promouvoir.
                promotionSquare = to;        // Case où la promotion a lieu.
            }
        }

        // --- Changer le tour (si pas de promotion en attente) ---
        // Le tour ne change pas immédiatement si une promotion est en attente,
        // car le joueur doit d'abord choisir la nouvelle pièce.
        if (!promotionPending) {
            whiteTurn = !whiteTurn;
        }
        
        // Mettre à jour le bitboard général des pièces après toutes les modifications.
        bitboardPieces = 0ULL;
        for (const auto& pair : bitboards) {
            bitboardPieces |= pair.second;
        }
        
        return true; // Le coup a été effectué avec succès.
    }

    /**
     * @brief Gère la promotion d'un pion à une nouvelle pièce choisie par le joueur.
     * Cette fonction est appelée après qu'un pion ait atteint la dernière rangée et
     * que le joueur ait sélectionné le type de pièce désiré.
     * @param square La case où le pion est promu.
     * @param newType Le type de pièce vers laquelle le pion doit être promu (Queen, Rook, Bishop, Knight).
     */
    void ChessLogic::promotePawn(int square, PieceType newType) {
        // Vérifications de sécurité : s'assurer qu'une promotion est bien en attente
        // et qu'elle concerne la bonne case.
        if (!promotionPending || square != promotionSquare) {
             std::cerr << "Erreur: Tentative de promotion invalide ou sur une mauvaise case." << std::endl;
             return; 
        }

        // Déterminer le nom du pion à retirer et le nom de la nouvelle pièce à ajouter.
        std::string pawnName = promotionWhite ? "wP" : "bP";
        std::string newPieceName;

        // Retirer le pion de la case de promotion.
        if (bitboards.count(pawnName) && (bitboards[pawnName] & (1ULL << square)) != 0) {
            bitboards[pawnName] &= ~(1ULL << square);
        } else {
             std::cerr << "Erreur: Le pion à promouvoir n'a pas été trouvé à la case attendue." << std::endl;
             return; // Grave erreur logique.
        }

        // Ajouter la nouvelle pièce au bitboard correspondant.
        switch (newType) {
            case PieceType::Queen:  newPieceName = promotionWhite ? "wQ" : "bQ"; break;
            case PieceType::Rook:   newPieceName = promotionWhite ? "wR" : "bR"; break;
            case PieceType::Bishop: newPieceName = promotionWhite ? "wB" : "bB"; break;
            case PieceType::Knight: newPieceName = promotionWhite ? "wN" : "bN"; break;
            default: 
                std::cerr << "Erreur: Type de pièce de promotion invalide spécifié." << std::endl;
                // Restaurer le pion si le type est invalide pour éviter un état de jeu corrompu.
                bitboards[pawnName] |= (1ULL << square); 
                return; 
        }
        // Ajoute la nouvelle pièce sur la case de promotion.
        bitboards[newPieceName] |= (1ULL << square);

        // Réinitialiser les drapeaux de promotion une fois la promotion terminée.
        promotionPending = false;
        promotionSquare = -1;

        // Changer le tour après la promotion (car le tour n'avait pas changé après `makeMove`).
        whiteTurn = !whiteTurn;

        // Mettre à jour le bitboard général des pièces pour refléter le changement.
        bitboardPieces = 0ULL;
        for (const auto& pair : bitboards) {
            bitboardPieces |= pair.second;
        }
    }


    /**
     * @brief Fournit l'état actuel du plateau de jeu sous une forme facilement utilisable pour l'affichage.
     * Convertit les bitboards internes en une map qui associe chaque case occupée à un objet Piece.
     * @return Une std::map<int, Piece> où la clé est l'index de la case (0-63) et la valeur est l'objet Piece.
     */
    std::map<int, Piece> ChessLogic::getCurrentBoardState() const {
        std::map<int, Piece> boardState; // La map qui contiendra l'état du plateau.
        // Parcourt tous les bitboards (un pour chaque type et couleur de pièce).
        for (const auto& pair : bitboards) {
            const std::string& pieceName = pair.first;
            uint64_t bb = pair.second;
            
            // Itérer sur chaque bit défini dans le bitboard pour trouver les positions des pièces.
            while (bb != 0) {
                int square = CUSTOM_CTZLL(bb);
                bb &= (bb - 1);
                
                // Construire l'objet Piece à partir du nom de la pièce et de la case.
                PieceColor color = (pieceName[0] == 'w') ? PieceColor::White : PieceColor::Black;
                PieceType type;
                switch (pieceName[1]) {
                    case 'P': type = PieceType::Pawn; break;
                    case 'N': type = PieceType::Knight; break;
                    case 'B': type = PieceType::Bishop; break;
                    case 'R': type = PieceType::Rook; break;
                    case 'Q': type = PieceType::Queen; break;
                    case 'K': type = PieceType::King; break;
                    default: type = PieceType::None; break; // Gérer les cas inconnus ou erreurs.
                }
                boardState[square] = Piece(type, color); // Ajoute la pièce à la map de l'état du plateau.
            }
        }
        return boardState; // Retourne l'état complet du plateau.
    }

    // Dans ChessLogic.cpp

ChessGameStatus ChessLogic::getGameState() const {
    bool whiteToMove = whiteTurn;

    // 1) Échec et mat ou Pat
    if (isKingInCheck(whiteToMove)) {
        if (noLegalMovesAvailable(whiteToMove)) {
            return ChessGameStatus::Checkmate;
        }
    } else {
        if (noLegalMovesAvailable(whiteToMove)) {
            return ChessGameStatus::Stalemate;
        }
    }

    // 2) Règle des 50 coups
    if (is50MoveRuleDraw()) {
        return ChessGameStatus::Draw50Move;
    }

    // 3) Nulle par répétition de position
    if (isThreeFoldRepetitionDraw()) {
        return ChessGameStatus::DrawRepetition;
    }

    // 4) Matériel insuffisant
    if (isInsufficientMaterial()) {
        return ChessGameStatus::DrawMaterial;
    }

    // 5) Partie en cours
    return ChessGameStatus::Playing;
}


bool ChessLogic::isStalemate() const {
    bool currentKingInCheck = isKingInCheck(whiteTurn);
    if (currentKingInCheck) {
        return false; // Si le roi est en échec, ce n'est pas un pat
    }

    // Vérifie s'il y a des coups légaux
    for (int from = 0; from < 64; ++from) {
        Piece piece = getPieceAtSquare(from);
        if (!piece.isEmpty() && ((piece.color == PieceColor::White) == whiteTurn)) {
            if (!getLegalMoves(from).empty()) {
                return false; // Il y a au moins un coup légal, donc pas de pat
            }
        }
    }
    return true; // Aucun coup légal et pas en échec = Pat
}

bool ChessLogic::isInsufficientMaterial() const {
    int whitePieces = 0;
    int blackPieces = 0;
    int whiteBishops = 0;
    int blackBishops = 0;
    int whiteKnights = 0;
    int blackKnights = 0;
    bool hasPawns = false;
    bool hasRooksQueens = false;

    // Couleur des cases des fous (true = case noire, false = case blanche)
    std::vector<bool> whiteBishopSquares;
    std::vector<bool> blackBishopSquares;

    for (const auto& pair : bitboards) {
        const std::string& pieceName = pair.first;
        uint64_t bb = pair.second;

        // Pions, tours ou dames → matériel suffisant immédiatement
        if (pieceName[1] == 'P') hasPawns = true;
        if (pieceName[1] == 'R' || pieceName[1] == 'Q') hasRooksQueens = true;

        while (bb) {
            int sq = CUSTOM_CTZLL(bb); // ✅ Utilisation de ton macro portable
            bb &= (bb - 1);            // Efface le bit le moins significatif

            if (pieceName[0] == 'w') {
                whitePieces++;
                if (pieceName[1] == 'B') {
                    whiteBishops++;
                    whiteBishopSquares.push_back(((sq / 8) + (sq % 8)) % 2);
                }
                if (pieceName[1] == 'N') whiteKnights++;
            } else {
                blackPieces++;
                if (pieceName[1] == 'B') {
                    blackBishops++;
                    blackBishopSquares.push_back(((sq / 8) + (sq % 8)) % 2);
                }
                if (pieceName[1] == 'N') blackKnights++;
            }
        }
    }

    // 1) Matériel suffisant dès qu'il y a un pion, une tour ou une dame
    if (hasPawns || hasRooksQueens) {
        return false;
    }

    // 2) K vs K
    if (whitePieces == 1 && blackPieces == 1) return true;

    // 3) K+B vs K ou K+N vs K
    if ((whitePieces == 2 && (whiteBishops == 1 || whiteKnights == 1) && blackPieces == 1) ||
        (blackPieces == 2 && (blackBishops == 1 || blackKnights == 1) && whitePieces == 1)) {
        return true;
    }

    // 4) K+N+N vs K → Draw (deux cavaliers ne peuvent pas mater seuls)
    if ((whitePieces == 3 && whiteKnights == 2 && blackPieces == 1) ||
        (blackPieces == 3 && blackKnights == 2 && whitePieces == 1)) {
        return true;
    }

    // 5) K+B vs K+B (Draw uniquement si les deux fous sont sur la même couleur de case)
    if (whitePieces == 2 && blackPieces == 2 && whiteBishops == 1 && blackBishops == 1) {
        if (!whiteBishopSquares.empty() && !blackBishopSquares.empty() &&
            whiteBishopSquares[0] == blackBishopSquares[0]) {
            return true;
        }
    }

    return false;
}


void ChessLogic::generateZobristKeys() {
    std::mt19937_64 rng(std::random_device{}()); // Générateur de nombres aléatoires 64-bit

    // Clés pour les pièces sur chaque case
    for (int sq = 0; sq < 64; ++sq) {
        ZobristPieceKeys[sq]["wP"] = rng();
        ZobristPieceKeys[sq]["wR"] = rng();
        ZobristPieceKeys[sq]["wN"] = rng();
        ZobristPieceKeys[sq]["wB"] = rng();
        ZobristPieceKeys[sq]["wQ"] = rng();
        ZobristPieceKeys[sq]["wK"] = rng();
        ZobristPieceKeys[sq]["bP"] = rng();
        ZobristPieceKeys[sq]["bR"] = rng();
        ZobristPieceKeys[sq]["bN"] = rng();
        ZobristPieceKeys[sq]["bB"] = rng();
        ZobristPieceKeys[sq]["bQ"] = rng();
        ZobristPieceKeys[sq]["bK"] = rng();
    }

    // Clé pour le trait
    ZobristSideToMoveKey = rng();

    // Clés pour les droits de roque (16 combinaisons possibles)
    for (int i = 0; i < 16; ++i) {
        ZobristCastlingKeys[i] = rng();
    }

    // Clés pour la colonne de prise en passant (8 colonnes)
    for (int i = 0; i < 8; ++i) {
        ZobristEnPassantKeys[i] = rng();
    }
}

uint64_t ChessLogic::calculateZobristHash() const {
    uint64_t hash = 0ULL;

    // Hash des pièces
    for (const auto& pair : bitboards) {
        const std::string& pieceName = pair.first;
        uint64_t bb = pair.second;
        while (bb) {
            int sq = CUSTOM_CTZLL(bb);
            bb &= (bb - 1);
            hash ^= ZobristPieceKeys[sq].at(pieceName);
        }
    }

    // Hash du trait
    if (whiteTurn) {
        hash ^= ZobristSideToMoveKey;
    }

    // Hash des droits de roque (représentation binaire des 4 droits)
    int castlingRights = 0;
    if (!whiteRookKingsideMoved && !whiteKingMoved) castlingRights |= 1;  // K (roi blanc côté roi)
    if (!whiteRookQueensideMoved && !whiteKingMoved) castlingRights |= 2; // Q (roi blanc côté dame)
    if (!blackRookKingsideMoved && !blackKingMoved) castlingRights |= 4;  // k (roi noir côté roi)
    if (!blackRookQueensideMoved && !blackKingMoved) castlingRights |= 8; // q (roi noir côté dame)
    hash ^= ZobristCastlingKeys[castlingRights];

    // Hash de la case de prise en passant
    if (enPassantSquare != -1) {
        hash ^= ZobristEnPassantKeys[enPassantSquare % 8]; // Seule la colonne compte pour Zobrist
    }

    return hash;
}

// Cette fonction est appelée APRES les modifications des bitboards mais AVANT de changer le tour
void ChessLogic::updateZobristHashForMove(const Piece& movingPiece, int from, int to,
                                       const Piece& capturedPiece, int capturedPawnSq,
                                       bool isCastling, int rookFrom, int rookTo, PieceType promotionType) {
    // Retirer la pièce de départ et d'arrivée (si elle capture)
    currentZobristHash ^= ZobristPieceKeys[from][movingPiece.getName()]; // Retire l'ancienne position
    if (!capturedPiece.isEmpty()) {
        currentZobristHash ^= ZobristPieceKeys[to][capturedPiece.getName()]; // Retire la pièce capturée
    } else if (movingPiece.type == PieceType::Pawn && to == enPassantSquare && capturedPawnSq != -1) {
        // En Passant, retire le pion capturé de sa case réelle
        currentZobristHash ^= ZobristPieceKeys[capturedPawnSq][(movingPiece.color == PieceColor::White ? "bP" : "wP")];
    }


    // Mettre à jour la clé du trait (sera inversée par le ^= ZobristSideToMoveKey)
    currentZobristHash ^= ZobristSideToMoveKey; // Inversion du trait

    // Retirer l'ancienne clé de droits de roque et ajouter la nouvelle
    // (cela dépend des drapeaux de roque avant et après le coup)
    int oldCastlingRights = 0;
    if (!whiteRookKingsideMoved && !whiteKingMoved) oldCastlingRights |= 1;
    if (!whiteRookQueensideMoved && !whiteKingMoved) oldCastlingRights |= 2;
    if (!blackRookKingsideMoved && !blackKingMoved) oldCastlingRights |= 4;
    if (!blackRookQueensideMoved && !blackKingMoved) oldCastlingRights |= 8;
    currentZobristHash ^= ZobristCastlingKeys[oldCastlingRights];

    // Retirer l'ancienne clé en passant
    if (enPassantSquare != -1) {
        currentZobristHash ^= ZobristEnPassantKeys[enPassantSquare % 8];
    }

    
    if (promotionType != PieceType::None) {
        std::string promotedPieceName;
        if (promotionType == PieceType::Queen) promotedPieceName = movingPiece.color == PieceColor::White ? "wQ" : "bQ";
        else if (promotionType == PieceType::Rook) promotedPieceName = movingPiece.color == PieceColor::White ? "wR" : "bR";
        else if (promotionType == PieceType::Bishop) promotedPieceName = movingPiece.color == PieceColor::White ? "wB" : "bB";
        else if (promotionType == PieceType::Knight) promotedPieceName = movingPiece.color == PieceColor::White ? "wN" : "bN";
        currentZobristHash ^= ZobristPieceKeys[to][promotedPieceName];
    } else {
        currentZobristHash ^= ZobristPieceKeys[to][movingPiece.getName()];
    }


    // Gérer le roque (déplacement de la tour)
    if (isCastling) {
        std::string rookName = movingPiece.color == PieceColor::White ? "wR" : "bR";
        currentZobristHash ^= ZobristPieceKeys[rookFrom][rookName];
        currentZobristHash ^= ZobristPieceKeys[rookTo][rookName];
    }
}

bool ChessLogic::is50MoveRuleDraw() const {
    return fiftyMoveCounter >= 100;
}

bool ChessLogic::isThreeFoldRepetitionDraw() const {
    int count = 0;
    for (auto hash : positionHistory) {
        if (hash == currentZobristHash) {
            ++count;
            if (count >= 3) return true;
        }
    }
    return false;
}


bool ChessLogic::noLegalMovesAvailable(bool whiteToMove) const {
    for (int square = 0; square < 64; ++square) {
        Piece piece = getPieceAtSquare(square);
        
        if (piece.isEmpty()) continue;

        // Vérifie si la pièce correspond au joueur courant
        if ((whiteToMove && piece.color != PieceColor::White) ||
            (!whiteToMove && piece.color != PieceColor::Black)) {
            continue;
        }

        auto moves = getLegalMoves(square);
        if (!moves.empty()) return false;
    }
    return true;
}


} // namespace Jr