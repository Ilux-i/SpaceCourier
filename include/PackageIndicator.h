#pragma once
#include <SFML/Graphics.hpp>

class PackageIndicator {
public:
    PackageIndicator();
    
    void setVisible(bool visible);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
private:
    sf::RectangleShape icon; // Fallback
    sf::RectangleShape background; // Fallback
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded;
    
    bool isVisible;
    float pulseTimer;
    
    void loadTexture();
    void createIcon();
};