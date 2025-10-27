#include "Core/Game.h"
#include <iostream>
#include <exception>

int main() {
    try {
        std::cout << "========================================" << std::endl;
        std::cout << "  PIXELRACER - Alpha 0.1" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;

        Game game;
        game.run();

        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Game exited successfully!" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}