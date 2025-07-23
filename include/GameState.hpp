#pragma once

#include <SFML/Graphics.hpp>
#include <memory> // Pour std::unique_ptr

namespace Jr {

// Déclaration anticipée du StateManager pour éviter les dépendances circulaires
class StateManager;

/**
 * @brief Classe de base abstraite pour tous les états du jeu.
 *
 * Chaque état concret (Menu, Jeu, Aide, etc.) doit hériter de cette classe
 * et implémenter ses méthodes virtuelles pures.
 */
class GameState {
protected:
    // Référence au StateManager pour permettre aux états de changer d'état.
    StateManager& stateManager;
    sf::RenderWindow& window;

public:
    /**
     * @brief Constructeur de GameState.
     * @param manager Référence au StateManager.
     * @param win Référence à la fenêtre de rendu SFML.
     */
    GameState(StateManager& manager, sf::RenderWindow& win);

    /**
     * @brief Destructeur virtuel pour assurer la bonne destruction des classes dérivées.
     */
    virtual ~GameState() = default;

    /**
     * @brief Gère les événements spécifiques à cet état.
     * @param event L'événement SFML à traiter.
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Met à jour la logique de l'état.
     * @param deltaTime Le temps écoulé depuis la dernière mise à jour.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Dessine les éléments de l'état sur la fenêtre.
     */
    virtual void draw() = 0;

    /**
     * @brief Appelé lors de l'entrée dans cet état.
     * Peut être utilisé pour initialiser des ressources spécifiques à l'état.
     */
    virtual void onEnter() {}

    /**
     * @brief Appelé lors de la sortie de cet état.
     * Peut être utilisé pour nettoyer des ressources spécifiques à l'état.
     */
    virtual void onExit() {}
};

} // namespace Jr