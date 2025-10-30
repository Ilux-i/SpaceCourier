#pragma once
#include "GameState.h"
#include "Menu.h"
#include "Level.h"
#include <SFML/Graphics.hpp>

class Game {
public:
    Game();
    
    void run();
    
private:
    sf::RenderWindow window;
    GameState currentState;
    GameState previousState;
    
    Menu mainMenu;
    Menu pauseMenu;
    Level currentLevel;
    
    void processEvents();
    void update(float deltaTime);
    void render();
    
    void changeState(GameState newState);
    void setupMenus();

    void handleGameEvents(const sf::Event& event);
    void handleContinuousInput(); 
};