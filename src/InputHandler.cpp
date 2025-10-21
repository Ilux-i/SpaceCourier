#include "InputHandler.h"
#include <iostream>
#include <cmath>

InputHandler::InputHandler() {}

// Обновляет состояние клавиши
void InputHandler::update() {
    for (auto& [key, state] : keyStates_) {
        state.wasPressed = state.isPressed;
    }
}

// Обработчик событий
void InputHandler::handleEvent(const sf::Event& event) {

    if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::KeyReleased>()) {
        // События клавиши
        const sf::Event::KeyPressed* keyPressedEvent = event.getIf<sf::Event::KeyPressed>();
        const sf::Event::KeyReleased* keyReleasedEvent = event.getIf<sf::Event::KeyReleased>();
        
        // Определяем какая клавиша и состояние
        sf::Keyboard::Scancode key;
        bool isPressed;
        
        // Обробатывает состояние клавиши
        if (keyPressedEvent) {
            key = keyPressedEvent->scancode;
            isPressed = true;
        } else if (keyReleasedEvent) {
            key = keyReleasedEvent->scancode;
            isPressed = false;
        } else {
            return;
        }
        
        // Обновляем состояние клавиши
        keyStates_[key].isPressed = isPressed;
        
        // Если клавиша только что нажата - вызываем callback
        if (isPressed && actionCallbacks_.count(key)) {
            for (const auto& callback : actionCallbacks_.at(key)) {
                callback();
            }
        }
        
    }
}

// Гарантирует выполнение функции только при взаимодействии с кнопками
void InputHandler::bindAction(sf::Keyboard::Scancode key, std::function<void()> pressedCallback) {
    actionCallbacks_[key].push_back(pressedCallback);
}

// Проверка на зажатие
bool InputHandler::isKeyPressed(sf::Keyboard::Scancode key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second.isPressed;
}

// Проверка на нажатие
bool InputHandler::isKeyJustPressed(sf::Keyboard::Scancode key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second.isPressed && !it->second.wasPressed;
}

// Движение
sf::Vector2f InputHandler::getMovementVector() const {
    sf::Vector2f movement(0.f, 0.f);
    
    using Scancode = sf::Keyboard::Scancode;
    
    // Движение влево
    if (isKeyPressed(Scancode::A) || isKeyPressed(Scancode::Left)) {
        movement.x -= 1.f;
    }
    
    // Движение вправо
    if (isKeyPressed(Scancode::D) || isKeyPressed(Scancode::Right)) {
        movement.x += 1.f;
    }
    
    // Движение вверх
    if (isKeyPressed(Scancode::W) || isKeyPressed(Scancode::Up)) {
        movement.y -= 1.f;
    }
    
    // Движение вниз
    if (isKeyPressed(Scancode::S) || isKeyPressed(Scancode::Down)) {
        movement.y += 1.f;
    }
    
    // Диагональное движение
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    return movement;
}