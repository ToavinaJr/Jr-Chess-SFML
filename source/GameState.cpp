#include "../include/GameState.hpp"
#include "../include/StateManager.hpp" // Inclus pour la définition complète de StateManager

namespace Jr {

GameState::GameState(StateManager& manager, sf::RenderWindow& win)
    : stateManager(manager), window(win) {
    
}


} // namespace Jr