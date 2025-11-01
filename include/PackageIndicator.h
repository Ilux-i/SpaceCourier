#pragma once
#include <SFML/Graphics.hpp>

class PackageIndicator {
public:
    PackageIndicator();
    
    void setVisible(bool visible);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
private:
    sf::RectangleShape icon;
    sf::RectangleShape background;
    bool isVisible;
    float pulseTimer;
    
    void createIcon();
};