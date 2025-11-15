#include "Game.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode({1200, 800}), sf::String("Space Courier"))
    , currentState(GameState::MAIN_MENU)
    , previousState(GameState::MAIN_MENU)
{
    window.setFramerateLimit(60);
    
    if (!soundSystem.initialize()) {
        std::cout << "⚠️  Звуковая система не загружена, продолжаем без звука" << std::endl;
    }
    
    setupMenus();
    setupLevelSelectMenu();
}

void Game::run() {
    std::cout << "🚀 Запуск игры Space Courier!" << std::endl;
    
    sf::Clock clock;
    while (window.isOpen() && currentState != GameState::EXIT) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update(deltaTime);
        render();
    }
    
    std::cout << "👋 Игра завершена!" << std::endl;
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        // Обработка событий в зависимости от состояния
        switch (currentState) {
            case GameState::MAIN_MENU:
                mainMenu.handleEvent(*event, window);
                break;
            case GameState::PAUSED:
                pauseMenu.handleEvent(*event, window);
                break;
            case GameState::PLAYING:
                // ПЕРЕДАЕМ СОБЫТИЯ В InputHandler ДЛЯ ИГРОВОГО ПРОЦЕССА
                handleGameEvents(*event);
                break;
            case GameState::LEVEL_SELECT:
                levelSelectMenu.handleEvent(*event, window);
                break;
            case GameState::OPTIONS:
                // TODO: Реализовать позже
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
        case GameState::EXIT:
            break;
    }
    
    window.display();
}

void Game::changeState(GameState newState) {
    previousState = currentState;
    currentState = newState;
    
    // Переключаем музыку в зависимости от состояния
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
            soundSystem.playMusic(MusicType::MAIN_MENU);  // Или специальная музыка для выбора уровня
            break;
        case GameState::PAUSED:
            soundSystem.pauseMusic();
            break;
        default:
            break;
    }
    
    std::cout << "🔄 Смена состояния: " << static_cast<int>(previousState) 
              << " -> " << static_cast<int>(currentState) << std::endl;
}

void Game::setupMenus() {
    // Настройка главного меню
    mainMenu.setTitle("SPACE COURIER");
    mainMenu.clearButtons();
    mainMenu.addButton("START GAME", [this]() {
        std::cout << "🎮 Запуск игры!" << std::endl;
        levelManager.loadLevel(1);
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 300));
    
    mainMenu.addButton("LEVEL SELECT", [this]() {
        std::cout << "🗂️ Выбор уровня" << std::endl;
        setupLevelSelectMenu();
        changeState(GameState::LEVEL_SELECT);
    }, sf::Vector2f(450, 380));
    
    mainMenu.addButton("OPTIONS", [this]() {
        std::cout << "⚙️ Настройки" << std::endl;
        // TODO: Реализовать позже
    }, sf::Vector2f(450, 460));
    
    mainMenu.addButton("EXIT GAME", [this]() {
        std::cout << "🚪 Выход из игры" << std::endl;
        changeState(GameState::EXIT);
    }, sf::Vector2f(450, 540));
    
    // Настройка меню паузы
    pauseMenu.setTitle("GAME PAUSED");
    pauseMenu.clearButtons();
    pauseMenu.addButton("RESUME GAME", [this]() {
        std::cout << "▶️ Продолжить игру" << std::endl;
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 300));
    
    pauseMenu.addButton("RESTART LEVEL", [this]() {
        std::cout << "🔄 Перезапуск уровня" << std::endl;
        // ЗАГРУЖАЕМ ТЕКУЩИЙ УРОВЕНЬ ЗАНОВО
        levelManager.loadLevel(levelManager.getCurrentLevelNumber());
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 380));
    
    pauseMenu.addButton("MAIN MENU", [this]() {
        std::cout << "🏠 В главное меню" << std::endl;
        changeState(GameState::MAIN_MENU);
    }, sf::Vector2f(450, 460));
    
    pauseMenu.addButton("EXIT GAME", [this]() {
        std::cout << "🚪 Выход из игры" << std::endl;
        changeState(GameState::EXIT);
    }, sf::Vector2f(450, 540));
}

void Game::setupLevelSelectMenu() {
    levelSelectMenu.setTitle("SELECT LEVEL");
    levelSelectMenu.clearButtons();
    
    // Динамически создаём кнопки для доступных уровней
    int unlockedLevels = levelManager.getUnlockedLevels();
    for (int i = 1; i <= 5; ++i) {
        if (i <= unlockedLevels) {
            levelSelectMenu.addButton("LEVEL " + std::to_string(i), [this, i]() {
                std::cout << "🎮 Выбран уровень " << i << std::endl;
                levelManager.loadLevel(i);
                changeState(GameState::PLAYING);
            }, sf::Vector2f(450, 200 + i * 80));
        } else {
            levelSelectMenu.addButton("LOCKED", []() {
                std::cout << "🔒 Уровень заблокирован!" << std::endl;
            }, sf::Vector2f(450, 200 + i * 80));
        }
    }
    
    levelSelectMenu.addButton("BACK", [this]() {
        changeState(GameState::MAIN_MENU);
    }, sf::Vector2f(450, 600));
}

void Game::handleGameEvents(const sf::Event& event) {
    // Обработка клавиши E для взаимодействия с посылками
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::E) {
            std::cout << "🎮 Нажата E в игровом режиме" << std::endl;
            levelManager.getCurrentLevel().handleEInteraction(); // ИСПРАВЛЕНО
        }
        
        // Обработка Escape для паузы
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            changeState(GameState::PAUSED);
        }
    }
}

void Game::handleContinuousInput() {
    // Проверяем состояние клавиш движения
    bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) || 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    
    bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    
    bool moveUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) || 
                  sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    
    bool moveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) || 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);
    
    // Создаем вектор движения
    sf::Vector2f movement(0.f, 0.f);
    
    if (moveLeft) movement.x -= 1.f;
    if (moveRight) movement.x += 1.f;
    if (moveUp) movement.y -= 1.f;
    if (moveDown) movement.y += 1.f;
    
    // Нормализуем вектор если есть движение
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    // Передаем движение игроку
    levelManager.getCurrentLevel().getPlayer().move(movement); // ИСПРАВЛЕНО
    
    // Обработка прыжка
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)) {
        levelManager.getCurrentLevel().getPlayer().jump(); // ИСПРАВЛЕНО
    }
}

void Game::checkLevelCompletion() {
    // РЕАЛЬНАЯ ПРОВЕРКА ЗАВЕРШЕНИЯ УРОВНЯ
    if (levelManager.getCurrentLevel().isLevelComplete()) {
        levelManager.markLevelComplete();
        std::cout << "🎉 Уровень " << levelManager.getCurrentLevelNumber() << " завершён!" << std::endl;
        
        // Переходим к выбору уровня
        changeState(GameState::LEVEL_SELECT);
    }
}