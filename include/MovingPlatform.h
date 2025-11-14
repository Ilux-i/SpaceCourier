#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class MovingPlatform : public Entity {
public:
    MovingPlatform(const sf::Vector2f& size, const sf::Vector2f& startPos, 
                   const sf::Vector2f& endPos, float speed, bool horizontal = true);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    // Для переноса игрока
    sf::Vector2f getMovement() const;
    
private:
    sf::RectangleShape shape; // Fallback
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded;
    
    sf::Vector2f startPosition;
    sf::Vector2f endPosition;
    sf::Vector2f currentTarget;
    float movementSpeed;
    bool isHorizontal;
    
    void loadTexture();
    void updateMovement(float deltaTime);
};