#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class HealthSystem {
public:
    HealthSystem();
    
    void takeDamage();
    void heal();
    bool isAlive() const;
    int getHealth() const;
    void reset();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
private:
    int health;
    int maxHealth;
    float invincibilityTimer;
    bool isInvincible;
    
    void createHearts();
    std::vector<sf::CircleShape> hearts;
};