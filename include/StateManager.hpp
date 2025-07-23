#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory> // Pour std::unique_ptr

#include "GameState.hpp" // Déclaration de GameState

namespace Jr {

/**
 * @brief Gère les différents états du jeu (Menu, Jouer, Aide, Auteur).
 *
 * Le StateManager maintient une pile d'états de jeu. Seul l'état au sommet
 * de la pile est actif et reçoit les entrées, est mis à jour et dessiné.
 */
class StateManager {
private:
    std::vector<std::unique_ptr<GameState>> states; // Pile d'états
    sf::RenderWindow& window; // Référence à la fenêtre de rendu

public:
    /**
     * @brief Constructeur du StateManager.
     * @param win Référence à la fenêtre de rendu SFML.
     */
    explicit StateManager(sf::RenderWindow& win);

    /**
     * @brief Ajoute un nouvel état au sommet de la pile et l'active.
     * @tparam TType Le type de l'état à ajouter (doit hériter de GameState).
     * @tparam TArgs Les types des arguments pour le constructeur de l'état.
     * @param args Les arguments pour le constructeur de l'état.
     */
    template <typename TType, typename... TArgs>
    void pushState(TArgs&&... args) {
        if (!states.empty()) {
            states.back()->onExit(); // Appelle onExit de l'état précédent
        }
        // Crée une nouvelle instance de l'état et l'ajoute à la pile.
        states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
        states.back()->onEnter(); // Appelle onEnter du nouvel état
    }

    /**
     * @brief Supprime l'état du sommet de la pile.
     * Si la pile devient vide, le jeu peut se terminer.
     */
    void popState();

    /**
     * @brief Remplace l'état actuel par un nouvel état.
     * @tparam TType Le type du nouvel état (doit hériter de GameState).
     * @tparam TArgs Les types des arguments pour le constructeur de l'état.
     * @param args Les arguments pour le constructeur de l'état.
     */
    template <typename TType, typename... TArgs>
    void changeState(TArgs&&... args) {
        if (!states.empty()) {
            states.back()->onExit(); // Appelle onExit de l'état actuel
            states.pop_back();       // Supprime l'état actuel
        }
        // Ajoute le nouvel état.
        states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
        states.back()->onEnter(); // Appelle onEnter du nouvel état
    }

    /**
     * @brief Gère les événements pour l'état actif.
     * @param event L'événement SFML à traiter.
     */
    void handleInput(const sf::Event& event);

    /**
     * @brief Met à jour la logique pour l'état actif.
     * @param deltaTime Le temps écoulé depuis la dernière mise à jour.
     */
    void update(float deltaTime);

    /**
     * @brief Dessine l'état actif.
     */
    void draw();

    /**
     * @brief Vérifie si la pile d'états est vide.
     * @return True si la pile est vide, False sinon.
     */
    bool isEmpty() const;
};

} // namespace Jr