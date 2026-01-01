cmake_minimum_required(VERSION 3.10)
project(Chess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Assure-toi que le dossier 'include' est bien recherché pour les en-têtes
include_directories(include)

# Copier les assets vers le répertoire de build
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Définir toutes les sources une seule fois
set(SOURCES
    source/main.cpp
    source/Board.cpp
    source/ChessLogic.cpp
    source/FontManager.cpp
    source/TextureManager.cpp
    source/Game.cpp
    source/StateManager.cpp
    source/GameState.cpp
    source/MenuState.cpp
    source/PlayingState.cpp
    source/HelpState.cpp
    source/AboutState.cpp
    source/Button.cpp
    source/GameOverState.cpp
    source/AIPlayer.cpp
    source/GameConfigState.cpp
)

# Créer l'exécutable avec toutes les sources définies
add_executable(Chess ${SOURCES})

# Lier les bibliothèques SFML nécessaires
target_link_libraries(Chess sfml-graphics sfml-window sfml-system)