#include "InputHandler.h"
#include <iostream>
#include <cmath>

InputHandler::InputHandler() {
    setupDefaultBindings();
}

void InputHandler::setupDefaultBindings() {
    std::cout << "🎮 InputHandler: Setting up default bindings..." << std::endl;
}

void InputHandler::update() {
    for (auto& [key, state] : keyStates_) {
        state.wasPressed = state.isPressed;
    }
}

void InputHandler::handleEvent(const sf::Event& event) {
    // SFML 3.0.2 синтаксис - используем is<T>() и getIf<T>()
    if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::KeyReleased>()) {
        // Получаем указатель на событие клавиши
        const sf::Event::KeyPressed* keyPressedEvent = event.getIf<sf::Event::KeyPressed>();
        const sf::Event::KeyReleased* keyReleasedEvent = event.getIf<sf::Event::KeyReleased>();
        
        // Определяем какая клавиша и состояние
        sf::Keyboard::Scancode key;
        bool isPressed;
        
        if (keyPressedEvent) {
            key = keyPressedEvent->scancode;
            isPressed = true;
        } else if (keyReleasedEvent) {
            key = keyReleasedEvent->scancode;
            isPressed = false;
        } else {
            return; // Не должно происходить, но на всякий случай
        }
        
        // Обновляем состояние клавиши
        keyStates_[key].isPressed = isPressed;
        
        // Если клавиша только что нажата - вызываем callback'и
        if (isPressed && actionCallbacks_.count(key)) {
            for (const auto& callback : actionCallbacks_.at(key)) {
                callback();
            }
        }
        
        // Логирование для отладки (можно убрать позже)
        if (isPressed) {
            std::cout << "🎮 Key pressed: " << static_cast<int>(key) << std::endl;
        }
    }
}

void InputHandler::bindAction(sf::Keyboard::Scancode key, std::function<void()> pressedCallback) {
    actionCallbacks_[key].push_back(pressedCallback);
    std::cout << "🎮 Bound action to key: " << static_cast<int>(key) << std::endl;
}

bool InputHandler::isKeyPressed(sf::Keyboard::Scancode key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second.isPressed;
}

bool InputHandler::isKeyJustPressed(sf::Keyboard::Scancode key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second.isPressed && !it->second.wasPressed;
}

sf::Vector2f InputHandler::getMovementVector() const {
    sf::Vector2f movement(0.f, 0.f);
    
    using Scancode = sf::Keyboard::Scancode;
    
    // Движение влево (A или стрелка влево)
    if (isKeyPressed(Scancode::A) || isKeyPressed(Scancode::Left)) {
        movement.x -= 1.f;
    }
    
    // Движение вправо (D или стрелка вправо)
    if (isKeyPressed(Scancode::D) || isKeyPressed(Scancode::Right)) {
        movement.x += 1.f;
    }
    
    // Движение вверх (W или стрелка вверх)
    if (isKeyPressed(Scancode::W) || isKeyPressed(Scancode::Up)) {
        movement.y -= 1.f;
    }
    
    // Движение вниз (S или стрелка вниз)
    if (isKeyPressed(Scancode::S) || isKeyPressed(Scancode::Down)) {
        movement.y += 1.f;
    }
    
    // Нормализуем вектор для диагонального движения
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    return movement;
}