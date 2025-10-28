#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

class InputHandler {
public:
    InputHandler();
    
    void update();
    
    void handleEvent(const sf::Event& event);
    
    void bindAction(sf::Keyboard::Scancode key, std::function<void()> pressedCallback);
    
    bool isKeyPressed(sf::Keyboard::Scancode key) const;
    bool isKeyJustPressed(sf::Keyboard::Scancode key) const;
    
    sf::Vector2f getMovementVector() const;

    void handleInstantJump(sf::Keyboard::Scancode key, std::function<void()> jumpCallback);

private:
    struct KeyState {
        bool isPressed = false;
        bool wasPressed = false;
    };
    
    std::unordered_map<sf::Keyboard::Scancode, KeyState> keyStates_;
    std::unordered_map<sf::Keyboard::Scancode, std::vector<std::function<void()>>> actionCallbacks_;
    
    void setupDefaultBindings();
};