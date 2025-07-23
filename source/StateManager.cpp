#include "../include/StateManager.hpp"
#include <iostream>

namespace Jr {

StateManager::StateManager(sf::RenderWindow& win)
    : window(win) {
}

void StateManager::popState() {
    if (!states.empty()) {
        states.back()->onExit(); // Appelle onExit de l'état sortant
        states.pop_back();       // Supprime l'état du sommet
        if (!states.empty()) {
            states.back()->onEnter(); // Appelle onEnter de l'état sur lequel on revient
        }
    }
}

void StateManager::handleInput(const sf::Event& event) {
    if (!states.empty()) {
        states.back()->handleInput(event); // Délègue la gestion des entrées à l'état actif
    }
}

void StateManager::update(float deltaTime) {
    if (!states.empty()) {
        states.back()->update(deltaTime); // Délègue la mise à jour à l'état actif
    }
}

void StateManager::draw() {
    if (!states.empty()) {
        states.back()->draw(); // Délègue le dessin à l'état actif
    }
}

bool StateManager::isEmpty() const {
    return states.empty();
}

} // namespace Jr