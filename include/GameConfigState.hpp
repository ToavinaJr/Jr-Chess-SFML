#pragma once

#include "GameState.hpp"
#include "Button.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp"
#include "PlayingState.hpp"
#include <SFML/Graphics.hpp>

namespace Jr {

/**
 * @class GameConfigState
 * @brief État pour configurer une nouvelle partie (mode, camp, temps, difficulté)
 */
class GameConfigState : public GameState {
private:
    FontManager& fontManager;
    TextureManager& textureManager;
    
    // Boutons pour les modes de jeu
    Button btnHumanVsHuman;
    Button btnHumanVsAI;
    Button btnAIVsHuman;
    
    // Boutons pour le choix du camp (si vs IA)
    Button btnWhite;
    Button btnBlack;
    Button btnRandom;
    
    // Boutons pour la difficulté de l'IA
    Button btnEasy;
    Button btnMedium;
    Button btnHard;
    
    // Boutons pour la cadence
    Button btn1min;
    Button btn3min;
    Button btn5min;
    Button btn10min;
    Button btn15min;
    
    // Bouton pour démarrer
    Button btnStart;
    Button btnBack;
    
    // Configuration sélectionnée
    PlayingState::GameMode selectedMode = PlayingState::GameMode::HumanVsHuman;
    PlayingState::PlayerSide selectedSide = PlayingState::PlayerSide::White;
    int selectedDepth = 3; // 1=facile, 3=moyen, 5=difficile
    float selectedTime = 600.0f; // 10 minutes par défaut
    
    // Textes
    std::vector<sf::Text> labels;

public:
    GameConfigState(StateManager& manager, sf::RenderWindow& win, FontManager& fm, TextureManager& tm);
    ~GameConfigState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
    void onEnter() override;
};

} // namespace Jr
