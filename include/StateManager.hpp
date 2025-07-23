#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "GameState.hpp"

namespace Jr {

class StateManager {
private:
    std::vector<std::unique_ptr<GameState>> states;
    sf::RenderWindow& window;

public:
    explicit StateManager(sf::RenderWindow& win);

    template <typename TType, typename... TArgs>
    void pushState(TArgs&&... args);

    void popState();

    template <typename TType, typename... TArgs>
    void changeState(TArgs&&... args);

    void handleInput(const sf::Event& event);

    void update(float deltaTime);

    void draw();

    bool isEmpty() const;

    GameState* getCurrentState();
};

} // namespace Jr

// Implémentations des fonctions templates à mettre dans le header (en bas)
#include "StateManager.inl"
