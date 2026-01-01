#include "../include/GameConfigState.hpp"
#include "../include/StateManager.hpp"
#include "../include/PlayingState.hpp"
#include "../include/constants.hpp"
#include <iostream>

namespace Jr {

GameConfigState::GameConfigState(StateManager& manager, sf::RenderWindow& win, FontManager& fm, TextureManager& tm)
    : GameState(manager, win), fontManager(fm), textureManager(tm),
      btnHumanVsHuman("Humain vs Humain", fm.getFont(FONT_PATH), 16, sf::Vector2f(200, 45), 
                      sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnHumanVsAI("Humain vs IA", fm.getFont(FONT_PATH), 16, sf::Vector2f(200, 45), 
                   sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnAIVsHuman("IA vs IA", fm.getFont(FONT_PATH), 16, sf::Vector2f(200, 45), 
                   sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      
      btnWhite("Blancs", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
               sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnBlack("Noirs", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
               sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnRandom("Aléatoire", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
                sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      
      btnEasy("Facile", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
              sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnMedium("Moyen", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
                sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btnHard("Difficile", fm.getFont(FONT_PATH), 16, sf::Vector2f(120, 38), 
              sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      
      btn1min("1 min", fm.getFont(FONT_PATH), 16, sf::Vector2f(90, 38), 
              sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btn3min("3 min", fm.getFont(FONT_PATH), 16, sf::Vector2f(90, 38), 
              sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btn5min("5 min", fm.getFont(FONT_PATH), 16, sf::Vector2f(90, 38), 
              sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btn10min("10 min", fm.getFont(FONT_PATH), 16, sf::Vector2f(90, 38), 
               sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      btn15min("15 min", fm.getFont(FONT_PATH), 16, sf::Vector2f(90, 38), 
               sf::Color(50, 50, 50), sf::Color(70, 70, 70), sf::Color(100, 150, 100)),
      
      btnStart("Démarrer", fm.getFont(FONT_PATH), 20, sf::Vector2f(180, 50), 
               sf::Color(0, 120, 0), sf::Color(0, 150, 0), sf::Color(0, 180, 0)),
      btnBack("Retour", fm.getFont(FONT_PATH), 18, sf::Vector2f(140, 45), 
              sf::Color(120, 0, 0), sf::Color(150, 0, 0), sf::Color(180, 0, 0))
{
    // Définir les positions des boutons - adaptées à la fenêtre 888x568
    btnHumanVsHuman.setPosition(sf::Vector2f(50, 80));
    btnHumanVsAI.setPosition(sf::Vector2f(280, 80));
    btnAIVsHuman.setPosition(sf::Vector2f(510, 80));
    
    btnWhite.setPosition(sf::Vector2f(50, 180));
    btnBlack.setPosition(sf::Vector2f(190, 180));
    btnRandom.setPosition(sf::Vector2f(330, 180));
    
    btnEasy.setPosition(sf::Vector2f(50, 280));
    btnMedium.setPosition(sf::Vector2f(190, 280));
    btnHard.setPosition(sf::Vector2f(330, 280));
    
    btn1min.setPosition(sf::Vector2f(50, 380));
    btn3min.setPosition(sf::Vector2f(160, 380));
    btn5min.setPosition(sf::Vector2f(270, 380));
    btn10min.setPosition(sf::Vector2f(380, 380));
    btn15min.setPosition(sf::Vector2f(490, 380));
    
    btnStart.setPosition(sf::Vector2f(350, 480));
    btnBack.setPosition(sf::Vector2f(50, 480));
}

void GameConfigState::onEnter() {
    std::cout << "=== Entering GameConfigState ===" << std::endl;
    std::cout << "Initial mode: " << static_cast<int>(selectedMode) << std::endl;
    std::cout << "Initial side: " << static_cast<int>(selectedSide) << std::endl;
    std::cout << "Initial depth: " << selectedDepth << std::endl;
    std::cout << "Initial time: " << selectedTime << std::endl;
    
    // Initialiser les labels
    labels.clear();
    const sf::Font& font = fontManager.getFont(FONT_PATH);
    
    sf::Text titleText("Configuration de la Partie", font, 32);
    titleText.setPosition(280, 20);
    titleText.setFillColor(TEXT_COLOR);
    labels.push_back(titleText);
    
    sf::Text modeLabel("Mode de jeu:", font, 20);
    modeLabel.setPosition(50, 50);
    modeLabel.setFillColor(TEXT_COLOR);
    labels.push_back(modeLabel);
    
    sf::Text timeLabel("Cadence:", font, 20);
    timeLabel.setPosition(50, 350);
    timeLabel.setFillColor(TEXT_COLOR);
    labels.push_back(timeLabel);
    
    // Sélectionner les boutons par défaut
    btnHumanVsHuman.setSelected(true);
    btnWhite.setSelected(true);
    btnMedium.setSelected(true);
    btn10min.setSelected(true);
    
    std::cout << "Default buttons selected" << std::endl;
}

void GameConfigState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.popState();
        return;
    }
    
    // Le bouton isClicked() utilise MouseButtonReleased, donc on doit aussi écouter cet événement
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        std::cout << "[DEBUG] Mouse Released at: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
        
        // Mode de jeu
        if (btnHumanVsHuman.isClicked(event)) {
            std::cout << "[CLICK] HumanVsHuman selected" << std::endl;
            selectedMode = PlayingState::GameMode::HumanVsHuman;
            btnHumanVsHuman.setSelected(true);
            btnHumanVsAI.setSelected(false);
            btnAIVsHuman.setSelected(false);
            std::cout << "Mode changed to: " << static_cast<int>(selectedMode) << std::endl;
        }
        if (btnHumanVsAI.isClicked(event)) {
            std::cout << "[CLICK] HumanVsAI selected" << std::endl;
            selectedMode = PlayingState::GameMode::HumanVsAI;
            btnHumanVsHuman.setSelected(false);
            btnHumanVsAI.setSelected(true);
            btnAIVsHuman.setSelected(false);
            std::cout << "Mode changed to: " << static_cast<int>(selectedMode) << std::endl;
        }
        if (btnAIVsHuman.isClicked(event)) {
            std::cout << "[CLICK] AIvsAI selected" << std::endl;
            selectedMode = PlayingState::GameMode::AIvsAI;
            btnHumanVsHuman.setSelected(false);
            btnHumanVsAI.setSelected(false);
            btnAIVsHuman.setSelected(true);
            std::cout << "Mode changed to: " << static_cast<int>(selectedMode) << std::endl;
        }
        
        // Camp
        if (btnWhite.isClicked(event)) {
            selectedSide = PlayingState::PlayerSide::White;
            btnWhite.setSelected(true);
            btnBlack.setSelected(false);
            btnRandom.setSelected(false);
        }
        if (btnBlack.isClicked(event)) {
            selectedSide = PlayingState::PlayerSide::Black;
            btnWhite.setSelected(false);
            btnBlack.setSelected(true);
            btnRandom.setSelected(false);
        }
        if (btnRandom.isClicked(event)) {
            selectedSide = PlayingState::PlayerSide::Random;
            btnWhite.setSelected(false);
            btnBlack.setSelected(false);
            btnRandom.setSelected(true);
        }
        
        // Difficulté - profondeurs réduites pour éviter les crashs
        if (btnEasy.isClicked(event)) {
            selectedDepth = 1;  // Très rapide
            btnEasy.setSelected(true);
            btnMedium.setSelected(false);
            btnHard.setSelected(false);
        }
        if (btnMedium.isClicked(event)) {
            selectedDepth = 2;  // Raisonnable
            btnEasy.setSelected(false);
            btnMedium.setSelected(true);
            btnHard.setSelected(false);
        }
        if (btnHard.isClicked(event)) {
            selectedDepth = 3;  // Plus lent mais jouable
            btnEasy.setSelected(false);
            btnMedium.setSelected(false);
            btnHard.setSelected(true);
        }
        
        // Temps
        if (btn1min.isClicked(event)) {
            selectedTime = 60.0f;
            btn1min.setSelected(true);
            btn3min.setSelected(false);
            btn5min.setSelected(false);
            btn10min.setSelected(false);
            btn15min.setSelected(false);
        }
        if (btn3min.isClicked(event)) {
            selectedTime = 180.0f;
            btn1min.setSelected(false);
            btn3min.setSelected(true);
            btn5min.setSelected(false);
            btn10min.setSelected(false);
            btn15min.setSelected(false);
        }
        if (btn5min.isClicked(event)) {
            selectedTime = 300.0f;
            btn1min.setSelected(false);
            btn3min.setSelected(false);
            btn5min.setSelected(true);
            btn10min.setSelected(false);
            btn15min.setSelected(false);
        }
        if (btn10min.isClicked(event)) {
            selectedTime = 600.0f;
            btn1min.setSelected(false);
            btn3min.setSelected(false);
            btn5min.setSelected(false);
            btn10min.setSelected(true);
            btn15min.setSelected(false);
        }
        if (btn15min.isClicked(event)) {
            selectedTime = 900.0f;
            btn1min.setSelected(false);
            btn3min.setSelected(false);
            btn5min.setSelected(false);
            btn10min.setSelected(false);
            btn15min.setSelected(true);
        }
        
        // Démarrer
        if (btnStart.isClicked(event)) {
            std::cout << "Starting game: Mode=" << static_cast<int>(selectedMode) 
                      << " Side=" << static_cast<int>(selectedSide)
                      << " Depth=" << selectedDepth 
                      << " Time=" << selectedTime << "s" << std::endl;
            
            // Passer tous les paramètres directement au constructeur
            stateManager.changeState<PlayingState>(textureManager, fontManager, 
                                                   selectedMode, selectedSide, 
                                                   selectedDepth, selectedTime);
            return; // Important: ne rien faire après car 'this' est détruit
        }
        
        // Retour
        if (btnBack.isClicked(event)) {
            stateManager.popState();
            return;
        }
    }
}

void GameConfigState::update(float deltaTime) {
    // Pas de mise à jour nécessaire pour cet état
}

void GameConfigState::draw() {
    window.clear(BACKGROUND_COLOR);
    
    const sf::Font& font = fontManager.getFont(FONT_PATH);
    
    // Dessiner tous les labels de base
    for (const auto& label : labels) {
        window.draw(label);
    }
    
    // Dessiner tous les boutons de mode
    btnHumanVsHuman.draw(window);
    btnHumanVsAI.draw(window);
    btnAIVsHuman.draw(window);  // IA vs IA
    
    // Camp et difficulté (seulement si Humain vs IA)
    if (selectedMode == PlayingState::GameMode::HumanVsAI) {
        sf::Text sideLabel("Votre camp:", font, 20);
        sideLabel.setPosition(50, 150);
        sideLabel.setFillColor(TEXT_COLOR);
        window.draw(sideLabel);
        
        btnWhite.draw(window);
        btnBlack.draw(window);
        btnRandom.draw(window);
    }
    
    // Difficulté IA (pour Humain vs IA et IA vs IA)
    if (selectedMode != PlayingState::GameMode::HumanVsHuman) {
        sf::Text diffLabel("Difficulté IA:", font, 20);
        diffLabel.setPosition(50, 250);
        diffLabel.setFillColor(TEXT_COLOR);
        window.draw(diffLabel);
        
        btnEasy.draw(window);
        btnMedium.draw(window);
        btnHard.draw(window);
    }
    
    // Temps
    btn1min.draw(window);
    btn3min.draw(window);
    btn5min.draw(window);
    btn10min.draw(window);
    btn15min.draw(window);
    
    // Boutons d'action
    btnStart.draw(window);
    btnBack.draw(window);
    
    window.display();
}

} // namespace Jr
