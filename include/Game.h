#pragma once
#include "GameState.h"
#include "Menu.h"
#include "LevelManager.h" // ДОБАВЛЯЕМ
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
    Menu levelSelectMenu; // ДОБАВЛЯЕМ МЕНЮ ВЫБОРА УРОВНЯ
    LevelManager levelManager; // ЗАМЕНЯЕМ Level НА LevelManager
    
    void processEvents();
    void update(float deltaTime);
    void render();
    
    void changeState(GameState newState);
    void setupMenus();
    void setupLevelSelectMenu(); // ДОБАВЛЯЕМ НОВЫЙ МЕТОД

    void handleGameEvents(const sf::Event& event);
    void handleContinuousInput(); 
    void checkLevelCompletion(); // ДОБАВЛЯЕМ ПРОВЕРКУ ЗАВЕРШЕНИЯ УРОВНЯ
};