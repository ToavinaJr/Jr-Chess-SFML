#include "../include/GameState.hpp"
#include "../include/StateManager.hpp" 

namespace Jr {

GameState::GameState(StateManager& manager, sf::RenderWindow& win)
    : stateManager(manager), window(win) {
    
}


} // namespace Jr