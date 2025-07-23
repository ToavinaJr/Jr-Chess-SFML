#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory> // Pour std::unique_ptr

#include "GameState.hpp" // Déclaration de GameState

namespace Jr {

/**
 * @class StateManager
 * @brief Gère la pile des états du jeu (menu, partie, aide, etc.).
 * 
 * Cette classe maintient une pile d'objets dérivés de GameState.
 * Seul l'état au sommet de la pile est actif : il reçoit les entrées, 
 * est mis à jour et dessiné.
 */
class StateManager {
private:
    /// Pile des états actifs, gérés via std::unique_ptr
    std::vector<std::unique_ptr<GameState>> states;

    /// Référence à la fenêtre SFML de rendu
    sf::RenderWindow& window;

public:
    /**
     * @brief Constructeur du StateManager.
     * @param win Référence à la fenêtre SFML utilisée pour le rendu.
     */
    explicit StateManager(sf::RenderWindow& win);

    /**
     * @brief Ajoute un nouvel état au sommet de la pile et l'active.
     * 
     * Si un état est déjà actif, sa méthode onExit() est appelée avant de
     * pousser le nouvel état. Le nouvel état appelle sa méthode onEnter().
     * 
     * @tparam TType Type de l'état à créer (doit hériter de GameState).
     * @tparam TArgs Types des arguments à passer au constructeur de l'état.
     * @param args Arguments du constructeur du nouvel état.
     */
    template <typename TType, typename... TArgs>
    void pushState(TArgs&&... args) {
        if (!states.empty()) {
            states.back()->onExit(); ///< Notifie l'état actuel qu'il quitte l'activation
        }
        states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
        states.back()->onEnter(); ///< Notifie le nouvel état qu'il est activé
    }

    /**
     * @brief Supprime l'état au sommet de la pile.
     * 
     * Si la pile devient vide après suppression, le jeu peut se terminer.
     */
    void popState();

    /**
     * @brief Remplace l'état actif actuel par un nouvel état.
     * 
     * Appelle onExit() de l'état actuel puis le supprime, puis pousse le nouvel état
     * et appelle sa méthode onEnter().
     * 
     * @tparam TType Type du nouvel état (doit hériter de GameState).
     * @tparam TArgs Types des arguments du constructeur du nouvel état.
     * @param args Arguments du constructeur.
     */
    template <typename TType, typename... TArgs>
    void changeState(TArgs&&... args) {
        if (!states.empty()) {
            states.back()->onExit();
            states.pop_back();
        }
        states.push_back(std::make_unique<TType>(*this, window, std::forward<TArgs>(args)...));
        states.back()->onEnter();
    }

    /**
     * @brief Délègue la gestion des événements à l'état actif.
     * @param event Événement SFML à traiter.
     */
    void handleInput(const sf::Event& event);

    /**
     * @brief Met à jour la logique du jeu dans l'état actif.
     * @param deltaTime Temps écoulé depuis la dernière mise à jour (en secondes).
     */
    void update(float deltaTime);

    /**
     * @brief Dessine l'état actif sur la fenêtre.
     */
    void draw();

    /**
     * @brief Indique si la pile d'états est vide.
     * @return true si aucun état n'est présent, false sinon.
     */
    bool isEmpty() const;
};

} // namespace Jr
