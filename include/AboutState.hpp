#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "Button.hpp"
#include <string>
#include <memory>

namespace Jr {

class AboutState : public GameState {
private:
    sf::Text titleText;
    FontManager& fontManager;

    sf::RectangleShape backgroundShape;
    sf::RectangleShape titleSeparator;

    std::unique_ptr<Button> backButton;

    // Textes en bas de page (footer)
    sf::Text footer1; 
    sf::Text footer2; 
    sf::Text footer3; 
    sf::Text footer4; 

    void centerText(sf::Text& text, float yOffset);

public:
    AboutState(StateManager& manager, sf::RenderWindow& win, FontManager& fm);
    ~AboutState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
    void onEnter() override;
};

} // namespace Jr
