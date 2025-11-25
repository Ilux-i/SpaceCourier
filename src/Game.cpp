#include "Game.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode({1200, 800}), sf::String("Space Courier"))
    , currentState(GameState::MAIN_MENU)
    , previousState(GameState::MAIN_MENU)
{
    window.setFramerateLimit(60);
    
    if (soundSystem.initialize()) {
        soundSystem.playMusic(MusicType::MAIN_MENU);
    }
    
    setupMenus();
    setupLevelSelectMenu();
    setupOptionsMenu();
    setupVictoryMenu();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen() && currentState != GameState::EXIT) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update(deltaTime);
        render();
    } 
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        switch (currentState) {
            case GameState::MAIN_MENU:
                mainMenu.handleEvent(*event, window);
                break;
            case GameState::PAUSED:
                pauseMenu.handleEvent(*event, window);
                break;
            case GameState::PLAYING:
                handleGameEvents(*event);
                break;
            case GameState::LEVEL_SELECT:
                levelSelectMenu.handleEvent(*event, window);
                break;
            case GameState::OPTIONS:
                optionsMenu.handleEvent(*event, window);
                break;
            case GameState::VICTORY:
                victoryMenu.handleEvent(*event, window);
                break;
            case GameState::EXIT:
                window.close();
                break;
        }
    }
}

void Game::update(float deltaTime) {
    soundSystem.update(deltaTime);

    switch (currentState) {
        case GameState::MAIN_MENU:
            mainMenu.update(deltaTime);
            break;
        case GameState::PAUSED:
            pauseMenu.update(deltaTime);
            break;
        case GameState::PLAYING:
            handleContinuousInput();
            levelManager.getCurrentLevel().update(deltaTime);
            checkLevelCompletion();
            break;
        case GameState::LEVEL_SELECT:
            levelSelectMenu.update(deltaTime);
            break;
        case GameState::OPTIONS:
            optionsMenu.update(deltaTime);
            break;
        case GameState::VICTORY:
            victoryMenu.update(deltaTime);
            break;
        case GameState::EXIT:
            break;
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 40));
    
    switch (currentState) {
        case GameState::MAIN_MENU:
            mainMenu.draw(window);
            break;
        case GameState::PAUSED:
            levelManager.getCurrentLevel().draw(window);
            pauseMenu.draw(window);
            break;
        case GameState::PLAYING:
            levelManager.getCurrentLevel().draw(window);
            break;
        case GameState::LEVEL_SELECT:
            levelSelectMenu.draw(window);
            break;
        case GameState::OPTIONS:
            optionsMenu.draw(window);
            break;
        case GameState::VICTORY:
            victoryMenu.draw(window);
            break;
        case GameState::EXIT:
            break;
    }
    
    window.display();
}

void Game::changeState(GameState newState) {
    previousState = currentState;
    currentState = newState;
    
    switch (newState) {
        case GameState::MAIN_MENU:
            soundSystem.playMusic(MusicType::MAIN_MENU);
            break;
        case GameState::PLAYING:
            {
                int level = levelManager.getCurrentLevelNumber();
                MusicType musicType = static_cast<MusicType>(static_cast<int>(MusicType::LEVEL_1) + level - 1);
                soundSystem.playMusic(musicType);
            }
            break;
        case GameState::LEVEL_SELECT:
            soundSystem.playMusic(MusicType::MAIN_MENU);
            break;
        case GameState::VICTORY:
            soundSystem.playMusic(MusicType::VICTORY);
            break;
        case GameState::PAUSED:
            soundSystem.pauseMusic();
            break;
        default:
            break;
    }
    
}

void Game::setupMenus() {
    mainMenu.setTitle("SPACE COURIER");
    mainMenu.clearButtons();
    
    mainMenu.addButton("START GAME", [this]() {
        int lastLevel = levelManager.getLastUnlockedLevel();
        levelManager.loadLevel(lastLevel);
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 300));
    
    mainMenu.addButton("LEVEL SELECT", [this]() {
        setupLevelSelectMenu();
        changeState(GameState::LEVEL_SELECT);
    }, sf::Vector2f(450, 380));
    
    mainMenu.addButton("OPTIONS", [this]() {
        changeState(GameState::OPTIONS);
    }, sf::Vector2f(450, 460));
    
    mainMenu.addButton("EXIT GAME", [this]() {
        changeState(GameState::EXIT);
    }, sf::Vector2f(450, 540));
    
    pauseMenu.setTitle("GAME PAUSED");
    pauseMenu.clearButtons();
    pauseMenu.addButton("RESUME GAME", [this]() {
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 300));
    
    pauseMenu.addButton("RESTART LEVEL", [this]() {
        int currentLevel = levelManager.getCurrentLevelNumber();
        levelManager.loadLevel(currentLevel);
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 380));
    
    pauseMenu.addButton("MAIN MENU", [this]() {
        changeState(GameState::MAIN_MENU);
    }, sf::Vector2f(450, 460));
    
    pauseMenu.addButton("EXIT GAME", [this]() {
        changeState(GameState::EXIT);
    }, sf::Vector2f(450, 540));

    victoryMenu.setMainMenuCallback([this]() {
        changeState(GameState::MAIN_MENU);
    });

    victoryMenu.setExitCallback([this]() {
        changeState(GameState::EXIT);
    });
}

void Game::setupLevelSelectMenu() {
    levelSelectMenu.setTitle("SELECT LEVEL");
    levelSelectMenu.clearButtons();
    
    int unlockedLevels = levelManager.getUnlockedLevels();
    
    for (int i = 1; i <= 5; ++i) {
        if (i <= unlockedLevels) {
            levelSelectMenu.addButton("LEVEL " + std::to_string(i), [this, i]() {
                levelManager.loadLevel(i);
                changeState(GameState::PLAYING);
            }, sf::Vector2f(450, 150 + i * 70));
        } else {
            levelSelectMenu.addButton("LOCKED", []() {
            }, sf::Vector2f(450, 150 + i * 70)); 
        }
    }
    
    levelSelectMenu.addButton("BACK", [this]() {
        changeState(GameState::MAIN_MENU);
    }, sf::Vector2f(450, 600));
}

void Game::handleGameEvents(const sf::Event& event) {
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::E) {
            levelManager.getCurrentLevel().handleEInteraction();
        }
        
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            changeState(GameState::PAUSED);
        }
    }
}

void Game::handleContinuousInput() {
    bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) || 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    
    bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    
    bool moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) || 
                  sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    
    bool moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) || 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);
    
    sf::Vector2f movement(0.f, 0.f);
    
    if (moveLeft) movement.x -= 1.f;
    if (moveRight) movement.x += 1.f;
    if (moveUp) movement.y -= 1.f;
    if (moveDown) movement.y += 1.f;
    
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    levelManager.getCurrentLevel().getPlayer().move(movement);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)) {
        levelManager.getCurrentLevel().getPlayer().jump();
    }
}

void Game::loadNextLevel() {
    int currentLevel = levelManager.getCurrentLevelNumber();
    int nextLevel = currentLevel + 1;
    
    if (nextLevel <= 5 && nextLevel <= levelManager.getUnlockedLevels()) {
        levelManager.loadLevel(nextLevel);
        changeState(GameState::PLAYING);
    } else {
        setupLevelSelectMenu();
        changeState(GameState::LEVEL_SELECT);
    }
}

void Game::setupVictoryMenu() {
    victoryMenu.setMainMenuCallback([this]() {
        changeState(GameState::MAIN_MENU);
    });
    
    victoryMenu.setExitCallback([this]() {
        changeState(GameState::EXIT);
    });
    
    victoryMenu.setNextLevelCallback([this]() {
        loadNextLevel();
    });
    
}

void Game::checkLevelCompletion() {
    if (levelManager.getCurrentLevel().isLevelComplete()) {
        levelManager.markLevelComplete();
        int completedLevel = levelManager.getCurrentLevelNumber();
        
        victoryMenu.setLevelInfo(completedLevel, 5);
        changeState(GameState::VICTORY);
    }
}

void Game::setupOptionsMenu() {
    optionsMenu.setBackCallback([this]() {
        changeState(previousState);
    });
    
    optionsMenu.setVolumeCallback([this](float volume) {
        soundSystem.setMusicVolume(volume);
    });
    
    optionsMenu.setMusicVolume(soundSystem.getMusicVolume());
}