#include "Game.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode({1200, 800}), sf::String("Space Courier"))
    , currentState(GameState::MAIN_MENU)
    , previousState(GameState::MAIN_MENU)
{
    window.setFramerateLimit(60);
    setupMenus();
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
    switch (currentState) {
        case GameState::MAIN_MENU:
            mainMenu.update(deltaTime);
            break;
        case GameState::PAUSED:
            pauseMenu.update(deltaTime);
            break;
        case GameState::PLAYING:
            // ОБРАБАТЫВАЕМ НЕПРЕРЫВНЫЙ ВВОД ДЛЯ ДВИЖЕНИЯ
            handleContinuousInput();
            currentLevel.update(deltaTime);
            break;
        case GameState::LEVEL_SELECT:
        case GameState::OPTIONS:
        case GameState::EXIT:
            // TODO: Реализовать позже
            break;
    }
}

void Game::render() {
    window.clear(sf::Color(30, 30, 60, 255));
    
    switch (currentState) {
        case GameState::MAIN_MENU:
            mainMenu.draw(window);
            break;
        case GameState::PAUSED:
            currentLevel.draw(window); // Фон - текущий уровень
            pauseMenu.draw(window);    // Поверх - меню паузы
            break;
        case GameState::PLAYING:
            currentLevel.draw(window);
            break;
        case GameState::LEVEL_SELECT:
        case GameState::OPTIONS:
        case GameState::EXIT:
            // TODO: Реализовать позже
            break;
    }
    
    window.display();
}

void Game::changeState(GameState newState) {
    previousState = currentState;
    currentState = newState;
    std::cout << "🔄 Смена состояния: " << static_cast<int>(previousState) 
              << " -> " << static_cast<int>(currentState) << std::endl;
}

void Game::setupMenus() {
    // Настройка главного меню
    mainMenu.setTitle("SPACE COURIER");
    mainMenu.clearButtons();
    mainMenu.addButton("START GAME", [this]() {
        std::cout << "🎮 Запуск игры!" << std::endl;
        changeState(GameState::PLAYING);
    }, sf::Vector2f(450, 300));
    
    mainMenu.addButton("LEVEL SELECT", [this]() {
        std::cout << "🗂️ Выбор уровня" << std::endl;
        // TODO: Реализовать позже
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
        currentLevel = Level();
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

void Game::handleGameEvents(const sf::Event& event) {
    // Обработка клавиши E для взаимодействия с посылками
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::E) {
            std::cout << "🎮 Нажата E в игровом режиме" << std::endl;
            currentLevel.handleEInteraction();
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
    currentLevel.getPlayer().move(movement);
    
    // Обработка прыжка
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)) {
        currentLevel.getPlayer().jump();
    }
}