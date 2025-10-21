#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "InputHandler.h"

int main() {
    
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "Space Courier - Input System Demo");
    window.setFramerateLimit(60);
    
    // Инициализируем систему ввода
    InputHandler inputHandler;
    
    // Привязываем действия к клавишам
    using Scancode = sf::Keyboard::Scancode;
    
    // Кнопочки
    inputHandler.bindAction(Scancode::Space, []() {
        std::cout << "Прыжок" << std::endl;
    });
    
    inputHandler.bindAction(Scancode::E, []() {
        std::cout << "Действие" << std::endl;
    });

    // Игровые объекты
    sf::CircleShape player(25.f);
    player.setFillColor(sf::Color(220, 50, 50, 255));
    player.setPosition({100.f, 400.f});
    
    sf::RectangleShape platform({400.f, 20.f});
    platform.setFillColor(sf::Color(100, 200, 100, 255));
    platform.setPosition({300.f, 600.f});
    
    float playerSpeed = 5.0f;

    // Игровой цикл
    while (window.isOpen()) {
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
        auto position = player.getPosition();
        position.x += movement.x * playerSpeed;
        position.y += movement.y * playerSpeed;
        
        // Границы экрана
        if (position.x < 0.f) position.x = 0.f;
        if (position.x > 1150.f) position.x = 1150.f;
        if (position.y < 0.f) position.y = 0.f;
        if (position.y > 750.f) position.y = 750.f;
        
        player.setPosition(position);

        // Отрисовка
        window.clear(sf::Color(30, 30, 60, 255));
        window.draw(platform);
        window.draw(player);
        window.display();
    }
    
    return 0;
}