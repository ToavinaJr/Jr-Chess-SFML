cmake_minimum_required(VERSION 3.10)
project(Chess)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
include_directories(include)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
add_executable(Chess
    source/main.cpp
    source/Board.cpp
    source/ChessLogicManager.cpp
    source/FontManager.cpp
    source/TextureManager.cpp
)
target_link_libraries(Chess sfml-graphics sfml-window sfml-system)
