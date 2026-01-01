#include "../include/PlayingState.hpp"
#include "../include/StateManager.hpp"
#include "../include/ChessLogic.hpp"
#include "../include/GameOverState.hpp"
#include "../include/AIPlayer.hpp"

#include <iostream>
#include <random>

namespace Jr {

PlayingState::PlayingState(StateManager& manager, sf::RenderWindow& win, TextureManager& tm, FontManager& fm,
                           GameMode mode, PlayerSide side, int aiDepth, float clockSeconds)
    : GameState(manager, win),
      textureManager(tm),
      fontManager(fm),
      chessLogic(),
      board(textureManager, fontManager, chessLogic),
      whiteTimeLeft(clockSeconds),
      blackTimeLeft(clockSeconds),
      gameMode(mode),
      playerSide(side),
      aiPlayer(aiDepth),
      lastMoveCount(0),
      aiIsThinking(false)
{
    std::cout << "[PlayingState] Constructor called with:" << std::endl;
    std::cout << "  - Mode: " << static_cast<int>(mode) << std::endl;
    std::cout << "  - Side: " << static_cast<int>(side) << std::endl;
    std::cout << "  - AI Depth: " << aiDepth << std::endl;
    std::cout << "  - Clock: " << clockSeconds << "s" << std::endl;
}

void PlayingState::onEnter() {
    chessLogic.initializeBoard(); // Réinitialise le plateau à chaque nouvelle partie
    clockRunning = true;
    isViewingHistory = false;
    
    // Setup UI panels
    sidebarBg.setSize(sf::Vector2f(SIDEBAR_WIDTH, WINDOW_HEIGHT));
    sidebarBg.setPosition(BOARD_WIDTH, 0);
    sidebarBg.setFillColor(BACKGROUND_COLOR);
    
    // Panel pour les captures
    capturePanel.setSize(sf::Vector2f(SIDEBAR_WIDTH - 20, 180));
    capturePanel.setPosition(BOARD_WIDTH + 10, 10);
    capturePanel.setFillColor(PANEL_BG_COLOR);
    capturePanel.setOutlineColor(sf::Color(80, 80, 80));
    capturePanel.setOutlineThickness(2);
    
    // Panel pour l'horloge
    clockPanel.setSize(sf::Vector2f(SIDEBAR_WIDTH - 20, 100));
    clockPanel.setPosition(BOARD_WIDTH + 10, 200);
    clockPanel.setFillColor(PANEL_BG_COLOR);
    clockPanel.setOutlineColor(sf::Color(80, 80, 80));
    clockPanel.setOutlineThickness(2);
    
    // Panel pour l'historique
    historyPanel.setSize(sf::Vector2f(SIDEBAR_WIDTH - 20, WINDOW_HEIGHT - 360));
    historyPanel.setPosition(BOARD_WIDTH + 10, 310);
    historyPanel.setFillColor(PANEL_BG_COLOR);
    historyPanel.setOutlineColor(sf::Color(80, 80, 80));
    historyPanel.setOutlineThickness(2);
    
    // Boutons de navigation
    navButtonBack.setSize(sf::Vector2f(60, 30));
    navButtonBack.setPosition(BOARD_WIDTH + 20, WINDOW_HEIGHT - 45);
    navButtonBack.setFillColor(sf::Color(70, 70, 70));
    navButtonBack.setOutlineColor(ACCENT_COLOR);
    navButtonBack.setOutlineThickness(1);
    
    navButtonForward.setSize(sf::Vector2f(60, 30));
    navButtonForward.setPosition(BOARD_WIDTH + 90, WINDOW_HEIGHT - 45);
    navButtonForward.setFillColor(sf::Color(70, 70, 70));
    navButtonForward.setOutlineColor(ACCENT_COLOR);
    navButtonForward.setOutlineThickness(1);
    
    // Initialiser lastMoveCount
    lastMoveCount = static_cast<int>(chessLogic.getMoveHistory().size());
    
    // Si playerSide == Random, choisir aléatoirement
    if (playerSide == PlayerSide::Random) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> d(0, 1);
        playerSide = d(gen) == 0 ? PlayerSide::White : PlayerSide::Black;
    }
    
    // Si mode AI vs AI ou (Humain vs IA et humain joue noirs), l'IA commence
    if (gameMode == GameMode::AIvsAI || 
        (gameMode == GameMode::HumanVsAI && playerSide == PlayerSide::Black)) {
        // L'IA doit jouer en premier
        aiIsThinking = true;
        std::cout << "IA réfléchit (profondeur " << aiPlayer.getDepth() << ")..." << std::endl;
        AIMove aiMove = aiPlayer.findBestMove(chessLogic);
        std::cout << "IA joue: " << aiMove.from << " -> " << aiMove.to << std::endl;
        if (aiMove.from != -1) {
            chessLogic.makeMove(aiMove.from, aiMove.to);
            if (chessLogic.isPromotionPending()) {
                chessLogic.promotePawn(chessLogic.getPromotionSquare(), PieceType::Queen);
            }
            board.updatePieceSprites();;
            lastMoveCount = static_cast<int>(chessLogic.getMoveHistory().size());
        }
        aiIsThinking = false;
    }
    
    std::cout << "Entering PlayingState." << std::endl;
}

void PlayingState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        float mx = event.mouseButton.x;
        float my = event.mouseButton.y;
        
        // Vérifier si on clique sur le thumb du scrollbar
        if (scrollThumb.getGlobalBounds().contains(mx, my)) {
            isDraggingScrollbar = true;
            scrollDragStartY = my;
            scrollStartOffset = historyScrollOffset;
            return;
        }
        
        // Vérifier si on clique sur la piste du scrollbar
        if (scrollbar.getGlobalBounds().contains(mx, my)) {
            const auto& moves = chessLogic.getMoveHistory();
            int maxScroll = std::max(0, static_cast<int>(moves.size()) - maxVisibleMoves);
            if (maxScroll > 0) {
                float scrollbarHeight = historyPanel.getSize().y - 40;
                float thumbHeight = std::max(20.0f, scrollbarHeight * maxVisibleMoves / moves.size());
                float clickY = my - scrollbar.getPosition().y;
                float ratio = clickY / scrollbarHeight;
                historyScrollOffset = static_cast<int>(ratio * maxScroll);
                historyScrollOffset = std::max(0, std::min(historyScrollOffset, maxScroll));
            }
            return;
        }
        
        // Vérifier si on clique sur un coup dans l'historique
        for (size_t i = 0; i < moveClickAreas.size(); ++i) {
            if (moveClickAreas[i].contains(mx, my)) {
                // Cliquer sur le coup i signifie aller au snapshot i+1
                int targetSnapshot = static_cast<int>(i) + 1;
                if (targetSnapshot < chessLogic.getSnapshotCount()) {
                    chessLogic.restoreSnapshot(targetSnapshot);
                    board.clearSelection(); // Réinitialiser la sélection
                    board.updatePieceSprites();
                    isViewingHistory = (targetSnapshot < chessLogic.getSnapshotCount() - 1);
                }
                return;
            }
        }
        
        // Bouton retour
        if (navButtonBack.getGlobalBounds().contains(mx, my)) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx > 0) {
                chessLogic.restoreSnapshot(idx - 1);
                board.clearSelection(); // Réinitialiser la sélection
                board.updatePieceSprites();
                isViewingHistory = (idx - 1 < chessLogic.getSnapshotCount() - 1);
            }
            return;
        }
        
        // Bouton avancer
        if (navButtonForward.getGlobalBounds().contains(mx, my)) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx + 1 < chessLogic.getSnapshotCount()) {
                chessLogic.restoreSnapshot(idx + 1);
                board.clearSelection(); // Réinitialiser la sélection
                board.updatePieceSprites();
                isViewingHistory = (idx + 1 < chessLogic.getSnapshotCount() - 1);
            }
            return;
        }
        
        // Clic sur le plateau (seulement si on est à la position actuelle et pas en train de penser)
        if (!isViewingHistory && !aiIsThinking) {
            board.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        isDraggingScrollbar = false;
    }
    
    if (event.type == sf::Event::MouseMoved && isDraggingScrollbar) {
        float my = event.mouseMove.y;
        float deltaY = my - scrollDragStartY;
        
        const auto& moves = chessLogic.getMoveHistory();
        int maxScroll = std::max(0, static_cast<int>(moves.size()) - maxVisibleMoves);
        
        if (maxScroll > 0) {
            float scrollbarHeight = historyPanel.getSize().y - 40;
            float thumbHeight = std::max(20.0f, scrollbarHeight * maxVisibleMoves / moves.size());
            float maxThumbTravel = scrollbarHeight - thumbHeight;
            
            float deltaScroll = (deltaY / maxThumbTravel) * maxScroll;
            historyScrollOffset = scrollStartOffset + static_cast<int>(deltaScroll);
            historyScrollOffset = std::max(0, std::min(historyScrollOffset, maxScroll));
        }
    }
    
    // Scroll de la molette pour l'historique
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (historyPanel.getGlobalBounds().contains(event.mouseWheelScroll.x, event.mouseWheelScroll.y)) {
            historyScrollOffset -= static_cast<int>(event.mouseWheelScroll.delta * 2);
            const auto& moves = chessLogic.getMoveHistory();
            int maxScroll = std::max(0, static_cast<int>(moves.size()) - maxVisibleMoves);
            historyScrollOffset = std::max(0, std::min(historyScrollOffset, maxScroll));
        }
    }
    
    // Navigation clavier
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx > 0) {
                chessLogic.restoreSnapshot(idx - 1);
                board.clearSelection(); // Réinitialiser la sélection
                board.updatePieceSprites();
                isViewingHistory = (idx - 1 < chessLogic.getSnapshotCount() - 1);
            }
        } else if (event.key.code == sf::Keyboard::Right) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx + 1 < chessLogic.getSnapshotCount()) {
                chessLogic.restoreSnapshot(idx + 1);
                board.clearSelection(); // Réinitialiser la sélection
                board.updatePieceSprites();
                isViewingHistory = (idx + 1 < chessLogic.getSnapshotCount() - 1);
            }
        } else if (event.key.code == sf::Keyboard::Escape) {
            std::cout << "Escape pressed in PlayingState. Returning to Menu." << std::endl;
            stateManager.popState();
        }
    }
}

void PlayingState::update(float deltaTime) {
    // Mettre à jour l'horloge seulement si on n'est pas en train de visualiser l'historique
    if (clockRunning && !isViewingHistory) {
        if (chessLogic.getWhiteTurn()) {
            whiteTimeLeft -= deltaTime;
            if (whiteTimeLeft <= 0.0f) {
                whiteTimeLeft = 0.0f;
                clockRunning = false;
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Temps écoulé ! Les Noirs gagnent !");
                return;
            }
        } else {
            blackTimeLeft -= deltaTime;
            if (blackTimeLeft <= 0.0f) {
                blackTimeLeft = 0.0f;
                clockRunning = false;
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Temps écoulé ! Les Blancs gagnent !");
                return;
            }
        }
    }
    
    // Auto-scroll vers le bas quand on est à la position actuelle
    if (!isViewingHistory) {
        const auto& moves = chessLogic.getMoveHistory();
        int maxScroll = std::max(0, static_cast<int>(moves.size()) - maxVisibleMoves);
        historyScrollOffset = maxScroll;
    }
    
    // Détecter si un nouveau coup a été joué et faire jouer l'IA si nécessaire
    int currentMoveCount = static_cast<int>(chessLogic.getMoveHistory().size());
    if (!isViewingHistory && !aiIsThinking && currentMoveCount > lastMoveCount) {
        lastMoveCount = currentMoveCount;
        
        // Déterminer si l'IA doit jouer
        bool aiShouldPlay = false;
        
        if (gameMode == GameMode::HumanVsAI) {
            // Humain contre IA
            if (playerSide == PlayerSide::White) {
                // Humain joue les blancs, IA joue les noirs
                aiShouldPlay = !chessLogic.getWhiteTurn();
            } else {
                // Humain joue les noirs, IA joue les blancs
                aiShouldPlay = chessLogic.getWhiteTurn();
            }
        } else if (gameMode == GameMode::AIvsAI) {
            // Les deux côtés sont joués par l'IA
            aiShouldPlay = true;
        }
        
        if (aiShouldPlay) {
            aiIsThinking = true;
            std::cout << "IA commence à réfléchir (depth=" << aiPlayer.getDepth() << ")..." << std::endl;
            
            // Lancer l'IA dans un thread séparé
            ChessLogic logicCopy = chessLogic;
            aiFuture = std::async(std::launch::async, [this, logicCopy]() {
                return aiPlayer.findBestMove(logicCopy);
            });
        }
    }
    
    // Vérifier si l'IA a terminé sa réflexion
    if (aiIsThinking && aiFuture.valid()) {
        if (aiFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            AIMove best = aiFuture.get();
            std::cout << "IA a trouvé: " << best.from << " -> " << best.to << " (score=" << best.score << ")" << std::endl;
            
            if (best.from != -1) {
                chessLogic.makeMove(best.from, best.to);
                if (chessLogic.isPromotionPending()) {
                    chessLogic.promotePawn(chessLogic.getPromotionSquare(), PieceType::Queen);
                }
                board.updatePieceSprites();
                lastMoveCount = static_cast<int>(chessLogic.getMoveHistory().size());
            }
            aiIsThinking = false;
        }
    }

    // Vérifier l'état du jeu seulement si on n'est pas en train de visualiser l'historique
    if (!isViewingHistory) {
        ChessGameStatus state = chessLogic.getGameState();

        switch (state) {
            case ChessGameStatus::Checkmate:
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Échec et mat !");
                std::cout << "Checkmate! Game Over." << std::endl;
                break;

            case ChessGameStatus::Stalemate:
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Pat !");
                break;

            case ChessGameStatus::Draw50Move:
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Match nul (règle des 50 coups) !");
                std::cout << "Draw by 50-move rule! Game Over." << std::endl;
                break;

            case ChessGameStatus::DrawRepetition:
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Match nul (répétition de position) !");
                std::cout << "Draw by repetition! Game Over." << std::endl;
                break;

            case ChessGameStatus::DrawMaterial:
                stateManager.changeState<GameOverState>(fontManager, textureManager, "Pat !");
                break;

            case ChessGameStatus::Playing:
            default:
                break;
        }
    }
}


void PlayingState::draw() {
    board.draw(window);
    
    // Draw sidebar
    window.draw(sidebarBg);
    window.draw(capturePanel);
    window.draw(clockPanel);
    window.draw(historyPanel);
    
    const sf::Font& font = fontManager.getFont(FONT_PATH);
    
    // === PANEL CAPTURES ===
    sf::Text captureTitle("Pièces capturées", font, 16);
    captureTitle.setPosition(BOARD_WIDTH + 20, 15);
    captureTitle.setFillColor(TEXT_COLOR);
    window.draw(captureTitle);
    
    const auto& capturedWhite = chessLogic.getCapturedByWhite();
    const auto& capturedBlack = chessLogic.getCapturedByBlack();
    int diff = chessLogic.getMaterialScoreDifference();
    
    // Pièces capturées par les blancs (pièces noires)
    sf::Text whiteLabel("Blancs:", font, 14);
    whiteLabel.setPosition(BOARD_WIDTH + 20, 40);
    whiteLabel.setFillColor(sf::Color::White);
    window.draw(whiteLabel);
    
    float cx = BOARD_WIDTH + 90;
    float cy = 40;
    for (size_t i = 0; i < capturedWhite.size(); ++i) {
        sf::Sprite s;
        const sf::Texture& tex = textureManager.getTexture("../assets/images/pieces/chess_maestro_bw/" + capturedWhite[i].getTextureFileName());
        s.setTexture(tex);
        float scale = 20.0f / tex.getSize().x;
        s.setScale(scale, scale);
        s.setPosition(cx + (i % 8) * 25, cy + (i / 8) * 25);
        window.draw(s);
    }
    
    // Pièces capturées par les noirs (pièces blanches)
    sf::Text blackLabel("Noirs:", font, 14);
    blackLabel.setPosition(BOARD_WIDTH + 20, 100);
    blackLabel.setFillColor(sf::Color::Black);
    window.draw(blackLabel);
    
    cx = BOARD_WIDTH + 90;
    cy = 100;
    for (size_t i = 0; i < capturedBlack.size(); ++i) {
        sf::Sprite s;
        const sf::Texture& tex = textureManager.getTexture("../assets/images/pieces/chess_maestro_bw/" + capturedBlack[i].getTextureFileName());
        s.setTexture(tex);
        float scale = 20.0f / tex.getSize().x;
        s.setScale(scale, scale);
        s.setPosition(cx + (i % 8) * 25, cy + (i / 8) * 25);
        window.draw(s);
    }
    
    // Différence de points
    sf::Text diffText;
    diffText.setFont(font);
    diffText.setCharacterSize(18);
    diffText.setPosition(BOARD_WIDTH + 20, 155);
    if (diff > 0) {
        diffText.setString("+" + std::to_string(diff));
        diffText.setFillColor(ACCENT_COLOR);
    } else if (diff < 0) {
        diffText.setString(std::to_string(diff));
        diffText.setFillColor(sf::Color::Red);
    } else {
        diffText.setString("=");
        diffText.setFillColor(TEXT_COLOR);
    }
    window.draw(diffText);
    
    // === PANEL HORLOGE ===
    sf::Text clockTitle("Temps", font, 16);
    clockTitle.setPosition(BOARD_WIDTH + 20, 205);
    clockTitle.setFillColor(TEXT_COLOR);
    window.draw(clockTitle);
    
    auto formatTime = [](float seconds) -> std::string {
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%d:%02d", mins, secs);
        return std::string(buf);
    };
    
    sf::Text whiteTime(formatTime(whiteTimeLeft), font, 24);
    whiteTime.setPosition(BOARD_WIDTH + 20, 235);
    whiteTime.setFillColor(chessLogic.getWhiteTurn() && !isViewingHistory ? ACCENT_COLOR : sf::Color::White);
    window.draw(whiteTime);
    
    sf::Text blackTime(formatTime(blackTimeLeft), font, 24);
    blackTime.setPosition(BOARD_WIDTH + 180, 235);
    blackTime.setFillColor(!chessLogic.getWhiteTurn() && !isViewingHistory ? ACCENT_COLOR : sf::Color::White);
    window.draw(blackTime);
    
    // === PANEL HISTORIQUE ===
    sf::Text historyTitle("Historique", font, 16);
    historyTitle.setPosition(BOARD_WIDTH + 20, 315);
    historyTitle.setFillColor(TEXT_COLOR);
    window.draw(historyTitle);
    
    const auto& moves = chessLogic.getMoveHistory();
    int currentIdx = chessLogic.getCurrentSnapshotIndex();
    
    // Effacer les zones cliquables précédentes
    moveClickAreas.clear();
    
    // Créer une zone de clip pour l'historique (pour éviter le débordement)
    sf::View historyView = window.getView();
    sf::FloatRect historyViewport(
        (BOARD_WIDTH + 10.0f) / WINDOW_WIDTH,
        320.0f / WINDOW_HEIGHT,
        (SIDEBAR_WIDTH - 20.0f) / WINDOW_WIDTH,
        (WINDOW_HEIGHT - 370.0f) / WINDOW_HEIGHT
    );
    historyView.setViewport(historyViewport);
    historyView.setCenter(
        BOARD_WIDTH + SIDEBAR_WIDTH / 2.0f,
        320.0f + (WINDOW_HEIGHT - 370.0f) / 2.0f
    );
    historyView.setSize(SIDEBAR_WIDTH - 20.0f, WINDOW_HEIGHT - 370.0f);
    
    // Appliquer la vue pour le clipping
    window.setView(historyView);
    
    float hy = 345;
    int startIdx = historyScrollOffset;
    int endIdx = std::min(static_cast<int>(moves.size()), startIdx + maxVisibleMoves);
    
    for (int i = startIdx; i < endIdx; ++i) {
        sf::Text moveText;
        moveText.setFont(font);
        moveText.setCharacterSize(14);
        
        std::string moveStr = std::to_string(i + 1) + ". " + moves[i];
        moveText.setString(moveStr);
        moveText.setPosition(BOARD_WIDTH + 25, hy);
        
        // Highlight le coup actuel
        if (i == currentIdx - 1) {
            // Dessiner un rectangle de fond pour le coup sélectionné
            sf::RectangleShape highlight;
            highlight.setSize(sf::Vector2f(SIDEBAR_WIDTH - 50, 20));
            highlight.setPosition(BOARD_WIDTH + 20, hy - 2);
            highlight.setFillColor(sf::Color(80, 120, 60, 100));
            window.draw(highlight);
            
            moveText.setFillColor(ACCENT_COLOR);
            moveText.setStyle(sf::Text::Bold);
        } else {
            moveText.setFillColor(TEXT_COLOR);
        }
        
        window.draw(moveText);
        
        // Enregistrer la zone cliquable (en coordonnées globales)
        sf::FloatRect clickArea(BOARD_WIDTH + 20, hy - 2, SIDEBAR_WIDTH - 50, 20);
        moveClickAreas.push_back(clickArea);
        
        hy += 22;
    }
    
    // Restaurer la vue par défaut
    window.setView(window.getDefaultView());
    
    // Dessiner une scrollbar si nécessaire
    if (static_cast<int>(moves.size()) > maxVisibleMoves) {
        float scrollbarHeight = historyPanel.getSize().y - 40;
        float scrollbarThumbHeight = std::max(20.0f, scrollbarHeight * maxVisibleMoves / moves.size());
        int maxScroll = std::max(1, static_cast<int>(moves.size()) - maxVisibleMoves);
        float scrollbarY = 345 + (scrollbarHeight - scrollbarThumbHeight) * historyScrollOffset / maxScroll;
        
        // Piste du scrollbar
        scrollbar.setSize(sf::Vector2f(10, scrollbarHeight));
        scrollbar.setPosition(BOARD_WIDTH + SIDEBAR_WIDTH - 30, 345);
        scrollbar.setFillColor(sf::Color(60, 60, 60, 150));
        window.draw(scrollbar);
        
        // Thumb du scrollbar (draggable)
        scrollThumb.setSize(sf::Vector2f(10, scrollbarThumbHeight));
        scrollThumb.setPosition(BOARD_WIDTH + SIDEBAR_WIDTH - 30, scrollbarY);
        scrollThumb.setFillColor(isDraggingScrollbar ? sf::Color(100, 180, 100) : ACCENT_COLOR);
        window.draw(scrollThumb);
    }
    
    // Boutons de navigation
    window.draw(navButtonBack);
    window.draw(navButtonForward);
    
    sf::Text backText("<", font, 18);
    backText.setPosition(BOARD_WIDTH + 38, WINDOW_HEIGHT - 42);
    backText.setFillColor(TEXT_COLOR);
    window.draw(backText);
    
    sf::Text forwardText(">", font, 18);
    forwardText.setPosition(BOARD_WIDTH + 108, WINDOW_HEIGHT - 42);
    forwardText.setFillColor(TEXT_COLOR);
    window.draw(forwardText);
    
    // Indicateur si on est en mode visualisation
    if (isViewingHistory) {
        sf::Text viewingText("Mode visualisation", font, 12);
        viewingText.setPosition(BOARD_WIDTH + 160, WINDOW_HEIGHT - 40);
        viewingText.setFillColor(sf::Color::Yellow);
        window.draw(viewingText);
    }
}

} // namespace Jr