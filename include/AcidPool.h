#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class AcidPool : public Entity {
public:
    AcidPool(const sf::Vector2f& size, const sf::Vector2f& position);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    bool isPlayerInAcid() const;
    void setPlayerInAcid(bool inAcid);
    float getDamageCooldown() const;
    void resetDamageCooldown();
    
private:
    sf::RectangleShape shape; // Fallback
    sf::Texture texture;
    sf::Sprite sprite;
    bool textureLoaded;
    
    bool playerInAcid;
    float damageCooldown;
    
    void loadTexture();
};