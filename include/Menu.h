#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>

class Menu {
public:
    Menu();
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    
    void addButton(const std::string& text, std::function<void()> action, const sf::Vector2f& position);
    void clearButtons();
    void setTitle(const std::string& title);
    
private:
    struct Button {
        sf::RectangleShape shape;
        std::function<void()> action;
        bool isHovered;
    };
    
    std::vector<Button> buttons;
    std::string title;
    
    void setupMainMenu();
};