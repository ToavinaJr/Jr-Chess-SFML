#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory> // Pour std::unique_ptr

#include "TextureManager.hpp"
#include "FontManager.hpp"
#include "ChessLogic.hpp"
#include "Board.hpp"
#include "constants.hpp"
#include "StateManager.hpp"
#include "MenuState.hpp"
#include "PlayingState.hpp"
#include "HelpState.hpp"
#include "AboutState.hpp"

namespace Jr {

/**
 * @class Game
 * @brief Classe principale orchestrant l'exécution du jeu d'échecs.
 *
 * La classe `Game` est le point d'entrée principal de l'application.  
 * Elle est responsable de :
 * - L'initialisation de la fenêtre SFML (`sf::RenderWindow`).
 * - Le chargement et la gestion des assets (textures, polices).
 * - La gestion de la boucle de jeu (événements, mise à jour, rendu).
 * - La délégation des différentes logiques d'affichage et d'interaction
 *   à travers un système d'états (`StateManager`).
 *
 * @note Cette classe utilise un **State Pattern** via `StateManager` :
 * - L'état initial est `MenuState`.
 * - D'autres états possibles incluent `PlayingState`, `HelpState` et `AboutState`.
 */
class Game {
private:
    sf::RenderWindow window;      ///< Fenêtre principale du jeu.
    TextureManager textureManager;///< Gestionnaire des textures (chargement et cache).
    FontManager fontManager;      ///< Gestionnaire des polices (chargement et cache).
    StateManager stateManager;    ///< Gestionnaire d'états pour naviguer entre les différents écrans.

    /**
     * @brief Charge tous les assets nécessaires (textures, polices).
     * 
     * Cette méthode est appelée une seule fois au lancement du jeu
     * pour précharger les ressources afin d'éviter les retards pendant la partie.
     */
    void loadAssets();

    /**
     * @brief Gère les événements SFML.
     *
     * Interprète les actions de l'utilisateur (clics, clavier, fermeture de fenêtre)
     * et les délègue à l'état courant via `stateManager`.
     */
    void handleEvents();

    /**
     * @brief Met à jour la logique du jeu.
     *
     * Appelle la méthode `update()` de l'état actuel géré par `stateManager`.
     *
     * @param deltaTime Temps écoulé (en secondes) depuis la dernière frame.
     */
    void update(float deltaTime);

    /**
     * @brief Rendu graphique de la scène.
     *
     * Efface l'écran, demande à l'état courant d'afficher ses éléments, puis affiche la nouvelle frame.
     */
    void render();

public:
    /**
     * @brief Constructeur par défaut.
     *
     * Initialise la fenêtre de jeu, configure le `StateManager`
     * et définit l'état initial (`MenuState`).
     */
    Game();

    /**
     * @brief Lance la boucle principale du jeu.
     *
     * Cette fonction exécute la boucle infinie tant que la fenêtre est ouverte :
     * - Gestion des événements.
     * - Mise à jour de la logique.
     * - Rendu graphique.
     *
     * @return Un code de sortie (0 si tout s'est bien déroulé).
     */
    int run();
};

} // namespace Jr
