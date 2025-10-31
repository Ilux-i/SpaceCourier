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
    
    bool loadFont(const std::string& fontPath);
    
private:
    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        std::function<void()> action;
        bool isHovered;
        std::string label;
        
        // Конструктор для инициализации text
        Button(const sf::Font& font, const std::string& labelText = "") 
            : text(font, labelText, 28), isHovered(false), label(labelText) {}
    };
    
    std::vector<Button> buttons;
    sf::Font font;
    std::string title;
    bool fontLoaded;
    
    void setupMainMenu();
    void createText(sf::Text& text, const std::string& content, unsigned int size, const sf::Vector2f& position) const;
};