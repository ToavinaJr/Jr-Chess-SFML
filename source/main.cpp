#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../include/constants.hpp"
#include "../include/TextureManager.hpp"
#include "../include/FontManager.hpp"
#include "../include/ChessLogicManager.hpp"
#include "../include/Board.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(Jr::WINDOW_WIDTH, Jr::WINDOW_HEIGHT), Jr::WINDOW_TITLE);
    window.setFramerateLimit(60);

    // Instancier les managers
    Jr::TextureManager textureManager;
    Jr::FontManager fontManager;
    Jr::ChessLogic chessLogic;

    // Précharger les textures (si vous voulez)
    std::vector<std::string> pieceTexturePaths = {
        "../assets/images/pieces/chess_maestro_bw/wP.png",
        "../assets/images/pieces/chess_maestro_bw/wR.png",
        "../assets/images/pieces/chess_maestro_bw/wN.png",
        "../assets/images/pieces/chess_maestro_bw/wB.png",
        "../assets/images/pieces/chess_maestro_bw/wQ.png",
        "../assets/images/pieces/chess_maestro_bw/wK.png",
        "../assets/images/pieces/chess_maestro_bw/bP.png",
        "../assets/images/pieces/chess_maestro_bw/bR.png",
        "../assets/images/pieces/chess_maestro_bw/bN.png",
        "../assets/images/pieces/chess_maestro_bw/bB.png",
        "../assets/images/pieces/chess_maestro_bw/bQ.png",
        "../assets/images/pieces/chess_maestro_bw/bK.png"
    };
    try {
        textureManager.preloadTextures(pieceTexturePaths);
        fontManager.getFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"); // Précharger la police
    } catch (const std::runtime_error& e) {
        std::cerr << "Erreur de chargement des assets: " << e.what() << std::endl;
        return 1; // Quitter si les assets ne peuvent pas être chargés
    }
    
    // Initialiser la logique du jeu
    chessLogic.initializeBoard();

    // Créer le Board qui utilise les managers
    Jr::Board board(textureManager, fontManager, chessLogic);
    
    while( window.isOpen() ) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                board.handleMouseClick(event.mouseButton.x, event.mouseButton.y); // Appel direct de la méthode du Board
                std::cout <<"Click " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
            }
        }
        
        board.draw(window);
        window.display();
    }
    
    return 0;
}