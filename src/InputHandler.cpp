#include "InputHandler.h"
#include <iostream>
#include <cmath>

InputHandler::InputHandler() {}

void InputHandler::update() {
    for (auto& [key, state] : keyStates_) {
        state.wasPressed = state.isPressed;
    }
}

void InputHandler::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::KeyReleased>()) {
        const sf::Event::KeyPressed* keyPressedEvent = event.getIf<sf::Event::KeyPressed>();
        const sf::Event::KeyReleased* keyReleasedEvent = event.getIf<sf::Event::KeyReleased>();
        
        sf::Keyboard::Scancode key;
        bool isPressed;
        
        if (keyPressedEvent) {
            key = keyPressedEvent->scancode;
            isPressed = true;
        } else if (keyReleasedEvent) {
            key = keyReleasedEvent->scancode;
            isPressed = false;
        } else {
            return;
        }
        
        keyStates_[key].isPressed = isPressed;
        
        if (isPressed && actionCallbacks_.count(key)) {
            std::cout << "🎯 Вызов callback для клавиши: " << static_cast<int>(key) << std::endl;
            for (const auto& callback : actionCallbacks_.at(key)) {
                callback();
            }
        }
    }
}

void InputHandler::bindAction(sf::Keyboard::Scancode key, std::function<void()> pressedCallback) {
    actionCallbacks_[key].push_back(pressedCallback);
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
    
    if (isKeyPressed(Scancode::A) || isKeyPressed(Scancode::Left)) {
        movement.x -= 1.f;
    }
    
    if (isKeyPressed(Scancode::D) || isKeyPressed(Scancode::Right)) {
        movement.x += 1.f;
    }
    
    if (isKeyPressed(Scancode::W) || isKeyPressed(Scancode::Up)) {
        movement.y -= 1.f;
    }
    
    if (isKeyPressed(Scancode::S) || isKeyPressed(Scancode::Down)) {
        movement.y += 1.f;
    }
    
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    return movement;
}