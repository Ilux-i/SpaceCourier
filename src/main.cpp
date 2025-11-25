#include "Game.h"
#include <iostream>

int main() {
    try {
        Game game;
        game.run();
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ Unknown exception occurred!" << std::endl;
        return 1;
    }
    
    return 0;
}