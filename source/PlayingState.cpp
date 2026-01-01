#include "../include/PlayingState.hpp"
#include "../include/StateManager.hpp"
#include "../include/ChessLogic.hpp"
#include "../include/GameOverState.hpp"

#include <iostream>

namespace Jr {

PlayingState::PlayingState(StateManager& manager, sf::RenderWindow& win, TextureManager& tm, FontManager& fm)
    : GameState(manager, win),
      textureManager(tm), // Références aux managers globaux
      fontManager(fm),
      chessLogic(), // Initialise la logique du jeu pour cette partie
      board(textureManager, fontManager, chessLogic) // Le board est construit avec les managers et la logique
{
    // Le constructeur ne fait pas le setup complet, on le fait dans onEnter.
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
    
    std::cout << "Entering PlayingState." << std::endl;
}

void PlayingState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        float mx = event.mouseButton.x;
        float my = event.mouseButton.y;
        
        // Bouton retour
        if (navButtonBack.getGlobalBounds().contains(mx, my)) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx > 0) {
                chessLogic.restoreSnapshot(idx - 1);
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
                board.updatePieceSprites();
                isViewingHistory = (idx + 1 < chessLogic.getSnapshotCount() - 1);
            }
            return;
        }
        
        // Clic sur le plateau (seulement si on est à la position actuelle)
        if (!isViewingHistory) {
            board.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
        }
    }
    
    // Navigation clavier
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx > 0) {
                chessLogic.restoreSnapshot(idx - 1);
                board.updatePieceSprites();
                isViewingHistory = (idx - 1 < chessLogic.getSnapshotCount() - 1);
            }
        } else if (event.key.code == sf::Keyboard::Right) {
            int idx = chessLogic.getCurrentSnapshotIndex();
            if (idx + 1 < chessLogic.getSnapshotCount()) {
                chessLogic.restoreSnapshot(idx + 1);
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
    
    float hy = 345;
    int displayStart = std::max(0, static_cast<int>(moves.size()) - 12);
    
    for (size_t i = displayStart; i < moves.size() && i < displayStart + 12; ++i) {
        sf::Text moveText;
        moveText.setFont(font);
        moveText.setCharacterSize(14);
        
        std::string moveStr = std::to_string(i + 1) + ". " + moves[i];
        moveText.setString(moveStr);
        moveText.setPosition(BOARD_WIDTH + 25, hy);
        
        // Highlight le coup actuel
        if (static_cast<int>(i) == currentIdx - 1) {
            moveText.setFillColor(ACCENT_COLOR);
            moveText.setStyle(sf::Text::Bold);
        } else {
            moveText.setFillColor(TEXT_COLOR);
        }
        
        window.draw(moveText);
        hy += 22;
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