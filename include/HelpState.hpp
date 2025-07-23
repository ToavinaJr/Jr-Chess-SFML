#pragma once

#include "GameState.hpp"
#include "FontManager.hpp"
#include "Button.hpp"
#include <vector>
#include <string>
#include <memory>

namespace Jr {

class HelpState : public GameState {
private:
    sf::Text titleText;
    sf::Text contentText;
    FontManager& fontManager;

    std::unique_ptr<Button> backButton;

    void centerText(sf::Text& text, float yOffset);

public:
    HelpState(StateManager& manager, sf::RenderWindow& win, FontManager& fm);
    ~HelpState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void draw() override;
    void onEnter() override;
};

} // namespace Jr
