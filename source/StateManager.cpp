#include "../include/StateManager.hpp"

namespace Jr {

StateManager::StateManager(sf::RenderWindow& win)
    : window(win) {}

void StateManager::popState() {
    if (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
    }
    if (states.empty()) {
        window.close();
    }
}

void StateManager::handleInput(const sf::Event& event) {
    if (!states.empty()) {
        states.back()->handleInput(event);
    }
}

void StateManager::update(float deltaTime) {
    if (!states.empty()) {
        states.back()->update(deltaTime);
    }
}

void StateManager::draw() {
    if (!states.empty()) {
        window.clear();
        states.back()->draw();
        window.display();
    }
}

bool StateManager::isEmpty() const {
    return states.empty();
}

GameState* StateManager::getCurrentState() {
    return states.empty() ? nullptr : states.back().get();
}

} // namespace Jr
