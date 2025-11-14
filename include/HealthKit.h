#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class HealthKit : public Entity {
public:
    HealthKit(const sf::Vector2f& position);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    bool isCollected() const;
    void collect();
    void respawn();
    
private:
    sf::CircleShape shape; // Fallback
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded;
    
    bool collected;
    
    void loadTexture();
};