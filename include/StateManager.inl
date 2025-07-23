#pragma once

#include <memory>

namespace Jr {

template <typename TType, typename... TArgs>
void StateManager::pushState(TArgs&&... args) {
    if (!states.empty()) {
        states.back()->onExit();
    }
    states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
    states.back()->onEnter();
}

template <typename TType, typename... TArgs>
void StateManager::changeState(TArgs&&... args) {
    if (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
    }
    states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
    states.back()->onEnter();
}

} // namespace Jr
