#include <iostream>
#include "../include/Game.hpp" // Incluez la nouvelle classe Game

int main() {
    try {
        Jr::Game game; // Crée une instance de votre jeu
        return game.run(); // Lance la boucle de jeu
    } catch (const std::runtime_error& e) {
        // Attrape les erreurs critiques (comme l'échec de chargement des assets)
        std::cerr << "Une erreur inattendue s'est produite: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        // Attrape toute autre exception non gérée
        std::cerr << "Une erreur inconnue s'est produite." << std::endl;
        return 1;
    }
}