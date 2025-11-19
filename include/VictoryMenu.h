#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

class VictoryMenu {
public:
    VictoryMenu();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    void setMainMenuCallback(std::function<void()> callback);
    void setExitCallback(std::function<void()> callback);
    void setNextLevelCallback(std::function<void()> callback); // 🔥 НОВЫЙ КОЛБЭК
    
    void setLevelInfo(int currentLevel, int totalLevels); // 🔥 НОВЫЙ МЕТОД
    
private:
    void setupUI();
    
    sf::Font font;
    bool fontLoaded;
    
    std::vector<sf::Text> titles;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    
    std::function<void()> mainMenuCallback;
    std::function<void()> exitCallback;
    std::function<void()> nextLevelCallback; // 🔥 НОВЫЙ КОЛБЭК
    
    float animationTimer;
    int currentLevel;    // 🔥 ТЕКУЩИЙ УРОВЕНЬ
    int totalLevels;     // 🔥 ВСЕГО УРОВНЕЙ
    bool isFinalLevel;   // 🔥 ФЛАГ ФИНАЛЬНОГО УРОВНЯ
};