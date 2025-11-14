#pragma once
#include "GameState.h"
#include "Menu.h"
#include "LevelManager.h"
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
    Menu levelSelectMenu;
    LevelManager levelManager;
    
    void processEvents();
    void update(float deltaTime);
    void render();
    
    void changeState(GameState newState);
    void setupMenus();
    void setupLevelSelectMenu();

    void handleGameEvents(const sf::Event& event);
    void handleContinuousInput(); 
    void checkLevelCompletion();
};