#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "../include/constants.hpp"
#include "../include/Board.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(Jr::WINDOW_WIDTH, Jr::WINDOW_HEIGHT), Jr::WINDOW_TITLE);
    auto board = std::make_unique<Jr::Board>();

    while( window.isOpen() ) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                board->handleClick(event.mouseButton.x, event.mouseButton.y);
                std::cout <<"Click " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
            }
        }

        window.clear(sf::Color::Black);
        board->draw(window);
        window.display();
    }
    
    return 0;
}