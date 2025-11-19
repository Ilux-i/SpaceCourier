#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "LevelManager.h"
#include "InputHandler.h"
#include "SoundSystem.h"
#include "OptionsMenu.h"
#include "VictoryMenu.h"

enum class GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    LEVEL_SELECT,
    OPTIONS,
    VICTORY,
    EXIT
};

class Game {
public:
    Game();
    void run();
    
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void changeState(GameState newState);
    
    void setupMenus();
    void setupLevelSelectMenu();
    void setupOptionsMenu();
    void setupVictoryMenu();
    void loadNextLevel();
    
    void handleGameEvents(const sf::Event& event);
    void handleContinuousInput();
    void checkLevelCompletion();
    
    sf::RenderWindow window;
    GameState currentState;
    GameState previousState;
    
    Menu mainMenu;
    Menu pauseMenu;
    Menu levelSelectMenu;
    OptionsMenu optionsMenu;
    VictoryMenu victoryMenu;
    
    LevelManager levelManager;
    InputHandler inputHandler;
    SoundSystem soundSystem;
};