#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "InputHandler.h"
#include "Level.h"

int main() {
    std::cout << "🚀 Space Courier - Game Launch!" << std::endl;
    
    try {
        // Используем английский заголовок как в тесте
        sf::RenderWindow window(sf::VideoMode({1200, 800}), sf::String("Space Courier"));
        window.setFramerateLimit(60);
        
        // Инициализируем систему ввода и уровень
        InputHandler inputHandler;
        Level level;
        
        // Привязываем действия к клавишам
        using Scancode = sf::Keyboard::Scancode;
        
        inputHandler.bindAction(Scancode::Space, [&level]() {
            level.getPlayer().jump();
        });
        
        inputHandler.bindAction(Scancode::E, []() {
            std::cout << "📦 Package interaction" << std::endl;
        });

        // Таймер для дельты времени
        sf::Clock clock;
        
        std::cout << "🎮 Controls:" << std::endl;
        std::cout << "   WASD/Arrows - Movement" << std::endl;
        std::cout << "   SPACE - Jump" << std::endl;
        std::cout << "   E - Interact" << std::endl;
        std::cout << "   ESC - Exit" << std::endl;

        // Игровой цикл
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            
            inputHandler.update();
            
            // Обработка событий
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
                
                inputHandler.handleEvent(*event);
                
                if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyEvent->scancode == Scancode::Escape) {
                        window.close();
                    }
                }
            }

            // Движение игрока
            sf::Vector2f movement = inputHandler.getMovementVector();
            level.getPlayer().move(movement);
            
            // Обновление уровня
            level.update(deltaTime);

            // Отрисовка
            window.clear(sf::Color(30, 30, 60, 255)); // Space background
            level.draw(window);
            window.display();
        }
        
        std::cout << "👋 Game finished!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ Unknown exception occurred!" << std::endl;
        return 1;
    }
    
    return 0;
}